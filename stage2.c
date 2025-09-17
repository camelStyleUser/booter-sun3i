#include "config.h"
#include "mmclib.h"
#include "slib.h"
#ifndef CONFIG_NO2NDSTAGE
extern char memlog[1024];
int bootmmc(int num){
 if(init_mmc_dev(num)){
  #ifndef CONFIG_CONCISE_LOGS
  puts_nonl("MMC");
  print_dec(num);
  puts(":FAIL");
  #endif
  return 0;
 }
 read_block(0,(unsigned int*)(memlog+512));
 shutdown_mmc(num);
 return 1;
}
void stage2(void){
#ifdef CONFIG_PRIO_MMC2
if(bootmmc(2)) return;
#ifndef CONFIG_NO_BACKUP_MMC
bootmmc(0);
#endif
#else
if(bootmmc(0)) return;
#ifndef CONFIG_NO_BACKUP_MMC
bootmmc(2);
#endif
#endif
}
#else
void stage2(void){

}
#endif
