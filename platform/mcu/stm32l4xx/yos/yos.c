/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <yos.h>
#include <k_api.h>
#include <aos/kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32_wifi.h"
#define AOS_START_STACK 2048

#define WIFI_PRODUCT_INFO_SIZE                      ES_WIFI_MAX_SSID_NAME_SIZE

ktask_t *g_aos_init;

extern int application_start(int argc, char **argv);
void trace_start(void);
extern int aos_framework_init(void);

static int init_wifi()
{
    /* init wifi*/
    char moduleinfo[WIFI_PRODUCT_INFO_SIZE];
    uint8_t mac[6];
    WIFI_Status_t wifi_res = WIFI_Init();
    if (WIFI_STATUS_OK != wifi_res )
    {
        printf("Failed to initialize WIFI module\n");
        return -1;
    }
    /* Retrieve the WiFi module mac address to confirm that it is detected and communicating. */
    WIFI_GetModuleName(moduleinfo);
    printf("Module initialized successfully: %s",moduleinfo);

    WIFI_GetModuleID(moduleinfo);
    printf(" %s",moduleinfo);

    WIFI_GetModuleFwRevision(moduleinfo);
    printf(" %s\n",moduleinfo);

    printf("Retrieving the WiFi module MAC address:");
    wifi_res = WIFI_GetMAC_Address( (uint8_t*)mac);
    if ( WIFI_STATUS_OK == wifi_res)
    {
        printf(" %02x:%02x:%02x:%02x:%02x:%02x\n",
                mac[0], mac[1], mac[2],
                mac[3], mac[4], mac[5]);
    }
    else
    {
        printf("Failed to get MAC address\n");
    }
    return 0;
}

static void hal_init()
{
    init_wifi();
}

extern void hw_start_hal(void);

static void aos_init(void)
{
    int i = 0;

    stm32_soc_init();

#ifdef BOOTLOADER
    main();
#else
    hal_init();
    hw_start_hal();

    vfs_init();
    vfs_device_init();

    for (i = 0; i < 10; i++) {
        vflash_register_partition(i);
    }

    aos_cli_init();
    aos_kv_init();
    aos_loop_init();

    trace_start();

    aos_framework_init();
    application_start(0, NULL);
#endif
}


void aos_start(void)
{
    krhino_init();
    krhino_task_dyn_create(&g_aos_init, "aos-init", 0, AOS_DEFAULT_APP_PRI, 0, AOS_START_STACK, (task_entry_t)aos_init, 1);
    krhino_start();
}

void entry_main(void)
{
    aos_start();
}


