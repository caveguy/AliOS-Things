/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 *
 * Alibaba Group retains all right, title and interest (including all
 * intellectual property rights) in and to this computer program, which is
 * protected by applicable intellectual property laws.  Unless you have
 * obtained a separate written license from Alibaba Group., you are not
 * authorized to utilize all or a part of this computer program for any
 * purpose (including reproduction, distribution, modification, and
 * compilation into object code), and you must immediately destroy or
 * return to Alibaba Group all copies of this computer program.  If you
 * are licensed by Alibaba Group, your rights to utilize this computer
 * program are limited by the terms of that license.  To obtain a license,
 * please contact Alibaba Group.
 *
 * This computer program contains trade secrets owned by Alibaba Group.
 * and, unless unauthorized by Alibaba Group in writing, you agree to
 * maintain the confidentiality of this computer program and related
 * information and to not disclose this computer program and related
 * information to any other person or entity.
 *
 * THIS COMPUTER PROGRAM IS PROVIDED AS IS WITHOUT ANY WARRANTIES, AND
 * Alibaba Group EXPRESSLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED,
 * INCLUDING THE WARRANTIES OF MERCHANTIBILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, TITLE, AND NONINFRINGEMENT.
 */

#include <stdlib.h>
#include "awss_wifimgr.h"
#include "platform/platform.h"
#include "log.h"
#include "awss_main.h"
#include "passwd.h"
#include "json_parser.h"
#include "utils.h"
#include "enrollee.h"
#include "os.h"
#include "awss_cmp.h"
#include "awss_notify.h"
#include "work_queue.h"
#include "zconfig_utils.h"

#define WIFI_APINFO_LIST_LEN    (512)
#define DEV_SIMPLE_ACK_LEN      (64)

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

static char g_req_msg_id[MSG_REQ_ID_LEN];
static platform_netaddr_t g_wifimgr_req_sa;

static void wifimgr_scan_request();
static struct work_struct scan_work = {
    .func = (work_func_t)&wifimgr_scan_request,
    .prio = 1, /* smaller digit means higher priority */
    .name = "scan",
};

static void wifimgr_scan_tx_wifilist();
static struct work_struct scan_tx_wifilist_work = {
    .func = (work_func_t)&wifimgr_scan_tx_wifilist,
    .prio = 1, /* smaller digit means higher priority */
    .name = "scan",
};

static char wifi_scan_runninng = 0;
static void *g_scan_mutex;

typedef struct scan_list {
    list_head_t entry;
    void *data;
} scan_list_t;

static LIST_HEAD(g_scan_list);

int wifimgr_scan_init(void)
{
    if (wifi_scan_runninng)
        return 0;

    g_scan_mutex = HAL_MutexCreate();
    INIT_LIST_HEAD(&g_scan_list);
    wifi_scan_runninng = 1;
    return 0;
}

static void wifimgr_scan_tx_wifilist()
{
    scan_list_t *item =  NULL, *next = NULL;

    char topic[TOPIC_LEN_MAX] = {0};
    awss_build_topic((const char *)TOPIC_AWSS_WIFILIST, topic, TOPIC_LEN_MAX);

    HAL_MutexLock(g_scan_mutex);
    list_for_each_entry_safe(item, next, &g_scan_list, entry, scan_list_t) {
        if (item && item->data) {
            if (0 != awss_cmp_coap_ob_send(item->data, strlen((char *)item->data) + 1,
                                           &g_wifimgr_req_sa, topic, NULL)) {
                awss_debug("sending failed.");
            }
            os_free(item->data);
        }
        list_del(&item->entry);
        os_free(item);
        item= NULL;
    }
    HAL_MutexUnlock(g_scan_mutex);
}

static int awss_scan_cb(const char ssid[PLATFORM_MAX_SSID_LEN],
           const uint8_t bssid[ETH_ALEN],
           enum AWSS_AUTH_TYPE auth,
           enum AWSS_ENC_TYPE encry,
           uint8_t channel, signed char rssi,
           int last_ap)
{
#define ONE_AP_INFO_LEN_MAX           (141)
    static char *aplist = NULL;
    static int msg_len = 0;

    if (aplist == NULL) {
        aplist = os_zalloc(WIFI_APINFO_LIST_LEN);
        if (aplist == NULL)
            return SHUB_ERR;
        msg_len = 0;
        msg_len += snprintf(aplist + msg_len, WIFI_APINFO_LIST_LEN - msg_len - 1, "{\"awssVer\":%s, \"wifiList\":[", AWSS_VER);
    }

    if ((ssid != NULL) && (ssid[0] != '\0')) {
        uint8_t bssid_connected[ETH_ALEN];
        char *other_apinfo = os_zalloc(64);
        char *encode_ssid = os_zalloc(OS_MAX_SSID_LEN * 2 + 1);
        os_wifi_get_ap_info(NULL, NULL, bssid_connected);
        if (other_apinfo && encode_ssid) {
            if (memcmp(bssid_connected, bssid, ETH_ALEN) == 0) {
                snprintf(other_apinfo, 64 - 1, "\"auth\":\"%d\",\"connected\":\"1\"", auth);
            } else {
                snprintf(other_apinfo, 64 - 1, "\"auth\":\"%d\"", auth);
            }
            if (is_utf8(ssid, strlen(ssid))) {
                msg_len += snprintf(aplist + msg_len, WIFI_APINFO_LIST_LEN - msg_len - 1,
                                   "{\"ssid\":\"%s\",\"bssid\":\"%02X:%02X:%02X:%02X:%02X:%02X\",\"rssi\":\"%d\",%s},",
                                   ssid, bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5],
                                   rssi > 0 ? rssi - 256 : rssi, other_apinfo);
            } else {
                utils_hex_to_str((unsigned char *)ssid, strlen(ssid), encode_ssid, OS_MAX_SSID_LEN * 2);
                msg_len += snprintf(aplist + msg_len, WIFI_APINFO_LIST_LEN - msg_len - 1,
                                   "{\"xssid\":\"%s\",\"bssid\":\"%02X:%02X:%02X:%02X:%02X:%02X\",\"rssi\":\"%d\",%s},",
                                   encode_ssid, bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5],
                                   rssi > 0 ? rssi - 256 : rssi, other_apinfo);
            }
        }

        if (other_apinfo) os_free(other_apinfo);
        if (encode_ssid) os_free(encode_ssid);
    }
    awss_debug("last_ap:%u\r\n", last_ap);

    if (last_ap || WIFI_APINFO_LIST_LEN < msg_len + ONE_AP_INFO_LEN_MAX + strlen(AWSS_ACK_FMT)) {
        if (aplist[msg_len - 1] == ',') {
            msg_len--;    /* eating the last ',' */
        }
        msg_len += snprintf(aplist + msg_len, WIFI_APINFO_LIST_LEN - msg_len - 1, "]}");

        uint32_t tlen = DEV_SIMPLE_ACK_LEN + msg_len + 1;
        msg_len = 0;
        char *msg_aplist = os_zalloc(tlen);
        if (!msg_aplist) {
            os_free(aplist);
            aplist = NULL;
            return SHUB_ERR;
        }

        snprintf(msg_aplist, tlen - 1, AWSS_ACK_FMT, g_req_msg_id, 200, aplist);
        os_free(aplist);
        aplist = NULL;

        scan_list_t *list = (scan_list_t *)malloc(sizeof(scan_list_t));
        if (!list) {
            awss_debug("scan list fail\n");
            os_free(msg_aplist);
            list = NULL;
            return SHUB_ERR;
        }
        list->data = msg_aplist;
        HAL_MutexLock(g_scan_mutex);
        list_add(&list->entry, &g_scan_list);
        HAL_MutexUnlock(g_scan_mutex);

        if (last_ap) queue_work(&scan_tx_wifilist_work);
        awss_debug("sending message to app: %s\n", msg_aplist);
    }

    return 0;
}

static void wifimgr_scan_request()
{
    wifimgr_scan_init();
    os_wifi_scan(&awss_scan_cb);
}
/*
 * @desc: ????getWifiList??Ϣ
 *
 */
int wifimgr_process_get_wifilist_request(void *ctx, void *resource,void *remote, void *request)
{
    char buf[DEV_SIMPLE_ACK_LEN] = {0};
    char *msg = NULL, *id = NULL;
    int len = 0, id_len = 0;

    msg = awss_cmp_get_coap_payload(request, &len);
    if (msg == NULL || len == 0)
        return -1;

    queue_work(&scan_work);

    id = json_get_value_by_name(msg, len, "id", &id_len, 0);
    memset(g_req_msg_id, 0, sizeof(g_req_msg_id));
    if (id && id_len < sizeof(g_req_msg_id) - 1)
        memcpy(g_req_msg_id, id, id_len);

    snprintf(buf, DEV_SIMPLE_ACK_LEN - 1, AWSS_ACK_FMT, g_req_msg_id, 200, "\"success\"");

    awss_debug("sending message to app: %s\n", buf);
    char topic[TOPIC_LEN_MAX] = {0};
    awss_build_topic((const char *)TOPIC_AWSS_WIFILIST, topic, TOPIC_LEN_MAX);
    memcpy(&g_wifimgr_req_sa, remote, sizeof(g_wifimgr_req_sa));
    if (0 != awss_cmp_coap_send_resp(buf, strlen(buf), &g_wifimgr_req_sa, topic, request)) {
        awss_debug("sending failed.");
    }

    return SHUB_OK;
}

static int wifimgr_process_get_device_info(void *ctx, void *resource, void *remote, void *request, char is_mcast)
{
    char *buf = NULL;
    char *dev_info = NULL;
    int len = 0, id_len = 0;
    char *msg = NULL, *id = NULL;
    char req_msg_id[MSG_REQ_ID_LEN] = {0};

    buf = os_zalloc(DEV_INFO_LEN_MAX);
    if (!buf)
        goto DEV_INFO_ERR;
    dev_info = os_zalloc(DEV_INFO_LEN_MAX);
    if (!dev_info)
        goto DEV_INFO_ERR;
    msg = awss_cmp_get_coap_payload(request, &len);
    id = json_get_value_by_name(msg, len, "id", &id_len, 0);
    if (id && id_len < MSG_REQ_ID_LEN)
        memcpy(req_msg_id, id, id_len);

    awss_build_dev_info(AWSS_NOTIFY_DEV_RAND, buf, DEV_INFO_LEN_MAX);
    snprintf(dev_info, DEV_INFO_LEN_MAX - 1, "{%s}", buf);

    awss_debug("dev_info:%s\r\n", dev_info);
    memset(buf, 0x00, DEV_INFO_LEN_MAX);
    snprintf(buf, DEV_INFO_LEN_MAX - 1, AWSS_ACK_FMT, req_msg_id, 200, dev_info);

    os_free(dev_info);

    awss_debug("sending message to app: %s", buf);
    char topic[TOPIC_LEN_MAX] = {0};
    if (is_mcast)
        awss_build_topic((const char *)TOPIC_AWSS_GETDEVICEINFO_MCAST, topic, TOPIC_LEN_MAX);
    else
        awss_build_topic((const char *)TOPIC_AWSS_GETDEVICEINFO_UCAST, topic, TOPIC_LEN_MAX);
    if (0 != awss_cmp_coap_send_resp(buf, strlen(buf), remote, topic, request)) {
        awss_debug("sending failed.");
    }

    os_free(buf);
    return SHUB_OK;

DEV_INFO_ERR:
    if (buf) os_free(buf);
    if (dev_info) os_free(dev_info);

    return -1;
}

int wifimgr_process_mcast_get_device_info(void *ctx, void *resource, void *remote, void *request)
{
    return wifimgr_process_get_device_info(ctx, resource, remote, request, 1);
}

int wifimgr_process_ucast_get_device_info(void *ctx, void *resource, void *remote, void *request)
{
    return wifimgr_process_get_device_info(ctx, resource, remote, request, 0);
}

#define WLAN_CONNECTION_TIMEOUT     (30 * 1000) //30 seconds
int switch_ap_done = 0;

int wifimgr_process_switch_ap_request(void *ctx, void *resource, void *remote, void *request)
{
    char ssid[PLATFORM_MAX_SSID_LEN * 2 + 1] = {0}, passwd[PLATFORM_MAX_PASSWD_LEN + 1] = {0};
    int str_len = 0, success = 1, i  = 0, len = 0, enc_lvl = SEC_LVL_OPEN;
    char req_msg_id[MSG_REQ_ID_LEN] = {0};
    char *str = NULL, *buf = NULL;
    char msg[128] = {0};
    char ssid_found = 0;

    static char switch_ap_parsed = 0;
    if (switch_ap_parsed != 0)
        return SHUB_ERR;
    switch_ap_parsed = 1;

    buf = awss_cmp_get_coap_payload(request, &len);
    str = json_get_value_by_name(buf, len, "id", &str_len, 0);
    memcpy(req_msg_id, str, str_len > MSG_REQ_ID_LEN - 1 ? MSG_REQ_ID_LEN - 1 : str_len);
    awss_debug("switch ap, len:%u, %s\r\n", len, buf);
    buf = json_get_value_by_name(buf, len, "params", &len, 0);

    do {
        snprintf(msg, sizeof(msg) - 1, AWSS_ACK_FMT, req_msg_id, 200, "\"success\"");

        str_len = 0;
        str = json_get_value_by_name(buf, len, "ssid", &str_len, 0);
        awss_debug("ssid, len:%u, %s\r\n", str_len, str != NULL ? str : "NULL");
        if (str && (str_len < PLATFORM_MAX_SSID_LEN)) {
            memcpy(ssid, str, str_len);
            ssid_found = 1;
        }

        if (!ssid_found) {
            str_len = 0;
            str = json_get_value_by_name(buf, len, "xssid", &str_len, 0);
            if (str && (str_len < PLATFORM_MAX_SSID_LEN * 2 - 1)) {
                memcpy(ssid, str, str_len);
                uint8_t decoded[OS_MAX_SSID_LEN] = {0};
                int len = str_len / 2;
                utils_str_to_hex(ssid, str_len, decoded, OS_MAX_SSID_LEN);
                memcpy(ssid, (const char *)decoded, len);
                ssid[len] = '\0';
            } else {
                snprintf(msg, sizeof(msg) - 1, AWSS_ACK_FMT, req_msg_id, -1, "\"ssid error\"");
                success = 0;
                break;
            }
        }

        str_len = 0;
        str = json_get_value_by_name(buf, len, "cipherType", &str_len, 0);
        awss_debug("enr");
        if (!str) {
            success = 0;
            snprintf(msg, sizeof(msg) - 1, AWSS_ACK_FMT, req_msg_id, -4, "\"no security level error\"");
            break;
        }

        enc_lvl = atoi(str);
        if (enc_lvl != os_get_conn_encrypt_type()) {
            success = 0;
            snprintf(msg, sizeof(msg) - 1, AWSS_ACK_FMT, req_msg_id, -4, "\"security level error\"");
            break;
        }

        str_len = 0;
        str = json_get_value_by_name(buf, len, "passwd", &str_len, 0);
        // TODO: empty passwd is allow? json parse "passwd":"" result is NULL?
        switch (enc_lvl) {
            case SEC_LVL_AES256:
                snprintf(msg, sizeof(msg) - 1, AWSS_ACK_FMT, req_msg_id, -4, "\"aes256 not support\"");
                success = 0;
                break;
            default:
                break;
        }

        if (success == 0)
            break;

        if (0 == enc_lvl) {
            if (str_len < PLATFORM_MAX_PASSWD_LEN) {
                memcpy(passwd, str, str_len);
            } else {
                snprintf(msg, sizeof(msg) - 1, AWSS_ACK_FMT, req_msg_id, -2, "\"passwd len error\"");
                success = 0;
            }
        } else {
            if (str_len < (PLATFORM_MAX_PASSWD_LEN * 2) - 1) {
                char encoded[PLATFORM_MAX_PASSWD_LEN * 2 + 1] = {0};
                memcpy(encoded, str, str_len);
                aes_decrypt_string(encoded, passwd, str_len, os_get_conn_encrypt_type(), 1); //64bytes=2x32bytes
            } else {
                snprintf(msg, sizeof(msg) - 1, AWSS_ACK_FMT, req_msg_id, -3, "\"passwd len error\"");
                success = 0;
            }
        }

        if (success && is_utf8(passwd, strlen(passwd)) == 0) {
            snprintf(msg, sizeof(msg) - 1, AWSS_ACK_FMT, req_msg_id,
                     enc_lvl == SEC_LVL_OPEN ? -2 : -3 , "\"passwd content error\"");
            success = 0;
        }
    } while (0);

    awss_devinfo_notify_stop();
    awss_connectap_notify_stop();

    awss_debug("Sending message to app: %s", msg);
    awss_debug("switch to ap: '%s' '%s'", ssid, passwd);
    char topic[TOPIC_LEN_MAX] = {0};
    awss_build_topic((const char *)TOPIC_AWSS_SWITCHAP, topic, TOPIC_LEN_MAX);
    for (i = 0; i < 5; i ++) {
        if (0 != awss_cmp_coap_send_resp(msg, strlen(msg), remote, topic, request)) {
            awss_debug("sending failed.");
        } else {
            awss_debug("sending succeeded.");
        }
    }

    os_msleep(1000);

    if (!success)
        goto SWITCH_AP_END;

    awss_debug("connect '%s' '%s'", ssid, passwd);
    if (0 != os_awss_connect_ap(WLAN_CONNECTION_TIMEOUT,
                                ssid, passwd,
                                AWSS_AUTH_TYPE_INVALID,
                                AWSS_ENC_TYPE_INVALID,
                                NULL, 0)) {
        while (1) {
            if (0 == os_awss_connect_ap(WLAN_CONNECTION_TIMEOUT,
                                        (char *)DEFAULT_SSID, (char *)DEFAULT_PASSWD,
                                        AWSS_AUTH_TYPE_INVALID,
                                        AWSS_ENC_TYPE_INVALID,
                                        NULL, 0)) {
                break;
            }
            os_msleep(2000);
        }
    } else {
        switch_ap_done = 1;
        awss_cancel_aha_monitor();

        void zconfig_force_destroy(void);
        zconfig_force_destroy();

        produce_random(aes_random, sizeof(aes_random));
    }
    awss_debug("connect '%s' '%s' exit", ssid, passwd);

SWITCH_AP_END:
    switch_ap_parsed = 0;
    return SHUB_OK;
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
