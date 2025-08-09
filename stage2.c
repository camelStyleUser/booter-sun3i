#include "config.h"
#include "mmclib.h"
#include "slib.h"
#ifndef CONFIG_NO2NDSTAGE
int bootmmc(int num){
if(init_mmc_dev(num)){puts("MMC:FAIL");return 0;}
read_block(0,(unsigned int*)(0x80000000));
shutdown_mmc_dev(num);
return 1;
}
void stage2(void){
#ifdef CONFIG_PRIO_MMC2
if(bootmmc(2)) return;
bootmmc(0);
#else
if(bootmmc(0)) return;
bootmmc(2);
#endif
}
#else
void stage2(void){

}
#endif
