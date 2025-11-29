#define ASSEMBLY
#include "config.h"
#define FEL_START	0xffff0020
#define ADDR_CCM	0x01c20000
#define CCM_REG_APB_GAT	ADDR_CCM+0x10
#define UART0_BASE	0x01c21000
#define APB_UART0_SHIFT	5
#define APB_CLOCK 30000000//from BT0
#define BAUD 115200
#define DIVISORVAL ((APB_CLOCK+(BAUD*8))/(BAUD*16)) //same logic as BT0
#define BAUDLO (DIVISORVAL&0xff)
#define BAUDHI (DIVISORVAL>>8)
#define PIO_BASE 0x01c20800
.global _real_start
.global uart_putc
.global uart_isavail
.global uart_getc
.global delay
.global disable_icache
.global stack
.global old_sp
#ifdef CONFIG_MEM_LOG
.global memlogcurchar
.global memlog
#define MEMLOG_END memlog+2048
#endif
_real_start://SAVE OUR CODE
	b _start
	nop
	nop
	nop
	nop
_start://TODO:actually make the entry
	ldr r0, =old_sp
        str sp, [r0]
        ldr sp, =(stack+1020)
	//magic
	ldr r0, =(ADDR_CCM)
	ldr r1,[r0,#0x8]
	bic r1,r1,#(0x3<<9)
	orr r1,r1,#(0x1<<9)
	str r1,[r0,#0x8]
	ldr r1,[r0,#0xc]
	orr r1,r1,#(1<<6)
	str r1,[r0,#0xc]
	ldr r1,[r0,#0x10]
	orr r1,r1,#(1<<4)
	str r1,[r0,#0x10]
	ldr r1,[r0,#0x0]
	ldr r2, =(0x247F0000)
	bic r1,r1,r2
	ldr r2, =(0x200F0000)
	orr r1,r1,r2
	str r1,[r0,#0x0]
	mov r2,#50
        1:
        subs r2,r2,#1
        bne 1b
	ldr r1,[r0,#0x8]
	bic r1,r1,#0xdb
	orr r1,r1,#0x48
	str r1,[r0,#0x8]
	bic r1,r1,#0x600
	orr r1,r1,#0x400
	str r1,[r0,#0x8]
	//end of magic
	mrc p15,0,r0,cr1,cr0,0
	orr r0,r0,#(1<<12)
	mcr p15,0,r0,cr1,cr0,0
	bl uart_init
	ldr r0,=0x00ffffff
	ldr r1,=0x00000000
	str r0,[r1]
	bl main
	ldr r0, =old_sp
	ldr sp, [r0]
	ldr r0, =0
	b FEL_START
uart_init:	//enable APB
		//enable UART
		//make sure to make uart pins actually uart
		//init UART to use whatever baud it needs to
	#ifndef CONFIG_MEM_LOG
	ldr r0,=(CCM_REG_APB_GAT)
	ldr r1,[r0]
	bic r1,r1,#(1<<APB_UART0_SHIFT)
	str r1,[r0]
	mov r2,#200
	1:
	subs r2,r2,#1
	bne 1b
	ldr r1,[r0]
	orr r1,r1,#(1<<APB_UART0_SHIFT)
	str r1,[r0]

        ldr r0,=(PIO_BASE)
        ldr r1,[r0,#0]
        bic r1,r1,#(7<<8)
        orr r1,r1,#(3<<8)//PA2 <3>
        str r1,[r0,#0]
        ldr r1,[r0,#0x14]//pull=1
        bic r1,r1,#(3<<4)
        orr r1,r1,#(1<<4)
        str r1,[r0,#0x14]
        ldr r1,[r0,#0x1c]//drv=1
        bic r1,r1,#(3<<4)
        orr r1,r1,#(1<<4)
        str r1,[r0,#0x1c]
        ldr r1,[r0,#0]
        bic r1,r1,#(7<<16)
        orr r1,r1,#(3<<16)//PA4 <3>
        str r1,[r0,#0]
        ldr r1,[r0,#0x14]//pull=1
        bic r1,r1,#(3<<8)
        orr r1,r1,#(1<<8)
        str r1,[r0,#0x14]
        ldr r1,[r0,#0x1c]//drv=1
        bic r1,r1,#(3<<8)
        orr r1,r1,#(1<<8)
        str r1,[r0,#0x1c]

	ldr r0,=(UART0_BASE)
	mov r1,#0x80
	str r1,[r0,#0xc]//write divisor latches
	ldr r1,=(BAUDLO)
	str r1,[r0,#0x0]
	ldr r1,=(BAUDHI)
	str r1,[r0,#0x4]
	mov r1,#0x3//8-bit mode
	str r1,[r0,#0xc]
	mov r1,#0x7//clean fifos and enable them
	str r1,[r0,#0x8]
        mov r0,#0x1000000
        b delay
	#else
	bx lr
        #endif
uart_putc://checks r0 for char to output
	#ifndef CONFIG_MEM_LOG
push {r4,r5}
ldr r2,=(PIO_BASE)
ldr r1,[r2,#0]
mov r3,r1
bic r1,r1,#(7<<8)
orr r1,r1,#(3<<8)//PA2 <3>
str r1,[r2,#0]
ldr r1,[r2,#0x14]//pull=1
mov r4,r1
bic r1,r1,#(3<<4)
orr r1,r1,#(1<<4)
str r1,[r2,#0x14]
ldr r1,[r2,#0x1c]//drv=1
mov r5,r1
bic r1,r1,#(3<<4)
orr r1,r1,#(1<<4)
str r1,[r2,#0x1c]
ldr r1,[r2,#0]
bic r1,r1,#(7<<16)
orr r1,r1,#(3<<16)//PA4 <3>
str r1,[r2,#0]
ldr r1,[r2,#0x14]//pull=1
bic r1,r1,#(3<<8)
orr r1,r1,#(1<<8)
str r1,[r2,#0x14]
ldr r1,[r2,#0x1c]//drv=1
bic r1,r1,#(3<<8)
orr r1,r1,#(1<<8)
str r1,[r2,#0x1c]
	ldr r1,=(UART0_BASE)
	1:
        ldr r2,[r1,#0x14]
	tst r2,#0x40
	beq 1b//wait until transmitter empty
	str r0,[r1]
        1:
        ldr r2,[r1,#0x14]
        tst r2,#0x40
	beq 1b
ldr r2,=(PIO_BASE)
str r3,[r2,#0x0]
str r4,[r2,#0x14]
str r5,[r2,#0x1c]
pop {r4,r5}
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
        1:
	ldr r0,[r1,#0x14]
	tst r0,#0x1
	beq 1b
	ldr r0,[r1]
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
