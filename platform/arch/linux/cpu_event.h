#ifndef CPU_EVENT_H
#define CPU_EVENT_H

typedef void (*cpu_event_handler)(const void *);

typedef struct {
    cpu_event_handler handler;
    const void       *arg;
} cpu_event_t;

int cpu_notify_event(cpu_event_t *event);
void *cpu_event_malloc(int size);
void cpu_event_free(void *p);

static inline int cpu_call_handler(cpu_event_handler handler, const void *arg)
{
    cpu_event_t event = {
        .handler = handler,
        .arg = arg,
    };
    return cpu_notify_event(&event);
}

#endif /* CPU_EVENT_H */

