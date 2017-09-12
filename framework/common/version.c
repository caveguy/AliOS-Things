/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>
#include <aos/aos.h>

#ifndef AOS_EXPORT
#define AOS_EXPORT
#endif

#ifndef AOS_WEAK
#define AOS_WEAK __attribute__((weak))
#endif

#define TAG "AOS_VERSION"

AOS_EXPORT AOS_WEAK const char   *get_aos_product_model(void)
{
    return (const char *)SYSINFO_PRODUCT_MODEL;
}

AOS_EXPORT AOS_WEAK const char   *get_aos_device_name(void)
{
    return (const char *)SYSINFO_DEVICE_NAME;
}

#ifdef SYSINFO_OS_BINS
#define OS_MAX_VERSION_LEN 64
char  os_version[OS_MAX_VERSION_LEN];
#endif

AOS_EXPORT AOS_WEAK const char   *get_aos_kernel_version(void)
{
    return (const char *)aos_version_get();
}


AOS_EXPORT AOS_WEAK const char   *get_aos_app_version(void)
{
    return (const char *)SYSINFO_APP_VERSION;
}

AOS_EXPORT AOS_WEAK const char   *get_aos_os_version(void)
{
#ifdef SYSINFO_OS_BINS
    snprintf(os_version, OS_MAX_VERSION_LEN, "%s_%s", get_aos_kernel_version(), get_aos_app_version());
    return os_version;
#else
    return (const char *)SYSINFO_APP_VERSION;
#endif
}

AOS_EXPORT AOS_WEAK void dump_sys_info(void)
{
    LOGI(TAG, "app_version: %s", get_aos_app_version());
#ifdef SYSINFO_OS_BINS
    LOGI(TAG, "kernel_version: %s", get_aos_kernel_version());
#endif
    LOGI(TAG, "product_model: %s", get_aos_product_model());
    LOGI(TAG, "device_name: %s", get_aos_device_name());
}

