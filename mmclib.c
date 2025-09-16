/*taken from u-boot sunxi_mmc too(but actually adapted)*/
/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Aaron <leafy.myeh@allwinnertech.com>
 *
 * MMC driver for allwinner sunxi platform.
 *
 * This driver is used by the (ARM) SPL with the legacy MMC interface, and
 * by U-Boot proper using the full DM interface. The actual hardware access
 * code is common, and comes first in this file.
 * The legacy MMC interface implementation comes next, followed by the
 * proper DM_MMC implementation at the end.
 */
/*minor modifications by 6f6626/camelStyleUser*/
#include "mmcdrv.h"
#include "slib.h"
#include "head.h"
int sd_version=1;
unsigned int rca=1;
unsigned int cid[4];
unsigned int csd[4];
int init_mmc_dev(int num){
 struct mmc_cmd cmd;
 int i;
 if(init_mmc(num)) return -1;
 delay(0x10000);
 cmd.cmdidx=0;
 cmd.arg=0;
 cmd.rsptyp=MMC_RSP_NONE;
 if(send_cmd(&cmd)) return -1;
 delay(0x100000);
 cmd.cmdidx=8;//WTF
 cmd.arg=0x1aa;
 cmd.rsptyp=MMC_RSP_R7;
 if(send_cmd(&cmd)) goto a41initer;
 if((cmd.resp[0]&0xff)==0xaa) sd_version=2;
 a41initer:
 for (i = 0; i < 25; i++) {
  cmd.cmdidx = 55;
  cmd.arg = 0;
  cmd.rsptyp = MMC_RSP_R1;
  send_cmd(&cmd);
  cmd.cmdidx = 41;
  cmd.arg = ((sd_version == 2) ? (1 << 30) : 0)|(1<<20);
  cmd.rsptyp = MMC_RSP_R3;
  send_cmd(&cmd);
  if (cmd.resp[0] & (1U << 31)) goto init_succ;
  delay(0x200000);
 }
 //something before here didn't work then
 //sd_version=0;
 //for (i=0; i < 25; i++) {
 // cmd.cmdidx=1;
 // cmd.arg=0;
 // cmd.rsptyp=MMC_RSP_R3;
 // send_cmd(&cmd);
 // if (cmd.resp[0] & (1U << 31)) goto init_succ;
 // delay(0x200000);
 //}
 return -1;
 init_succ:
 cmd.cmdidx=2;
 cmd.arg=0;
 cmd.rsptyp=MMC_RSP_R2;
 if(send_cmd(&cmd)) return -1;
 for(int i=0;i<4;i++) cid[i]=cmd.resp[i];
 cmd.cmdidx=3;
 cmd.arg=rca<<16;
 cmd.rsptyp=MMC_RSP_R6;
 if(send_cmd(&cmd)) return -1;
 rca=cmd.resp[0]>>16;
 cmd.cmdidx=9;
 cmd.rsptyp=MMC_RSP_R2;
 cmd.arg=rca<<16;
 if(send_cmd(&cmd)) return -1;
 for(int i=0;i<4;i++) csd[i]=cmd.resp[i];
 cmd.cmdidx=7;
 cmd.arg=rca<<16;
 cmd.rsptyp=MMC_RSP_R1b;
 if(send_cmd(&cmd)) return -1;
 cmd.cmdidx=16;
 cmd.arg=512;
 cmd.rsptyp=MMC_RSP_R1;
 if(send_cmd(&cmd)) return -1;
 //uhh i sure hope it worked
 return 0;
}
int read_block(int blockpos,unsigned int *data){
 struct mmc_cmd cmd;
 cmd.cmdidx=17;
 cmd.arg=(sd_version==2)?blockpos:(blockpos*512);
 cmd.rsptyp=MMC_RSP_R1|MMC_RSP_HASDATA;
 cmd.blksz=512;
 cmd.blocks=1;
 cmd.data=(unsigned char*)data;
 return send_cmd(&cmd);
}
int write_block(int blockpos,unsigned int *data){
 struct mmc_cmd cmd;
 cmd.cmdidx=24;
 cmd.arg=(sd_version==2)?blockpos:(blockpos*512);
 cmd.rsptyp=MMC_RSP_R1|MMC_RSP_HASDATA|MMC_RSP_ISW;
 cmd.blksz=512;
 cmd.blocks=1;
 cmd.data=(unsigned char*)data;
 return send_cmd(&cmd);
}
int shutdown_mmc_dev(int num){
 struct mmc_cmd cmd;
 cmd.cmdidx=15;
 cmd.rsptyp=MMC_RSP_NONE;
 cmd.arg=rca<<16;
 if(send_cmd(&cmd)) return -1;
 return shutdown_mmc(num);
}
