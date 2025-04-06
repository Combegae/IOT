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
#include "uart-mmio.h"

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
    cookie_t cookie;
    app_start(&cookie);
    for (;;) {
        //core_disable_irqs();
        write_amap(&cookie);
        core_halt();
    }
}

void app_start(cookie_t *cookie) {
    uarts_init(); // initialise les base addresses des uarts.
    cookie->uartno = UART0;
    cookie->tail=0;
    cookie->head=0;
    vic_setup_irqs();
    uart_init_fct(UART0, read_listener, cookie);
    uart_send_string(UART0, "\nSystème en cours d'execution\n");

}




void read_listener(void *addr) {
    cookie_t *cookie = addr;
    uint8_t carac; // Rajouter le ring get a la place de son uart_receive
    while (!cookie->processing && !ring_empty()) { // tant qu'on ne  traite pas une ligne et qu'on continue de recevoir des caractères de l'utilisateur.
        carac = ring_get();
        cookie->line[cookie->head++]=(char)carac;
        cookie->processing = (carac == '\r'); // passe a Vrai si on a le char de fin de ligne de commande.
        //write_amap(cookie);
    }
    bool_t dropped=0;
    while (cookie->processing && ring_get())
        dropped=1;
    if (dropped){
        ;
         //beep(); // signal dropped bytes...
    }

}

void write_listener(void *addr) {
    cookie_t *cookie = addr;
    write_amap(cookie);
}

void write_amap(cookie_t *cookie) { // Code pour exe le shell.
    //uart_send_string(UART0, "test");
    while (cookie->tail < cookie->head) {
        uint8_t code = cookie->line[cookie->tail];
        uart_send(cookie->uartno, code);
        cookie->tail++;
        if (code == '\r') {
            uart_send(cookie->uartno, '\n');
            uart_send_string(UART0, "Exe commande");
            //shell(cookie->line,cookie->head);
            cookie->tail= cookie->head = 0;
            cookie->processing = 0;
        }
    }
}




void panic() {
    for (;;);
}
