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

#ifndef ZENOH_PICO_SYSTEM_LINK_WS_H
#define ZENOH_PICO_SYSTEM_LINK_WS_H

#include <stdint.h>

#include "zenoh-pico/collections/string.h"

#if Z_LINK_WS == 1


//TODO update for reading/writing via CBs
typedef struct {
    void *_sock;
    void *_raddr;
} _z_ws_socket_t;

void *_z_create_endpoint_ws(const char *s_addr, const char *port);
void _z_free_endpoint_ws(void *addr_arg);

void *_z_open_ws(void *raddr_arg, uint32_t tout);
void *_z_listen_ws(void *raddr_arg);
void _z_close_ws(void *sock_arg);
size_t _z_read_exact_ws(void *sock_arg, uint8_t *ptr, size_t len);
size_t _z_read_ws(void *sock_arg, uint8_t *ptr, size_t len);
size_t _z_send_ws(void *sock_arg, const uint8_t *ptr, size_t len);
#endif

#endif /* ZENOH_PICO_SYSTEM_LINK_WS_H */
