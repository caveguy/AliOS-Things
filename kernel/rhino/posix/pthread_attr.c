#include <pthread.h>

int pthread_attr_init(pthread_attr_t *attr)
{
    attr->stacksize                 = DEFAULT_THREAD_STACK_SIZE;
    attr->schedparam.sched_priority = DEFAULT_THREAD_PRIORITY;
    return 0;
}

int pthread_attr_destroy(pthread_attr_t *attr)
{
    memset(attr, 0, sizeof(pthread_attr_t));
    return 0;
}

int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate)
{
    return 0;
}

int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate)
{
   *detachstate = 0;
    return 0;
}

int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy)
{
    attr->schedpolicy = policy;
    return 0;
}


int pthread_attr_getschedpolicy(pthread_attr_t const *attr, int *policy)
{
   *policy = (int)attr->schedpolicy;
    return 0;
}

int pthread_attr_setschedparam(pthread_attr_t *attr,
                                      const struct sched_param *param)
{
    attr->schedparam.sched_priority = param->sched_priority;
    return 0;
}

int pthread_attr_getschedparam(const pthread_attr_t *attr,
                                      struct sched_param *param)

{
    param->sched_priority = attr->schedparam.sched_priority;
    return 0;
}

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize)
{
    attr->stacksize = stacksize;
    return 0;
}

int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize)
{
   *stacksize = attr->stacksize;
    return 0;
}

int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr)
{
    attr->stackaddr = stackaddr;
    return 0;
}

int pthread_attr_getstackaddr(const pthread_attr_t *attr, void **stackaddr)
{
   *stackaddr = attr->stackaddr;
    return 0;
}

int pthread_attr_setstack(pthread_attr_t *attr,
                                 void *stackaddr, size_t stacksize)
{
    attr->stackaddr = stackaddr;
    attr->stacksize = stacksize;
    return 0;
}

int pthread_attr_getstack(const pthread_attr_t *attr,
                                 void **stackaddr, size_t *stacksize)
{
   *stackaddr = attr->stackaddr;
   *stacksize = attr->stacksize;
    return 0;
}

int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize)
{
    return 0;
}

int pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize)
{
    return 0;
}

int pthread_attr_setscope(pthread_attr_t *attr, int scope)
{
    return 0;
}

int pthread_attr_getscope(const pthread_attr_t *attr, int *scope)
{
    return 0;
}

