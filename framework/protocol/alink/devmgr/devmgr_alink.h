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

#ifndef __DEVMGR_ALINK__H__
#define __DEVMGR_ALINK__H__

#include <unistd.h>
#ifdef __cplusplus
extern "C" {
#endif

#define PROTOCL_ZIGBEE                  "zigbee"
#define PROTOCL_RF433                   "rf433"

#define METHOD_DEVICE_REGISTER_SUB      "device.registerSub"
#define METHOD_DEVICE_UNREGISTER_SUB    "device.unregisterSub"
#define METHOD_DEVICE_LOGIN_SUB         "device.loginSub"
#define METHOD_DEVICE_LOGOUT_SUB        "device.logoutSub"

#define DEVMGR_PERMITJOIN_ANY_MODEL     (0xffffffff)
/*���������������أ�Ĭ�ϳ�ʱʱ��*/
#define DEVMGR_DEFAULT_PERMITJOIN_DURATION  240


//"sign":"hmacmd5(secret, rand+sn)"
#define PARAMS_DEVICE_REGISTER_SUB_FMT  ("{\"shortModel\":\"%08x\",\"rand\":\"%s\",\"sign\":\"%s\","\
                    "\"sn\":\"%s\",\"model\":\"%s\",\"mac\":\"%s\",\"version\":\"%s\",\"name\":\"%s\","\
                    "\"barcode\":\"%s\",\"cid\":\"%s\",\"description\":\"%s\",\"features\":%s}")

#define PARAMS_DEVICE_UNREGISTER_SUB_FMT    ("{\"uuid\":\"%s\",\"rand\":\"%s\",\"sign\":\"%s\"}")
//"sign" : "hmacmd5(sub device's token, rand + sub device's uuid)"
#define PARAMS_DEVICE_LOGIN_SUB_FMT     ("{\"uuid\":\"%s\",\"rand\":\"%s\",\"sign\":\"%s\"}")
#define PARAMS_DEVICE_LOGOUT_SUB_FMT    ("{\"uuid\":\"%s\"}")
#define FEATURE_DEVICE_LOGIN_SUB_FMT    ("{\"linkCapability\":[{\"protocol\":\"%s\",\"profile\":\"%s\"}]}")
#define RESULT_DEVICE_REGISTER_SUB_FMT  ("{\"uuid\":\"%s\",\"token\":\"%s\"}")


int devmgr_network_up_event_handler(dev_info_t *devinfo);
uint32_t devmgr_get_permitjoin_short_model();

/*ͨ����������豸״̬���*/
/*
--> {"uuid":"42E6E69DAEF6483FBBA412A28AF7CD76","attrSet":["OnOff"],"OnOff":{"value":"1","when":"1404443369"}}
*/
const char *devmgr_get_device_signature(uint32_t short_model, const char rand[SUBDEV_RAND_BYTES], char *sign_buff, uint32_t buff_size);
int devmgr_login_device(dev_info_t *devinfo);
int devmgr_logout_device(dev_info_t *devinfo);
int devmgr_register_device(dev_info_t *devinfo);
int devmgr_unregister_device(dev_info_t *devinfo);
int devmgr_annunciate_device_status(const char *devid, char *params);
int devmgr_link_state_event_handler(dev_info_t *devinfo, link_state_t state);
int devmgr_network_leave_event_handler(dev_info_t *devinfo);
int devmgr_network_event_cb(network_event_t event);
void devmgr_delay_disable_join(int duration);
int devmgr_alink_init();
void devmgr_alink_exit();

#ifdef __cplusplus
}
#endif
#endif

