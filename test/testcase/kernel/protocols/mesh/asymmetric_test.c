#include "yunit.h"

#include "yos/framework.h"
#include "yos/kernel.h"

#include "umesh.h"
#include "core/link_mgmt.h"
#include "core/sid_allocator.h"
#include "core/router_mgr.h"
#include "utilities/logging.h"
#include "utilities/encoding.h"
#include "cli/cli.h"

#include "dda_util.h"

static void two_nodes_case(void)
{
    uint8_t index;

    /* topology:
     *   leader     leader
     *   11 <------ 12
     */

    set_rssi_ext(IF_WIFI, 11, 12, 0, 1);

    ur_mesh_set_mode(MODE_RX_ON);
    cmd_to_agent("start");
    check_cond_wait((DEVICE_STATE_LEADER == mm_get_device_state()), 15);
    YUNIT_ASSERT(ur_router_get_default_router() == SID_ROUTER);

    start_node_ext(12, MODE_RX_ON, -1, -1);
    check_p2p_str_wait("leader", 12, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 12, "testcmd router", 2);

    for (index = 0; index < 10; index++) {
        yos_msleep(5* 1000);
        YUNIT_ASSERT(DEVICE_STATE_LEADER == ur_mesh_get_device_state());
    }

    stop_node(12);
    cmd_to_agent("stop");
}

static void three_nodes_case(void)
{
    uint8_t index;
    char    index_str[3];
    char ping_cmd[64];
    const ur_netif_ip6_address_t *myaddr;

    /* topology:
     *   leader     router      router
     *   13 <------> 12  <----->
     *    |                      11
     *    |-------------------->
     *
     */

    set_rssi_ext(IF_WIFI, 13, 12, 1, 1);
    set_rssi_ext(IF_WIFI, 12, 11, 1, 1);
    set_rssi_ext(IF_WIFI, 13, 11, 1, 0);

    start_node_ext(13, MODE_RX_ON, -1, -1);
    check_p2p_str_wait("leader", 13, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 13, "testcmd router", 2);

    start_node_ext(12, MODE_RX_ON, -1, -1);
    check_p2p_str_wait("router", 12, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 12, "testcmd router", 2);

    ur_mesh_set_mode(MODE_RX_ON);
    cmd_to_agent("start");
    check_cond_wait((DEVICE_STATE_ROUTER == mm_get_device_state()), 15);
    YUNIT_ASSERT(ur_router_get_default_router() == SID_ROUTER);

    myaddr = ur_mesh_get_ucast_addr();
    snprintf(ping_cmd, sizeof ping_cmd, "send 12 ping " IP6_ADDR_FMT, IP6_ADDR_DATA(myaddr->addr));

    for (index = 0; index < 5; index++) {
        cmd_to_master(ping_cmd);
        snprintf(index_str, sizeof(index_str), "%d", index + 1);
        check_p2p_str_wait(index_str, 12, "testcmd icmp_acked", 5);
        yos_msleep(5 * 1000);
    }

    stop_node(12);
    stop_node(13);
    cmd_to_agent("stop");
}


void test_uradar_asymmetric_link_case(void)
{
    run_times(two_nodes_case(), 1);
    run_times(three_nodes_case(), 1);
}
