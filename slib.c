#include "head.h"
#include "config.h"
char shared_hex_dec[17]="0123456789abcdef";
void puts_nonl(char* string){
        char* val=string;
        while(*val) uart_putc(*(val++));
}
void puts(char* string){
	puts_nonl(string);
	uart_putc((int)'\n');
}
void print_hex(unsigned int value){
	int shift;
	puts_nonl("0x");
	for(shift=28;shift>-4;shift-=4){
		uart_putc(shared_hex_dec[(value>>shift)&0xf]);
	}
}
void print_dec(int value){
	if(value<0){
		puts_nonl("-");
		value=-value;
		#ifndef CONFIG_EDGE_OPTIM
		if(value<0){
			puts_nonl("2147483648");
			return;
		}
		#endif
	}
	int ripple=0;
	int divisor=1000000000;
	int digit=0;
	for(;divisor>0;divisor=(divisor/10)){
		digit=(value/divisor)%10;
		if(digit==0&&(!ripple)&&(!(divisor==1))) continue;
		ripple=1;
		uart_putc(shared_hex_dec[digit]);
	}
}
