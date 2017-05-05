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
#include "fifo.h"


/*
 * internal helper to calculate the unused elements in a fifo
 */
static uint32_t fifo_unused(struct k_fifo *fifo)
{
    return (fifo->mask + 1) - (fifo->in - fifo->out);
}



static int8_t is_power_of_2(uint32_t n)
{
    return (n != 0 && ((n & (n - 1)) == 0));
}


/*static fifio alloc do not need raw_malloc*/

int8_t fifo_init(struct k_fifo *fifo, void *buffer, uint32_t size)
{
    /*
     * round down to the next power of 2, since our 'let the indices
     * wrap' technique works only in this case.
     */
    if (!is_power_of_2(size)) {

        return 1;
    }

    fifo->in = 0;
    fifo->out = 0;
    fifo->data = buffer;

    if (size < 2) {
        fifo->mask = 0;
        return 1;
    }

    fifo->mask = size - 1;
    fifo->free_bytes = size;
    fifo->size = size;

    return 0;
}


static void fifo_copy_in(struct k_fifo *fifo, const void *src,
                         uint32_t len, uint32_t off)
{
    uint32_t l;

    uint32_t size = fifo->mask + 1;

    off &= fifo->mask;

    l = fifo_min(len, size - off);

    memcpy((unsigned  char *)fifo->data + off, src, l);
    memcpy(fifo->data, (unsigned  char *)src + l, len - l);


}

/*fifo write in*/
uint32_t fifo_in(struct k_fifo *fifo, const void *buf, uint32_t len)
{
    uint32_t l;

    CPSR_ALLOC();

    YUNOS_CRITICAL_ENTER();

    l = fifo_unused(fifo);

    if (len > l)
    { len = l; }

    fifo_copy_in(fifo, buf, len, fifo->in);
    fifo->in += len;

    fifo->free_bytes -= len;

    YUNOS_CRITICAL_EXIT();
    return len;
}

uint32_t fifo_in_full_reject(struct k_fifo *fifo, const void *buf, uint32_t len)
{
    uint32_t l;

    l = fifo_unused(fifo);

    if (len > l) {
        return 0;
    }

    fifo_copy_in(fifo, buf, len, fifo->in);
    fifo->in += len;

    fifo->free_bytes -= len;
    return len;
}

uint32_t fifo_in_full_reject_lock(struct k_fifo *fifo, const void *buf, uint32_t len)
{
    uint32_t l;

    CPSR_ALLOC();

    YUNOS_CRITICAL_ENTER();

    l = fifo_unused(fifo);

    if (len > l) {
        YUNOS_CRITICAL_EXIT();
        return 0;
    }

    fifo_copy_in(fifo, buf, len, fifo->in);
    fifo->in += len;

    fifo->free_bytes -= len;

    YUNOS_CRITICAL_EXIT();
    return len;
}



static void kfifo_copy_out(struct k_fifo *fifo, void *dst,
                           uint32_t len, uint32_t off)
{
    uint32_t l;
    uint32_t size = fifo->mask + 1;

    off &= fifo->mask;

    l = fifo_min(len, size - off);

    memcpy(dst, (unsigned  char *)fifo->data + off, l);
    memcpy((unsigned  char *)dst + l, fifo->data, len - l);

}

static uint32_t internal_fifo_out_peek(struct k_fifo *fifo,
                                       void *buf, uint32_t len)
{
    uint32_t l;

    l = fifo->in - fifo->out;

    if (len > l)
    { len = l; }

    kfifo_copy_out(fifo, buf, len, fifo->out);
    return len;
}

/*fifo read out but data remain in fifo*/

uint32_t fifo_out_peek(struct k_fifo *fifo,
                       void *buf, uint32_t len)
{

    uint32_t ret_len;

    CPSR_ALLOC();

    YUNOS_CRITICAL_ENTER();

    ret_len = internal_fifo_out_peek(fifo, buf, len);

    YUNOS_CRITICAL_EXIT();

    return ret_len;

}

/*fifo read out*/

uint32_t fifo_out(struct k_fifo *fifo, void *buf, uint32_t len)
{
    CPSR_ALLOC();

    YUNOS_CRITICAL_ENTER();

    len = internal_fifo_out_peek(fifo, buf, len);
    fifo->out += len;

    fifo->free_bytes += len;

    YUNOS_CRITICAL_EXIT();

    return len;
}

/*fifo read out all*/

uint32_t fifo_out_all(struct k_fifo *fifo, void *buf)
{
    uint32_t len;

    CPSR_ALLOC();

    YUNOS_CRITICAL_ENTER();

    len = fifo->size - fifo->free_bytes;

    if (len == 0) {

        YUNOS_CRITICAL_EXIT();
        return 0;
    }

    len = internal_fifo_out_peek(fifo, buf, len);
    fifo->out += len;

    fifo->free_bytes += len;

    YUNOS_CRITICAL_EXIT();

    return len;
}

