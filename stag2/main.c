#include "config.h"
#include "slib.h"
char* old_sp;
char stack[2048];
#ifdef CONFIG_MEM_LOG
char memlog[2048]={0xfe,0x01,0xfe,0x01,0xfe,0x01,0xfe,0x01};
char* memlogcurchar=memlog+4;
#endif
int main(int dram_size){
puts_nonl("AHH,free at last(dram:");
print_dec(dram_size);
puts(")");
return 0;
}
