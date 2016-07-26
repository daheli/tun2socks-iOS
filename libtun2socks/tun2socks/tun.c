//
//  tun.c
//  tun2socks
//
//  Created by LEI on 7/26/16.
//  Copyright © 2016 TouchingApp. All rights reserved.
//

#include "tun.h"
#include <ev.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

typedef struct tun_ctx {
    ev_io read_io;
    ev_io write_io;
    int mtu;
    int in_fds[2];
    int out_fds[2];
    char *buf;
    tun_cb read_cb;
    tun_cb write_cb;
} tun_ctx_t;

tun_ctx_t tun_ctx;

void tun_pipe_read_cb(EV_P_ ev_io *w, int revents);
void tun_pipe_write_cb(EV_P_ ev_io *w, int revents);

int tun_init(int mtu) {
    tun_ctx.mtu = mtu;
    if (pipe(tun_ctx.in_fds) < 0) {
        return -1;
    }
    if (pipe(tun_ctx.out_fds) < 0) {
        return -1;
    }
    return 0;
}

void tun_start(tun_cb read_cb, tun_cb write_cb) {
    tun_ctx.buf = malloc(sizeof(char) * (tun_ctx.mtu + 2));
    tun_ctx.read_cb = read_cb;
    tun_ctx.write_cb = write_cb;
    ev_io_init(&tun_ctx.read_io, tun_pipe_read_cb, tun_ctx.in_fds[0], EV_READ);
    ev_io_start(EV_DEFAULT_ &tun_ctx.read_io);
    ev_io_init(&tun_ctx.write_io, tun_pipe_write_cb, tun_ctx.out_fds[0], EV_READ);
    ev_io_start(EV_DEFAULT_ &tun_ctx.write_io);
}

void tun_stop() {
    free(tun_ctx.buf);
}

int tun_mtu() {
    return tun_ctx.mtu;
}

char* tun_buf() {
    return tun_ctx.buf;
}

void tun_input(char *data, int data_len) {
    int len = data_len + 2;
    uint8_t message[tun_ctx.mtu + 2];
    memcpy(message + 2, data, data_len);
    message[0] = data_len / 256;
    message[1] = data_len % 256;
    write(tun_ctx.in_fds[1] , message , len);
}

void tun_pipe_read_cb(EV_P_ ev_io *w, int revents) {
    int fd = tun_ctx.in_fds[0];
    uint8_t data[2];
    ssize_t bytes = read(fd, data, 2);
    if (bytes != 2) {
        return;
    }
    int data_len = data[0] * 256 + data[1];
    bytes = read(fd, tun_ctx.buf, data_len);
    if (bytes != data_len) {
        return;
    }
    tun_ctx.read_cb(tun_ctx.buf, data_len);
}

void tun_write(char *data, int data_len) {
    int len = data_len + 2;
    uint8_t message[tun_ctx.mtu + 2];
    memcpy(message + 2, data, data_len);
    message[0] = data_len / 256;
    message[1] = data_len % 256;
    write(tun_ctx.out_fds[1] , message , len);
}

void tun_pipe_write_cb(EV_P_ ev_io *w, int revents) {
    int fd = tun_ctx.out_fds[0];
    uint8_t data[2];
    ssize_t bytes = read(fd, data, 2);
    if (bytes != 2) {
        return;
    }
    int data_len = data[0] * 256 + data[1];
    bytes = read(fd, tun_ctx.buf, data_len);
    if (bytes != data_len) {
        return;
    }
    tun_ctx.write_cb(tun_ctx.buf, data_len);
}
