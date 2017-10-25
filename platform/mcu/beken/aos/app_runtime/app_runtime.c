/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include "hal/soc/soc.h"

#ifdef AOS_BINS
const int *syscall_ktbl = NULL;
const int *syscall_ftbl = NULL;

extern unsigned int _app_data_ram_begin;
extern unsigned int _app_data_ram_end;
extern unsigned int _app_data_flash_begin;
extern unsigned int _app_bss_start;
extern unsigned int _app_bss_end;
extern unsigned int _app_heap_start;
extern unsigned int _app_heap_end;
extern int application_start(int argc, char **argv);

struct app_info_t {
     void (*app_entry)(void *ksyscall_tbl, void *fsyscall_tbl, int argc, char *argv[]);
     unsigned int data_ram_start;
     unsigned int data_ram_end;
     unsigned int data_flash_begin;
     unsigned int bss_start;
     unsigned int bss_end;
     unsigned int heap_start;
     unsigned int heap_end;
};

int aos_application_init(void)
{
    LOG("aos application init.");
    return 0;
}

static void app_entry(void *ksyscall_tbl, void *fsyscall_tbl, int argc, char *argv[])
{
    /* syscall_ktbl & syscall_ftbl assignment must be first */
    syscall_ktbl = (int *)ksyscall_tbl;
    syscall_ftbl = (int *)fsyscall_tbl;

    aos_application_init();
    application_start(argc, argv);
}

__attribute__ ((used, section(".app_info"))) struct app_info_t app_info = {
    app_entry,
    &_app_data_ram_begin,
    &_app_data_ram_end,
    &_app_data_flash_begin,
    &_app_bss_start,
    &_app_bss_end,
    &_app_heap_start,
    &_app_heap_end
};
#endif

