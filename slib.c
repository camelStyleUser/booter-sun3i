#include "head.h"
char shared_hex_dec[17]="0123456789abcdef";
void puts(char* string){
	char* val=string;
	while(*val) uart_putc((int)*(val++));
	uart_putc((int)'\n');
}
void puts_nonl(char* string){
	char* val=string;
        while(*val) uart_putc((int)*(val++));
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
		if(value<0){
			puts_nonl("2147483648");
			return;
		}
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
