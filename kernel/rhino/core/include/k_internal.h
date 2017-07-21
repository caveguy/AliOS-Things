/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef K_INTERNAL_H
#define K_INTERNAL_H

extern kstat_t g_sys_stat;
extern uint8_t g_idle_task_spawned;

extern runqueue_t g_ready_queue;

/* System lock */
extern uint8_t g_sched_lock;
extern uint8_t g_intrpt_nested_level;

/* highest pri ready task object */
extern ktask_t *g_preferred_ready_task;

/* current active task */
extern ktask_t *g_active_task;

/* idle attribute */
extern ktask_t      g_idle_task;
extern idle_count_t g_idle_count;
extern cpu_stack_t  g_idle_task_stack[YUNOS_CONFIG_IDLE_TASK_STACK_SIZE];

/* tick attribute */
extern tick_t     g_tick_count;
extern klist_t    g_tick_head[YUNOS_CONFIG_TICK_HEAD_ARRAY];
extern sys_time_t g_sys_time_tick;

#if (YUNOS_CONFIG_SYSTEM_STATS > 0)
extern kobj_list_t g_kobj_list;
#endif

#if (YUNOS_CONFIG_TIMER > 0)
extern klist_t     g_timer_head;
extern tick_t      g_timer_count;
extern uint32_t    g_timer_ctrl;
extern ktask_t     g_timer_task;
extern cpu_stack_t g_timer_task_stack[YUNOS_CONFIG_TIMER_TASK_STACK_SIZE];
extern ksem_t      g_timer_sem;
extern kmutex_t    g_timer_mutex;
#endif

#if (YUNOS_CONFIG_DYNTICKLESS > 0)
extern tick_t g_next_intrpt_ticks;
extern tick_t g_pend_intrpt_ticks;
extern tick_t g_elapsed_ticks;
#endif

#if (YUNOS_CONFIG_TICK_TASK > 0)
extern ktask_t     g_tick_task;
extern cpu_stack_t g_tick_task_stack[YUNOS_CONFIG_TICK_TASK_STACK_SIZE];
extern ksem_t      g_tick_sem;
#endif

#if (YUNOS_CONFIG_DISABLE_SCHED_STATS > 0)
extern hr_timer_t g_sched_disable_time_start;
extern hr_timer_t g_sched_disable_max_time;
extern hr_timer_t g_cur_sched_disable_max_time;
#endif

#if (YUNOS_CONFIG_DISABLE_INTRPT_STATS > 0)
extern uint16_t   g_intrpt_disable_times;
extern hr_timer_t g_intrpt_disable_time_start;
extern hr_timer_t g_intrpt_disable_max_time;
extern hr_timer_t g_cur_intrpt_disable_max_time;
#endif

#if (YUNOS_CONFIG_HW_COUNT > 0)
extern hr_timer_t g_sys_measure_waste;
#endif

#if (YUNOS_CONFIG_CPU_USAGE_STATS > 0)
extern ktask_t      g_cpu_usage_task;
extern cpu_stack_t  g_cpu_task_stack[YUNOS_CONFIG_CPU_USAGE_TASK_STACK];
extern idle_count_t g_idle_count_max;
extern uint32_t     g_cpu_usage;
extern uint32_t     g_cpu_usage_max;
#endif

#if (YUNOS_CONFIG_TASK_SCHED_STATS > 0)
extern ctx_switch_t g_sys_ctx_switch_times;
#endif

#if (YUNOS_CONFIG_KOBJ_DYN_ALLOC > 0)
extern kqueue_t  g_dyn_queue;
extern void     *g_dyn_queue_msg[YUNOS_CONFIG_K_DYN_QUEUE_MSG];
extern ktask_t   g_dyn_mem_proc_task;
#endif

#if (YUNOS_CONFIG_WORKQUEUE > 0)
extern klist_t       g_workqueue_list_head;
extern kmutex_t      g_workqueue_mutex;
#endif

#if (YUNOS_CONFIG_MM_BESTFIT > 0)
extern klist_t            g_mm_region_list_head;
extern k_mm_region_head_t g_kmm_region_head;
#if (YUNOS_CONFIG_MM_REGION_MUTEX == 1)
extern kmutex_t           g_mm_region_mutex;
#endif
#endif

#if (YUNOS_CONFIG_MM_TLF > 0)
extern k_mm_head         *g_kmm_head;
#endif

#define K_OBJ_STATIC_ALLOC 1u
#define K_OBJ_DYN_ALLOC    2u

#define NULL_PARA_CHK(para)            \
        do {                           \
            if (para == NULL) {        \
                return YUNOS_NULL_PTR; \
            }                          \
        } while (0)

#define INTRPT_NESTED_LEVEL_CHK()\
        do {                                       \
            if (g_intrpt_nested_level > 0u) {      \
                YUNOS_CRITICAL_EXIT();             \
                return YUNOS_NOT_CALLED_BY_INTRPT; \
            }                                      \
        } while (0)

void preferred_ready_task_get(runqueue_t *rq);

void core_sched(void);
void runqueue_init(runqueue_t *rq);

void ready_list_add(runqueue_t *rq, ktask_t *task);
void ready_list_add_head(runqueue_t *rq, ktask_t *task);
void ready_list_add_tail(runqueue_t *rq, ktask_t *task);
void ready_list_rm(runqueue_t *rq, ktask_t *task);
void ready_list_head_to_tail(runqueue_t *rq, ktask_t *task);

void time_slice_update(uint8_t task_pri);
void timer_task_sched(void);

void pend_list_reorder(ktask_t *task);
void pend_task_wakeup(ktask_t *task);
void pend_to_blk_obj(blk_obj_t *blk_obj, ktask_t *task, tick_t timeout);
void pend_task_rm(ktask_t *task);

#ifndef YUNOS_CONFIG_PERF_NO_PENDEND_PROC
kstat_t pend_state_end_proc(ktask_t *task);
#endif

void         idle_task(void *p_arg);
void         idle_count_set(idle_count_t value);
idle_count_t idle_count_get(void);

void tick_list_init(void);
void tick_task_start(void);
void tick_list_rm(ktask_t *task);
void tick_list_insert(ktask_t *task, tick_t time);
void tick_list_update(void);

uint8_t mutex_pri_limit(ktask_t *tcb, uint8_t pri);
void    mutex_task_pri_reset(ktask_t *tcb);
uint8_t mutex_pri_look(ktask_t *tcb, kmutex_t *mutex_rel);

kstat_t task_pri_change(ktask_t *task, uint8_t new_pri);

void k_err_proc(kstat_t err);

void timer_init(void);

void intrpt_disable_measure_start(void);
void intrpt_disable_measure_stop(void);
void dyn_mem_proc_task_start(void);
void cpu_usage_stats_start(void);

kstat_t ringbuf_init(k_ringbuf_t *p_ringbuf, void *buf, size_t len, size_t type,
                     size_t block_size);
kstat_t ringbuf_reset(k_ringbuf_t *p_ringbuf);
kstat_t ringbuf_push(k_ringbuf_t *p_ringbuf, void *data, size_t len);
kstat_t ringbuf_head_push(k_ringbuf_t *p_ringbuf, void *data, size_t len);
kstat_t ringbuf_pop(k_ringbuf_t *p_ringbuf, void *pdata, size_t *plen);
uint8_t ringbuf_is_full(k_ringbuf_t *p_ringbuf);
uint8_t ringbuf_is_empty(k_ringbuf_t *p_ringbuf);

#endif /* K_INTERNAL_H */

