#ifndef _SAL_H_
#define _SAL_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    /* WiFi */
    TCP_SERVER,
    TCP_CLIENT,
    SSL_CLIENT,
    UDP_BROADCAST,
    UDP_UNICAST,
    /*WiFi end */
    /* Add others hereafter */
} CONN_TYPE;

/* Fill necessary fileds according to the socket type. */
typedef struct {
    int fd; /* fd that are used in socket level */
    CONN_TYPE type;
    char *addr; /* remote ip or domain */
    int32_t r_port; /* remote port (set to -1 if not used) */
    int32_t l_port; /* local port (set to -1 if not used) */
    uint32_t tcp_keep_alive; /* tcp keep alive value (set to 0 if not used) */
} at_conn_t;

typedef enum netconn_evt {
    NETCONN_EVT_RCVPLUS,
    NETCONN_EVT_RCVMINUS,
    NETCONN_EVT_SENDPLUS,
    NETCONN_EVT_SENDMINUS,
    NETCONN_EVT_ERROR
} netconn_evt_t;

typedef void (*netconn_evt_cb_t)(int s, enum netconn_evt evt);

typedef struct sal_op_s {
    char *version; /* Reserved for furture use. */

    /**
     * Init SAL and call low level AT init.
     *
     * @return  0 - success, -1 - failure
     */
    int (*init)(void);

    /**
     * Start a socket connection via AT.
     *
     * @param[in]  c - connect parameters which are used to setup
     *                 the socket connection.
     *
     * @return  0 - success, -1 - failure
     */
    int (*start)(at_conn_t *c);

    /**
     * Send data via AT.
     * This function does not return until all data sent.
     *
     * @param[in]  fd - the file descripter to operate on.
     * @param[in]  data - pointer to data to send.
     * @param[in]  len - length of the data.
     * @param[in]  remote_ip - remote port number.
     * @param[in]  remote_port - remote port number.
     *
     * @return  0 - success, -1 - failure
     */
    int (*send)(int fd, uint8_t *data, uint32_t len,
                char remote_ip[16], int32_t remote_port);

    /**
     * Receive data from AT.
     * This function returns either because expected length data read,
     * or no more data left. This OUT "len" reflects the real length read.
     * The caller is epected to check the returned len.
     *
     * @param[in]   fd - the file descripter to operate on.
     * @param[out]  data - pointer to data to send.
     * @param[out]  len - expected length of the data when IN,
     *                    and real read len when OUT.
     * @param[out]  remote_ip - remote ip address. Caller manages the memory.
     * @param[out]  remote_port - remote port number.
     *
     * @return  0 - success, -1 - failure
     */
    int (*recv)(int fd, uint8_t *buf, uint32_t *len,
                char remote_ip[16], int32_t remote_port);

    /**
     * Get IP information of the corresponding domain.
     * Currently only one IP string is returned.
     *
     * @param[in]   domain - the domain string.
     * @param[out]  ip - the place to hold the dot-formatted ip string.
     *
     * @return  0 - success, -1 - failure
     */
    int (*domain_to_ip)(char *domain, char ip[16]);

    /**
     * Close the socket connection.
     *
     * @param[in]  fd - the file descripter to operate on.
     * @param[in]  remote_port - remote port number.
     *
     * @return  0 - success, -1 - failure
     */
    int (*close)(int fd, int32_t remote_port);

    /**
     * Destroy SAL or exit low level state if necessary.
     *
     * @return  0 - success, -1 - failure
     */
    int (*deinit)(void);

    /**
     * Register network connection event callback function.
     */
    int (*register_netconn_evt_cb)(netconn_evt_cb_t cb);
} sal_op_t;

/* Define it in platform code. */
extern sal_op_t sal_op;

#ifdef __cplusplus
}
#endif

#endif
