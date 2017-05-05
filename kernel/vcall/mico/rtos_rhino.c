#include <k_api.h>
#include <string.h>
#include <stdlib.h>
#include "mico_rtos.h"
#include "mico_rtos_common.h"
#include "common.h"

extern void cpu_dis_interpt(void);
extern void cpu_en_interpt(void);

static int32_t critical_nest;

void mico_rtos_enter_critical(void)
{
    cpu_dis_interpt();
    critical_nest++;
}

void mico_rtos_exit_critical(void)
{
    critical_nest--;

    if (critical_nest == 0) {
        cpu_en_interpt();
    }
}

OSStatus mico_rtos_create_thread(mico_thread_t *thread, uint8_t priority, const char *name, mico_thread_function_t function, uint32_t stack_size, mico_thread_arg_t arg)
{
    kstat_t ret;

    ret = yunos_task_dyn_create((ktask_t **)thread, name, (void *)arg, priority, 0, stack_size, (task_entry_t)function, 1);

    if (ret == YUNOS_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

OSStatus mico_rtos_delete_thread(mico_thread_t *thread)
{
    kstat_t ret;

    ret = yunos_task_dyn_del(*((ktask_t **)thread));

    if (ret == YUNOS_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

void mico_rtos_suspend_thread(mico_thread_t *thread)
{
    if (thread == NULL) {
        yunos_task_suspend(g_active_task);
    } else {
        yunos_task_suspend(*((ktask_t **)thread));
    }
}

void mico_rtos_suspend_all_thread(void)
{
    yunos_sched_disable();
}

long mico_rtos_resume_all_thread(void)
{
    kstat_t ret;

    ret = yunos_sched_enable();

    if (ret == YUNOS_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

OSStatus mico_rtos_thread_join(mico_thread_t *thread)
{
    ktask_t *t;

    t = *((ktask_t **)thread);
    /*fixme*/
    while (t->task_state != K_DELETED) {
        mico_rtos_delay_milliseconds(10);
    }

    return kNoErr;
}


OSStatus mico_rtos_thread_force_awake(mico_thread_t *thread)
{
    kstat_t ret;

    ret = yunos_task_wait_abort(*((ktask_t **)thread));

    if (ret == YUNOS_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}


bool mico_rtos_is_current_thread(mico_thread_t *thread)
{
    ktask_t *t;

    t = *((ktask_t **)thread);

    if (t == g_active_task) {
        return true;
    }

    return false;
}


mico_thread_t *mico_rtos_get_current_thread(void)
{
    ktask_t **t;
    t = &g_active_task;

    return (mico_thread_t *)(t);
}


OSStatus mico_rtos_delay_milliseconds(uint32_t num_ms)
{
    uint32_t ticks;

    ticks = num_ms / YUNOS_CONFIG_TICKS_PER_SECOND;

    if (ticks == 0) {
        ticks = 1;
    }

    yunos_task_sleep(ticks);

    return kNoErr;
}

OSStatus mico_rtos_print_thread_status(char *pcWriteBuffer, int xWriteBufferLen)
{
    pcWriteBuffer[0] = 'n';
    pcWriteBuffer[1] = '\0';

    return kNoErr;
}

OSStatus mico_rtos_init_semaphore(mico_semaphore_t *semaphore, int count)
{
    kstat_t ret;

    ret = yunos_sem_dyn_create((ksem_t **)semaphore, "sema", count);

    if (ret == YUNOS_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

OSStatus mico_rtos_set_semaphore(mico_semaphore_t *semaphore)
{
    kstat_t ret;

    ret = yunos_sem_give(*((ksem_t **)semaphore));

    if (ret == YUNOS_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

OSStatus mico_rtos_get_semaphore(mico_semaphore_t *semaphore, uint32_t timeout_ms)
{
    kstat_t ret;

    if (timeout_ms == MICO_NEVER_TIMEOUT) {
        ret =  yunos_sem_take(*((ksem_t **)semaphore), YUNOS_WAIT_FOREVER);
    } else {
        ret =  yunos_sem_take(*((ksem_t **)semaphore), timeout_ms / YUNOS_CONFIG_TICKS_PER_SECOND);
    }

    if (ret == YUNOS_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

OSStatus mico_rtos_deinit_semaphore(mico_semaphore_t *semaphore)
{
    kstat_t ret;

    ret =  yunos_sem_dyn_del(*((ksem_t **)semaphore));

    if (ret == YUNOS_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

OSStatus mico_rtos_init_mutex(mico_mutex_t *mutex)
{
    kstat_t ret;

    ret = yunos_mutex_dyn_create((kmutex_t **)mutex, "mutex");

    if (ret == YUNOS_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

OSStatus mico_rtos_lock_mutex(mico_mutex_t *mutex)
{
    kstat_t ret;

    ret = yunos_mutex_lock(*((kmutex_t **)mutex), YUNOS_WAIT_FOREVER);

    if (ret == YUNOS_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

OSStatus mico_rtos_unlock_mutex(mico_mutex_t *mutex)
{
    kstat_t ret;

    ret = yunos_mutex_lock(*((kmutex_t **)mutex), YUNOS_WAIT_FOREVER);

    if (ret == YUNOS_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

OSStatus mico_rtos_deinit_mutex(mico_mutex_t *mutex)
{
    kstat_t ret;

    ret = yunos_mutex_dyn_del(*((kmutex_t **)mutex));

    if (ret == YUNOS_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

OSStatus mico_rtos_init_queue(mico_queue_t *queue, const char *name, uint32_t message_size, uint32_t number_of_messages)
{
    kstat_t ret;

    if (name == NULL) {
        name = "default_queue";
    }

    ret = yunos_buf_queue_dyn_create((kbuf_queue_t **)queue, name, number_of_messages * (message_size + COMPRESS_LEN(message_size)), message_size);

    if (ret == YUNOS_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}


OSStatus mico_rtos_push_to_queue(mico_queue_t *queue, void *message, uint32_t timeout_ms)
{
    kstat_t ret;
    kbuf_queue_t *q = *((kbuf_queue_t **)queue);

    timeout_ms = timeout_ms;

    ret = yunos_buf_queue_send(q, message, q->max_msg_size);

    if (ret == YUNOS_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

OSStatus mico_rtos_pop_from_queue(mico_queue_t *queue, void *message, uint32_t timeout_ms)
{
    kstat_t ret;
    size_t msg_len;

    ret = yunos_buf_queue_recv(*((kbuf_queue_t **)queue), timeout_ms / YUNOS_CONFIG_TICKS_PER_SECOND, message, &msg_len);

    if (ret == YUNOS_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

OSStatus mico_rtos_deinit_queue(mico_queue_t *queue)
{
    kstat_t ret;

    ret = yunos_buf_queue_dyn_del(*((kbuf_queue_t **)queue));

    if (ret == YUNOS_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}


bool mico_rtos_is_queue_empty(mico_queue_t *queue)
{
    bool ret;
    CPSR_ALLOC();

    kbuf_queue_t *q = *((kbuf_queue_t **)queue);

    YUNOS_CRITICAL_ENTER();

    if (q->cur_num == 0) {
        ret = true;
    } else {
        ret = false;;
    }

    YUNOS_CRITICAL_EXIT();

    return ret;
}

bool mico_rtos_is_queue_full(mico_queue_t *queue)
{
    bool ret;
    CPSR_ALLOC();

    kbuf_queue_t *q = *((kbuf_queue_t **)queue);
    uint32_t max_msg_num;

    YUNOS_CRITICAL_ENTER();

    max_msg_num = (q->ringbuf.end - q->ringbuf.buf) / (q->max_msg_size + COMPRESS_LEN(q->max_msg_size));

    if (q->cur_num == max_msg_num) {
        ret =  true;
    } else {
        ret = false;
    }

    YUNOS_CRITICAL_EXIT();

    return ret;
}

mico_time_t mico_rtos_get_time(void)
{
    return (mico_time_t)(yunos_sys_tick_get() * YUNOS_CONFIG_TICKS_PER_SECOND);
}

OSStatus mico_rtos_init_timer(mico_timer_t *timer, uint32_t time_ms, timer_handler_t function, void *arg)
{
    kstat_t ret;

    ret = yunos_timer_dyn_create((ktimer_t **)timer, "timer", (timer_cb_t)function, time_ms, time_ms, arg, 0);

    if (ret == YUNOS_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

OSStatus mico_rtos_start_timer(mico_timer_t *timer)
{
    kstat_t ret;

    ret = yunos_timer_start(*((ktimer_t **)timer));

    if (ret == YUNOS_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

OSStatus mico_rtos_stop_timer(mico_timer_t *timer)
{
    kstat_t ret;

    ret = yunos_timer_stop(*((ktimer_t **)timer));

    if (ret == YUNOS_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}


OSStatus mico_rtos_reload_timer(mico_timer_t *timer)
{
    kstat_t ret;

    ret = yunos_timer_stop(*((ktimer_t **)timer));

    if (ret == YUNOS_SUCCESS) {
        return kNoErr;
    } else {
        return kGeneralErr;
    }

    ret = yunos_timer_start(*((ktimer_t **)timer));

    if (ret == YUNOS_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

OSStatus mico_rtos_deinit_timer(mico_timer_t *timer)
{
    kstat_t ret;

    yunos_timer_stop(*((ktimer_t **)timer));
    ret = yunos_timer_dyn_del(*((ktimer_t **)timer));

    if (ret == YUNOS_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

bool mico_rtos_is_timer_running(mico_timer_t *timer)
{
    ktimer_t *t;

    t = *((ktimer_t **)timer);

    if (t->timer_state == TIMER_ACTIVE) {

        return true;
    }

    return false;
}

