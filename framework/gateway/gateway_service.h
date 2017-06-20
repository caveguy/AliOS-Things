#ifndef GATEWAY_SERVICE_H
#define GATEWAY_SERVICE_H

#include <yos/types.h>
#include <yos/framework.h>

#ifndef bool
#define bool unsigned char
#endif

int gateway_service_init(void);
void gateway_service_deinit(void);
int gateway_service_start(void);
void gateway_service_stop(void);

#endif
