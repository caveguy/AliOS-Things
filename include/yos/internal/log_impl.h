/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef YOS_LOG_IMPL_H
#define YOS_LOG_IMPL_H

#if defined(__cplusplus)
extern "C"
{
#endif

extern unsigned int aos_log_level;
static inline unsigned int aos_log_get_level(void)
{
    return aos_log_level;
}

enum log_level_bit {
    YOS_LL_V_NONE_BIT = -1,
    YOS_LL_V_FATAL_BIT,
    YOS_LL_V_ERROR_BIT,
    YOS_LL_V_WARN_BIT,
    YOS_LL_V_INFO_BIT,
    YOS_LL_V_DEBUG_BIT,
    YOS_LL_V_MAX_BIT
};

#define YOS_LOG_LEVEL aos_log_get_level()

#define YOS_LL_V_NONE  0
#define YOS_LL_V_ALL  0xFF
#define YOS_LL_V_FATAL (1 << YOS_LL_V_FATAL_BIT)
#define YOS_LL_V_ERROR (1 << YOS_LL_V_ERROR_BIT)
#define YOS_LL_V_WARN  (1 << YOS_LL_V_WARN_BIT)
#define YOS_LL_V_INFO  (1 << YOS_LL_V_INFO_BIT)
#define YOS_LL_V_DEBUG (1 << YOS_LL_V_DEBUG_BIT)

/*
 * color def.
 * see http://stackoverflow.com/questions/3585846/color-text-in-terminal-applications-in-unix
 */
#define COL_DEF "\x1B[0m"   //white
#define COL_RED "\x1B[31m"  //red
#define COL_GRE "\x1B[32m"  //green
#define COL_BLU "\x1B[34m"  //blue
#define COL_YEL "\x1B[33m"  //yellow
#define COL_WHE "\x1B[37m"  //white
#define COL_CYN "\x1B[36m"
#define COL_MAG "\x1B[35m"

#define GLOBAL_LOG_TAG "\e[65m"    //LOG TAG, use ESC characters, g(cli) ascii is 65

#include <yos/kernel.h>
extern int csp_printf(const char *fmt, ...);
#ifdef CONFIG_LOGMACRO_DETAILS
#define log_print(CON, MOD, COLOR, LVL, FMT, ...) \
    do { \
        if (CON) { \
            long long ms = aos_now_ms();; \
            csp_printf(GLOBAL_LOG_TAG COLOR " [%4d.%03d]<%s> %s [%s#%d] : ", (int)(ms/1000), (int)(ms%1000), LVL, MOD, __FUNCTION__, __LINE__); \
            csp_printf(GLOBAL_LOG_TAG FMT COL_DEF "\r\n", ##__VA_ARGS__); \
        } \
    } while (0)

#else
#define log_print(CON, MOD, COLOR, LVL, FMT, ...) \
    do { \
        if (CON) { \
            csp_printf(GLOBAL_LOG_TAG "[%06d]<" LVL "> "FMT"\n", (unsigned)aos_now_ms(), ##__VA_ARGS__); \
        } \
    } while (0)

#endif

#define void_func(fmt, ...)

#ifndef os_printf
#ifndef csp_printf
int csp_printf(const char *fmt, ...);
#else
extern int csp_printf(const char *fmt, ...);
#endif
#else
extern int csp_printf(const char *fmt, ...);
#endif

#undef LOGF
#undef LOGE
#undef LOGW
#undef LOGI
#undef LOGD
#undef LOG

#define LOG_IMPL(fmt, ...) \
            log_print(1, "YOS", COL_DEF, "V", fmt, ##__VA_ARGS__)

#ifdef NDEBUG
#define CONFIG_LOGMACRO_SILENT
#endif

#ifdef DEBUG
#define LOGD_IMPL(mod, fmt, ...) \
            log_print(YOS_LOG_LEVEL & YOS_LL_V_DEBUG, mod, COL_WHE, "D", fmt, ##__VA_ARGS__)
#else
#define LOGD_IMPL(mod, fmt, ...) void_func(fmt, ##__VA_ARGS__)
#endif

#ifdef CONFIG_LOGMACRO_SILENT
#define LOGF_IMPL(mod, fmt, ...) void_func(fmt, ##__VA_ARGS__)
#define LOGE_IMPL(mod, fmt, ...) void_func(fmt, ##__VA_ARGS__)
#define LOGW_IMPL(mod, fmt, ...) void_func(fmt, ##__VA_ARGS__)
#define LOGI_IMPL(mod, fmt, ...) void_func(fmt, ##__VA_ARGS__)

#else

#define LOGF_IMPL(mod, fmt, ...) \
            log_print(YOS_LOG_LEVEL & YOS_LL_V_FATAL, mod, COL_RED, "F", fmt, ##__VA_ARGS__)
#define LOGE_IMPL(mod, fmt, ...) \
            log_print(YOS_LOG_LEVEL & YOS_LL_V_ERROR, mod, COL_YEL, "E", fmt, ##__VA_ARGS__)
#define LOGW_IMPL(mod, fmt, ...) \
            log_print(YOS_LOG_LEVEL & YOS_LL_V_WARN, mod, COL_BLU, "W", fmt, ##__VA_ARGS__)
#define LOGI_IMPL(mod, fmt, ...) \
            log_print(YOS_LOG_LEVEL & YOS_LL_V_INFO, mod, COL_GRE, "I", fmt, ##__VA_ARGS__)

#endif /* CONFIG_LOGMACRO_SILENT */

#if defined(__cplusplus)
}
#endif

#endif /* YOS_LOG_IMPL_H */

