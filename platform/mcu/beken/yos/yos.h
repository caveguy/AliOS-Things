/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef YOS_H
#define YOS_H

void soc_driver_init(void);

void soc_system_init(void);

void yos_start(void);

struct app_info_t {
     int (*app_entry)(int argc, char *argv[]);
     unsigned int data_ram_start;
     unsigned int data_ram_end;
     unsigned int data_flash_begin;
     unsigned int bss_start;
     unsigned int bss_end;
     unsigned int heap_start;
     unsigned int heap_end;
};

#endif /* YOS_H */

