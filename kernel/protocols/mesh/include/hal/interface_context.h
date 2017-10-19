/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UR_INTERFACE_CONTEXT_H
#define UR_INTERFACE_CONTEXT_H

#include "umesh_hal.h"
#include "umesh_types.h"
#include "core/topology.h"
#include "core/sid_allocator.h"
#include "utilities/message.h"
#include "utilities/timer.h"

typedef enum interface_state_s {
    INTERFACE_UP,
    INTERFACE_DOWN,
} interface_state_t;

typedef enum attach_state_s {
    ATTACH_IDLE,
    ATTACH_REQUEST,
    ATTACH_SID_REQUEST,
    ATTACH_DONE,
} attach_state_t;

typedef struct channel_list_s {
    const uint8_t *channels;
    uint8_t num;
} channel_list_t;

typedef struct scan_result_s {
    slist_t       next;
    mac_address_t addr;
    uint16_t      meshnetid;
    uint8_t       channel;
    int8_t        rssi;
    node_mode_t   leader_mode;
    uint16_t      net_size;
} scan_result_t;

typedef void (* discovered_handler_t)(neighbor_t *nbr);

typedef struct network_data_s {
    uint8_t  version;
    uint16_t size;
} network_data_t;

enum {
    MCAST_CACHE_ENTRIES_SIZE = 32,
};

typedef struct mcast_entry_s {
    uint8_t subnetid;
    uint16_t sid;
    uint8_t  sequence;
    uint8_t  lifetime;
} mcast_entry_t;

enum {
    CMD_QUEUE,
    DATA_QUEUE,
    PENDING_QUEUE,
    QUEUE_SIZE = 3,
};

enum {
    EVENTS_NUM = 2,
};

typedef struct router_cb_s {
    ur_error_t (*start)(void);
    ur_error_t (*stop)(void);
    ur_error_t (*handle_neighbor_updated)(neighbor_t *);
    ur_error_t (*handle_message_received)(const uint8_t *data,
                                          uint16_t length);
    ur_error_t (*handle_subscribe_event)(uint8_t event, uint8_t *data,
                                         uint8_t len);
    uint16_t   (*get_next_hop_sid)(uint16_t dest_sid);
} router_cb;

typedef struct subscribe_events_s {
    uint8_t events[EVENTS_NUM];
    uint8_t num;
} subscribe_events_t;

struct network_context_s;
typedef struct router_s {
    uint8_t            id;
    uint8_t            sid_type;
    router_cb          cb;
    subscribe_events_t events;
    slist_t            next;
    struct network_context_s *network;
} router_t;

typedef struct frag_info_s {
    uint16_t tag;
    uint16_t offset;
} frag_info_t;

typedef struct hal_context_s {
    slist_t              next;
    umesh_hal_module_t *module;
    channel_list_t       channel_list;
    uint8_t              def_channel;
    mac_address_t        mac_addr;

    // queue
    message_queue_t      send_queue[QUEUE_SIZE];
    message_queue_t      recv_queue;
    ur_timer_t           sending_timer;
    message_t            *send_message;
    frag_info_t          frag_info;
    frame_t              frame;
    ur_link_stats_t      link_stats;

    // neighbors
    slist_t               neighbors_list;
    uint8_t               neighbors_num;
    ur_timer_t            update_nbr_timer;
    ur_timer_t            link_quality_update_timer;
    ur_timer_t            link_request_timer;

    // discovery
    uint8_t               discovery_channel;
    uint8_t               discovery_times;
    uint8_t               discovery_timeouts;
    ur_timer_t            discovery_timer;
    scan_result_t         discovery_result;
    discovered_handler_t  discovered_handler;

    // hal configurations
    uint32_t              discovery_interval;
    uint32_t              attach_request_interval;
    uint32_t              sid_request_interval;
    uint32_t              link_request_interval;
    uint32_t              link_request_mobile_interval;
    uint32_t              neighbor_alive_interval;
    uint32_t              advertisement_interval;

    int                   last_sent;  // 0 success, -1 fail
} hal_context_t;

typedef struct network_context_s {
    slist_t           next;
    uint8_t           index;
    uint8_t           flags;

    hal_context_t     *hal;

    interface_state_t state;
    // attach
    attach_state_t    attach_state;
    uint16_t          path_cost;
    uint16_t          sid;
    uint16_t          channel;
    uint16_t          meshnetid;
    uint16_t          candidate_meshnetid;
    uint8_t           *one_time_key;

    neighbor_t        *attach_node;
    neighbor_t        *attach_candidate;
    uint8_t           retry_times;
    uint8_t           leader_times;
    uint8_t           migrate_times;
    ur_timer_t        attach_timer;
    ur_timer_t        advertisement_timer;
    ur_timer_t        migrate_wait_timer;
    uint32_t          migrate_interval;
    uint32_t          notification_interval;
    uint32_t          net_scan_interval;
    uint16_t          prev_netid;
    uint16_t          prev_path_cost;

    // network data
    network_data_t    network_data;

    // sid
    allocator_t       sid_base;

    // mcast
    uint8_t           mcast_sequence;
    ur_timer_t        mcast_timer;
    mcast_entry_t     mcast_entry[MCAST_CACHE_ENTRIES_SIZE];

    // routing
    router_t          *router;
} network_context_t;

#endif  /* UR_INTERFACE_CONTEXT_H */
