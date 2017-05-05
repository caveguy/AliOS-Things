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

#ifndef UR_NETWORK_MGMT_H
#define UR_NETWORK_MGMT_H

#include "mesh_mgmt.h"
#include "interface_context.h"

ur_error_t handle_discovery_request(uint8_t *payload, uint16_t length,
                                    const mesh_src_t *src,
                                    const mac_address_t *dest);
ur_error_t handle_discovery_response(uint8_t *payload, uint16_t length,
                                     const mesh_src_t *src,
                                     const mac_address_t *dest);
ur_error_t nm_start_discovery(void);
ur_error_t nm_stop_discovery(void);

#endif  /* UR_NETWORK_MGMT_H */

