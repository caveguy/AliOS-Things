/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
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
#include <string.h>
#include <hal/base.h>
#include <hal/wifi.h>
#include "common.h"
#include "stm32_wifi.h"

#define  WIFI_CONNECT_MAX_ATTEMPT_COUNT  3

hal_wifi_module_t sim_yos_wifi_stm23l475;


static int wifi_init(hal_wifi_module_t *m)
{
    printf("wifi init success!!\n");
    return 0;
};

static void wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
{
    WIFI_Status_t wifi_res = WIFI_GetMAC_Address(mac);
    if ( WIFI_STATUS_OK != wifi_res)
    {
        printf("Failed to get MAC address\n");
    }
};

extern void ParseIP(char* ptr, uint8_t* arr);

static void connet_wifi_ap(void *arg)
{
    WIFI_Status_t wifi_res;
    int connect_counter = 0;
    uint8_t ip_address[4];
    hal_wifi_ip_stat_t wifi_pnet;
    hal_wifi_init_type_t *init_para = (hal_wifi_init_type_t *)arg;

    do 
    {
        printf("\rConnecting to AP: %s  Attempt %d/%d ...", init_para->wifi_ssid, ++connect_counter, WIFI_CONNECT_MAX_ATTEMPT_COUNT);
        wifi_res = WIFI_Connect(init_para->wifi_ssid, init_para->wifi_key, init_para->access_sec);
        if (wifi_res == WIFI_STATUS_OK) break;
    } while (connect_counter < WIFI_CONNECT_MAX_ATTEMPT_COUNT);

    /* Slight delay since the module seems to take some time prior to being able
        to retrieve its IP address after a connection. */
    yos_msleep(500);
    if (wifi_res == WIFI_STATUS_OK)
    {
        WifiStatusHandler(1);
        printf("\nConnected to AP %s\n", init_para->wifi_ssid);
    }
    else
    {
        WifiStatusHandler(2);
        printf("\nFailed to connect to AP %s\n", init_para->wifi_ssid);
    }
   
    if ( WIFI_STATUS_OK != WIFI_GetIP_Address(ip_address))
    {
        printf("Fail to get IP address\n");
    }
    else
    {
        snprintf(wifi_pnet.ip, sizeof(wifi_pnet.ip), "%d.%d.%d.%d", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
        printf("Get IP Address: %s\n", wifi_pnet.ip);
        NetCallback(&wifi_pnet);
    }
}

static int wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    if (NULL == m || NULL == init_para)
    {
        printf("wifi_start: invalid parameter\n");
        return -1;
    }

    yos_loop_schedule_work(0, connet_wifi_ap, (void*)init_para, NULL, NULL);

    return 0;
}

static int wifi_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv)
{
    return 0;
}

static int get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type)
{
    return 0;
}

static int get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat)
{
    return 0;
}

static void start_scan(hal_wifi_module_t *m)
{
    return;
}

static void start_scan_adv(hal_wifi_module_t *m)
{
    return;
}

static int power_off(hal_wifi_module_t *m)
{
    return 0;
}

static int power_on(hal_wifi_module_t *m)
{
    return 0;
}

static int suspend(hal_wifi_module_t *m)
{
    return 0;
}

static int suspend_station(hal_wifi_module_t *m)
{
    return 0;
}

static int suspend_soft_ap(hal_wifi_module_t *m)
{

    return 0;
}

static int set_channel(hal_wifi_module_t *m, int ch)
{
    return 0;
}

static void start_monitor(hal_wifi_module_t *m)
{
    return;
}

static void stop_monitor(hal_wifi_module_t *m)
{
    return;
}

static void register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    return;
}

static void register_wlan_mgnt_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    return;
}

static int wlan_send_80211_raw_frame(hal_wifi_module_t *m, uint8_t *buf, int len)
{
    return 0;
}

void NetCallback(hal_wifi_ip_stat_t *pnet)
{
    if (sim_yos_wifi_stm23l475.ev_cb == NULL)
        return;
    if (sim_yos_wifi_stm23l475.ev_cb->ip_got == NULL)
        return;

    sim_yos_wifi_stm23l475.ev_cb->ip_got(&sim_yos_wifi_stm23l475, pnet, NULL);
}

void connected_ap_info(hal_wifi_ap_info_adv_t *ap_info, char *key, int key_len)
{
    if (sim_yos_wifi_stm23l475.ev_cb == NULL)
        return;
    if (sim_yos_wifi_stm23l475.ev_cb->para_chg == NULL)
        return;

    sim_yos_wifi_stm23l475.ev_cb->para_chg(&sim_yos_wifi_stm23l475, ap_info, key, key_len, NULL);
}

void WifiStatusHandler(int status)
{
    if (sim_yos_wifi_stm23l475.ev_cb == NULL)
        return;
    if (sim_yos_wifi_stm23l475.ev_cb->stat_chg == NULL)
        return;

    sim_yos_wifi_stm23l475.ev_cb->stat_chg(&sim_yos_wifi_stm23l475, status, NULL);
}

void ApListCallback(hal_wifi_scan_result_t *pApList)
{
    int i;
    
    printf("AP %d: \r\n", pApList->ap_num);
    for(i=0; i<pApList->ap_num; i++) {
        printf("\t %s rssi %d\r\n", pApList->ap_list[i].ssid, pApList->ap_list[i].ap_power);
    }
    if (sim_yos_wifi_stm23l475.ev_cb == NULL)
        return;
    if (sim_yos_wifi_stm23l475.ev_cb->scan_compeleted == NULL)
        return;

    sim_yos_wifi_stm23l475.ev_cb->scan_compeleted(&sim_yos_wifi_stm23l475, 
        (hal_wifi_scan_result_t*)pApList, NULL);
}

void ApListAdvCallback(hal_wifi_scan_result_adv_t *pApAdvList)
{
    if (sim_yos_wifi_stm23l475.ev_cb == NULL)
        return;
    if (sim_yos_wifi_stm23l475.ev_cb->scan_adv_compeleted == NULL)
        return;

    sim_yos_wifi_stm23l475.ev_cb->scan_adv_compeleted(&sim_yos_wifi_stm23l475, 
        pApAdvList, NULL);
}

hal_wifi_module_t sim_yos_wifi_stm23l475 = {
    .base.name           = "sim_yos_wifi_stm23l475",
    .init                =  wifi_init,
    .get_mac_addr        =  wifi_get_mac_addr,
    .start               =  wifi_start,
    .start_adv           =  wifi_start_adv,
    .get_ip_stat         =  get_ip_stat,
    .get_link_stat       =  get_link_stat,
    .start_scan          =  start_scan,
    .start_scan_adv      =  start_scan_adv,
    .power_off           =  power_off,
    .power_on            =  power_on,
    .suspend             =  suspend,
    .suspend_station     =  suspend_station,
    .suspend_soft_ap     =  suspend_soft_ap,
    .set_channel         =  set_channel,
    .start_monitor       =  start_monitor,
    .stop_monitor        =  stop_monitor,
    .register_monitor_cb =  register_monitor_cb,
    .register_wlan_mgnt_monitor_cb = register_wlan_mgnt_monitor_cb,
    .wlan_send_80211_raw_frame = wlan_send_80211_raw_frame
};

