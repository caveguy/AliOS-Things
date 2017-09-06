/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "yunit.h"

#include "yos/yos.h"
#include "yos/kernel.h"

#include "core/mesh_mgmt.h"

#include "dda_util.h"

extern void ur_ut_send_cmd_to_ddm(const char *cmd);
void topo_test_function(uint16_t first_node, uint16_t num, uint32_t timeout)
{
    uint16_t index;

    yos_msleep(2 * 1000);
    set_full_rssi(first_node, first_node + num - 1);

    for (index = 1; index < num; index++) {
        start_node_ext(index + first_node, MODE_RX_ON, -1, -1);
    }
    cmd_to_agent("mode FIXED");
    cmd_to_agent("start");
    check_cond_wait(num == umesh_mm_get_meshnetsize(), timeout);

    ur_ut_send_cmd_to_ddm("sendall sids");
    yos_msleep(2 * 1000);

    for (index = 1; index < num; index++) {
        stop_node(index + first_node);
    }
    cmd_to_agent("stop");
}

