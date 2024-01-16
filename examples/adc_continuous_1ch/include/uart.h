    #ifndef __uart_h__
#define __uart_h__

#include <config.h>

extern volatile char uartPrintBuf[32];

void uartBegin(uint32_t baudRate);
void uartPrint(const char *msg);
#define uartPrintf(fmt, ...) sprintf(uartPrintBuf, fmt, __VA_ARGS__); uartPrint(uartPrintBuf);

#endif