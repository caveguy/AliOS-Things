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

#include <reent.h>
#include <sys/errno.h>
#include <k_api.h>
#include <sys/unistd.h>
#include <sys/errno.h>
#include "hal/soc/soc.h"
#include "board.h"

extern uart_dev_t uart_0;

int _execve_r(struct _reent *ptr, const char *name, char *const *argv, char *const *env)
{
    ptr->_errno = ENOTSUP;
    return -1;
}

int _fcntl_r(struct _reent *ptr, int fd, int cmd, int arg)
{
    ptr->_errno = ENOTSUP;
    return -1;
}

int _fork_r(struct _reent *ptr)
{
    ptr->_errno = ENOTSUP;
    return -1;
}

int _getpid_r(struct _reent *ptr)
{
    ptr->_errno = ENOTSUP;
    return 0;
}

int _isatty_r(struct _reent *ptr, int fd)
{
    if (fd >= 0 && fd < 3) {
        return 1;
    }

    ptr->_errno = ENOTSUP;
    return -1;
}

int _kill_r(struct _reent *ptr, int pid, int sig)
{
    ptr->_errno = ENOTSUP;
    return -1;
}

int _link_r(struct _reent *ptr, const char *old, const char *new)
{
    ptr->_errno = ENOTSUP;
    return -1;
}

_off_t _lseek_r(struct _reent *ptr, int fd, _off_t pos, int whence)
{
    ptr->_errno = ENOTSUP;
    return 0;
}

int _mkdir_r(struct _reent *ptr, const char *name, int mode)
{
    ptr->_errno = ENOTSUP;
    return 0;
}

int _open_r(struct _reent *ptr, const char *file, int flags, int mode)
{
    ptr->_errno = ENOTSUP;
    return 0;
}

int _close_r(struct _reent *ptr, int fd)
{
    ptr->_errno = ENOTSUP;
    return 0;
}

_ssize_t _read_r(struct _reent *ptr, int fd, void *buf, size_t nbytes)
{
    ptr->_errno = ENOTSUP;
    return 0;
}

/*
 * implement _write_r here
 */
_ssize_t _write_r(struct _reent *ptr, int fd, const void *buf, size_t nbytes)
{
    char *tmp = buf;

    switch (fd) {
        case STDOUT_FILENO: /*stdout*/
        case STDERR_FILENO: /* stderr */
            break;
        default:
            errno = EBADF;
            return -1;
    }

    for (int i = 0; i < nbytes; i++) {
        if (*tmp == '\n')
            hal_uart_send(&uart_0, (const void*)"\r", 1, 0);
        hal_uart_send(&uart_0, (const void*)tmp, 1, 0);
        tmp ++;
    }

    return nbytes;
}

int _fstat_r(struct _reent *ptr, int fd, struct stat *pstat)
{
    ptr->_errno = ENOTSUP;
    return -1;
}

int _rename_r(struct _reent *ptr, const char *old, const char *new)
{
    ptr->_errno = ENOTSUP;
    return 0;
}

void *_sbrk_r(struct _reent *ptr, ptrdiff_t incr)
{
    ptr->_errno = ENOTSUP;
    return NULL;
}

int _stat_r(struct _reent *ptr, const char *file, struct stat *pstat)
{
    ptr->_errno = ENOTSUP;
    return 0;
}

_CLOCK_T_ _times_r(struct _reent *ptr, struct tms *ptms)
{
    ptr->_errno = ENOTSUP;
    return -1;
}

int _unlink_r(struct _reent *ptr, const char *file)
{
    ptr->_errno = ENOTSUP;
    return 0;
}

int _wait_r(struct _reent *ptr, int *status)
{
    ptr->_errno = ENOTSUP;
    return -1;
}

int _gettimeofday_r(struct _reent *ptr, struct timeval *tv, void *__tzp)
{
     uint64_t t = yos_now_ms();
     tv->tv_sec = t / 1000;
     tv->tv_usec = ( t % 1000 ) * 1000;
     return 0;
}

void *_malloc_r(struct _reent *ptr, size_t size)
{

    return 0;
}

void *_realloc_r(struct _reent *ptr, void *old, size_t newlen)
{

    return 0;
}

void *_calloc_r(struct _reent *ptr, size_t size, size_t len)
{
    return 0;
}

void _free_r(struct _reent *ptr, void *addr)
{


}

void _exit(int status)
{
    while (1);
}

void _system(const char *s)
{
    return;
}

void abort(void)
{
    while (1);
}

