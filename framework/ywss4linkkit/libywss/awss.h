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
#ifndef _AWSS_H_
#define _AWSS_H_

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

/**
 * @brief   start wifi setup service
 *
 * @retval  -1 : wifi setup fail
 * @retval  0 : sucess
 * @note: awss_report_cloud must been called to enable wifi setup service
 */
int awss_start();

/**
 * @brief   stop wifi setup service
 *
 * @retval  -1 : failure
 * @retval  0 : sucess
 * @note
 *      if awss_stop is called before exit of awss_start, awss and notify will stop.
 */
int awss_stop();

/**
 * @brief   make sure user touches devic belong to themselves
 *
 * @retval  -1 : failure
 * @retval  0 : sucess
 */
int awss_config_press();

/**
 * @brief   report token to cloud after wifi setup success
 *
 * @retval  -1 : failure
 * @retval  0 : sucess
 */
int awss_report_cloud();

/**
 * @brief   report reset to cloud.
 *
 * @retval  -1 : failure
 * @retval  0 : sucess
 * @note
 *      device will save reset flag if device dosen't connect cloud, device will fails to send reset to cloud.
 *      when connection between device and cloud is ready, device will retry to report reset to cloud.
 */
int awss_report_reset();

enum awss_event_t {
    AWSS_START = 0x10000,      // AWSS start without enbale, just supports device discover
    AWSS_ENABLE,               // AWSS enable
    AWSS_LOCK_CHAN,            // AWSS lock channel(Got AWSS sync packet)
    AWSS_PASSWD_ERR,           // AWSS decrypt passwd error
    AWSS_CONNECT_ADHA,         // AWSS try to connnect adha (device discover, router solution)
    AWSS_CONNECT_ADHA_FAIL,    // AWSS fails to connect adha
    AWSS_CONNECT_AHA,          // AWSS try to connect aha (AP solution)
    AWSS_CONNECT_AHA_FAIL,     // AWSS fails to connect aha
    AWSS_SETUP_NOTIFY,         // AWSS sends out device setup information (AP and router solution)
    AWSS_CONNECT_ROUTER,       // AWSS try to connect destination router
    AWSS_CONNECT_ROUTER_FAIL,  // AWSS fails to connect destination router.
    AWSS_GOT_IP,               // AWSS connects destination successfully and got ip address
    AWSS_SUC_NOTIFY,           // AWSS sends out success notify (AWSS sucess)
    AWSS_BIND_NOTIFY,          // AWSS sends out bind notify information to support bind between user and device
    AWSS_RESET = 0x3000,            // Linkkit reset success (just got reset response from cloud without any other operation)
};

/**
 * @brief   report reset to cloud when device connects cloud.
 *
 * @retval  -1 : failure
 * @retval  0 : sucess
 * @note
 *      if device dosen't connect cloud, device will fails to send reset to cloud.
 *      when connection between devicea and cloud is ready, device will retry to report reset to cloud.
 */
int awss_regist_event_monitor_cb(void (*monitor_cb)(int event));

/**
 * @brief get awss event monitor callback registered.
 *
 * @param none
 *
 * @return callback function pointer.
 */
void *awss_get_event_monitor_cb();

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
