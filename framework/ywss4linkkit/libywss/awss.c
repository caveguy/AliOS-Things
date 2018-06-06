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
#include "awss.h"
#include "awss_main.h"
#include "zconfig_utils.h"
#include "enrollee.h"
#include "awss_cmp.h"
#include "awss_notify.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

extern int switch_ap_done;
static uint8_t adha_switch = 0;
static uint8_t awss_stopped = 0;
static void *g_awss_monitor_cb = NULL;

static void adha_monitor(void)
{
    adha_switch = 1;
}

#define ADHA_WORK_CYCLE      (5 * 1000)

static void aha_monitor(void);
#define AHA_MONITOR_TIMEOUT_MS  (1 * 60 * 1000)
static volatile char aha_timeout;

static void aha_monitor(void)
{
    aha_timeout = 1;
}

int aha_is_timeout()
{
    return aha_timeout > 0;
}

int awss_cancel_aha_monitor()
{
    HAL_Sys_Cancel_Task(aha_monitor, NULL);
    aha_timeout = 0;
    return 0;
}

static void awss_open_aha_monitor()
{
    char ssid[PLATFORM_MAX_SSID_LEN + 1] = {0};
    os_wifi_get_ap_info(ssid , NULL, NULL);
    awss_debug("aha monitor, ssid:%s, strlen:%d\n", ssid, strlen(ssid));
    if (strlen(ssid) > 0 && strcmp(ssid, DEFAULT_SSID)) { // not adha AP
        return;
    }
    aha_timeout = 0;
    HAL_Sys_Post_Task(AHA_MONITOR_TIMEOUT_MS, aha_monitor, NULL);
}

int awss_report_cloud()
{
    awss_cmp_online_init();
    awss_report_token();
    awss_cmp_local_init();
    awss_connectap_notify_stop();
    awss_connectap_notify();
#ifndef AWSS_DISABLE_REGISTRAR
    awss_registrar_init();
#endif
    awss_check_reset();
    return 0;
}

int awss_success_notify()
{
    awss_cmp_local_init();
    awss_suc_notify_stop();
    awss_suc_notify();
    return 0;
}

int awss_regist_event_monitor_cb(void (*monitor_cb)(int event))
{
    g_awss_monitor_cb = monitor_cb;
}

void *awss_get_event_monitor_cb()
{
    return g_awss_monitor_cb;
}

int awss_start()
{
    char ssid[PLATFORM_MAX_SSID_LEN + 1] = {0};
    produce_random(aes_random, sizeof(aes_random));

    do {
        awss_stopped = 0;
        __awss_start();

        do {
            while (1) {
                memset(ssid, 0, sizeof(ssid));
                os_wifi_get_ap_info(ssid , NULL, NULL);
                awss_debug("start, ssid:%s, strlen:%d\n", ssid, strlen(ssid));
                if (strlen(ssid) > 0 && strcmp(ssid, ADHA_SSID))  // not adha AP
                    break;

                if (os_sys_net_is_ready()) { // skip the adha failed
                    awss_cmp_local_init();

                    adha_switch = 0;
                    HAL_Sys_Post_Task(ADHA_WORK_CYCLE, adha_monitor, NULL);
                    while (!adha_switch)
                        os_msleep(200);
                    adha_switch = 0;

                    awss_cmp_local_deinit();
                }

                if (switch_ap_done || awss_stopped)
                    break;
                __awss_start();
            }
            if (switch_ap_done || awss_stopped)
                break;
            if (strlen(ssid) > 0 && strcmp(ssid, DEFAULT_SSID))  // not AHA
                break;

            awss_open_aha_monitor();

            awss_cmp_local_init();
            char dest_ap = 0;
            while (!aha_is_timeout()) {
                memset(ssid, 0, sizeof(ssid));
                os_wifi_get_ap_info(ssid , NULL, NULL);
                if (os_sys_net_is_ready() &&
                    strlen(ssid) > 0 && strcmp(ssid, DEFAULT_SSID)) {  // not AHA
                    dest_ap = 1;
                    break;
                }
                os_msleep(200);
            }

            awss_cmp_local_deinit();
            if (switch_ap_done || awss_stopped)
                break;

            if (dest_ap == 1)
                break;
            __awss_start();
        } while (1);

        if (os_sys_net_is_ready())
            break;
    } while (1);

    awss_success_notify();

    return 0;
}

int awss_stop()
{
    HAL_Sys_Cancel_Task(adha_monitor, NULL);
    HAL_Sys_Cancel_Task(aha_monitor, NULL);
    __awss_stop();
    awss_cmp_local_deinit();
    awss_stopped = 1;
    return 0;
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
