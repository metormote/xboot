/************************************************************************/
/* XBoot Extensible AVR Bootloader                                      */
/*                                                                      */
/* ANT Module                                      .                    */
/*                                                                      */
/* ant.h                                          .                     */
/*                                                                      */
/* Erik Rosen <erik.rosen@iop.io>.........                              */
/*                                                                      */
/* Copyright (c) 2013 Erik Rosen....                                    */
/*                                                                      */
/* Permission is hereby granted, free of charge, to any person          */
/* obtaining a copy of this software and associated documentation       */
/* files(the "Software"), to deal in the Software without restriction,  */
/* including without limitation the rights to use, copy, modify, merge, */
/* publish, distribute, sublicense, and/or sell copies of the Software, */
/* and to permit persons to whom the Software is furnished to do so,    */
/* subject to the following conditions:                                 */
/*                                                                      */
/* The above copyright notice and this permission notice shall be       */
/* included in all copies or substantial portions of the Software.      */
/*                                                                      */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,      */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF   */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND                */
/* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS  */
/* BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   */
/* ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN    */
/* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE     */
/* SOFTWARE.                                                            */
/*                                                                      */
/************************************************************************/


#ifndef ANT_H_
#define ANT_H_

#include "xboot.h"

#if (ANT_UART_NUMBER == 0)
#define ANT_UART_RX_PIN             2
#define ANT_UART_TX_PIN             3
#else
#define ANT_UART_RX_PIN             6
#define ANT_UART_TX_PIN             7
#endif
#define ANT_UART_PORT               token_paste2(PORT, ANT_UART_PORT_NAME)
#define ANT_UART_DEVICE_PORT        token_paste2(ANT_UART_PORT_NAME, ANT_UART_NUMBER)
#define ANT_UART_DEVICE             token_paste2(USART, ANT_UART_DEVICE_PORT)
#define ANT_UART_RX_PIN_CTRL        token_paste3(ANT_UART_PORT.PIN, ANT_UART_RX_PIN, CTRL)
#define ANT_UART_TX_PIN_CTRL        token_paste3(ANT_UART_PORT.PIN, ANT_UART_TX_PIN, CTRL)

// BAUD Rate Values
// Known good at 2MHz
#if (F_CPU == 2000000L) && (ANT_UART_BAUD_RATE == 57600)
#define ANT_UART_BSEL_VALUE         26
#define ANT_UART_BSCALE_VALUE       -3
#define ANT_UART_CLK2X              1
// Known good at 32MHz
#elif (F_CPU == 32000000L) && (ANT_UART_BAUD_RATE == 57600)
#define ANT_UART_BSEL_VALUE         34
#define ANT_UART_BSCALE_VALUE       0
#define ANT_UART_CLK2X              0
#else
#define ANT_UART_BSEL_VALUE         ((F_CPU) / ((uint32_t)ANT_UART_BAUD_RATE * 16) - 1)
#define ANT_UART_BSCALE_VALUE       0
#define ANT_UART_CLK2X              0
#endif

#define ANT_RTS_PORT              token_paste2(PORT, ANT_RTS_PORT_NAME)
#define ANT_RESET_PORT            token_paste2(PORT, ANT_RESET_PORT_NAME)
#define ANT_SUSPEND_PORT          token_paste2(PORT, ANT_SUSPEND_PORT_NAME)
#define ANT_SLEEP_PORT            token_paste2(PORT, ANT_SLEEP_PORT_NAME)

// nonzero if character has been received
#define ant_uart_char_received() (ANT_UART_DEVICE.STATUS & USART_RXCIF_bm)
// current character in UART receive buffer
#define ant_uart_cur_char() ANT_UART_DEVICE.DATA
// send character
#define ant_uart_send_char(c) ANT_UART_DEVICE.DATA = (c)
// send character, block until it is completely sent
#define ant_uart_send_char_blocking(c) do {ant_uart_send_char(c); \
    while (!(ANT_UART_DEVICE.STATUS & USART_TXCIF_bm)) { } \
    ANT_UART_DEVICE.STATUS |= USART_TXCIF_bm; } while (0)


void ant_init(uint16_t device_no);
void ant_deinit(void);
void ant_load_firmware(void);

#endif /* ANT_H_ */