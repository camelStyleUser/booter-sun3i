#include "config.h"
#include "slib.h"
#ifdef CONFIG_MEM_LOG
char memlog[2048]={0xfe,0x01,0xfe,0x01,0xfe,0x01,0xfe,0x01};
char* memlogcurchar=memlog+4;
#endif
int main(int dram_size){
puts("AHH,free at last");
return 0;
}
