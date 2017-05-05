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

#include <stdint.h>
#include <stddef.h>
#include "memory.h"

extern void *soc_mm_alloc(size_t size);
extern void soc_mm_free(void *mem);

static ur_mem_stats_t g_stats;

void *ur_mem_alloc(uint16_t size)
{
    void *mem;

    mem = soc_mm_alloc((size_t)size);
    if (mem) {
        g_stats.num += size;
    }
    return mem;
}

void ur_mem_free(void *mem, uint16_t size)
{
    g_stats.num -= size;
    soc_mm_free(mem);
}

const ur_mem_stats_t *ur_mem_get_stats(void)
{
    return &g_stats;
}
