//
//  tun2socks.c
//  tun2socks
//
//  Created by LEI on 7/24/16.
//  Copyright © 2016 TouchingApp. All rights reserved.
//

#include "tun2socks.h"
#include <signal.h>
#include <ev.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "tcp_priv.h"
#include "timers.h"
#include "init.h"
#include "ip_addr.h"
#include "netif.h"
#include "ip6.h"
#include "tun.h"

struct options {
    ip4_addr_t netif_ip4addr;
    ip4_addr_t netif_ip4netmask;
    int mtu;
};

static struct netif the_netif;
static int have_netif = 0;
static struct options options;

struct tcp_pcb *listener;

static int quitting = 0;


static struct ev_signal sigint_watcher;
static struct ev_signal sigterm_watcher;

static ev_periodic tcp_timer;


// Functions
// Signals
void handle_signal();
void unhandle_signal();
void signal_cb(EV_P_ ev_signal *w, int revents);

// LWIP
void init_lwip();
void init_tcp_timer(EV_P);
void tcp_timer_cb (EV_P_ ev_periodic *w, int revents);
void tun_input_cb(char *data, int data_len);
err_t netif_init_func (struct netif *netif);
err_t netif_input_func(struct pbuf *p, struct netif *inp);
err_t netif_output_func (struct netif *netif, struct pbuf *p, const ip4_addr_t *ipaddr);
err_t netif_common_output_func (struct netif *netif, struct pbuf *p, const ip_addr_t *ipaddr);
err_t listener_accept_func (void *arg, struct tcp_pcb *newpcb, err_t err);

void terminate();


void start_tun2socks(uint32_t ip4_addr, uint32_t ip4_netmask, tun_cb tun_write_cb) {
    tun_init(4096);
    struct ev_loop *loop = EV_DEFAULT;
    ip4_addr_set_u32(&options.netif_ip4addr, ip4_addr);
    ip4_addr_set_u32(&options.netif_ip4netmask, ip4_netmask);
    handle_signal();
    init_lwip();
    init_tcp_timer(loop);
    tun_start(tun_input_cb, tun_write_cb);
    ev_run (loop, 0);
    unhandle_signal();
    return ;
}

void stop_tun2socks() {

}

void terminate() {
    assert(!quitting);
    // set quitting
    quitting = 1;
}

// Signal
void handle_signal() {
    signal(SIGPIPE, SIG_IGN);
    signal(SIGABRT, SIG_IGN);
    ev_signal_init(&sigint_watcher, signal_cb, SIGINT);
    ev_signal_init(&sigterm_watcher, signal_cb, SIGTERM);
    ev_signal_start(EV_DEFAULT, &sigint_watcher);
    ev_signal_start(EV_DEFAULT, &sigterm_watcher);
}

void unhandle_signal() {
    ev_signal_stop(EV_DEFAULT, &sigint_watcher);
    ev_signal_stop(EV_DEFAULT, &sigterm_watcher);
}

void signal_cb(EV_P_ ev_signal *w, int revents) {
    if (revents & EV_SIGNAL) {
        switch (w->signum) {
            case SIGINT:
            case SIGTERM:
                ev_unloop(EV_A_ EVUNLOOP_ALL);
        }
    }
}

void init_lwip() {
    assert(!quitting);
    assert(!have_netif);
    assert(!listener);

    lwip_init();

    ip_addr_t gw;
    ip_addr_set_any(0, &gw);

    // init netif
    if (!netif_add(&the_netif, &options.netif_ip4addr, &options.netif_ip4netmask, &gw, NULL, netif_init_func, netif_input_func)) {
        goto fail;
    }
    // TODO: ipv6
    have_netif = 1;
    // set netif up
    netif_set_up(&the_netif);
    // set netif pretend TCP
    netif_set_pretend_tcp(&the_netif, 1);
    // set netif default
    netif_set_default(&the_netif);
    // init listener
    struct tcp_pcb *l = tcp_new();
    if (!l) {
        goto fail;
    }
    // bind listener
    if (tcp_bind_to_netif(l, "ho0") != ERR_OK) {
        tcp_close(l);
        goto fail;
    }
    // listen listener
    if (!(listener = tcp_listen(l))) {
        tcp_close(l);
        goto fail;
    }
    // setup listener accept handler
    tcp_accept(listener, listener_accept_func);
    // TODO: ipv6
    return;
fail:
    if (!quitting) {
        terminate();
    }
}

// TCP Timer
void init_tcp_timer(EV_P) {
    ev_periodic_init(&tcp_timer, tcp_timer_cb, 0., TCP_TMR_INTERVAL/1000., 0);
    ev_periodic_start(loop, &tcp_timer);
}

void tcp_timer_cb (EV_P_ ev_periodic *w, int revents) {
    tcp_tmr();
    return;
}

void tun_input_cb(char *data, int data_len) {
    // TODO: UDP
    if (data_len > UINT16_MAX) {
        // Too long
        return;
    }
    struct pbuf *p = pbuf_alloc(PBUF_RAW, data_len, PBUF_POOL);
    if (!p) {
        return;
    }
    // write packet to pbuf
    assert(pbuf_take(p, data, data_len) == ERR_OK);
    // pass pbuf to input
    if (the_netif.input(p, &the_netif) != ERR_OK) {
        pbuf_free(p);
    }
}

err_t netif_init_func (struct netif *netif) {
    netif->name[0] = 'h';
    netif->name[1] = 'o';
    netif->output = netif_output_func;
    // TODO: ipv6
    //    netif->output_ip6 = netif_output_ip6_func;
    netif->mtu = options.mtu;
    return ERR_OK;
}

err_t netif_input_func (struct pbuf *p, struct netif *inp) {
    uint8_t ip_version = 0;
    if (p->len > 0) {
        ip_version = (((uint8_t *)p->payload)[0] >> 4);
    }
    switch (ip_version) {
        case 4:
            return ip_input(p, inp);
        case 6:
            //TODO: ipv6
            break;
    }
    pbuf_free(p);
    return ERR_OK;
}

err_t netif_output_func (struct netif *netif, struct pbuf *p, const ip4_addr_t *ipaddr) {
    return netif_common_output_func(netif, p, ipaddr);
}

err_t netif_common_output_func (struct netif *netif, struct pbuf *p, const ip_addr_t *ipaddr) {
    if (quitting) {
        return ERR_OK;
    }
    if (!p->next) {
        if (p->len > tun_mtu()) {
            goto out;
        }
        tun_write(p->payload, p->len);
    } else {
        int len = 0;
        do {
            if (p->len > tun_mtu() - len) {
                goto out;
            }
            memcpy(tun_buf() + len, p->payload, p->len);
            len += p->len;
        } while ((p = p->next) != NULL);

        tun_write(tun_buf(), len);
    }
out:
    return ERR_OK;
}

err_t listener_accept_func (void *arg, struct tcp_pcb *newpcb, err_t err) {
    return ERR_OK;
}

