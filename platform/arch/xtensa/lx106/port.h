/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef CPU_H
#define CPU_H
#include "xtensa/tie/xt_core.h"
#include "xtensa/hal.h"
#include "xtensa/config/core.h"
#include "xtensa/config/system.h"       /* required for XSHAL_CLIB */
#include "xtensa/xtruntime.h"
#include "c_types.h"
#include "ets_sys.h"

typedef void (* _xt_isr)(void *arg);

typedef struct __xt_isr_entry {
	_xt_isr	handler;
    void *	arg;
} _xt_isr_entry;

size_t cpu_intrpt_save(void);
void   cpu_intrpt_restore(size_t cpsr);
void   cpu_intrpt_switch(void);
void   cpu_task_switch(void);
void   cpu_first_task_start(void);
void  *cpu_task_stack_init(cpu_stack_t *base, size_t size, void *arg, task_entry_t entry);

#if 0
#define CPSR_ALLOC() size_t cpsr
#define RHINO_CPU_INTRPT_DISABLE() { cpsr = XTOS_SET_INTLEVEL(XCHAL_EXCM_LEVEL); }
#define RHINO_CPU_INTRPT_ENABLE()  { XTOS_RESTORE_JUST_INTLEVEL(cpsr); }
#else
#define INT_ENA_WDEV        0x3ff20c18
#define WDEV_TSF0_REACH_INT (BIT(27))
extern volatile uint32_t g_nmilock_cnt;
extern uint32_t WDEV_INTEREST_EVENT;

#define CPSR_ALLOC() size_t cpsr = 0
#define RHINO_CPU_INTRPT_DISABLE() do {       \
        if (NMIIrqIsOn == 0) {      \
            cpsr = XTOS_SET_INTLEVEL(XCHAL_EXCM_LEVEL);     \
            if (g_nmilock_cnt == 0) \
            {                       \
                __asm__ __volatile__("rsync":::"memory");       \
                REG_WRITE(INT_ENA_WDEV, 0); \
                __asm__ __volatile__("rsync":::"memory");       \
                REG_WRITE(INT_ENA_WDEV, WDEV_TSF0_REACH_INT);   \
                __asm__ __volatile__("rsync":::"memory");       \
            }                       \
            g_nmilock_cnt++;            \
        }   \
    } while(0)

#define RHINO_CPU_INTRPT_ENABLE()    do {   \
        if (NMIIrqIsOn == 0) {      \
            if (g_nmilock_cnt > 0) {g_nmilock_cnt--;}       \
            if  ( g_nmilock_cnt == 0 )  \
            {                           \
                __asm__ __volatile__("rsync":::"memory");       \
                REG_WRITE(INT_ENA_WDEV, WDEV_INTEREST_EVENT);   \
                __asm__ __volatile__("rsync":::"memory");       \
            }                           \
            XTOS_RESTORE_JUST_INTLEVEL(cpsr);               \
        }   \
    } while(0)
#endif

RHINO_INLINE uint8_t cpu_cur_get(void)
{
    return 0;
}

void _xt_isr_attach(uint8_t i, _xt_isr func, void *arg);

void ResetCcountVal(uint32_t cnt_val);

uint32_t xthal_get_ccount(void);

#endif /* CPU_H */

