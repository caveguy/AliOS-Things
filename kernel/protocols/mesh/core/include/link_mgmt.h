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

#ifndef UR_LINK_MGMT_H
#define UR_LINK_MGMT_H

#include "mesh_mgmt.h"

enum {
    LINK_REQUEST_MOBILE_TIMEOUT   = 1000,     /* ms */
    LINK_REQUEST_TIMEOUT          = 30000,   /* ms */
    LINK_ESTIMATE_TIMES           = 4,
    LINK_ESTIMATE_SENT_THRESHOLD  = 4,
    LINK_ESTIMATE_COEF            = 256,
    LINK_ESTIMATE_UPDATE_ALPHA    = 32,
    LINK_COST_MAX                 = 1024,
};

ur_error_t send_link_request(network_context_t *network,
                             const mac_address_t *dest,
                             uint8_t *tlvs, uint8_t length);

ur_error_t handle_link_request(uint8_t *payload, uint16_t length,
                               const mesh_src_t *src,
                               const mac_address_t *dest);
ur_error_t handle_link_accept_and_request(uint8_t *payload, uint16_t length,
                                          const mesh_src_t *src,
                                          const mac_address_t *dest);
ur_error_t handle_link_accept(uint8_t *payload, uint16_t length,
                              const mesh_src_t *src,
                              const mac_address_t *dest);
ur_error_t handle_link_reject(uint8_t *payload, uint16_t length,
                              const mesh_src_t *src,
                              const mac_address_t *dest);

void start_neighbor_updater(void);
void stop_neighbor_updater(void);

typedef void (* neighbor_updated_t)(hal_context_t *hal);
ur_error_t register_neighbor_updater(neighbor_updated_t updater);

void       neighbors_init(void);
neighbor_t *update_neighbor(const mesh_src_t *addr,
                            uint8_t *tlvs, uint16_t length, bool is_attach);
void       set_state_to_neighbor(void);
neighbor_t *get_neighbor_by_ueid(const uint8_t *ueid);
neighbor_t *get_neighbor_by_mac_addr(hal_context_t *hal,
                                     const uint8_t *mac_addr);
neighbor_t *get_neighbor_by_sid(hal_context_t *hal, uint16_t sid,
                                uint16_t meshnetid);
neighbor_t *get_neighbors(uint16_t *num);

#endif  /* UR_LINK_MGMT_H */
