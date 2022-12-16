//
// Copyright (c) 2022 ZettaScale Technology
//
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// http://www.eclipse.org/legal/epl-2.0, or the Apache License, Version 2.0
// which is available at https://www.apache.org/licenses/LICENSE-2.0.
//
// SPDX-License-Identifier: EPL-2.0 OR Apache-2.0
//
// Contributors:
//   ZettaScale Zenoh Team, <zenoh@zettascale.tech>
//

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#include <emscripten/websocket.h>



#include "zenoh-pico/collections/string.h"
#include "zenoh-pico/config.h"
#include "zenoh-pico/system/platform.h"
#include "zenoh-pico/utils/logging.h"


#if Z_LINK_WS == 1

typedef struct {
    int _fd;
} __z_ws_socket;



/*------------------ WS sockets ------------------*/
void *_z_create_endpoint_ws(const char *s_addr, const char *port) {

    struct addrinfo hints;
    struct addrinfo *addr = NULL;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_UNSPEC;  // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = IPPROTO_TCP;


    if (getaddrinfo(s_addr, port, &hints, &addr) < 0) return NULL;

    return addr;
}

void _z_free_endpoint_ws(void *arg) {

    struct addrinfo *self = (struct addrinfo *)arg;
    freeaddrinfo(self);

}

/*------------------ WS sockets ------------------*/
void *_z_open_ws(void *arg, uint32_t tout) {

    char addrstr[100];


     __z_ws_socket *ret = (__z_ws_socket *)z_malloc(sizeof(__z_ws_socket));
    struct addrinfo *raddr = (struct addrinfo *)arg;

    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(7887);

    int sock = socket(raddr->ai_family, raddr->ai_socktype,  raddr->ai_protocol);
    if (sock < 0) goto _Z_OPEN_TCP_ERROR_1;

    // WARNING: commented because setsockopt is not implemented in emscripten
    // z_time_t tv;
    // tv = 1000;
    // if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv)) < 0) goto _Z_OPEN_TCP_ERROR_2;

    struct addrinfo *it = NULL;
    for (it = raddr; it != NULL; it = it->ai_next) {
        if (connect(sock, it->ai_addr, it->ai_addrlen) < 0) {
            break;
            // WARNING: breaking here because connect returns -1 even if the
            // underlying socket is actually open.

            // if (it->ai_next == NULL) goto _Z_OPEN_TCP_ERROR_2;
        } else
            break;
    }


    z_sleep_ms(100);

    ret->_fd = sock;
    return ret;

_Z_OPEN_TCP_ERROR_2:
    close(sock);
    return NULL;

_Z_OPEN_TCP_ERROR_1:
    z_free(ret);
    return NULL;

}

void *_z_listen_ws(void *arg) {
    struct addrinfo *laddr = (struct addrinfo *)arg;
    (void)laddr;

    // @TODO: To be implemented

    return NULL;
}

void _z_close_ws(void *sock_arg) {
    __z_ws_socket *sock = (__z_ws_socket *) sock_arg;
    if (sock == NULL) return;
    close(sock->_fd);
    z_free(sock);

}

size_t _z_read_ws(void *sock_arg, uint8_t *ptr, size_t len) {
    // TODO: replace busy loops with timeouts.
    __z_ws_socket *sock = (__z_ws_socket *) sock_arg;

    int c = 0;
    int rb = 0;
    do {
        rb = recv(sock->_fd, ptr, len, 0);
        c++;
        z_sleep_ms(500);
    } while(rb<=0 && c < 500); //try 500 times

    return rb;
}

size_t _z_read_exact_ws(void *sock_arg, uint8_t *ptr, size_t len) {
    size_t n = len;
    size_t rb = 0;

    do {
        rb = _z_read_ws(sock_arg, ptr, n);
        if (rb == SIZE_MAX) return rb;

        n -= rb;
        ptr = ptr + (len - n);
    } while (n > 0);

    return len;
}

size_t _z_send_ws(void *sock_arg, const uint8_t *ptr, size_t len) {
    // TODO: replace busy loops with timeouts.

    __z_ws_socket *sock = (__z_ws_socket *) sock_arg;
    int c = 0;
    int res =  0;
    do {
        res = send(sock->_fd, ptr, len, 0);
        c++;
        z_sleep_ms(500);
    } while(res <= 0 && c < 500); // just try 500 times.
    return res;
}

#endif

#if Z_LINK_TCP == 1
#error "TCP not supported yet on emscripten port of Zenoh-Pico"
#endif

#if Z_LINK_UDP_UNICAST == 1 || Z_LINK_UDP_MULTICAST == 1
#error "UDP not supported yet on emscripten port of Zenoh-Pico"
#endif

#if Z_LINK_BLUETOOTH == 1
#error "Bluetooth not supported yet on emscripten port of Zenoh-Pico"
#endif

#if Z_LINK_SERIAL == 1
#error "SERIAL not supported yet on emscripten port of Zenoh-Pico"
#endif