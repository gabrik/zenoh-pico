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

#include "zenoh-pico/link/config/ws.h"

#include <stddef.h>
#include <string.h>

#include "zenoh-pico/config.h"
#include "zenoh-pico/link/manager.h"
#include "zenoh-pico/system/link/ws.h"

#if Z_LINK_WS == 1

char *_z_parse_port_segment_ws(char *address) {
    char *p_start = strrchr(address, ':');
    if (p_start == NULL) return NULL;
    p_start++;

    char *p_end = &address[strlen(address)];

    int len = p_end - p_start;
    char *port = (char *)z_malloc((len + 1) * sizeof(char));
    strncpy(port, p_start, len);
    port[len] = '\0';

    return port;
}

char *_z_parse_address_segment_ws(char *address) {
    char *p_start = &address[0];
    char *p_end = strrchr(address, ':');

    // IPv6
    if (*p_start == '[' && *(p_end - 1) == ']') {
        p_start++;
        p_end--;
        int len = p_end - p_start;
        char *ip6_addr = (char *)z_malloc((len + 1) * sizeof(char));
        strncpy(ip6_addr, p_start, len);
        ip6_addr[len] = '\0';

        return ip6_addr;
    }
    // IPv4
    else {
        int len = p_end - p_start;
        char *ip4_addr_or_domain = (char *)z_malloc((len + 1) * sizeof(char));
        strncpy(ip4_addr_or_domain, p_start, len);
        ip4_addr_or_domain[len] = '\0';

        return ip4_addr_or_domain;
    }

    return NULL;
}

int _z_f_link_open_ws(void *arg) {
    _z_link_t *self = (_z_link_t *)arg;

    uint32_t tout = Z_CONFIG_SOCKET_TIMEOUT;
    char *tout_as_str = _z_str_intmap_get(&self->_endpoint._config, WS_CONFIG_TOUT_KEY);
    if (tout_as_str != NULL) tout = strtoul(tout_as_str, NULL, 10);

    self->_socket._ws._sock = _z_open_ws(self->_socket._ws._raddr, tout);
    if (self->_socket._ws._sock == NULL) goto ERR;
    return 0;

ERR:
    return -1;
}

int _z_f_link_listen_ws(void *arg) {
    _z_link_t *self = (_z_link_t *)arg;

    self->_socket._ws._sock = _z_listen_ws(self->_socket._ws._raddr);
    if (self->_socket._ws._sock == NULL) goto ERR;

    return 0;

ERR:
    return -1;
}

void _z_f_link_close_ws(void *arg) {
    _z_link_t *self = (_z_link_t *)arg;

    _z_close_ws(self->_socket._ws._sock);
}

void _z_f_link_free_ws(void *arg) {
    _z_link_t *self = (_z_link_t *)arg;

    _z_free_endpoint_ws(self->_socket._ws._raddr);
}

size_t _z_f_link_write_ws(const void *arg, const uint8_t *ptr, size_t len) {
    const _z_link_t *self = (const _z_link_t *)arg;
    return _z_send_ws(self->_socket._ws._sock, ptr, len);
}

size_t _z_f_link_write_all_ws(const void *arg, const uint8_t *ptr, size_t len) {
    const _z_link_t *self = (const _z_link_t *)arg;
    return _z_send_ws(self->_socket._ws._sock, ptr, len);
}

size_t _z_f_link_read_ws(const void *arg, uint8_t *ptr, size_t len, _z_bytes_t *addr) {
    (void)(addr);
    const _z_link_t *self = (const _z_link_t *)arg;

    return _z_read_ws(self->_socket._ws._sock, ptr, len);
}

size_t _z_f_link_read_exact_ws(const void *arg, uint8_t *ptr, size_t len, _z_bytes_t *addr) {
    (void)(addr);
    const _z_link_t *self = (const _z_link_t *)arg;

    return _z_read_exact_ws(self->_socket._ws._sock, ptr, len);
}

uint16_t _z_get_link_mtu_ws(void) {
    // Maximum MTU for WS
    return 65535;
}

_z_link_t *_z_new_link_ws(_z_endpoint_t endpoint) {
    _z_link_t *lt = (_z_link_t *)z_malloc(sizeof(_z_link_t));

    lt->_capabilities = Z_LINK_CAPABILITY_RELIEABLE;
    lt->_mtu = _z_get_link_mtu_ws();

    lt->_endpoint = endpoint;

    lt->_socket._ws._sock = NULL;
    char *s_addr = _z_parse_address_segment_ws(endpoint._locator._address);
    char *s_port = _z_parse_port_segment_ws(endpoint._locator._address);
    lt->_socket._ws._raddr = _z_create_endpoint_ws(s_addr, s_port);
    z_free(s_addr);
    z_free(s_port);

    lt->_open_f = _z_f_link_open_ws;
    lt->_listen_f = _z_f_link_listen_ws;
    lt->_close_f = _z_f_link_close_ws;
    lt->_free_f = _z_f_link_free_ws;

    lt->_write_f = _z_f_link_write_ws;
    lt->_write_all_f = _z_f_link_write_all_ws;
    lt->_read_f = _z_f_link_read_ws;
    lt->_read_exact_f = _z_f_link_read_exact_ws;

    return lt;
}
#endif
