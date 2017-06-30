/*
 * Copyright (C) 2017 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/time.h>
#include "yos/log.h"
#include "alink_export.h"
#include "json_parser.h"
#include "yos/framework.h"
#include "yos/network.h"
#include "kvmgr.h"
#include <netmgr.h>
#include <yos/cli.h>
#include <yos/cloud.h>

/* raw data device means device post byte stream to cloud,
 * cloud translate byte stream to json value by lua script
 * for each product model, vendor need to sumbit a lua script
 * doing this job
 *
 * undefine RAW_DATA_DEVICE, sample will post json string to cloud
 */
//#define RAW_DATA_DEVICE

#define Method_PostData    "postDeviceData"
#define Method_PostRawData "postDeviceRawData"
#define Method_GetAlinkTime "getAlinkTime"

#define PostDataFormat      "{\"ErrorCode\":{\"value\":\"%d\"},\"Hue\":{\"value\":\"%d\"},\"Luminance\":{\"value\":\"%d\"},\"Switch\":{\"value\":\"%d\"},\"WorkMode\":{\"value\":\"%d\"}}"
#define post_data_buffer_size    (512)
static void do_report(void);
#ifndef RAW_DATA_DEVICE
static char post_data_buffer[post_data_buffer_size];
#else
static char raw_data_buffer[post_data_buffer_size];

/* rawdata byte order
 *
 * rawdata[0] = 0xaa;
 * rawdata[1] = 0x07;
 * rawdata[2] = device.power;
 * rawdata[3] = device.work_mode;
 * rawdata[4] = device.temp_value;
 * rawdata[5] = device.light_value;
 * rawdata[6] = device.time_delay;
 * rawdata[7] = 0x55;
 */
#define RAW_DATA_SIZE           (8)
uint8_t device_state_raw_data[RAW_DATA_SIZE] = {
    0xaa, 0x07, 1, 8, 10, 50, 10, 0x55
};
#endif

enum {
    ATTR_ERRORCODE_INDEX,
    ATTR_HUE_INDEX,
    ATTR_LUMINANCE_INDEX,
    ATTR_SWITCH_INDEX,
    ATTR_WORKMODE_INDEX,
    ATTR_MAX_NUMS
};
#define DEVICE_ATTRS_NUM   (ATTR_MAX_NUMS)

int device_state[] = {0, 10, 50, 1, 2};/* default value */
char *device_attr[] = {
    "ErrorCode",
    "Hue",
    "Luminance",
    "Switch",
    "WorkMode",
    NULL
};

void helper_api_test(void);
void activate_button_pressed(void *arg);

static void cloud_connected(int cb_type, const char *json_buffer) {
    do_report();
}

static void cloud_disconnected(int cb_type, const char *json_buffer) { printf("alink cloud disconnected!\n"); }

int callback_upgrade_device(const char *params)
{
    printf("alink device start to upgrade. \n");
}

int callback_cancel_upgrade_device(const char *params)
{
    printf("alink device stop to upgrade. \n");
}

#ifndef RAW_DATA_DEVICE
static void cloud_get_device_status(int cb_type, const char *json_buffer)
{
    do_report();
    printf("---> get device status :  %s\n",json_buffer);
}

static void cloud_set_device_status(int cb_type, const char *json_buffer)
{
    int attr_len = 0, value_len = 0, value = 0, i;
    char *value_str = NULL, *attr_str = NULL;

    printf("---> set device status :  %s\n",json_buffer);
    for (i = 0; device_attr[i]; i++) {
        attr_str = json_get_value_by_name((char *)json_buffer, strlen(json_buffer),
                device_attr[i], &attr_len, NULL);
        value_str = json_get_value_by_name(attr_str, attr_len,
                "value", &value_len, NULL);

        if (value_str && value_len > 0) {
            char last_char = *(value_str+value_len);
            *(value_str + value_len) = 0;
            value = atoi(value_str);
            *(value_str + value_len) = last_char;
            device_state[i] = value;
        }
    }
    do_report();
}

#else
static char a2x(char ch)
{
    switch (ch) {
        case '1':
            return 1;
        case '2':
            return 2;
        case '3':
            return 3;
        case '4':
            return 4;
        case '5':
            return 5;
        case '6':
            return 6;
        case '7':
            return 7;
        case '8':
            return 8;
        case '9':
            return 9;
        case 'A':
        case 'a':
            return 10;
        case 'B':
        case 'b':
            return 11;
        case 'C':
        case 'c':
            return 12;
        case 'D':
        case 'd':
            return 13;
        case 'E':
        case 'e':
            return 14;
        case 'F':
        case 'f':
            return 15;
        default:
            break;;
    }

    return 0;
}

int uart_send(const uint8_t *raw_data, int raw_data_len)
{
    //TODO: implement uart send here
    int i;
    for (i = 0; i < raw_data_len; i++)
        printf("%02x ", raw_data[i]);

    printf("\n");
    return 0;
}

int raw_data_unserialize(char *json_buffer, uint8_t *raw_data, int *raw_data_len)
{
    int attr_len = 0, i = 0;
    char *attr_str = NULL;

    assert(json_buffer && raw_data && raw_data_len);

    attr_str = json_get_value_by_name(json_buffer, strlen(json_buffer),
            "rawData", &attr_len, NULL);

    if (!attr_str || !attr_len || attr_len > *raw_data_len * 2)
        return -1;

    for (i = 0; i < attr_len; i += 2) {
        raw_data[i / 2] = a2x(attr_str[i]) << 4;
        raw_data[i / 2] += a2x(attr_str[i + 1]);
    }

    raw_data[i / 2] = '\0';
    *raw_data_len = i / 2;

    return 0;
}

static void cloud_get_device_raw_data(int cb_type, const char *json_buffer)
{
    int ret = 0, raw_data_len = RAW_DATA_SIZE;
    uint8_t raw_data[RAW_DATA_SIZE] = { 0 };

    ret = raw_data_unserialize(json_buffer, raw_data, &raw_data_len);
    if (!ret)
        uart_send(raw_data, raw_data_len);
}

static void cloud_set_device_raw_data(int cb_type, const char *json_buffer)
{
    int ret = 0, raw_data_len = RAW_DATA_SIZE;
    uint8_t raw_data[RAW_DATA_SIZE] = { 0 };

    ret = raw_data_unserialize(json_buffer, raw_data, &raw_data_len);
    if (!ret) {
        /* update device state */
        memcpy(device_state_raw_data, raw_data, raw_data_len);
        uart_send(raw_data, raw_data_len);
    }
}

int alink_post_raw_data(uint8_t *byte_stream, int len)
{
        int i, size;
#define RawDataHeader   "{\"rawData\":\""
#define RawDataTail     "\", \"length\":\"%d\"}"

        size = strlen(RawDataHeader);
        strncpy(raw_data_buffer, RawDataHeader, post_data_buffer_size);
        for (i = 0; i < len; i++) {
           size += snprintf(raw_data_buffer + size,
                   post_data_buffer_size - size, "%02X", byte_stream[i]);
        }

        size += snprintf(raw_data_buffer + size,
                post_data_buffer_size - size, RawDataTail, len * 2);

        return alink_report(Method_PostRawData, raw_data_buffer);
}
#endif

static uint32_t work_time = 60*60*10*1000; //default work time 1ms

static void do_report(void)
{
    //TODO: async
    //yos_loop_schedule_work(1000, activate_button_pressed, NULL, NULL, NULL);
    //helper_api_test();
#ifdef RAW_DATA_DEVICE
    /*
     * Note: post data to cloud,
     * use sample alink_post_raw_data()
     * or alink_post()
     */
    /* sample for raw data device */
    alink_post_raw_data(device_state_raw_data, RAW_DATA_SIZE);

#else
    /* sample for json data device */
    snprintf(post_data_buffer, post_data_buffer_size, PostDataFormat,
            device_state[ATTR_ERRORCODE_INDEX],
            device_state[ATTR_HUE_INDEX],
            device_state[ATTR_LUMINANCE_INDEX],
            device_state[ATTR_SWITCH_INDEX],
            device_state[ATTR_WORKMODE_INDEX]);
    printf("start report async\n");
    yos_cloud_report(Method_PostData, post_data_buffer, NULL, NULL);
#endif
}

int alink_get_time(unsigned int *utc_time)
{
#define TIME_STR_LEN    (32)
    char buf[TIME_STR_LEN] = { 0 }, *attr_str;
    int size = TIME_STR_LEN, attr_len = 0;
    int ret;

    ret = alink_query(Method_GetAlinkTime, "{}", buf, &size);
    if (!ret) {
        attr_str = json_get_value_by_name(buf, size, "time", &attr_len, NULL);
        if (attr_str && utc_time) {
            sscanf(attr_str, "%u", utc_time);
        }
    }

    return ret;
}

void helper_api_test(void)
{
    unsigned int time;
    int ret = alink_get_time(&time);
    assert(!ret);

    printf("get alink utc time: %d\n", time);
}

void awss_demo(void)
{
#if 0
    char ssid[PLATFORM_MAX_SSID_LEN] = { 0 };
    char passwd[PLATFORM_MAX_PASSWD_LEN] = { 0 };
#define WLAN_CONNECTION_TIMEOUT     (30 * 1000) //30 seconds

    /* after system booting, read ssid & passwd from flash */
    vendor_read_ssid_passwd_from_flash(ssid, passwd);

    if (ssid is empty)
        awss_start(); /* Note: awss_start() will block until success */
#endif

    /* call alink_start() after system got IP address */

    /* activate device after alink connnected, see activate_button_pressed */
}

/* activate sample */
char active_data_tx_buffer[128];
#define ActivateDataFormat    "{\"ErrorCode\": { \"value\": \"%d\" }}"
void activate_button_pressed(void* arg)
{
    sprintf(active_data_tx_buffer, ActivateDataFormat, 1);
    printf("active send:%s", active_data_tx_buffer);
    alink_report_async(Method_PostData, (char *)active_data_tx_buffer, NULL, NULL);

    sprintf(active_data_tx_buffer, ActivateDataFormat, 0);
    printf("send:%s", active_data_tx_buffer);
    alink_report_async(Method_PostData, (char *)active_data_tx_buffer, NULL, NULL);
}

static void handle_active_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    activate_button_pressed(NULL);
}

static struct cli_command ncmd = {
    .name = "active_alink",
    .help = "active_alink [start]",
    .function = handle_active_cmd
};

static void handle_model_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    #define MAX_MODEL_LENGTH 30
    char model[MAX_MODEL_LENGTH] = "light";
    int  model_len = sizeof(model);
    yos_kv_get("model", model, &model_len);

    if (argc == 1) {
        printf("Usage: model light/gateway. Model is currently %s\r\n", model);
        return;
    }

    if (strcmp(argv[1], "gateway") == 0) {
        if (strcmp(model, argv[1])) {
            yos_kv_del("alink");
            yos_kv_set("model", "gateway", MAX_MODEL_LENGTH, 1);
            printf("Swith model to gateway, please reboot\r\n");
        } else {
            printf("Current model is already gateway\r\n");
        }
    } else {
        if (strcmp(model, argv[1])) {
            yos_kv_del("alink");
            yos_kv_set("model", "light", MAX_MODEL_LENGTH, 1);
            printf("Swith model to light, please reboot\r\n");
        } else {
            printf("Current model is already light\r\n");
        }
    }
}

static struct cli_command modelcmd = {
    .name = "model",
    .help = "model light/gateway",
    .function = handle_model_cmd
};

static void handle_uuid_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    extern int cloud_is_connected(void);
    extern char *config_get_main_uuid(void);
    if (cloud_is_connected) {
        printf("uuid: %s\r\n", config_get_main_uuid());
    } else {
        printf("alink is not connected\r\n");
    }
}

static struct cli_command uuidcmd = {
    .name = "uuid",
    .help = "uuid",
    .function = handle_uuid_cmd
};

enum SERVER_ENV {
    DAILY = 0,
    SANDBOX,
    ONLINE,
    DEFAULT
};
const char *env_str[] = {"daily", "sandbox", "online", "default"};

void usage(void)
{
    printf("\nalink_sample -e enviroment -t work_time -l log_level\n");
    printf("\t -e alink server environment, 'daily', 'sandbox' or 'online'(default)\n");
    printf("\t -t work time, unit is s\n");
    printf("\t -l log level, trace/debug/info/warn/error/fatal/none\n");
    printf("\t -h show help text\n");
}
enum MESH_ROLE {
    MESH_MASTER = 0,
    MESH_NODE
};

static int env = DEFAULT;
static int mesh_mode = MESH_NODE;
static char *mesh_num = "1";
static char log_level = 0;
extern char *optarg;

void parse_opt(int argc, char *argv[])
{
    char ch;

    while (argc > 1 && (ch = getopt(argc, argv, "e:t:l:m:n:h")) != -1) {
        switch (ch) {
        case 'e':
            if (!strcmp(optarg, "daily"))
                env = DAILY;
            else if (!strcmp(optarg, "sandbox"))
                env = SANDBOX;
            else if (!strcmp(optarg, "online"))
                env = ONLINE;
            else {
                env = ONLINE;
                printf("unknow opt %s, use default env\n", optarg);
            }
            break;
        case 't':
            work_time = atoi(optarg);
            break;
        case 'l':
            /*
            if (!strcmp(optarg, "trace"))
                log_level = YOS_LL_TRACE;
            else if (!strcmp(optarg, "debug"))
                log_level = ALINK_LL_DEBUG;
            else if (!strcmp(optarg, "info"))
                log_level = ALINK_LL_INFO;
            else if (!strcmp(optarg, "warn"))
                log_level = ALINK_LL_WARN;
            else if (!strcmp(optarg, "error"))
                log_level = ALINK_LL_ERROR;
            else if (!strcmp(optarg, "fatal"))
                log_level = ALINK_LL_FATAL;
            else if (!strcmp(optarg, "none"))
                log_level = ALINK_LL_NONE;
            else
                log_level = ALINK_LL_INFO;
                */
            break;
        case 'm':
            if (!strcmp(optarg, "master"))
                mesh_mode = MESH_MASTER;
            else if (!strcmp(optarg, "node"))
                mesh_mode = MESH_NODE;
            else {
                mesh_mode = MESH_NODE;
                printf("unknow opt %s, default to MESH_NODE\n", optarg);
            }
            break;
        case 'n':
            mesh_num = optarg;
            break;
        case 'h':
            usage();
            exit(0);
        default:
            break;
        }
    }

    printf("alink server: %s, work_time: %ds, log level: %d\n",
            env_str[env], work_time, log_level);
}
extern char *g_sn;

static int is_alink_started = 0;

static void alink_service_event(input_event_t *event, void *priv_data) {
    if (event->type != EV_WIFI) {
        return;
    }

    if (event->code != CODE_WIFI_ON_GOT_IP) {
        return;
    }

    if(is_alink_started == 0) {
        is_alink_started = 1;
        alink_start();
    }
}

static void alink_connect_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_SYS) {
        return;
    }

    if (event->code == CODE_SYS_ON_ALINK_ONLINE ) {
        yos_post_event(EV_SYS, CODE_SYS_ON_START_FOTA, 0);
        return;
    }
}
static int alink_cloud_report(const char *method, const char *json_buffer)
{
    return alink_report_async(method, json_buffer, NULL, NULL);
}

static void alink_cloud_connected(void) {
    yos_post_event(EV_YUNIO, CODE_YUNIO_ON_CONNECTED, 0);
    printf("alink cloud connected!\n");

    yos_cloud_register_backend(&alink_cloud_report);
    yos_cloud_trigger(CLOUD_CONNECTED, NULL);
}

static void alink_cloud_disconnected(void) {
    yos_cloud_trigger(CLOUD_DISCONNECTED, NULL);
}

static void alink_cloud_get_device_status(char *json_buffer)
{
    yos_cloud_trigger(GET_DEVICE_STATUS, json_buffer);
}

static void alink_cloud_set_device_status(char *json_buffer)
{
    yos_cloud_trigger(SET_DEVICE_STATUS, json_buffer);
}

static void alink_cloud_get_device_raw_data(char *json_buffer)
{
    yos_cloud_trigger(GET_DEVICE_RAWDATA, json_buffer);
}

static void alink_cloud_set_device_raw_data(char *json_buffer)
{
    yos_cloud_trigger(SET_DEVICE_RAWDATA, json_buffer);
}

static void alink_cloud_init(void)
{
    alink_register_callback(ALINK_CLOUD_CONNECTED, &alink_cloud_connected);
    alink_register_callback(ALINK_CLOUD_DISCONNECTED, &alink_cloud_disconnected);
    alink_register_callback(ALINK_GET_DEVICE_RAWDATA, &alink_cloud_get_device_raw_data);
    alink_register_callback(ALINK_SET_DEVICE_RAWDATA, &alink_cloud_set_device_raw_data);
    alink_register_callback(ALINK_GET_DEVICE_STATUS, &alink_cloud_get_device_status);
    alink_register_callback(ALINK_SET_DEVICE_STATUS, &alink_cloud_set_device_status);

    yos_cloud_register_callback(CLOUD_CONNECTED, &cloud_connected);
    yos_cloud_register_callback(CLOUD_DISCONNECTED, &cloud_disconnected);
    /*
     * NOTE: register ALINK_GET/SET_DEVICE_STATUS or ALINK_GET/SET_DEVICE_RAWDATA
     */
#ifdef RAW_DATA_DEVICE
    /*
     * TODO: before using callback ALINK_GET/SET_DEVICE_RAWDATA,
     * submit product_model_xxx.lua script to ALINK cloud.
     * ALINKTEST_LIVING_LIGHT_SMARTLED_LUA is done with it.
     */
    yos_cloud_register_callback(GET_DEVICE_RAWDATA, &cloud_get_device_raw_data);
    yos_cloud_register_callback(SET_DEVICE_RAWDATA, &cloud_set_device_raw_data);
#else
    yos_cloud_register_callback(GET_DEVICE_STATUS, &cloud_get_device_status);
    yos_cloud_register_callback(SET_DEVICE_STATUS, &cloud_set_device_status);
#endif
}

int application_start(int argc, char *argv[])
{
    parse_opt(argc, argv);

    //if(argc > 1 && strlen(argv[1]) <= 65){
    //    printf("reset sn to : %s\n",argv[1]);
    //    g_sn = argv[1];
    //}
    yos_set_log_level(YOS_LL_DEBUG);

    if (mesh_mode == MESH_MASTER) {
#ifdef CONFIG_YOS_DDM
        ddm_run(argc, argv);
#endif
        return 0;
    }

    //awss_demo();
    if (env == SANDBOX)
        alink_enable_sandbox_mode();
    else if (env == DAILY)
        alink_enable_daily_mode(NULL, 0);

    alink_cloud_init();

    yos_register_event_filter(EV_WIFI, alink_service_event, NULL);
    yos_register_event_filter(EV_SYS, alink_connect_event, NULL);

    netmgr_init();
    netmgr_start(false);

    cli_register_command(&ncmd);
    cli_register_command(&uuidcmd);
    cli_register_command(&modelcmd);

#ifdef CONFIG_YOS_DDA
    dda_enable(atoi(mesh_num));
    dda_service_init();
    dda_service_start();
#else
    yos_loop_run();
    printf("alink end.\n");
    alink_end();
#endif

    return 0;
}
