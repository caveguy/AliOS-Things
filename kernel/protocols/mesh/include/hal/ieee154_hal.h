/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UR_IEEE_154_HAL_H
#define UR_IEEE_154_HAL_H

enum {
    IEEE154_DISCOVERY_TIMEOUT           = 400,    /* ms */
    IEEE154_AUTH_REQUEST_TIMEOUT        = 3000,   /* ms */
    IEEE154_AUTH_RELAY_TIMEOUT          = 3000,   /* ms */
    IEEE154_AUTH_RESPONSE_TIMEOUT       = 3000,   /* ms */
    IEEE154_ATTACH_REQUEST_TIMEOUT      = 1000,   /* ms */
    IEEE154_SID_REQUEST_TIMEOUT         = 3000,   /* ms */
    IEEE154_LINK_QUALITY_MOBILE_TIMEOUT = 1000,   /* ms */
    IEEE154_LINK_QUALITY_TIMEOUT        = 30000,  /* ms */
    IEEE154_ADVERTISEMENT_TIMEOUT       = 20000,  /* ms, 20 seconds */
    IEEE154_NEIGHBOR_ALIVE_TIMEOUT      = 120000, /* ms, 2  mins */
    IEEE154_NOTIFICATION_TIMEOUT        = 60000,  /* ms, 1 mins */
    IEEE154_ADDR_CACHE_ALIVE_TIMEOUT    = 3,
};

#endif  /* UR_IEEE_154_HAL_H */
