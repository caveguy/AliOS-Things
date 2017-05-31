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

#if (YUNOS_CONFIG_EVENT_FLAG > 0)
static kstat_t event_create(kevent_t *event, const name_t *name, uint32_t flags,
                            uint8_t mm_alloc_flag)
{
    NULL_PARA_CHK(event);
    NULL_PARA_CHK(name);

    /* init the list */
    klist_init(&event->blk_obj.blk_list);
    event->blk_obj.blk_policy = BLK_POLICY_PRI;
    event->blk_obj.name       = name;
#if (YUNOS_CONFIG_KOBJ_SET > 0)
    event->blk_obj.handle = NULL;
#endif
    event->flags              = flags;
    event->mm_alloc_flag      = mm_alloc_flag;

#if (YUNOS_CONFIG_SYSTEM_STATS > 0)
    klist_insert(&(g_kobj_list.event_head), &event->event_item);
#endif

    event->blk_obj.obj_type = YUNOS_EVENT_OBJ_TYPE;

    TRACE_EVENT_CREATE(g_active_task, event, name, flags);

    return YUNOS_SUCCESS;
}

kstat_t yunos_event_create(kevent_t *event, const name_t *name, uint32_t flags)
{
    return event_create(event, name, flags, K_OBJ_STATIC_ALLOC);
}

kstat_t yunos_event_del(kevent_t *event)
{
    CPSR_ALLOC();

    klist_t *blk_list_head;

    NULL_PARA_CHK(event);
    INTRPT_NESTED_LEVEL_CHK();

    YUNOS_CRITICAL_ENTER();

    if (event->blk_obj.obj_type != YUNOS_EVENT_OBJ_TYPE) {
        YUNOS_CRITICAL_EXIT();
        return YUNOS_KOBJ_TYPE_ERR;
    }

    if (event->mm_alloc_flag != K_OBJ_STATIC_ALLOC) {
        YUNOS_CRITICAL_EXIT();
        return YUNOS_KOBJ_DEL_ERR;
    }

    blk_list_head = &event->blk_obj.blk_list;

    event->blk_obj.obj_type = YUNOS_OBJ_TYPE_NONE;

    while (!is_klist_empty(blk_list_head)) {
        pend_task_rm(yunos_list_entry(blk_list_head->next, ktask_t, task_list));
    }

    event->flags = 0u;

#if (YUNOS_CONFIG_SYSTEM_STATS > 0)
    klist_rm(&event->event_item);
#endif

    TRACE_EVENT_DEL(g_active_task, event);

    YUNOS_CRITICAL_EXIT_SCHED();

    return YUNOS_SUCCESS;
}

#if (YUNOS_CONFIG_KOBJ_DYN_ALLOC > 0)
kstat_t yunos_event_dyn_create(kevent_t **event, const name_t *name, uint32_t flags)
{
    kstat_t   stat;
    kevent_t *event_obj;

    if (event == NULL) {
        return YUNOS_NULL_PTR;
    }

    event_obj = yunos_mm_alloc(sizeof(kevent_t));

    if (event_obj == NULL) {
        return YUNOS_NO_MEM;
    }

    stat = event_create(event_obj, name, flags, K_OBJ_DYN_ALLOC);

    if (stat != YUNOS_SUCCESS) {
        yunos_mm_free(event_obj);
        return stat;
    }

    *event = event_obj;

    return stat;
}

kstat_t yunos_event_dyn_del(kevent_t *event)
{
    CPSR_ALLOC();

    klist_t *blk_list_head;

    NULL_PARA_CHK(event);
    INTRPT_NESTED_LEVEL_CHK();

    YUNOS_CRITICAL_ENTER();

    if (event->blk_obj.obj_type != YUNOS_EVENT_OBJ_TYPE) {
        YUNOS_CRITICAL_EXIT();
        return YUNOS_KOBJ_TYPE_ERR;
    }

    if (event->mm_alloc_flag != K_OBJ_DYN_ALLOC) {
        YUNOS_CRITICAL_EXIT();
        return YUNOS_KOBJ_DEL_ERR;
    }

    blk_list_head = &event->blk_obj.blk_list;

    event->blk_obj.obj_type = YUNOS_OBJ_TYPE_NONE;

    while (!is_klist_empty(blk_list_head)) {
        pend_task_rm(yunos_list_entry(blk_list_head->next, ktask_t, task_list));
    }

    event->flags = 0u;

#if (YUNOS_CONFIG_SYSTEM_STATS > 0)
    klist_rm(&event->event_item);
#endif

    YUNOS_CRITICAL_EXIT_SCHED();

    yunos_mm_free(event);

    return YUNOS_SUCCESS;
}
#endif

kstat_t yunos_event_get(kevent_t *event, uint32_t flags, uint8_t opt,
                        uint32_t *actl_flags, tick_t ticks)
{
    CPSR_ALLOC();

    kstat_t stat;
    uint8_t status;

    NULL_PARA_CHK(event);
    NULL_PARA_CHK(actl_flags);

    INTRPT_NESTED_LEVEL_CHK();

    if ((opt != YUNOS_AND) && (opt != YUNOS_OR) && (opt != YUNOS_AND_CLEAR) &&
        (opt != YUNOS_OR_CLEAR)) {
        return YUNOS_NO_THIS_EVENT_OPT;
    }

    YUNOS_CRITICAL_ENTER();

    if (event->blk_obj.obj_type != YUNOS_EVENT_OBJ_TYPE) {
        YUNOS_CRITICAL_EXIT();
        return YUNOS_KOBJ_TYPE_ERR;
    }

    /* if option is AND MASK or OR MASK */
    if (opt & YUNOS_FLAGS_AND_MASK) {
        if ((event->flags & flags) == flags) {
            status = YUNOS_TRUE;
        } else {
            status = YUNOS_FALSE;
        }
    } else {
        if ((event->flags & flags) > 0u) {
            status = YUNOS_TRUE;
        } else {
            status = YUNOS_FALSE;
        }
    }

    if (status == YUNOS_TRUE) {
        *actl_flags = event->flags;

        if (opt & YUNOS_FLAGS_CLEAR_MASK) {
            event->flags &= ~flags;
        }

        TRACE_EVENT_GET(g_active_task, event);
        YUNOS_CRITICAL_EXIT();

        return YUNOS_SUCCESS;
    }

    /* can't get event, and return immediately if wait_option is YUNOS_NO_WAIT */
    if (ticks == YUNOS_NO_WAIT) {
        YUNOS_CRITICAL_EXIT();
        return YUNOS_NO_PEND_WAIT;
    }

    /* system is locked so task can not be blocked just return immediately */
    if (g_sched_lock > 0u) {
        YUNOS_CRITICAL_EXIT();
        return YUNOS_SCHED_DISABLE;
    }

    /* remember the passed information */
    g_active_task->pend_option = opt;
    g_active_task->pend_flags  = flags;
    g_active_task->pend_info   = actl_flags;

    pend_to_blk_obj((blk_obj_t *)event, g_active_task, ticks);

    TRACE_EVENT_GET_BLK(g_active_task, event, ticks);

    YUNOS_CRITICAL_EXIT_SCHED();

#ifndef YUNOS_CONFIG_PERF_NO_PENDEND_PROC
    YUNOS_CPU_INTRPT_DISABLE();

    /* so the task is waked up, need know which reason cause wake up */
    stat = pend_state_end_proc(g_active_task);

    YUNOS_CPU_INTRPT_ENABLE();
#else
    stat = YUNOS_SUCCESS;
#endif

    return stat;
}

static kstat_t event_set(kevent_t *event, uint32_t flags, uint8_t opt)
{
    CPSR_ALLOC();

    klist_t  *iter;
    klist_t  *event_head;
    klist_t  *iter_temp;
    ktask_t  *task;
    uint8_t   status;
    uint32_t  cur_event_flags;

    /* this is only needed when system zero interrupt feature is enabled */
#if (YUNOS_CONFIG_INTRPT_GUARD > 0)
    soc_intrpt_guard();
#endif

    status = YUNOS_FALSE;

    YUNOS_CRITICAL_ENTER();

    if (event->blk_obj.obj_type != YUNOS_EVENT_OBJ_TYPE) {
        YUNOS_CRITICAL_EXIT();
        return YUNOS_KOBJ_TYPE_ERR;
    }

    event_head = &event->blk_obj.blk_list;

    /* if the set_option is AND_MASK, it just clears the flags and will return immediately */
    if (opt & YUNOS_FLAGS_AND_MASK) {
        event->flags &= flags;

        YUNOS_CRITICAL_EXIT();
        return YUNOS_SUCCESS;
    } else {
        event->flags |= flags;
    }

    cur_event_flags = event->flags;
    iter = event_head->next;

    /* if list is not empty */
    while (iter != event_head) {
        task = yunos_list_entry(iter, ktask_t, task_list);
        iter_temp = iter->next;

        if (task->pend_option & YUNOS_FLAGS_AND_MASK) {
            if ((cur_event_flags & task->pend_flags) == task->pend_flags) {
                status = YUNOS_TRUE;
            } else {
                status = YUNOS_FALSE;
            }
        } else {
            if (cur_event_flags & task->pend_flags) {
                status = YUNOS_TRUE;
            } else {
                status = YUNOS_FALSE;
            }
        }

        if (status == YUNOS_TRUE) {
            (*(uint32_t *)(task->pend_info)) = cur_event_flags;

            /* the task condition is met, just wake this task */
            pend_task_wakeup(task);

            TRACE_EVENT_TASK_WAKE(g_active_task, task, event);

            /* does it need to clear the flags */
            if (task->pend_option & YUNOS_FLAGS_CLEAR_MASK) {
                event->flags &= ~(task->pend_flags);
            }
        }

        iter = iter_temp;
    }

    YUNOS_CRITICAL_EXIT_SCHED();

    return YUNOS_SUCCESS;
}

kstat_t yunos_event_set(kevent_t *event, uint32_t flags, uint8_t opt)
{
    NULL_PARA_CHK(event);

    if ((opt != YUNOS_AND) && (opt != YUNOS_OR)) {
        return YUNOS_NO_THIS_EVENT_OPT;
    }

    return event_set(event, flags, opt);
}
#endif /* YUNOS_CONFIG_EVENT_FLAG */

