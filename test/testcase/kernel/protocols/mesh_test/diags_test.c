/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <aos/kernel.h>
#include <yunit.h>

#include "core/mesh_mgmt.h"
#include "hal/interfaces.h"
#include "tools/diags.h"
#include "umesh_utils.h"

void test_diags_case(void)
{
    network_context_t *network;
    ur_addr_t          dest;
    message_t *        message;
    mm_header_t *      mm_header;
    mm_timestamp_tv_t *timestamp;
    message_info_t *   info;
    uint8_t *          data;
    uint8_t *          data_orig;
    uint16_t           length;

    int32_t               num;
    const ur_mem_stats_t *mem_stats = ur_mem_get_stats();

    num = mem_stats->num;

    interface_start();
    network              = get_default_network_context();
    dest.netid           = 0x100;
    dest.addr.len        = SHORT_ADDR_SIZE;
    dest.addr.short_addr = 0x1000;
    YUNIT_ASSERT(UR_ERROR_NONE == send_trace_route_request(network, &dest));

    length    = sizeof(mm_header_t) + sizeof(mm_timestamp_tv_t);
    data      = (uint8_t *)ur_mem_alloc(length);
    data_orig = data;
    data += sizeof(mm_header_t);

    timestamp = (mm_timestamp_tv_t *)data;
    umesh_mm_init_tv_base((mm_tv_t *)timestamp, TYPE_TIMESTAMP);
    timestamp->timestamp = 10;
    data += sizeof(mm_timestamp_tv_t);

    message =
      mf_build_message(MESH_FRAME_TYPE_CMD, COMMAND_TRACE_ROUTE_RESPONSE,
                       data_orig, length, UT_MSG);
    if (message == NULL) {
        ur_mem_free(data_orig, length);
        return;
    }
    info          = message->info;
    info->network = network;
    memcpy(&info->dest, &dest, sizeof(info->dest));

    YUNIT_ASSERT(UR_ERROR_NONE == handle_diags_command(message, true));
    message_free(message);
    ur_mem_free(data_orig, length);

    length    = sizeof(mm_header_t) + sizeof(mm_timestamp_tv_t);
    data      = (uint8_t *)ur_mem_alloc(length);
    data_orig = data;
    data += sizeof(mm_header_t);
    timestamp = (mm_timestamp_tv_t *)data;
    umesh_mm_init_tv_base((mm_tv_t *)timestamp, TYPE_TIMESTAMP);
    timestamp->timestamp = umesh_now_ms();
    data += sizeof(mm_timestamp_tv_t);

    message = mf_build_message(MESH_FRAME_TYPE_CMD, COMMAND_TRACE_ROUTE_REQUEST,
                               data_orig, length, UT_MSG);
    if (message == NULL) {
        ur_mem_free(data_orig, length);
        return;
    }
    info          = message->info;
    info->network = network;
    memcpy(&info->dest, &dest, sizeof(info->dest));

    YUNIT_ASSERT(UR_ERROR_NONE == handle_diags_command(message, true));
    message_free(message);
    ur_mem_free(data_orig, length);
    interface_stop();

    aos_msleep(1000);
    mem_stats = ur_mem_get_stats();
    YUNIT_ASSERT(num == mem_stats->num);
}
