#include "config.h"
#include "slib.h"
#include "head.h"
#include "stage2.h"
#define CCM_BASE 0x01c20000
#define CCM_O_SDRAM_PLL_R_OFF 0x20
#define DRAMC_BASE 0x01c01000
#define DRAM_START 0x80000000
char stack[1024]={0};
#ifdef CONFIG_MEM_LOG
char memlog[1024]={0xfe,0x01,0xfe,0x01,0xfe,0x01};
char *memlogcurchar=memlog+2;
#endif
int old_sp=0;
int init_dram(void);
struct drampara{
char initBit;
char smBit1;
char smBit2;
char dram_type;
int bus_width;
char cols;
char rows;
char smBit3;
};
struct drampara params={.bus_width=0xfefdfcfb};
void main(void){//this is main
	//it is called after uart is up
	puts("BOOTER IS UP");
	puts_nonl("RUNNING ON SUN3I");
	if((*(uint*)(0xffff0604))==0x161900){
	puts(" F1E200");
	}else{
	puts(" UNKNOWN SOC");
	}
	int size;
	if((size=init_dram())!=-1){
		puts_nonl("DRAM IS UP.\nSIZE IS ");
		print_dec(size);
		puts(" MiB.");
	}else{
		puts("FAILED TO INIT DRAM");
	}
	//TODO:AAAA make it read a payload from the sd card
	if(size!=-1) stage2();
}
//TODO:document this stuff
void set_dram_clock(uint freq){
	int rfreq;
	rfreq=freq*1000000;//convert MHz to Hz
	int divisor;
	divisor=(rfreq-60000000)/(12000000);//wait is this 24MHz and /2
	for(volatile int i=0;i<2000;i++);
	*(volatile uint*)(CCM_BASE+CCM_O_SDRAM_PLL_R_OFF)=(divisor&0x3f)|((*(volatile uint*)(CCM_BASE+CCM_O_SDRAM_PLL_R_OFF))&0xffffcbc0)|0x8800;//TODO:understand wtf these bits are
	for(volatile int i=0;i<2000;i++);
}
void waitdramctrig1(void){
	*(volatile uint*)(DRAMC_BASE+0xc)=(*(volatile uint*)(DRAMC_BASE+0xc))|(1);
	while(((*(volatile uint*)(DRAMC_BASE+0xc))&1));
	return;
}
void waitdramctrig2(void){
        *(volatile uint*)(DRAMC_BASE+0x24)=1;
        while(((*(volatile uint*)(DRAMC_BASE+0x24))&1));
        return;
}

int confDRAMC(void){//uses global params
	uint tmp=params.bus_width>>4;//because DDR
	*(volatile uint*)(DRAMC_BASE+0x0)=params.smBit2|(1<<1)|(params.smBit1<<3)|\
	(params.smBit3<<4)|(params.rows-1)<<5|(params.cols-1)<<9|\
	(tmp<<13)|(params.initBit<<15)|(params.dram_type<<16);
	*(volatile uint*)(DRAMC_BASE+0xc)=(*(volatile uint*)(DRAMC_BASE+0xc))|(1<<19);
	waitdramctrig1();
	return 0;
}
int init_dram(void){
	*(volatile uint*)(CCM_BASE+0xc)=(*(volatile uint*)(CCM_BASE+0xc))|(1<<13);
	*(volatile uint*)(0x01c20a24)=(*(volatile uint*)0x01c20a24)|(1<<16);//TODO:this is DDR only, also wtf is this register?
	set_dram_clock((DRAM_FREQ)<<1);//i just do what BOOT0 does
	params.initBit=1;
	params.smBit1=1;
	params.smBit2=0;
	params.smBit3=0;
	params.dram_type=1;
	params.bus_width=16;
	params.cols=10;
	params.rows=13;
	*(volatile uint*)(0x01c20a24)=(*(volatile uint*)0x01c20a24)|(1<<16);//TODO:this is DDR only, also wtf is this register?
	*(volatile uint*)(DRAMC_BASE+0x04)=0xa7cec93a;
	*(volatile uint*)(DRAMC_BASE+0x08)=0x00570008;
	*(volatile uint*)(DRAMC_BASE+0x14)=0x2000;
	confDRAMC();
	int count=0;
	for(int i=0;i<8;i++){
		*(volatile uint*)(DRAMC_BASE+0xc)=(*(uint*)(DRAMC_BASE+0xc))|(i<<6);
		waitdramctrig2();
		if ((*(volatile uint *)(DRAMC_BASE + 0x24) & 0x30) != 0) {
			count = count + 1;
		}
	}
	if(count==8) params.dram_type=0;
	else params.dram_type=1;
	*(volatile uint*)(0x01c20a24)=(*(volatile uint*)0x01c20a24)|(1<<16);//TODO:this is DDR only, also wtf is this register?
	confDRAMC();
	if(params.dram_type==1){
		*(volatile uint*)(DRAMC_BASE+0)=(*(volatile uint*)(DRAMC_BASE+0))&0xfffe9fff;
		waitdramctrig1();
		int tmp1=0;
		if(DRAM_FREQ<91) tmp1=1;
		else if(DRAM_FREQ<151) tmp1=2;
		else tmp1=3;
		*(volatile uint*)(DRAMC_BASE+0x0c)=((*(volatile uint*)(DRAMC_BASE+0x0c))&0xfffffe3f)|tmp1<<6;
	}else{
	puts("ASSERT FAIL: NOT DDR");
	return -1;
	}
	for(int i=0;i<128;i++){
		*(volatile int*)(DRAM_START+0x200+i)=0x11111111;
		*(volatile int*)(DRAM_START+0x600+i)=0x22222222;
	}
	count=0;
	for(int i=0;i<128;i++){
		if((*(volatile int*)(DRAM_START+0x200+i))==0x22222222) count++;
	}
	if(count==128) params.cols=9;
	else params.cols=10;
	confDRAMC();
	volatile uint *offset1;
	volatile uint *offset2;
	if(params.cols==9){
		offset1=(volatile uint*)0x80c00000;
		offset2=(volatile uint*)0x80400000;
	}else{
		offset1=(volatile uint*)0x80600000;
		offset2=(volatile uint*)0x80200000;
	}
	for(int i=0;i<128;i++){
		*(volatile int*)(offset2+i)=0x33333333;
		*(volatile int*)(offset1+i)=0x44444444;
	}
	count=0;
	for(int i=0;i<128;i++){
		if((*(volatile int*)(offset2+i))==0x44444444) count++;
	}
	if(count==128) params.rows=12;
	else params.rows=13;
	*(volatile uint*)(DRAMC_BASE+0x10)=(DRAM_FREQ*499)>>6;
	params.initBit=0;
	confDRAMC();
	return params.rows==13?params.cols==10?0x40:0x20:0x10 ; //return DRAM size i guess
}
