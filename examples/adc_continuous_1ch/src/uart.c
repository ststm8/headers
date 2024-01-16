#include <uart.h>
#include <stdio.h>
#include <string.h>

void uartBegin(uint32_t baudRate)
{
#if defined(FAMILY_STM8L)
// for low-power device enable clock gating to USART1
// sfr_CLK.PCKENR1.PCKEN15 = 1;
#endif

    UART1_t *uart = &sfr_UART1;

    // set UART behaviour
    uart->CR1.byte = sfr_UART1_PSCR_RESET_VALUE; // enable UART, 8 data bits, no parity control
    uart->CR2.byte = sfr_UART1_PSCR_RESET_VALUE; // no interrupts, disable sender/receiver
    uart->CR3.byte = sfr_UART1_PSCR_RESET_VALUE; // no LIN support, 1 stop bit, no clock output(?)

    // set baudrate (note: BRR2 must be written before BRR1!)
    //WORD val16 = (uint16_t)(((uint32_t)16000000L) / baudRate);
    WORD val16 = (uint16_t)(((uint32_t)2000000L) / baudRate);
    uart->BRR2.byte = (uint8_t)(((val16 & 0xF000) >> 8) | (val16 & 0x000F));
    uart->BRR1.byte = (uint8_t)((val16 & 0x0FF0) >> 4);

    // enable transmission, no transmission
    uart->CR2.REN = 1; // enable receiver
    uart->CR2.TEN = 1; // enable sender
    // uart->CR2.TIEN = 1;  // enable transmit interrupt
    // uart->CR2.RIEN = 1; // enable receive interrupt
}

// ---

volatile char uartPrintBuf[32];

void uartPrint(const char *msg)
{
    BYTE i, j;
    j = strlen(msg);
    for (i = 0; i < j; i++)
    {
        while (!(sfr_UART1.SR.TXE))
            ;
        sfr_UART1.DR.byte = msg[i];
    }
}
