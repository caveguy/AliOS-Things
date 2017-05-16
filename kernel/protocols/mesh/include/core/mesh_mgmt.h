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

#ifndef UR_MM_H
#define UR_MM_H

#include "core/mesh_mgmt_tlvs.h"
#include "core/mesh_forwarder.h"
#include "core/topology.h"
#include "ipv6/ip6.h"
#include "hal/interface_context.h"

typedef enum device_state_s {
    DEVICE_STATE_DISABLED     = 0,
    DEVICE_STATE_DETACHED     = 1,
    DEVICE_STATE_ATTACHED     = 2,
    DEVICE_STATE_LEAF         = 3,
    DEVICE_STATE_LEADER       = 4,
    DEVICE_STATE_SUPER_ROUTER = 5,
    DEVICE_STATE_ROUTER       = 6,
} mm_device_state_t;

static inline const char *state2str(mm_device_state_t state)
{
    switch (state) {
        case DEVICE_STATE_DISABLED:
            return "disabled";
        case DEVICE_STATE_DETACHED:
            return "detached";
        case DEVICE_STATE_ATTACHED:
            return "attached";
        case DEVICE_STATE_LEAF:
            return "leaf";
        case DEVICE_STATE_LEADER:
            return "leader";
        case DEVICE_STATE_SUPER_ROUTER:
            return "super_router";
        case DEVICE_STATE_ROUTER:
            return "router";
        default:
            return "unknown";
    }
}

static inline uint16_t get_subnetsize_from_netinfo(mm_netinfo_tv_t *netinfo)
{
    return (netinfo->subnet_size_1 << 8) | netinfo->subnet_size_2;
}

static inline void set_subnetsize_to_netinfo(mm_netinfo_tv_t *netinfo,
                                             uint16_t subnetsize)
{
    netinfo->subnet_size_1 = (uint8_t)((subnetsize >> 8) & 0x7);
    netinfo->subnet_size_2 = (uint8_t)(subnetsize & 0xff);
}

typedef enum mm_command_s {
    COMMAND_COMMAND_MASK            = 0x7f,
    COMMAND_ADVERTISEMENT           = 0x1,
    COMMAND_DISCOVERY_REQUEST       = 0x2,
    COMMAND_DISCOVERY_RESPONSE      = 0x3,
    COMMAND_ATTACH_REQUEST          = 0x4,
    COMMAND_ATTACH_RESPONSE         = 0x5,
    COMMAND_DATA_REQUEST            = 0x6,
    COMMAND_DATA_RESPONSE           = 0x7,
    COMMAND_SID_REQUEST             = 0x8,
    COMMAND_SID_RESPONSE            = 0x9,
    COMMAND_TOPOLOGY_MGMT           = 0xa,
    COMMAND_NETWORK_DATA_REQUEST    = 0xb,
    COMMAND_NETWORK_DATA_RESPONSE   = 0xc,
    COMMAND_ADDRESS_QUERY           = 0xd,
    COMMAND_ADDRESS_QUERY_RESPONSE  = 0xe,
    COMMAND_ADDRESS_NOTIFICATION    = 0xf,
    COMMAND_LINK_REQUEST            = 0x10,
    COMMAND_LINK_ACCEPT             = 0x11,
    COMMAND_LINK_ACCEPT_AND_REQUEST = 0x12,
    COMMAND_LINK_REJECT             = 0x13,
    COMMAND_DEST_UNREACHABLE        = 0x14,
    COMMAND_ADDRESS_ERROR           = 0x15,
    COMMAND_ROUTING_INFO_UPDATE     = 0x16,

    COMMAND_RAW_DATA                = 0x70,

    // diags command
    COMMAND_TRACE_ROUTE_REQUEST     = 0x71,
    COMMAND_TRACE_ROUTE_RESPONSE    = 0x72,
} mm_command_t;

enum {
    ATTACH_REQUEST_RETRY_TIMES   = 2,
    ATTACH_SID_RETRY_TIMES       = 2,
    BECOME_LEADER_TIMEOUT        = 3,
    MIGRATE_TIMEOUT              = 3,

#ifndef CONFIG_YOS_UT
    ATTACH_CANDIDATE_TIMEOUT     = 30,     /* 30 * ADVERTISEMENT_TIMEOUT */
#else
    ATTACH_CANDIDATE_TIMEOUT     = 0,
#endif
};

enum {
    SUPER_ROUTER_SID = 0,
    LEADER_SID  = 0,
    BCAST_SID   = 0xffff,
    INVALID_SID = 0xfffe,
};

enum {
    PATH_COST_WEIGHT       = 256,
    SIZE_WEIGHT            = 1,
    PATH_COST_SWITCH_HYST = 384,
};

typedef struct mesh_mgmt_header_s {
    uint8_t command;
} mm_header_t;

typedef struct mesh_network_info_s {
    uint8_t       stable_version;
    uint8_t       temp_version;
    uint16_t      netid;
    uint16_t      size;
    ur_ip6_addr_t mcast;
    uint16_t      timeout;
} mesh_network_info_t;

enum {
    ROUTING_ID_DEFAULT = 0,
};

typedef ur_error_t (* interface_up_t)(void);
typedef ur_error_t (* interface_down_t)(void);

typedef struct mm_cb_s {
    interface_up_t interface_up;
    interface_down_t interface_down;
} mm_cb_t;

ur_error_t mm_init(void);
ur_error_t mm_deinit(void);
ur_error_t mm_start(mm_cb_t *cb);
ur_error_t mm_stop(void);

mm_device_state_t mm_get_device_state(void);

ur_error_t            mm_set_mesh_prefix(ur_ip6_prefix_t *prefix);
const ur_ip6_prefix_t *mm_get_mesh_prefix(void);

uint16_t            mm_get_local_sid(void);
ur_error_t          mm_set_local_sid(uint16_t sid);
neighbor_t          *mm_get_attach_node(network_context_t *network);
neighbor_t          *mm_get_attach_candidate(network_context_t *network);
uint8_t             *mm_get_local_ueid(void);
mm_device_state_t   mm_get_device_state(void);
attach_state_t      mm_get_attach_state(void);
void                mm_set_meshnetid(network_context_t *network, uint16_t meshnetid);
uint16_t            mm_get_meshnetid(network_context_t *network);
uint16_t            mm_get_meshnetsize(void);
const mac_address_t *mm_get_mac_address(void);
node_mode_t         mm_get_mode(void);
uint16_t            mm_get_path_cost(void);
ur_error_t          mm_set_mode(node_mode_t mode);
uint16_t            mm_get_channel(network_context_t *network);
void                mm_set_channel(network_context_t *network, uint16_t channel);
ur_error_t          mm_set_seclevel(int8_t level);
int8_t              mm_get_seclevel(void);

void       mm_init_tlv_base(mm_tlv_t *tlv, uint8_t type, uint8_t length);
void       mm_init_tv_base(mm_tv_t *tlv, uint8_t type);
mm_tv_t    *mm_get_tv(const uint8_t *data, const uint16_t length, uint8_t type);
ur_error_t mm_handle_frame_received(message_t *message);

ur_error_t attach_start(hal_context_t *hal, neighbor_t *node);
void become_leader(void);
void become_detached(void);

ur_error_t send_raw_data(network_context_t *network,
                         ur_addr_t *dest, ur_addr_t *dest2,
                         uint8_t *payload, uint8_t payload_length);
ur_error_t register_raw_data_receiver(umesh_raw_data_received receiver);

uint16_t tlvs_set_value(uint8_t *buf, const uint8_t *tlvs,
                        uint8_t tlvs_length);
int16_t tlvs_calc_length(const uint8_t *tlvs, uint8_t tlvs_length);

void set_command_type(message_info_t *info, uint8_t command);

static inline uint16_t mm_get_main_netid(network_context_t *network)
{
    return get_main_netid(mm_get_meshnetid(network));
}

static inline uint16_t mm_get_sub_netid(network_context_t *network)
{
    return get_sub_netid(mm_get_meshnetid(network));
}

static inline bool is_bcast_sid(ur_addr_t *addr)
{
    if (addr->addr.len == SHORT_ADDR_SIZE && addr->addr.short_addr == BCAST_SID) {
        return true;
    }
    return false;
}

#endif  /* UR_MM_H */
