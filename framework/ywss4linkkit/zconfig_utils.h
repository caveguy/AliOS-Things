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

#ifndef _SMART_UTILS_H_
#define _SMART_UTILS_H_

#include "os.h"

#include "zconfig_config.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

#ifndef DEBUG
    #define __zc_loglevel_printf(log_level, format, ...)	do { } while (0)
#else

#ifdef _PLATFORM_QCOM_
    extern int cmnos_printf(const char * format, ...);
    #define __zc_loglevel_printf(log_level, format, ...)	\
        do {													\
            cmnos_printf(format, ##__VA_ARGS__);					\
        } while (0)
#elif defined (_PLATFORM_ESPRESSIF_)
    extern int printf(const char *format, ...);
    #define __zc_loglevel_printf(log_level, format, ...)	\
        do {													\
            printf(format, ##__VA_ARGS__);					\
        } while (0)
#elif defined (_PLATFORM_REALTEK_) //realtek 8711
    extern int printf(const char *format, ...);
    #define __zc_loglevel_printf(log_level, format, ...)	\
        do {													\
            printf(format, ##__VA_ARGS__);					\
        } while (0)
#else
    #define __zc_loglevel_printf(log_level, format, ...)	\
    do {													\
            os_printf(format, ##__VA_ARGS__);		\
    } while (0)
#endif //end of _PLATFORM_QCOM_

#endif //end of ifndef DEBUG

//for library safety, close the log output 
#define log(format, ...)						__zc_loglevel_printf(LOGLEVEL_DEBUG, format, ##__VA_ARGS__)
#define info(format, ...)						__zc_loglevel_printf(LOGLEVEL_INFO, format, ##__VA_ARGS__)
#define warn(format, ...)						__zc_loglevel_printf(LOGLEVEL_WARN, format, ##__VA_ARGS__)
#define error(format, ...)						__zc_loglevel_printf(LOGLEVEL_ERROR, format, ##__VA_ARGS__)

#define bug_on(condition, format, ...)							\
do {											\
		if (condition) {							\
			error("########BUG ON %d!!!\r\n", __LINE__);			\
			error(format, ##__VA_ARGS__);					\
			while (1);							\
		}										\
} while (0)

#define warn_on(condition, format, ...)							\
do {											\
		if (condition) {							\
			warn("########WARNING ON %d!!!\r\n", __LINE__);			\
			warn(format, ##__VA_ARGS__);					\
		}									\
} while (0)

void dump_hex(unsigned char *data, int len, int tab_num);
void dump_ascii(unsigned char *data, int len, int tab_num);
void dump_mac(u8 *src, u8 *dst);

u16 zconfig_checksum_v3(u8 *data, u8 len);

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif	// _UTILS_H_
