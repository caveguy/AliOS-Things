/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

/*
 * mesh hal impl for rockchip rk1108.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/if_packet.h>
#include <sys/ioctl.h>
#include <netinet/ether.h>
#include <pthread.h>
#include <signal.h>
#include <aos/aos.h>

#include <netinet/tcp.h>
#include <netinet/ip.h>

#include <cpu_event.h>
#include <umesh_hal.h>
#include <umesh_80211.h>

#define DEFAULT_MTU_SIZE 1024

typedef struct pseudo_header {
    unsigned int s_ip; // source ip
    unsigned int d_ip; // destination ip
    unsigned char mbz; // zero
    unsigned char proto; // protocol
    unsigned short plen; // length
}pseudo_header;

typedef struct udp_hdr {
    unsigned short s_port;
    unsigned short d_port;
    unsigned short length;
    unsigned short cksum;
}udp_hdr;

typedef struct {
    uint32_t u_mtu;
    uint32_t b_mtu;
    uint8_t channel;
    uint8_t chn_num;
    const uint8_t *channels;
    umesh_handle_received_frame_t rxcb;

    void *context;
    umesh_hal_module_t *module;
    unsigned char bssid[6];
    unsigned char macaddr[6];

    frame_stats_t stats;
} mesh_hal_priv_t;

typedef struct {
    frame_t frm;
    frame_info_t fino;
    mesh_hal_priv_t *priv;
} compound_msg_t;

unsigned short csum(unsigned short *ptr,int nbytes)
{
    register long sum;
    unsigned short oddbyte;
    register short checksum;

    sum=0;
    while (nbytes>1) {
        sum+=*ptr++;
        nbytes-=2;
    }

    if (nbytes==1) {
        oddbyte=0;
        *((u_char*)&oddbyte)=*(u_char*)ptr;
        sum+=oddbyte;
    }

    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    checksum=(short)~sum;

    return checksum;
}

static void linuxhost_mesh_recv(frame_t *frm, frame_info_t *fino, void *cb_data)
{
    mesh_hal_priv_t *priv = cb_data;
    uint8_t control;

    if (!priv->rxcb)
        return;

    priv->rxcb(priv->context, frm, fino, 0);
}

static void pass_to_urmesh(const void *arg)
{
    compound_msg_t *cmsg = (compound_msg_t *)arg;

    linuxhost_mesh_recv(&cmsg->frm, &cmsg->fino, cmsg->priv);

    cpu_event_free(cmsg);
}

#define MESH_DATA_OFF 32
#define MESH_SSID_OFF 58
#define MESH_PAYLOAD_OFF 74
#define MESH_SRC_OFF 52
static void *wifi_recv_entry(void *arg)
{
    mesh_hal_priv_t *priv = (mesh_hal_priv_t *)arg;
    int NumTotalPkts;
    int sockfd;
    int sockopt;
    ssize_t numbytes;
    struct ifreq ifopts;
    uint8_t buf[2048];
    umesh_extnetid_t extnetid;
    const mac_address_t *mymac;
    uint8_t bcast_mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    hal_umesh_get_extnetid(priv->module, &extnetid);
    mymac = hal_umesh_get_mac_address(priv->module);

    memset(buf, 0, sizeof(buf));

    // create a raw socket
    if ((sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
        printf("listener: socket error\n");
        return NULL;
    }

    // Set interface to promiscuous mode
    strncpy(ifopts.ifr_name, "wlan0", IFNAMSIZ-1);
    ioctl(sockfd, SIOCGIFFLAGS, &ifopts);

    ifopts.ifr_flags |= IFF_PROMISC;
    ioctl(sockfd, SIOCSIFFLAGS, &ifopts);
    /* Allow the socket to be reused - incase connection is closed prematurely */
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof sockopt) == -1) {
        printf("setsockopt\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Bind to device
    if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, "wlan0", IFNAMSIZ-1) == -1)  {
        printf("SO_BINDTODEVICE\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    NumTotalPkts=0;

    while (1) {
        numbytes = recvfrom(sockfd, buf, 2048, 0, NULL, NULL);
        //printf("listener: got packet %d bytes , total packet Num :%d \n", numbytes, NumTotalPkts);
        if (numbytes < 0) {
            printf("Recvfrom error , failed to get packets\n");
            goto thr_exit;
        }

        // filter for umesh
        if (numbytes <= MESH_PAYLOAD_OFF) {
            continue;
        }

        if (memcmp(buf, bcast_mac, 6)) {
            continue;
        }

        if (memcmp(buf + MESH_SRC_OFF, mymac->addr, 6) == 0) {
            continue;
        }

        if (memcmp(buf + MESH_SSID_OFF, extnetid.netid, 6)) {
            continue;
        }

#if 0
        printf("process the packet: %d\n", numbytes);
        for (int i = 0; i < numbytes; i++)
            printf("%02x:", buf[i]);
        printf("\n");
#endif

        NumTotalPkts++;
        compound_msg_t *pf;
        pf = cpu_event_malloc(sizeof(*pf) + numbytes - MESH_PAYLOAD_OFF);
        bzero(pf, sizeof(*pf));
        pf->frm.len = numbytes - MESH_PAYLOAD_OFF;
        pf->frm.data = (void *)(pf + 1);
        pf->fino.channel = priv->channel;
        memcpy(pf->fino.peer.addr, buf + MESH_SRC_OFF, 6);
        pf->fino.peer.len = 8;
        pf->priv = priv;
        memcpy(pf->frm.data, buf + MESH_PAYLOAD_OFF, pf->frm.len);
        cpu_call_handler(pass_to_urmesh, pf);
    }

thr_exit:
    close(sockfd);
    return NULL;
}

static int linux_80211_mesh_init(umesh_hal_module_t *module, void *something)
{
    int sockfd;
    mesh_hal_priv_t *priv = module->base.priv_dev;

    // create socket to fetch mac addr
    if ((sockfd = socket(PF_PACKET, SOCK_RAW, IPPROTO_RAW)) == -1) {
        printf("send socket fail\n");
        return -1;
    }

    // get interface mac addr
    struct ifreq ifreq_c;
    memset(&ifreq_c, 0, sizeof(ifreq_c));
    strncpy(ifreq_c.ifr_name, "wlan0", IFNAMSIZ-1); // give name of Interface
    if ((ioctl(sockfd, SIOCGIFHWADDR, &ifreq_c)) < 0) { // get MAC Address
        printf("get interface mac addr error\n");
    }
    memcpy(priv->macaddr, ifreq_c.ifr_ifru.ifru_hwaddr.sa_data, 6);

    pthread_t th;
    pthread_create(&th, NULL, wifi_recv_entry, priv);

    printf("wifi_recv_entry Thread created successfully\n");
    close(sockfd);
    return 0;
}

static int send_frame(umesh_hal_module_t *module, frame_t *frame, mac_address_t *dest)
{
    void *pkt = NULL;
    int count = frame->len + MESH_DATA_OFF;
    int ret;
    int sockfd;
    char datagram[2048];
    char source_ip[32];
    char *pseudogram;

    memset(datagram, 0, 2048);

    // create socket
    if ((sockfd = socket(PF_INET, SOCK_RAW, IPPROTO_UDP)) == -1) {
        printf("send socket fail\n");
        return -1;
    }

    pkt = aos_malloc(count);
    if (pkt == NULL) {
        ret = -1;
        goto out;
    }

    ret = umesh_80211_make_frame(module, frame, dest, pkt);
    if (ret < 0) {
        goto out;
    }

    mesh_hal_priv_t *priv = module->base.priv_dev;

    // IP header
    struct iphdr *iph = (struct iphdr *)datagram;

    // UDP header
    udp_hdr *udphdr = (udp_hdr *)(datagram + sizeof(struct ip));
    struct sockaddr_in sin;

    // umesh packet
    memcpy((datagram + sizeof(struct iphdr) + sizeof(struct udp_hdr)), pkt, count);

    // address resolution
    struct ifreq ifreq_ip;
    memset(&ifreq_ip, 0, sizeof(ifreq_ip));
    strncpy(ifreq_ip.ifr_name, "wlan0", IFNAMSIZ-1); // give name of Interface
    if (ioctl(sockfd, SIOCGIFADDR, &ifreq_ip) < 0) { // get ipv4 address
        printf("error in SIOCGIFADDR\n");
    }

    strcpy(source_ip, inet_ntoa(((struct sockaddr_in*)&(ifreq_ip.ifr_ifru.ifru_addr))->sin_addr));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(80);
    sin.sin_addr.s_addr = inet_addr("255.255.255.255");

    // fill in the ip header
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof(struct iphdr) + sizeof(struct udp_hdr) + count;
    iph->id = htonl (54321); // id of this packet
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = IPPROTO_UDP;
    iph->check = 0; // set to 0 before calculating checksum
    iph->saddr = inet_addr(source_ip); // spoof the source ip address
    iph->daddr = sin.sin_addr.s_addr;

    // IP checksum
    iph->check = csum((unsigned short *)datagram, iph->tot_len);

    // UDP header
    udphdr->s_port = htons(80);
    udphdr->d_port = htons(1024);
    udphdr->length = htons(iph->tot_len - sizeof(struct ip));
    udphdr->cksum = 0;

    // UDP checksum
    pseudo_header psd;
    psd.s_ip = iph->saddr;
    psd.d_ip = iph->daddr;
    psd.mbz = 0;
    psd.proto = 0x11;
    psd.plen = udphdr->length;

    int psize = sizeof(psd) + ntohs(udphdr->length);
    pseudogram = aos_malloc(psize);
    if (pseudogram == NULL) {
        printf("pseudogram is null\n");
        ret = -1;
        goto out;
    }

    memcpy(pseudogram, &psd, sizeof(psd));
    memcpy(pseudogram+sizeof(psd), udphdr, sizeof(struct udp_hdr) + count);
    udphdr->cksum = csum((unsigned short*)pseudogram, psize);

    // IP_HDRINCL to tell the kernel that headers are included in the packet
    int one = 1;
    const int *val = &one;

    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
        printf("Error setting IP_HDRINCL\n");
        ret = -1;
        goto out;
    }

    // tell the kernel that allow broadcast transmission
    int broadcast = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) == -1) {
        printf("setsockopt (SO_BROADCAST)\n");
        ret = -1;
        goto out;
    }

    // send the packet
    if (sendto(sockfd, datagram, iph->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        printf("sendto failed\n");
        ret = -1;
        goto out;
    }
    // Data send successfully
    else
    {
        //printf("Packet Send. Length : %d \n" , iph->tot_len);
    }

    priv->stats.out_frames++;

out:
    aos_free(pkt);
    aos_free(pseudogram);
    close(sockfd);
    return ret;
}

static int linux_80211_mesh_send_ucast(umesh_hal_module_t *module, frame_t *frame,
                                       mac_address_t *dest,
                                       umesh_handle_sent_ucast_t sent,
                                       void *context)
{
    int error;
    mesh_hal_priv_t *priv = module->base.priv_dev;

    if (frame == NULL) {
        printf("frame is NULL, cannot proceed\n");
        return -1;
    }

    if (frame->len > priv->u_mtu) {
        printf("frame->len(%d) > MAX_FRAME_SIZE(%d), will not proceed\n", frame->len, priv->u_mtu);
        return -2;
    }

    frame->key_index = -1;
    error = send_frame(module, frame, dest);
    if (sent) {
        (*sent)(context, frame, error);
    }
    return error;
}

static int linux_80211_mesh_send_bcast(umesh_hal_module_t *module, frame_t *frame,
                                   umesh_handle_sent_bcast_t sent,
                                   void *context)
{
    int error;
    mesh_hal_priv_t *priv = module->base.priv_dev;
    mac_address_t dest;

    if (frame == NULL) {
        printf("frame is NULL, cannot proceed\n");
        return -1;
    }

    if (frame->len > priv->b_mtu) {
        printf("frame->len(%d) > MAX_FRAME_SIZE(%d), will not proceed\n", frame->len, priv->u_mtu);
        return -2;
    }

    dest.len = 8;
    memset(dest.addr, 0xff, sizeof(dest.addr));
    error = send_frame(module, frame, &dest);
    if (sent) {
        (*sent)(context, frame, error);
    }
    return error;
}

static int linux_80211_mesh_get_u_mtu(umesh_hal_module_t *module)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;
    return priv->u_mtu;
}

static int linux_80211_mesh_get_b_mtu(umesh_hal_module_t *module)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;
    return priv->b_mtu;
}

static int linux_80211_mesh_set_rxcb(umesh_hal_module_t *module,
                          umesh_handle_received_frame_t received, void *context)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;

    if (received == NULL) {
        return -1;
    }

    priv->rxcb = received;
    priv->context = context;
    return 0;
}

static const mac_address_t *linux_80211_mesh_get_mac_address(
                                        umesh_hal_module_t *module)
{
    static mac_address_t addr;
    mesh_hal_priv_t *priv = module->base.priv_dev;

    memcpy(addr.addr, priv->macaddr, 6);
    addr.len = 8;
    return &addr;
}

static int linux_80211_mesh_hal_get_channel(umesh_hal_module_t *module)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;
    return priv->channel;
}

static int linux_80211_mesh_hal_set_channel(umesh_hal_module_t *module, uint8_t channel)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;

    /* channel will appended to each data packet sent */
    priv->channel = channel;
    return 0;
}

static int linux_80211_mesh_get_channel_list(umesh_hal_module_t *module, const uint8_t **chnlist)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;
    if (chnlist == NULL) {
        return -1;
    }

    *chnlist = priv->channels;

    return priv->chn_num;
}

static const frame_stats_t *linux_80211_mesh_get_stats(struct umesh_hal_module_s *module)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;
    return &priv->stats;
}

static int linux_80211_wifi_mesh_set_extnetid(umesh_hal_module_t *module,
                                 const umesh_extnetid_t *extnetid)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;

    memcpy(priv->bssid, extnetid->netid, 6);
    return 0;
}

static void linux_80211_wifi_mesh_get_extnetid(umesh_hal_module_t *module,
                                  umesh_extnetid_t *extnetid)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;

    if (extnetid == NULL) {
        return;
    }
    extnetid->len = 6;
    memcpy(extnetid->netid, priv->bssid, extnetid->len);
}

static umesh_hal_module_t linux_80211_mesh_wifi_module;
static const uint8_t g_wifi_channels[] = {1, 6, 11, 2, 3, 4, 5, 7, 8, 9, 10, 12, 13};
static mesh_hal_priv_t wifi_priv = {
    .u_mtu = DEFAULT_MTU_SIZE,
    .b_mtu = DEFAULT_MTU_SIZE,
    .channel = 1,
    .chn_num = sizeof(g_wifi_channels),
    .channels = g_wifi_channels,
    .module = &linux_80211_mesh_wifi_module,
    .bssid = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5},
};

static umesh_hal_module_t linux_80211_mesh_wifi_module = {
    .base.name = "rtl_80211_mesh_wifi_module",
    .base.priv_dev = &wifi_priv,
    .type = MEDIA_TYPE_WIFI,
    .umesh_hal_init = linux_80211_mesh_init,
    .umesh_hal_send_ucast_request = linux_80211_mesh_send_ucast,
    .umesh_hal_send_bcast_request = linux_80211_mesh_send_bcast,
    .umesh_hal_register_receiver = linux_80211_mesh_set_rxcb,
    .umesh_hal_get_bcast_mtu = linux_80211_mesh_get_b_mtu,
    .umesh_hal_get_ucast_mtu = linux_80211_mesh_get_u_mtu,
    .umesh_hal_get_mac_address = linux_80211_mesh_get_mac_address,
    .umesh_hal_get_channel = linux_80211_mesh_hal_get_channel,
    .umesh_hal_set_channel = linux_80211_mesh_hal_set_channel,
    .umesh_hal_get_chnlist = linux_80211_mesh_get_channel_list,
    .umesh_hal_get_stats = linux_80211_mesh_get_stats,
    .umesh_hal_set_extnetid = linux_80211_wifi_mesh_set_extnetid,
    .umesh_hal_get_extnetid = linux_80211_wifi_mesh_get_extnetid,
};

void linux_wifi_register(void)
{
    hal_umesh_register_module(&linux_80211_mesh_wifi_module);
}
