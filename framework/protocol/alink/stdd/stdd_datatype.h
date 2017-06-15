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

#ifndef STDD_H
#define STDD_H
#include <stdint.h>
#include <unistd.h>
#include "alink_export_zigbee.h"

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif


/*�������ͷ��ඨ��*/
#define ALINK_DATATYPE_CLASS_NODATA                 0x01
#define ALINK_DATATYPE_CLASS_DATA                   0x02
#define ALINK_DATATYPE_CLASS_LOGIC                  0x03
#define ALINK_DATATYPE_CLASS_BITMAP                 0x04
#define ALINK_DATATYPE_CLASS_UINT                   0x05
#define ALINK_DATATYPE_CLASS_INT                    0x06
#define ALINK_DATATYPE_CLASS_ENUM                   0x07
#define ALINK_DATATYPE_CLASS_FLOAT                  0x08
#define ALINK_DATATYPE_CLASS_STRING                 0x09
#define ALINK_DATATYPE_CLASS_STRUCTURE              0x0a
#define ALINK_DATATYPE_CLASS_COLLECTION             0x0b
#define ALINK_DATATYPE_CLASS_OTHER                  0xff


/*** Data Types ***/
#define ZCL_DATATYPE_NO_DATA                            0x00
#define ZCL_DATATYPE_DATA8                              0x08    //DATA�����ַ�����Сдhex string��ʾ
#define ZCL_DATATYPE_DATA16                             0x09
#define ZCL_DATATYPE_DATA24                             0x0a
#define ZCL_DATATYPE_DATA32                             0x0b
#define ZCL_DATATYPE_DATA40                             0x0c
#define ZCL_DATATYPE_DATA48                             0x0d
#define ZCL_DATATYPE_DATA56                             0x0e
#define ZCL_DATATYPE_DATA64                             0x0f
#define ZCL_DATATYPE_BOOLEAN                            0x10    //BOOLEAN�����ַ�����10�����ַ�����ʾ
#define ZCL_DATATYPE_BITMAP8                            0x18    //BITMAP�����ַ�����10�����ַ�����ʾ
#define ZCL_DATATYPE_BITMAP16                           0x19
#define ZCL_DATATYPE_BITMAP24                           0x1a
#define ZCL_DATATYPE_BITMAP32                           0x1b
#define ZCL_DATATYPE_BITMAP40                           0x1c
#define ZCL_DATATYPE_BITMAP48                           0x1d
#define ZCL_DATATYPE_BITMAP56                           0x1e
#define ZCL_DATATYPE_BITMAP64                           0x1f
#define ZCL_DATATYPE_UINT8                              0x20    //UINT�����ַ�����10������ֵ��ʾ
#define ZCL_DATATYPE_UINT16                             0x21
#define ZCL_DATATYPE_UINT24                             0x22
#define ZCL_DATATYPE_UINT32                             0x23
#define ZCL_DATATYPE_UINT40                             0x24
#define ZCL_DATATYPE_UINT48                             0x25
#define ZCL_DATATYPE_UINT56                             0x26
#define ZCL_DATATYPE_UINT64                             0x27
#define ZCL_DATATYPE_INT8                               0x28    //INT�����ַ�����10������ֵ��ʾ
#define ZCL_DATATYPE_INT16                              0x29
#define ZCL_DATATYPE_INT24                              0x2a
#define ZCL_DATATYPE_INT32                              0x2b
#define ZCL_DATATYPE_INT40                              0x2c
#define ZCL_DATATYPE_INT48                              0x2d
#define ZCL_DATATYPE_INT56                              0x2e
#define ZCL_DATATYPE_INT64                              0x2f
#define ZCL_DATATYPE_ENUM8                              0x30    //ENUM�����ַ�����10������ֵ��ʾ
#define ZCL_DATATYPE_ENUM16                             0x31    //PREC�����ַ�����10������ֵ��ʾ
#define ZCL_DATATYPE_SEMI_PREC                          0x38
#define ZCL_DATATYPE_SINGLE_PREC                        0x39
#define ZCL_DATATYPE_DOUBLE_PREC                        0x3a
#define ZCL_DATATYPE_OCTET_STR                          0x41
#define ZCL_DATATYPE_CHAR_STR                           0x42
#define ZCL_DATATYPE_LONG_OCTET_STR                     0x43
#define ZCL_DATATYPE_LONG_CHAR_STR                      0x44
#define ZCL_DATATYPE_ARRAY                              0x48
#define ZCL_DATATYPE_STRUCT                             0x4c
#define ZCL_DATATYPE_SET                                0x50
#define ZCL_DATATYPE_BAG                                0x51
#define ZCL_DATATYPE_TOD                                0xe0
#define ZCL_DATATYPE_DATE                               0xe1    //DATA�����ַ�����10������ֵ��ʾ
#define ZCL_DATATYPE_UTC                                0xe2    //UTC�����ַ�����10������ֵ��ʾ
#define ZCL_DATATYPE_CLUSTER_ID                         0xe8    //CLUSTER_ID�����ַ�����hex string��ʾ
#define ZCL_DATATYPE_ATTR_ID                            0xe9    //ATTR_ID�����ַ�����hex string��ʾ
#define ZCL_DATATYPE_BAC_OID                            0xea
#define ZCL_DATATYPE_IEEE_ADDR                          0xf0    //IEEE_ADDR�����ַ�����Сдhex string��ʾ
#define ZCL_DATATYPE_128_BIT_SEC_KEY                    0xf1    //IEEE_ADDR�����ַ�����Сдhex string��ʾ
#define ZCL_DATATYPE_UNKNOWN                            0xff


/***
 * @desc:    ��������ģ�������profile
 * @para:    profile: �豸����profile�ṹָ�����飬��NULL����
 *           ͬһcluster�´���ͬ������profile�򸲸�ԭ�е�
 * @retc:    ZBNET_SUCCESS/ZBNET_FAILURE
 */
typedef int32_t (*zigbee_update_attr_profile_cb_t)(attr_profile_t *profile[]);


/***
 * @desc:    ��������ģ�������profile
 * @para:    profile: �豸����profile�ṹָ�����飬��NULL����
 *           ͬһcluster�´���ͬ������profile�򸲸�ԭ�е�
 * @retc:    ZBNET_SUCCESS/ZBNET_FAILURE
 */
typedef int32_t (*zigbee_update_cmd_profile_cb_t)(cmd_profile_t *profile[]);



/***
 * @desc:    ��ȡ�豸���Խӿڣ��豸����ͨ���豸״̬
 *           �ϱ��ӿ��첽�ϱ�
 * @para:    ieee_addr: �豸8Byte ieee��ַ
 *           endpoint_id: 1Byte endpoint id
 *           attr_set: Ҫ��ȡ����������ָ�����飬�������Ԫ��ΪNULL
 * @retc:    ZBNET_SUCCESS/ZBNET_FAILURE
 */
typedef int32_t (*zigbee_get_attr_cb_t)(uint8_t ieee_addr[IEEE_ADDR_BYTES], \
                                    uint8_t endpoint_id, const char *attr_set);


/***
 * @desc:    �����豸�������Խӿ�
 * @para:    ieee_addr: �豸8Byte ieee��ַ
 *           endpoint_id: 1Byte endpoint id
 *           attr_name: ��������
 *           attr_value: �ַ�������ֵ
 * @retc:    ZBNET_SUCCESS/ZBNET_FAILURE
 */
typedef int32_t (*zigbee_set_attr_cb_t)(uint8_t ieee_addr[IEEE_ADDR_BYTES], uint8_t endpoint_id, \
                                    const char *attr_name, const char *attr_value);


/***
 * @desc:    ִ���豸����ӿ�
 * @para:    ieee_addr: �豸8Byte ieee��ַ
 *           endpoint_id: 1Byte endpoint id
 *           cmd_name: ��������
 *           cmd_args: json�ַ�����ʽ�������
 * @retc:    ZBNET_SUCCESS/ZBNET_FAILURE
 */
typedef int32_t (*zigbee_exec_cmd_cb_t)(uint8_t ieee_addr[IEEE_ADDR_BYTES], uint8_t endpoint_id, \
                                    const char *cmd_name, const char *cmd_args);


/***
 * @desc:    �Ƴ��豸�ӿ�
 * @para:    ieee_addr: �豸8Byte ieee��ַ
 * @retc:    ZBNET_SUCCESS/ZBNET_FAILURE
 */
typedef int32_t (*zigbee_remove_device_cb_t)(uint8_t ieee_addr[IEEE_ADDR_BYTES]);


/***
 * @desc:    ��ʼpermitjoin�ӿ�
 * @para:    duration: permitjoin����ʱ�䣬��λ:�룬ȡֵ��Χ:1-255,255��ʾ����
 * @retc:    ZBNET_SUCCESS/ZBNET_FAILURE
 */
typedef int32_t (*zigbee_permit_join_cb_t)(uint8_t duration);


/*
*�����������ͽṹ��
*/
typedef struct data_type_s{
    uint8_t type_class;
    int8_t length;//0xff��ʾΪ�䳤
    char *name;
    uint8_t id;
}data_type_t;


data_type_t *stdd_get_datatype_by_name(const char *name, int name_len);
data_type_t *stdd_get_datatype_by_id(uint8_t type_id);

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
}
#endif

#endif

