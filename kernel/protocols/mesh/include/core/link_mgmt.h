/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UR_LINK_MGMT_H
#define UR_LINK_MGMT_H

#include "core/mesh_mgmt.h"

enum {
    LINK_ESTIMATE_TIMES           = 4,
    LINK_ESTIMATE_SENT_THRESHOLD  = 4,
    LINK_ESTIMATE_COEF            = 256,
    LINK_ESTIMATE_UPDATE_ALPHA    = 32,
    LINK_COST_MAX                 = 1024,
    LINK_COST_THRESHOLD           = 384,
};

ur_error_t send_link_request(network_context_t *network, ur_addr_t *dest,
                             uint8_t *tlvs, uint8_t tlvs_length);

ur_error_t handle_link_request(message_t *message);
ur_error_t handle_link_accept_and_request(message_t *message);
ur_error_t handle_link_accept(message_t *message);

uint8_t insert_mesh_header_ies(network_context_t *network,
                               message_info_t *info, int16_t hdr_ies_limit);
ur_error_t handle_mesh_header_ies(message_t *message);

void start_neighbor_updater(void);
void stop_neighbor_updater(void);

typedef void (* neighbor_updated_t)(neighbor_t *nbr);
ur_error_t register_neighbor_updater(neighbor_updated_t updater);

void       neighbors_init(void);
neighbor_t *update_neighbor(const message_info_t *info,
                            uint8_t *tlvs, uint16_t length, bool is_attach);
void       set_state_to_neighbor(void);
neighbor_t *get_neighbor_by_mac_addr(const uint8_t *addr);
neighbor_t *get_neighbor_by_sid(hal_context_t *hal, uint16_t sid,
                                uint16_t meshnetid);
neighbor_t *get_neighbors(uint16_t *num);

#endif  /* UR_LINK_MGMT_H */
