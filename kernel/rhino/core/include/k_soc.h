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

#ifndef K_SOC_H
#define K_SOC_H

#if (YUNOS_CONFIG_HW_COUNT > 0)
void       soc_hw_timer_init(void);
hr_timer_t soc_hr_hw_cnt_get(void);
lr_timer_t soc_lr_hw_cnt_get(void);
#define HR_COUNT_GET() soc_hr_hw_cnt_get()
#else  /* YUNOS_CONFIG_HW_COUNT */
#define HR_COUNT_GET() 0u
#endif /* YUNOS_CONFIG_HW_COUNT */

#if (YUNOS_CONFIG_TASK_SCHED_STATS > 0)
#define LR_COUNT_GET() soc_lr_hw_cnt_get()
#else  /* YUNOS_CONFIG_TASK_SCHED_STATS */
#define LR_COUNT_GET() 0u
#endif /* YUNOS_CONFIG_TASK_SCHED_STATS */

#if (YUNOS_CONFIG_INTRPT_GUARD > 0)
void soc_intrpt_guard(void);
#endif

#if (YUNOS_CONFIG_INTRPT_STACK_REMAIN_GET > 0)
size_t soc_intrpt_stack_remain_get(void);
#endif

#if (YUNOS_CONFIG_INTRPT_STACK_OVF_CHECK > 0)
void soc_intrpt_stack_ovf_check(void);
#endif

#if (YUNOS_CONFIG_DYNTICKLESS > 0)
void   soc_tick_interrupt_set(tick_t next_ticks,tick_t elapsed_ticks);
tick_t soc_elapsed_ticks_get(void);
#endif

#if (YUNOS_CONFIG_KOBJ_DYN_ALLOC > 0)
void  soc_sys_mem_init(void);
void *soc_mm_alloc(size_t size);
void  soc_mm_free(void *mem);
#endif

void soc_err_proc(kstat_t err);

void soc_apps_entry(void);

YUNOS_INLINE void soc_systick_handle(void)
{
    yunos_intrpt_enter();

    yunos_tick_proc();

    yunos_intrpt_exit();
}

size_t __attribute__ ((weak)) soc_get_cur_sp(void);
size_t __attribute__ ((weak)) soc_get_cur_pc(void);
void   __attribute__ ((weak)) soc_get_first_frame_info(size_t c_frame, size_t *n_frame,size_t *pc);
void   __attribute__ ((weak)) soc_get_subs_frame_info(size_t c_frame, size_t *n_frame,size_t *pc);

#endif /* K_SOC_H */

