/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once
#include <string>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"


struct data_to_send_t
{
    uint8_t button_state;
    uint pressed;
    uint pressed_since_last;
    float pot_percent;
    float light_intesity;
    float temp;
    float desk_height;
};

enum tcp_commands
{
    nothing, start, stop, pause
};


#define TCP_PORT 4242
#define DEBUG_printf printf
#define RECV_BUF_SIZE 1
#define TEST_ITERATIONS 10
#define POLL_TIME_S 1

typedef struct TCP_SERVER_T_ {
    struct tcp_pcb *server_pcb;
    struct tcp_pcb *client_pcb;
    uint8_t buffer_recv[RECV_BUF_SIZE];
    int sent_len;
    int recv_len;
    data_to_send_t data;
    uint8_t running;
} TCP_SERVER_T;

/**
 * @brief allocates space for TCP_SERVER_T
 */
TCP_SERVER_T* tcp_server_init(void);

/**
 * @brief Closes server
 * @param arg void pointer which should point to the server that should be closed
 */
err_t tcp_server_close(void *arg);

/**
 * @brief function for interpreting results
 * @param arg void pointer to server instance
 * @param status state you want to get result of
 */
err_t tcp_server_result(void *arg, int status);

/**
 * @brief sent callback function
 */
err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);

/**
 * @brief function to queue data to be sent
 */
err_t tcp_server_send_data(void *arg, struct tcp_pcb *tpcb);

/**
 * @brief recieve callback function
 */
err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);

/**
 * @brief poll callback function
 */
err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb);

/**
 * @brief error handling callback function
 */
void tcp_server_err(void *arg, err_t err);

/**
 * @brief callback function to accept connection from pc
 */
err_t tcp_server_accept(void *arg, struct tcp_pcb *client_pcb, err_t err);

/**
 * @brief function to open server
 */
bool tcp_server_open(void *arg);

