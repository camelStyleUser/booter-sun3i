#define ASSEMBLY
#include "config.h"
#define UART0_BASE	0x01c21000
.global uart_putc,uart_isavail,uart_getc,delay,disable_icache
#ifdef CONFIG_MEM_LOG
.global memlogcurchar
.global memlog
#define MEMLOG_END memlog+2048
#endif
uart_putc://checks r0 for char to output
	#ifndef CONFIG_MEM_LOG
	ldr r1,=(UART0_BASE)
	ldr r2,[r1,#0x14]
	tst r2,#0x40
	beq uart_putc//wait until transmitter empty
	str r0,[r1,#0x0]
	#else
	ldr r1,=(memlogcurchar)
	ldr r1,[r1]
	strb r0,[r1]
	add r1,r1,#1
	ldr r2,=(MEMLOG_END)
	cmp r1,r2
	blt memlog_isnotfull
	ldr r1,=(memlog)
	memlog_isnotfull:
	ldr r2,=(memlogcurchar)
	str r1,[r2]
	#endif
	bx lr
uart_isavail://return r0 - is any char available
	#ifndef CONFIG_MEM_LOG
	ldr r1,=(UART0_BASE)
	ldr r0,[r1,#0x14]
	and r0,r0,#0x1
	#else
	ldr r0,=0
	#endif
	bx lr
uart_getc://return r0 - char read from uart
	#ifndef CONFIG_MEM_LOG
	ldr r1,=(UART0_BASE)
	ldr r0,[r1,#0x14]
	tst r0,#0x1
	beq uart_getc
	ldr r0,[r1,#0x0]
	#endif
	bx lr
delay:	//r0- delay low
	delayin:
	subs r0,r0,#1
	bne delayin
	bx lr
disable_icache:
	mrc p15,0,r0,c1,c0,0
        bic r0,r0,#(1<<12)
        mcr p15,0,r0,c1,c0,0
	bx lr

