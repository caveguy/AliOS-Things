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

#ifndef UR_AR_H
#define UR_AR_H

#include <stdint.h>

#include "sid_allocator.h"
#include "mesh_forwarder.h"
#include "interface_context.h"
#include "sid_allocator.h"

enum {
    UR_MESH_ADDRESS_CACHE_SIZE = 8,
};

typedef enum {
    AQ_STATE_INVALID,
    AQ_STATE_CACHED,
    AQ_STATE_QUERY,
} cache_state_t;

typedef struct address_cache_s {
    uint8_t       ueid[8];
    uint16_t      meshnetid;
    uint16_t      sid;
    uint16_t      attach_sid;
    uint16_t      attach_netid;
    uint8_t       timeout;
    uint8_t       retry_timeout;
    cache_state_t state;
} address_cache_t;

enum {
    ADDRESS_QUERY_TIMEOUT             = 3,    /* seconds */
    ADDRESS_QUERY_RETRY_TIMEOUT       = 3,    /* seconds */
    ADDRESS_QUERY_STATE_UPDATE_PERIOD = 1000, /* ms */
};

enum {
    PF_ATTACH_QUERY = 0,
    TARGET_QUERY = 1,
};

typedef void (* address_resolved_handler_t)(network_context_t *network,
                                            address_cache_t *target,
                                            ur_error_t error);

void address_resolver_init(address_resolved_handler_t handler);
ur_error_t address_resolve(network_context_t *network, uint8_t query_type,
                           ur_node_id_t *target, ur_node_id_t *attach);
ur_error_t handle_address_query(uint8_t *payload, uint16_t length,
                                const mesh_src_t *src,
                                const mac_address_t *dest);
ur_error_t handle_address_query_response(uint8_t *payload, uint16_t length,
                                         const mesh_src_t *src,
                                         const mac_address_t *dest);
ur_error_t handle_address_notification(uint8_t *payload, uint16_t length,
                                       const mesh_src_t *src,
                                       const mac_address_t *dest);
ur_error_t handle_dest_unreachable(uint8_t *payload, uint16_t length,
                                   const mesh_src_t *src,
                                   const mac_address_t *dest);
ur_error_t handle_address_error(uint8_t *payload, uint16_t length,
                                const mesh_src_t *src,
                                const mac_address_t *dest);
ur_error_t send_address_notification(network_context_t *network,
                                     uint16_t dest);
ur_error_t send_dest_unreachable(network_context_t *network, sid_t *dest,
                                 sid_t *target);
ur_error_t send_address_error(network_context_t *network,
                              sid_t *dest);

#endif  /* UR_AR_H */
