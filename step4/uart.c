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

#include "uart.h"
#include "main.h"
#include "ring.h"
#include "isr.h"
#include "uart-mmio.h"
#include <stdint.h>

struct uart {
  uint8_t uartno; // the UART numéro
  void *bar;      // base address register for this UART
};

static struct uart uarts[NUARTS];

static void uart_init(const uint32_t uartno, void *bar) {
  struct uart *uart = &uarts[uartno];
  uart->uartno = uartno;
  uart->bar = bar;

  mmio_write32(uart->bar, UART_IMSC, 0x0000);
}


void uarts_init() { // 
  uart_init(UART0, UART0_BASE_ADDRESS);
  uart_init(UART1, UART1_BASE_ADDRESS);
  uart_init(UART2, UART2_BASE_ADDRESS);
}

void uart_enable(const uint32_t uartno) {
  const struct uart *uart = &uarts[uartno];
  // mmio_write32(uart->bar, UART_IMSC, UART_IMSC_RXIM | UART_IMSC_TXIM);
  mmio_write32(uart->bar, UART_IMSC, UART_IMSC_RXIM);
}

void uart_disable(const uint32_t uartno) {
  const struct uart *uart = &uarts[uartno];
  mmio_write32(uart->bar, UART_IMSC, 0);
}

void uart_init_fct(uint8_t no, void (*read_listener)(void *cookie), void *cookie){
  cookie_t *cookie2 = cookie;
  cookie2->event = read_listener;
  if(no == UART0){
    uart_enable(UART0);
    vic_enable_irq(UART0_IRQ, (void*)(uart_rx_handler), cookie2);
  }
  }
  

bool_t uart_read(uint8_t no, uint8_t* bits){
  return 0;
}

bool_t uart_write(uint8_t no, uint8_t* bits){
  return 1;
}

void uart_rx_handler(uint32_t irq, void* cookie) {
    uint8_t carac;
    cookie_t *cookie_2 = cookie;
    const struct uart *uart = &uarts[cookie_2->uartno];
    while (!(mmio_read8(uart->bar, UART_FR) & RECEIVE_FULL)) { // tant que y'a des char a lire.
        uart_receive(UART0, &carac);
        ring_put(carac);
    }
}









void receive_handler(uint8_t id, void* cookie_void) {
  cookie_t *cookie = cookie_void;
  const struct uart *uart = &uarts[cookie->uartno];
  while (mmio_read8(uart->bar, UART_FR) & RECEIVE_FULL)
    ;
  *cookie->c = mmio_read8(uart->bar, UART_DR);
  uart_send(cookie->uartno, *cookie->c); // On renvoie le char recu
}

void uart_receive(const uint8_t uartno, uint8_t *pt) {
  const struct uart *uart = &uarts[uartno];
  while (mmio_read8(uart->bar, UART_FR) & RECEIVE_FULL)
    ;
  *pt = mmio_read8(uart->bar, UART_DR);
  mmio_write16(uart->bar, UART_ICR, UART_ICR); // clear the interruption at the device level
}

/**
 * Sends a character through the given uart, this is a blocking call
 * until the character has been sent.
 */
void uart_send(const uint8_t uartno, const uint8_t s) {
  const struct uart *uart = &uarts[uartno];
  while (mmio_read8(uart->bar, UART_FR) & SEND_EMPTY)
    ;
  mmio_write8(uart->bar, UART_DR, s);
}

/**
 * This is a wrapper function, provided for simplicity,
 * it sends a C string through the given uart.
 */
void uart_send_string(const uint8_t uartno, const uint8_t *s) {
  while (*s != '\0') {
    uart_send(uartno, *s);
    s++;
  }
}
