/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <k_api.h>
#include <test_fw.h>

#include "timer_test.h"

#define TIMER0_ROUND 1
#define TIMER0_MAGIC 0x12345678

static ktask_t  *task_0_test;
static ktimer_t timer_0_test;
static ksem_t   sem_0_test;

static void timer_0_func(ktimer_t *timer, void *arg)
{
    TIMER_VAL_CHK(timer == &timer_0_test);
    TIMER_VAL_CHK((size_t)arg == TIMER0_MAGIC);

    yunos_sem_give(&sem_0_test);
}


static void timer_create_param_test(void)
{
    kstat_t ret;

    ret = yunos_timer_create(NULL, "timer_0_test", (timer_cb_t)timer_0_func,
                             TIMER0_ROUND, TIMER0_ROUND, (void *)TIMER0_MAGIC, 0);
    TIMER_VAL_CHK(ret == YUNOS_NULL_PTR);

    ret = yunos_timer_create(&timer_0_test, NULL, (timer_cb_t)timer_0_func,
                             TIMER0_ROUND, TIMER0_ROUND, (void *)TIMER0_MAGIC, 0);
    TIMER_VAL_CHK(ret == YUNOS_NULL_PTR);

    ret = yunos_timer_create(&timer_0_test, "timer_0_test", NULL, TIMER0_ROUND,
                             TIMER0_ROUND, (void *)TIMER0_MAGIC, 0);
    TIMER_VAL_CHK(ret == YUNOS_NULL_PTR);

    ret = yunos_timer_create(&timer_0_test, "timer_0_test",
                             (timer_cb_t)timer_0_func,
                             0, TIMER0_ROUND, (void *)TIMER0_MAGIC, 0);
    TIMER_VAL_CHK(ret == YUNOS_INV_PARAM);
}

static void timer_del_param_test(void)
{
    kstat_t ret;
    ktimer_t timer;

    memset(&timer, 0, sizeof(ktimer_t));

    ret = yunos_timer_del(NULL);
    TIMER_VAL_CHK(ret == YUNOS_NULL_PTR);

    ret = yunos_timer_start(&timer);
    TIMER_VAL_CHK(ret == YUNOS_KOBJ_TYPE_ERR);

    ret = yunos_timer_stop(&timer);
    TIMER_VAL_CHK(ret == YUNOS_KOBJ_TYPE_ERR);

    ret = yunos_timer_change(&timer, 10, 10);
    TIMER_VAL_CHK(ret == YUNOS_KOBJ_TYPE_ERR);

    ret = yunos_timer_del(&timer);
    TIMER_VAL_CHK(ret == YUNOS_KOBJ_TYPE_ERR);
}

static void task_timer0_entry(void *arg)
{
    kstat_t ret = 0;

    while (1) {
        /* check yunos_timer_create */
        timer_create_param_test();

        /* check yunos_timer_del */
        timer_del_param_test();

        ret = yunos_sem_create(&sem_0_test, "sem_0_test", 0);
        TIMER_VAL_CHK(ret == YUNOS_SUCCESS);

        ret = yunos_timer_create(&timer_0_test, "timer_0_test",
                                 (timer_cb_t)timer_0_func,
                                 TIMER0_ROUND, TIMER0_ROUND, (void *)TIMER0_MAGIC, 0);
        TIMER_VAL_CHK(ret == YUNOS_SUCCESS);

        ret = yunos_timer_del(&timer_0_test);
        TIMER_VAL_CHK(ret == YUNOS_SUCCESS);

        ret = yunos_timer_create(&timer_0_test, "timer_0_test",
                                 (timer_cb_t)timer_0_func,
                                 TIMER0_ROUND, TIMER0_ROUND, (void *)TIMER0_MAGIC, 1);
        TIMER_VAL_CHK(ret == YUNOS_SUCCESS);

        ret = yunos_sem_take(&sem_0_test, YUNOS_WAIT_FOREVER);
        TIMER_VAL_CHK(ret == YUNOS_SUCCESS);

        ret = yunos_timer_stop(&timer_0_test);
        TIMER_VAL_CHK(ret == YUNOS_SUCCESS);
        ret = yunos_timer_dyn_del(&timer_0_test);
        TIMER_VAL_CHK(ret == YUNOS_KOBJ_DEL_ERR);

        ret = yunos_timer_del(&timer_0_test);
        if (ret == YUNOS_SUCCESS) {
            test_case_success++;
            PRINT_RESULT("timer create&del", PASS);
        } else {
            test_case_fail++;
            PRINT_RESULT("timer create&del", FAIL);
        }

        yunos_sem_del(&sem_0_test);

        next_test_case_notify();
        yunos_task_dyn_del(task_0_test);
    }
}

kstat_t task_timer_create_del_test(void)
{
    kstat_t ret;

    ret = yunos_task_dyn_create(&task_0_test, "task_timer0_test", 0, 10,
                                0, TASK_TEST_STACK_SIZE, task_timer0_entry, 1);
    TIMER_VAL_CHK((ret == YUNOS_SUCCESS) || (ret == YUNOS_STOPPED));

    return 0;
}

