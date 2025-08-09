#ifndef __HEAD_H
#define __HEAD_H
void uart_putc(int);
int uart_isavail(void);
int uart_getc(void);
void delay(int);
void disable_icache(void);
#endif //__HEAD_H
