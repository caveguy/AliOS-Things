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
#include <test_fw.h>
#include "tasksem_test.h"

ktask_t *task_tasksem;
ktask_t *task_tasksem_co1;
ktask_t *task_tasksem_co2;
ksem_t   test_tasksem;
ksem_t   test_tasksem_co1;
ksem_t   test_tasksem_co2;

static test_func_t *module_runner;
static const char  *module_name;
static uint8_t      module_casenum;

static const test_case_t tasksem_case_runner[] = {
    tasksem_param_test,
    tasksem_count_test,
    tasksem_opr_test,
    tasksem_coopr1_test,
    tasksem_coopr2_test,
    NULL
};

void tasksem_test(void)
{
    if (test_case_register((test_case_t *)tasksem_case_runner) == 0) {
        test_case_run();
        test_case_unregister();
    }
}

void task_tasksem_entry_register(const char *name, test_func_t *runner,
                                 uint8_t casenum)
{
    module_runner  = runner;
    module_name    = name;
    module_casenum = casenum;
}

void task_tasksem_entry(void *arg)
{
    test_func_t *runner;
    uint8_t      caseidx;
    char         name[64];
    uint8_t      casenum;

    runner  = (test_func_t *)module_runner;
    casenum = module_casenum;
    caseidx = 0;

    while (1) {
        if (*runner == NULL) {
            break;
        }

        if (casenum > 2) {
            caseidx++;
            sprintf(name, "%s_%d", module_name, caseidx);
        } else {
            sprintf(name, "%s", module_name);
        }

        if ((*runner)() == 0) {
            test_case_success++;
            PRINT_RESULT(name, PASS);
        } else {
            test_case_fail++;
            PRINT_RESULT(name, FAIL);
        }
        runner++;
    }

    next_test_case_notify();
    yunos_task_dyn_del(yunos_cur_task_get());
}

