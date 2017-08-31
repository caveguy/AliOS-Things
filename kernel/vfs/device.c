/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include <yos/kernel.h>
#include <yos/yos.h>

#include <vfs_conf.h>
#include <vfs_err.h>
#include <vfs_register.h>
#include <event_device.h>

#if (YOS_CONFIG_VFS_POLL_SUPPORT > 0)
static int inited;

typedef struct {
    yos_mutex_t    mutex;
    poll_notify_t  poll_cb;
    struct pollfd *fd;
    void          *poll_data;
    int            counter;
    dlist_t        bufs;
    int            cache_count;
    dlist_t        buf_cache;
} event_dev_t;

typedef struct {
    dlist_t node;
    size_t len;
    char buf[];
} dev_event_t;

static int event_open(inode_t *node, file_t *file)
{
    event_dev_t *pdev = (event_dev_t *)yos_malloc(sizeof * pdev);
    bzero(pdev, sizeof * pdev);
    yos_mutex_new(&pdev->mutex);
    dlist_init(&pdev->bufs);
    dlist_init(&pdev->buf_cache);
    file->f_arg = pdev;
    return 0;
}

static int event_close(file_t *file)
{
    event_dev_t *pdev = file->f_arg;
    yos_mutex_free(&pdev->mutex);

    while (!dlist_empty(&pdev->bufs)) {
        dlist_t *n = pdev->bufs.next;
        dlist_del(n);
        yos_free(n);
    }

    while (!dlist_empty(&pdev->buf_cache)) {
        dlist_t *n = pdev->buf_cache.next;
        dlist_del(n);
        yos_free(n);
    }

    yos_free(pdev);

    return 0;
}

static ssize_t _event_write(file_t *f, const void *buf, size_t len, bool urgent)
{
    event_dev_t *pdev = f->f_arg;
    yos_mutex_lock(&pdev->mutex, YOS_WAIT_FOREVER);

    dev_event_t *evt;
    evt = (dev_event_t *)pdev->buf_cache.next;

    if (pdev->cache_count > 0 && evt->len == len) {
        dlist_del(&evt->node);
        pdev->cache_count --;
    } else {
        evt = (dev_event_t *)yos_malloc(sizeof(*evt) + len);
    }

    if (evt == NULL) {
        len = -1;
        goto out;
    }

    pdev->counter ++;

    evt->len = len;
    memcpy(evt->buf, buf, len);
    if (urgent) {
        dlist_add(&evt->node, &pdev->bufs);
    } else {
        dlist_add_tail(&evt->node, &pdev->bufs);
    }

    if (pdev->poll_cb != NULL) {
        pdev->fd->revents |= POLLIN;
        pdev->poll_cb(pdev->fd, pdev->poll_data);
    }
out:
    yos_mutex_unlock(&pdev->mutex);
    return len;
}

static ssize_t event_write(file_t *f, const void *buf, size_t len)
{
    return _event_write(f, buf, len, false);
}

static int event_ioctl(file_t *f, int cmd, unsigned long arg)
{
    int len = _GET_LEN(cmd);
    void *buf = (void *)arg;
    cmd = _GET_CMD(cmd);
    switch (cmd) {
        case IOCTL_WRITE_NORMAL:
            return _event_write(f, buf, len, false);
        case IOCTL_WRITE_URGENT:
            return _event_write(f, buf, len, true);
    }

    return -1;
}

static ssize_t event_read(file_t *f, void *buf, size_t len)
{
    event_dev_t *pdev = f->f_arg;
    int cnt = pdev->counter;

    if (!cnt) {
        return 0;
    }

    yos_mutex_lock(&pdev->mutex, YOS_WAIT_FOREVER);

    dev_event_t *evt = (dev_event_t *)pdev->bufs.next;
    dlist_del(&evt->node);
    cnt = (len > evt->len) ? evt->len : len;
    memcpy(buf, evt->buf, cnt);

    if (pdev->cache_count < 4) {
        dlist_add(&evt->node, &pdev->buf_cache);
        pdev->cache_count ++;
    } else {
        yos_free(evt);
    }

    pdev->counter --;

    yos_mutex_unlock(&pdev->mutex);

    return cnt;
}

static int event_poll(file_t *f, bool setup, poll_notify_t notify,
                      struct pollfd *fd, void *opa)
{
    event_dev_t *pdev = f->f_arg;

    yos_mutex_lock(&pdev->mutex, YOS_WAIT_FOREVER);
    if (!setup) {
        pdev->poll_cb = NULL;
        pdev->poll_data = NULL;
        goto out;
    }

    pdev->poll_cb = notify;
    pdev->fd = fd;
    pdev->poll_data = opa;

    if (pdev->counter) {
        pdev->fd->revents |= POLLIN;
        (*notify)(fd, opa);
    }
out:
    yos_mutex_unlock(&pdev->mutex);

    return 0;
}

static file_ops_t event_fops = {
    .open = event_open,
    .read = event_read,
    .write = event_write,
    .close = event_close,
    .poll = event_poll,
    .ioctl = event_ioctl,
};

int vfs_device_init(void)
{
    int ret;

    if (inited == 1) {
        return  VFS_SUCCESS;
    }

    ret = yos_register_driver("/dev/event", &event_fops, NULL);

    if (ret != VFS_SUCCESS) {
        return ret;
    }

    inited = 1;

    return VFS_SUCCESS;
}
#endif

