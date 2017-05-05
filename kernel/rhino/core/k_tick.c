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

#include <k_api.h>

void tick_list_init(void)
{
    uint8_t i;

    for (i = 0; i < YUNOS_CONFIG_TICK_HEAD_ARRAY; i++) {
        klist_init(&g_tick_head[i]);
    }

#if (YUNOS_CONFIG_DYNTICKLESS > 0)
    g_next_intrpt_ticks = (tick_t)-1;
#endif
}

YUNOS_INLINE void tick_list_pri_insert(klist_t *head, ktask_t *task)
{
    tick_t   val;
    klist_t *q;
    klist_t *list_start;
    klist_t *list_end;
    ktask_t  *task_iter_temp;

    list_start = list_end = head;
    val = task->tick_remain;

    for (q = list_start->next; q != list_end; q = q->next) {
        task_iter_temp = yunos_list_entry(q, ktask_t, tick_list);
        if ((task_iter_temp->tick_match - g_tick_count) > val) {
            break;
        }
    }

    klist_insert(q, &task->tick_list);

#if (YUNOS_CONFIG_DYNTICKLESS > 0)
    task_iter_temp = yunos_list_entry(head->next, ktask_t, tick_list);

    if (g_next_intrpt_ticks > task_iter_temp->tick_match - g_tick_count) {
        g_next_intrpt_ticks = task_iter_temp->tick_match - g_tick_count;
        soc_tick_interrupt_set(g_next_intrpt_ticks, g_elapsed_ticks);
    }
#endif
}

void tick_list_insert(ktask_t *task, tick_t time)
{
    klist_t *tick_head_ptr;
    uint16_t spoke;

    if (time > 0u) {
        task->tick_match  = g_tick_count + time;
        task->tick_remain = time;

        spoke = (uint16_t)(task->tick_match & (YUNOS_CONFIG_TICK_HEAD_ARRAY - 1u));
        tick_head_ptr = &g_tick_head[spoke];

        tick_list_pri_insert(tick_head_ptr, task);
        task->tick_head = tick_head_ptr;
    }
}

void tick_list_rm(ktask_t *task)
{
    klist_t *tick_head_ptr = task->tick_head;

    if (tick_head_ptr != NULL) {
        klist_rm(&task->tick_list);
        task->tick_head = NULL;
    }
}

void tick_list_update(void)
{
    CPSR_ALLOC();

    uint16_t spoke;
    klist_t *tick_head_ptr;
    ktask_t  *p_tcb;
    klist_t *iter;
    klist_t *iter_temp;

    YUNOS_CRITICAL_ENTER();

#if (YUNOS_CONFIG_DYNTICKLESS > 0)
    g_tick_count       += g_pend_intrpt_ticks;
    g_sys_time_tick    += g_pend_intrpt_ticks;
    g_pend_intrpt_ticks = 0u;
#else
    g_tick_count++;
    g_sys_time_tick++;
#endif

    spoke         = (uint16_t)(g_tick_count & (YUNOS_CONFIG_TICK_HEAD_ARRAY - 1u));
    tick_head_ptr = &g_tick_head[spoke];
    iter          = tick_head_ptr->next;

    while (YUNOS_TRUE) {
        /* search all the time list if possible */
        if (iter != tick_head_ptr) {
            iter_temp = iter->next;
            p_tcb     = yunos_list_entry(iter, ktask_t, tick_list);

            /* since time list is sorted by remain time, so just campare  the absolute time */
            if (g_tick_count == p_tcb->tick_match) {
                switch (p_tcb->task_state) {
                    case K_SLEEP:
                        p_tcb->blk_state  = BLK_FINISH;
                        p_tcb->task_state = K_RDY;
                        tick_list_rm(p_tcb);
                        ready_list_add(&g_ready_queue, p_tcb);
                        break;
                    case K_PEND:
                        tick_list_rm(p_tcb);
                        /* remove task on the block list because task is timeout */
                        klist_rm(&p_tcb->task_list);
                        ready_list_add(&g_ready_queue, p_tcb);
                        p_tcb->blk_state  = BLK_TIMEOUT;
                        p_tcb->task_state = K_RDY;
                        mutex_task_pri_reset(p_tcb);
                        p_tcb->blk_obj    = NULL;
                        break;
                    case K_PEND_SUSPENDED:
                        tick_list_rm(p_tcb);
                        /* remove task on the block list because task is timeout */
                        klist_rm(&p_tcb->task_list);
                        p_tcb->blk_state  = BLK_TIMEOUT;
                        p_tcb->task_state = K_SUSPENDED;
                        mutex_task_pri_reset(p_tcb);
                        p_tcb->blk_obj    = NULL;
                        break;
                    case K_SLEEP_SUSPENDED:
                        p_tcb->task_state = K_SUSPENDED;
                        p_tcb->blk_state  = BLK_FINISH;
                        tick_list_rm(p_tcb);
                        break;
                    default:
                        k_err_proc(YUNOS_SYS_FATAL_ERR);
                        break;
                }

                iter = iter_temp;
            } else {
                break;
            }
        } else {
            break;
        }
    }

#if (YUNOS_CONFIG_DYNTICKLESS > 0)

    if (tick_head_ptr->next != tick_head_ptr) {
        p_tcb = yunos_list_entry(tick_head_ptr->next, ktask_t, tick_list);
        g_next_intrpt_ticks = p_tcb->tick_match - g_tick_count;
    } else {
        g_next_intrpt_ticks = (tick_t)-1;
    }

    soc_tick_interrupt_set(g_next_intrpt_ticks, 0);
#endif

    YUNOS_CRITICAL_EXIT();
}

#if (YUNOS_CONFIG_TICK_TASK > 0)
static void tick_task_proc(void *para)
{
    kstat_t ret;

    (void)para;

    while (YUNOS_TRUE) {
        ret = yunos_task_sem_take(YUNOS_WAIT_FOREVER);
        if (ret == YUNOS_SUCCESS) {
            if (g_sys_stat == YUNOS_RUNNING) {
                tick_list_update();
            }
        }
    }
}

void tick_task_start(void)
{
    /* create tick task to caculate task sleep and timeout */
    yunos_task_create(&g_tick_task, "tick_task", NULL, YUNOS_CONFIG_TICK_TASK_PRI,
                      0, g_tick_task_stack, YUNOS_CONFIG_TICK_TASK_STACK_SIZE, tick_task_proc, 1);

    yunos_task_sem_create(&g_tick_task, &g_tick_sem, "tick_task_sem", 0);

}
#endif

