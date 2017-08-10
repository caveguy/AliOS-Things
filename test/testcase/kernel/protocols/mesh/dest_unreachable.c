#include "yunit.h"

#include "yos/framework.h"
#include "yos/kernel.h"

#include "umesh.h"
#include "core/link_mgmt.h"
#include "core/sid_allocator.h"
#include "core/router_mgr.h"
#include "utilities/logging.h"
#include "hal/hals.h"
#include "tools/cli.h"

#include "dda_util.h"

static void one_layer_case(void)
{
    char ping_cmd[64];
    const ur_netif_ip6_address_t *myaddr;

    /* topology:
     *   leader     router   router   mobile
     *   11 <------> 12 <---> 13 <---> 14
     */
    set_line_rssi(11, 14);

    umesh_set_mode(MODE_RX_ON);
    cmd_to_agent("start");
    check_cond_wait((DEVICE_STATE_LEADER == umesh_mm_get_device_state()), 15);
    YUNIT_ASSERT(ur_router_get_default_router() == SID_ROUTER);

    start_node_ext(12, MODE_RX_ON, -1, -1);
    check_p2p_str_wait("router", 12, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 12, "testcmd router", 2);

    start_node_ext(13, MODE_RX_ON, -1, -1);
    check_p2p_str_wait("router", 13, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 13, "testcmd router", 2);

    start_node_ext(14, MODE_MOBILE, -1, -1);
    check_p2p_str_wait("leaf", 14, "testcmd state", 10);
    check_p2p_str_wait("SID_ROUTER", 14, "testcmd router", 2);

    myaddr = umesh_get_ucast_addr();
    snprintf(ping_cmd, sizeof ping_cmd, "send 14 ping " IP6_ADDR_FMT, IP6_ADDR_DATA(myaddr->addr));
    cmd_to_master(ping_cmd);
    check_p2p_str_wait("1", 14, "testcmd icmp_acked", 5);

    /* topology:
     *   leader     router   router
     *   11 <------> 12 <---> 13
     *               |       mobile
     *               |  <---> 14
     */

    set_rssi_ext(IF_WIFI, 13, 14, 0, 0);
    set_rssi_ext(IF_WIFI, 12, 14, 1, 1);

    yos_msleep(WIFI_NEIGHBOR_ALIVE_TIMEOUT + WIFI_ADVERTISEMENT_TIMEOUT);
    myaddr = umesh_get_ucast_addr();
    snprintf(ping_cmd, sizeof ping_cmd, "send 14 ping " IP6_ADDR_FMT, IP6_ADDR_DATA(myaddr->addr));
    cmd_to_master(ping_cmd);
    snprintf(ping_cmd, sizeof ping_cmd, "send 14 ping " IP6_ADDR_FMT, IP6_ADDR_DATA(myaddr->addr));
    yos_msleep(WIFI_ADVERTISEMENT_TIMEOUT);
    cmd_to_master(ping_cmd);
    check_p2p_str_wait("2", 14, "testcmd icmp_acked", 10);

    stop_node(12);
    stop_node(13);
    stop_node(14);
    cmd_to_agent("stop");
}


void test_uradar_dest_become_unreachable_case(void)
{
    run_times(one_layer_case(), 1);
}
