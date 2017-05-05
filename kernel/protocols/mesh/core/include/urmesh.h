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


#ifndef UR_MESH_MESH_H
#define UR_MESH_MESH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "message.h"
#include "memory.h"
#include "ip6_address.h"
#include "sid_allocator.h"

typedef ur_error_t (* adapter_input_t)(void *message);
typedef ur_error_t (* adapter_interface_up_t)(void);
typedef ur_error_t (* adapter_interface_down_t)(void);
typedef ur_error_t (* adapter_interface_update_t)(void);
typedef struct ur_adapter_callback_s {
    adapter_input_t input;
    adapter_interface_up_t interface_up;
    adapter_interface_down_t interface_down;
    adapter_interface_update_t interface_update;
} ur_adapter_callback_t;

ur_error_t ur_mesh_ipv6_output(message_t *message, const ur_ip6_addr_t *ip6addr);
ur_error_t ur_mesh_input(message_t *p);

ur_error_t ur_mesh_init(void *config);
ur_error_t ur_mesh_start(void);
bool       ur_mesh_is_initialized(void);
ur_error_t ur_mesh_stop(void);
ur_error_t ur_mesh_register_callback(ur_adapter_callback_t *callback);

/* per device API */
uint8_t ur_mesh_get_device_state(void);
uint8_t ur_mesh_get_mode(void);
void ur_mesh_set_mode(uint8_t mode);
uint8_t ur_mesh_get_seclevel(void);
ur_error_t ur_mesh_set_seclevel(uint8_t level);

/* per network API */
typedef enum {
    UR_MESH_NET_CORE = 1,
    UR_MESH_NET_SUB = 2,
    UR_MESH_NET_DFL = UR_MESH_NET_SUB,
} ur_mesh_net_index_t;

const mac_address_t *ur_mesh_net_get_mac_address(ur_mesh_net_index_t nettype);
const uint16_t ur_mesh_net_get_meshnetid(ur_mesh_net_index_t nettype);
void ur_mesh_net_set_meshnetid(ur_mesh_net_index_t nettype, uint16_t meshnetid);
const uint16_t ur_mesh_net_get_sid(ur_mesh_net_index_t nettype);
const uint16_t ur_mesh_net_get_meshnetsize(ur_mesh_net_index_t nettype);
/* translation macros */
#define ur_mesh_get_meshnetid() ur_mesh_net_get_meshnetid(UR_MESH_NET_DFL)
#define ur_mesh_set_meshnetid(meshnetid) \
            ur_mesh_net_set_meshnetid(UR_MESH_NET_DFL, meshnetid)
#define ur_mesh_get_sid() ur_mesh_net_get_sid(UR_MESH_NET_DFL)
#define ur_mesh_get_meshnetsize() ur_mesh_net_get_meshnetsize(UR_MESH_NET_DFL)
#define ur_mesh_get_mac_address() ur_mesh_net_get_mac_address(UR_MESH_NET_DFL)

const ur_ip6_addr_t *ur_mesh_get_subscribed_mcast(void);
ur_error_t ur_mesh_set_subscribed_mcast(const ur_ip6_addr_t *addr);
bool ur_mesh_is_mcast_subscribed(const ur_ip6_addr_t *addr);
ur_error_t ur_mesh_add_addr(ur_ip6_addr_t *addr);
ur_error_t ur_mesh_del_addr(ur_ip6_addr_t *addr);
ur_error_t ur_mesh_resolve_dest(const ur_ip6_addr_t *dest, sid_t *dest_sid);
void ur_mesh_get_channel(channel_t *channel);

slist_t *ur_mesh_get_hals(void);
slist_t *ur_mesh_get_networks(void);

void ur_mesh_enable_whitelist(void);
void ur_mesh_disable_whitelist(void);
ur_error_t ur_mesh_add_whitelist(const mac_address_t *address);
ur_error_t ur_mesh_add_whitelist_rssi(const mac_address_t *address, int8_t rssi);
void ur_mesh_remove_whitelist(const mac_address_t*address);
void ur_mesh_clear_whitelist(void);

const ur_link_stats_t *ur_mesh_get_link_stats(void);
const frame_stats_t *ur_mesh_get_hal_stats(void);
const ur_message_stats_t *ur_mesh_get_message_stats(void);
const ur_mem_stats_t *ur_mesh_get_mem_stats(void);

#ifdef __cplusplus
}
#endif

#endif  /* UR_MESH_MESH_H */
