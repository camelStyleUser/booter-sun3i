/*taken from u-boot sunxi_mmc too(but actually adapted)*/
#include "mmcdrv.h"
#include "slib.h"
int sd_version=1;
int init_mmc_dev(int num){
 struct mmc_cmd cmd;
 volatile int i;
 volatile int j;
 init_mmc(num);
 cmd.cmdidx=0;
 cmd.arg=0;
 cmd.rsptyp=MMC_RSP_NONE;
 if(send_cmd(&cmd)) return -1;
 for(i=0;i<0x10000;i++);
 cmd.cmdidx=8;//WTF
 cmd.arg=0x1aa;//surely we always support 3.3, right?
 cmd.rsptyp=MMC_RSP_R7;
 cmd.resp[0]=0;
 if(send_cmd(&cmd)) return -1;
 if((cmd.resp[0]&0xff)==0xaa) sd_version=2;
 print_hex(cmd.resp[0]);
 return -1;
 for (i = 0; i < 25; i++) {
  cmd.cmdidx = 55;
  cmd.arg = 0;
  cmd.rsptyp = MMC_RSP_R1;
  send_cmd(&cmd);
  cmd.cmdidx = 41;
  cmd.arg = (sd_version == 2) ? (1 << 30) : 0;
  cmd.rsptyp = MMC_RSP_R3;
  send_cmd(&cmd);
  puts_nonl("A41:");
  print_hex(cmd.resp[0]);
  puts("");
  if (cmd.resp[0] & (1U << 31)) goto init_succ;
  for(j=0x2000;j>0;j--);
 }
 //something before here didn't work then
 return -1;
 init_succ:
 cmd.cmdidx=2;
 cmd.arg=0;
 cmd.rsptyp=MMC_RSP_R2;
 send_cmd(&cmd);
 cmd.cmdidx=3;
 cmd.arg=0;
 cmd.rsptyp=MMC_RSP_R3;
 send_cmd(&cmd);
 unsigned int rca=cmd.resp[0]>>16;
 cmd.cmdidx=7;
 cmd.arg=rca<<16;
 cmd.rsptyp=MMC_RSP_R1b;
 send_cmd(&cmd);
 cmd.cmdidx=16;
 cmd.arg=512;
 cmd.rsptyp=MMC_RSP_R1;
 send_cmd(&cmd);
 //uhh i sure hope it worked
 return 0;
}
int read_block(int blockpos,unsigned int *data){
 struct mmc_cmd cmd;
 cmd.cmdidx=17;
 cmd.arg=(sd_version==2)?blockpos:blockpos*512;
 cmd.rsptyp=MMC_RSP_R1|MMC_RSP_HASDATA;
 cmd.blksz=512;
 cmd.blocks=1;
 cmd.data=(unsigned char*)data;
 return send_cmd(&cmd);
}
int write_block(int blockpos,unsigned int *data){
 struct mmc_cmd cmd;
 cmd.cmdidx=24;
 cmd.arg=(sd_version==2)?blockpos:blockpos*512;
 cmd.rsptyp=MMC_RSP_R1|MMC_RSP_HASDATA|MMC_RSP_ISW;
 cmd.blksz=512;
 cmd.blocks=1;
 cmd.data=(unsigned char*)data;
 return send_cmd(&cmd);
}
int shutdown_mmc_dev(int num){
return shutdown_mmc(num);
}
