/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UR_TIMER_H
#define UR_TIMER_H

#include <stdint.h>

typedef void (* timer_handler_t)(void *args);
typedef void *ur_timer_t;

ur_timer_t ur_start_timer(uint32_t dt, timer_handler_t handler, void *args);
void ur_stop_timer(ur_timer_t *timer, void *args);
uint32_t umesh_now_ms(void);

#endif  /* UR_TIMER_H */
