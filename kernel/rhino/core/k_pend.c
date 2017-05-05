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

YUNOS_INLINE void pend_list_add(klist_t *head, ktask_t *task)
{
    klist_t *tmp;
    klist_t *list_start = head;
    klist_t *list_end   = head;

    for (tmp = list_start->next; tmp != list_end; tmp = tmp->next) {
        if (yunos_list_entry(tmp, ktask_t, task_list)->prio > task->prio) {
            break;
        }
    }

    klist_insert(tmp, &task->task_list);
}

void pend_task_wakeup(ktask_t *task)
{
    /* wake up task depend on the different state of task */
    switch (task->task_state) {
        case K_PEND:
            /* remove task on the block list because task is waken up */
            klist_rm(&task->task_list);
            /* add to the ready list again */
            ready_list_add(&g_ready_queue, task);
            task->task_state = K_RDY;
            break;
        case K_PEND_SUSPENDED:
            /* remove task on the block list because task is waken up */
            klist_rm(&task->task_list);
            task->task_state = K_SUSPENDED;
            break;
        default:
            k_err_proc(YUNOS_SYS_FATAL_ERR);
            break;
    }

    /* remove task on the tick list because task is waken up */
    tick_list_rm(task);

    task->blk_state = BLK_FINISH;
    task->blk_obj   = NULL;
}

void pend_to_blk_obj(blk_obj_t *blk_obj, ktask_t *task, tick_t timeout)
{
    /* task need to remember which object is blocked on */
    task->blk_obj = blk_obj;

    if (timeout != YUNOS_WAIT_FOREVER) {
#if (YUNOS_CONFIG_DYNTICKLESS > 0)
        g_elapsed_ticks = soc_elapsed_ticks_get();
        tick_list_insert(task, timeout + g_elapsed_ticks);
#else
        tick_list_insert(task, timeout);
#endif
    }

    task->task_state = K_PEND;

    /* remove from the ready list */
    ready_list_rm(&g_ready_queue, task);

    if (blk_obj->blk_policy == BLK_POLICY_FIFO) {
        /* add to the end of blocked objet list */
        klist_insert(&blk_obj->blk_list, &task->task_list);
    } else {
        /* add to the prio sorted block list */
        pend_list_add(&blk_obj->blk_list, task);
    }
}

void pend_task_rm(ktask_t *task)
{
    switch (task->task_state) {
        case K_PEND:
            /* remove task on the block list because task is waken up */
            klist_rm(&task->task_list);
            /*add to the ready list again*/
            ready_list_add(&g_ready_queue, task);
            task->task_state = K_RDY;
            break;
        case K_PEND_SUSPENDED:
            /* remove task on the block list because task is waken up */
            klist_rm(&task->task_list);
            task->task_state = K_SUSPENDED;
            break;
        default:
            k_err_proc(YUNOS_SYS_FATAL_ERR);
            break;
    }

    /* remove task on the tick list because task is waken up */
    tick_list_rm(task);
    task->blk_state = BLK_DEL;

    /* task is nothing blocked on so reset it to NULL */
    task->blk_obj = NULL;
}

void pend_list_reorder(ktask_t *task)
{
    if (task->blk_obj->blk_policy == BLK_POLICY_PRI) {
        /* remove it first and add it again in prio sorted list */
        klist_rm(&task->task_list);
        pend_list_add(&task->blk_obj->blk_list, task);
    }
}

#ifndef YUNOS_CONFIG_PERF_NO_PENDEND_PROC
kstat_t pend_state_end_proc(ktask_t *task)
{
    kstat_t status;

    switch (task->blk_state) {
        case BLK_FINISH:
            status = YUNOS_SUCCESS;
            break;
        case BLK_ABORT:
            status = YUNOS_BLK_ABORT;
            break;
        case BLK_TIMEOUT:
            status = YUNOS_BLK_TIMEOUT;
            break;
        case BLK_DEL:
            status = YUNOS_BLK_DEL;
            break;
        default:
            k_err_proc(YUNOS_BLK_INV_STATE);
            status = YUNOS_BLK_INV_STATE;
            break;
    }

    return status;
}
#endif

