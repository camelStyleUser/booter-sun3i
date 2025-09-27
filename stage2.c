#include "config.h"
#include "mmclib.h"
#include "slib.h"
#include "stage2.h"
#include "main.h"
#ifndef CONFIG_NO2NDSTAGE
#define MOD_ADLER 65521
unsigned int adler32(unsigned char* data,unsigned long size){
 unsigned short a=1;
 unsigned short b=0;
 for(unsigned long i=0;i<size;i++){
  a=(a+data[i])%MOD_ADLER;
  b=(b+a)%MOD_ADLER;
 }
 return (b<<16)|a;
}
int bootmmc(int num){
 switch(num){
  case 0:
  case sizeof(struct stage2_desc_sector)==0x200:
  break;
 }
 if(init_mmc_dev(num)){
  #ifndef CONFIG_CONCISE_LOGS
  puts_nonl("MMC");
  print_dec(num);
  puts(":FAIL");
  #endif
  return 0;
 }
 read_block(15,(unsigned int*)(0x80000000));
 struct stage2_desc_sector* desc=(struct stage2_desc_sector*)(0x80000000);
 if(desc->magic!=DESC_MAGIC){
  puts("NODESC");
  return 0;
 }
 unsigned int expect=desc->metaadler32;
 desc->metaadler32=0;
 unsigned int actual=adler32((unsigned char*)desc,0x200);
 if(actual!=expect){
  puts("CHKFAIL");
  #ifndef CONFIG_CONCISE_LOGS
  puts_nonl("EXP:");
  print_hex(expect);
  puts_nonl(",GOT:");
  print_hex(actual);
  #endif
  return 0;
 }
 if(desc->desc_ver!=CURR_DESC_VER){
  puts("BADVER");
  return 0;
 }
 unsigned int start=desc->blockpos;
 unsigned int len=desc->blocksize;
 expect=desc->codeadler32;
 //DO NOT USE DESC AFTER THIS POINT
 for(unsigned int i=0;i<len;i++){
  read_block(start+i,(unsigned int*)(0x80000000|(i*0x200)));//the compiler will probably figure out i*0x200 -> i<<9
 }                                                          // but probably not 0x80000000+ -> 0x80000000|
 actual=adler32((unsigned char*)(0x80000000),len*0x200);
 if(actual!=expect){
  puts("BADSTAGE2");
  #ifndef CONFIG_CONCISE_LOGS
  puts_nonl("EXP:");
  print_hex(expect);
  puts_nonl(",GOT:");
  print_hex(actual);
  #endif
 }
 shutdown_mmc(num);
 int (*entry)(int);//pass dram size in megabytes
 entry=(void*)(0x80000000);
 #ifndef CONFIG_CONCISE_LOGS
 puts("RUNSTAGE2");
 #endif
 actual=entry(dram_size);
 if(actual) return 0;
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
