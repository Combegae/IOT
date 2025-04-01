/*
 * Copyright: Olivier Gruber (olivier dot gruber at acm dot org)
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version. This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "main.h"
#include "isr.h"
#include "uart.h"
#include "ring.h"

extern uint32_t irq_stack_top;
extern uint32_t stack_top;

void check_stacks() {
    void *memsize = (void *) MEMORY;
    const void *addr = &stack_top;
    if (addr >= memsize)
        panic();

    addr = &irq_stack_top;
    if (addr >= memsize)
    panic();
}


    


/**
 * This is the C entry point,
 * upcalled once the hardware has been setup properly
 * in assembly language, see the startup.s file.
 */
// void _start(void) {
//     char c;
//     check_stacks(); // Vérifie la pile
//     uarts_init();
//     uart_enable(UART0);
//     vic_setup_irqs();
//     cookie_t cookie;
//     cookie.c = &c;
//     cookie.uartno = UART0;
//     vic_enable_irq(UART0_IRQ, receive_handler, &cookie);
//     uart_send_string(UART0, "\nSystème en cours d'execution\n");
//     for (;;) {
//         core_halt();
//     }
// }

void _start() {
    check_stacks();
    app_start();

    for (;;) {
        core_disable_irqs();
        process_uart(UART0);
        core_halt();
    }
}

void uart_init(uint8_t no, void (*read_listener)(void *cookie), void (*write_listener)(void *cookie), void *cookie);

bool_t uart_read(uint8_t no, uint8_t* bits);

bool_t uart_write(uint8_t no, uint8_t* bits);

void uart_rx_handler(void* cookie) {
    uint8_t code = uart_receive(UART0);
    while (code) {
        if (ring_full()){
            panic();
        }
        ring_put(code);
        code = uart_receive(UART0);
    }
    uart_interrupt_ack();
}

void app_start() {
    uart_init(UART0, read_listener, write_listener, &cookie);
}


void read_listener(void *addr) {
    cookie_t *cookie = addr;
    uint8_t code;
    while (!cookie->processing && uart_receive(cookie->uartno,&code)) {
        cookie->line[cookie->head++]=(char)code;
        cookie->processing = (code == '\n');
        write_amap(cookie);
    }
    bool_t dropped=0;
    while (cookie->processing && uart_receive(cookie->uartno, &code))
        dropped=1;
    if (dropped)
        //beep(); // signal dropped bytes...
}

void write_listener(void *addr) {
    cookie_t *cookie = addr;
    write_amap(cookie);
}

void write_amap(cookie_t *cookie) {
    while (cookie->tail < cookie->head) {
        uint8_t code = cookie->line[cookie->tail];
        if (!uart_write(cookie->uartno, &code))
            return;
        cookie->tail++;
        if (code == '\n') {
            //shell(cookie->line,cookie->head);
            cookie->tail= cookie->head = 0;
            cookie->processing = 0;
        }
    }
}



void panic() {
    for (;;);
}
