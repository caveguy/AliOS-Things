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

#ifndef K_CPU_H
#define K_CPU_H

#include <signal.h>

sigset_t cpu_intrpt_save(void);
void   cpu_intrpt_restore(sigset_t cpsr);
void   cpu_intrpt_switch(void);
void   cpu_first_task_start(void);
void  *cpu_task_stack_init(cpu_stack_t *base, size_t size, void *arg, task_entry_t entry);
void   cpu_task_switch(void);

#define CPSR_ALLOC()    sigset_t cpsr

#define YUNOS_CPU_INTRPT_DISABLE() { cpsr = cpu_intrpt_save(); }
#define YUNOS_CPU_INTRPT_ENABLE()  { cpu_intrpt_restore(cpsr); }

void   cpu_idle_hook(void);
void   cpu_init_hook(void);
void   cpu_start_hook(void);
void   cpu_task_create_hook(ktask_t *tcb);
void   cpu_task_del_hook(ktask_t *tcb);

#endif /* K_CPU_H */

