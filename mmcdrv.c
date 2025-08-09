//taken from u-boot sunxi_mmc driver
#include "config.h"
#include "mmcdrv.h"
#include "pio.h"
#include "slib.h"
#define CCM_BASE 0x01c20000
#define CCM_REG_AHB_GAT_O 0xc
#define CCM_REG_SD23_CLK_R_O 0x1c
#define CCM_REG_SD01_CLK_R_O 0x18
#define MMC_BASE_OFFSET 0x01c0f000
struct sunxi_mmc *mmc_offset;
int mmc_num;
volatile unsigned int* clkr;
int send_raw_cmd_value(int,struct sunxi_mmc*);
int mmc_set_mod_clk(unsigned int hz)
{
	unsigned int pll, pll_hz, div, n, oclk_dly, sclk_dly;
	u32 val = 0;
	if (hz <= 24000000) {
		pll = (0<<24);
		pll_hz = 24000000;
	} else {
		return -1;
	}

	div = pll_hz / hz;
	if (pll_hz % hz)
		div++;

	n = 0;
	while (div > 16) {
		n++;
		div = (div + 1) / 2;
	}

	if (n > 3) {
		return -1;
	}

	/* determine delays */
	if (hz <= 400000) {
		oclk_dly = 0;
		sclk_dly = 0;
	} else if (hz <= 25000000) {
		oclk_dly = 0;
		sclk_dly = 5;
	} else if (hz <= 52000000) {
		oclk_dly = 3;
		sclk_dly = 4;
	} else {
		/* hz > 52000000 */
		oclk_dly = 1;
		sclk_dly = 4;
	}
	val = (oclk_dly<<8) | (sclk_dly<<20);

	*clkr=(1<<31)| pll | (n<<16) |
	       (div-1) | val;

	return 0;
}
int init_mmc(int num){
 mmc_num=num;
 if(num==2){
  *(volatile uint*)(CCM_BASE+CCM_REG_AHB_GAT_O)=*(volatile uint*)(CCM_BASE+CCM_REG_AHB_GAT_O)|0x200;
  *(volatile uint*)(CCM_BASE+CCM_REG_SD23_CLK_R_O)=((*(volatile uint*)(CCM_BASE+CCM_REG_SD23_CLK_R_O))&0xffffff00)|0xb0;
  SUNXI_PIO_CTRL->banks[2].cfg[0]|=0x3300000;
  SUNXI_PIO_CTRL->banks[2].cfg[1]|=0x30000;
  SUNXI_PIO_CTRL->banks[2].pull[0]|=0x1500;
  clkr=(volatile uint*)(CCM_BASE+CCM_REG_SD23_CLK_R_O);
 }else{
  *(volatile uint*)(CCM_BASE+CCM_REG_AHB_GAT_O)=*(volatile uint*)(CCM_BASE+CCM_REG_AHB_GAT_O)|0x80;
  *(volatile uint*)(CCM_BASE+CCM_REG_SD01_CLK_R_O)=((*(volatile uint*)(CCM_BASE+CCM_REG_SD01_CLK_R_O))&0xffffff00)|0xb0;
  SUNXI_PIO_CTRL->banks[0].cfg[0]|=0x2220;
  SUNXI_PIO_CTRL->banks[0].pull[0]|=0x54;
  clkr=(volatile uint*)(CCM_BASE+CCM_REG_SD01_CLK_R_O);
 }
 mmc_offset=(struct sunxi_mmc*)(MMC_BASE_OFFSET+0x1000*num);
 mmc_offset->gctrl=0x7;
 int break_delay=0xffff;
 for(;break_delay>0;break_delay--){
  if((mmc_offset->gctrl&0x7)==0){
   mmc_offset->rint=mmc_offset->rint|0xffff;
   mmc_offset->gctrl=0x200;
   mmc_offset->ftrglevel=0x70008;
   goto success_reset;
  }
 }
 return -1;
 success_reset:
 mmc_offset->timeout=0xffffffff;
 mmc_offset->blksz=512;
 mmc_offset->width=0;
 mmc_offset->clkcr&=~SUNXI_MMC_CLK_ENABLE;
 send_raw_cmd_value(0x80202000,mmc_offset);
 //should change clock somewhere here
 mmc_set_mod_clk(100000);//100khz
 mmc_offset->clkcr&=~0xff;
 send_raw_cmd_value(0x80202000,mmc_offset);
 mmc_set_mod_clk(100000);//100khz
 mmc_offset->clkcr|=SUNXI_MMC_CLK_ENABLE;
 send_raw_cmd_value(0x80202000,mmc_offset);
 //TODO:ok, now what?
 //nothing?
 mmc_offset->gctrl=0x7;
 return 0;
}
int send_raw_cmd_value(int val,struct sunxi_mmc *ctrlr){
 ctrlr->cmd=val;
 int delay=0xffff;//for example
 while((ctrlr->cmd&SUNXI_MMC_CLK_ENABLE)!=0){
  if(!delay) return -1;
  delay--;
 }
return 0;
}
int shutdown_mmc(int num){
 if(num==2){
  *(volatile uint*)(CCM_BASE+CCM_REG_AHB_GAT_O)=*(volatile uint*)(CCM_BASE+CCM_REG_AHB_GAT_O)&(~0x200);
  *(volatile uint*)(CCM_BASE+CCM_REG_SD23_CLK_R_O)=*(volatile uint*)(CCM_BASE+CCM_REG_SD23_CLK_R_O)&0xffffffc0&(~0xb0);
  SUNXI_PIO_CTRL->banks[2].cfg[0]&=~0x3300000;
  SUNXI_PIO_CTRL->banks[2].cfg[1]&=~0x30000;
  SUNXI_PIO_CTRL->banks[2].pull[0]&=~0x1500;
 }else{
  *(volatile uint*)(CCM_BASE+CCM_REG_AHB_GAT_O)=*(volatile uint*)(CCM_BASE+CCM_REG_AHB_GAT_O)&(~0x200);
  *(volatile uint*)(CCM_BASE+CCM_REG_SD23_CLK_R_O)=*(volatile uint*)(CCM_BASE+CCM_REG_SD23_CLK_R_O)&0xffffffc0&(~0xb0);
  SUNXI_PIO_CTRL->banks[0].cfg[0]&=~0x2220;
  SUNXI_PIO_CTRL->banks[0].pull[0]&=~0x54;
 }
return 0;
}

int mmc_rint_wait(int timeout,uint wait_bit){
uint status;
do{
status=mmc_offset->rint;
timeout--;
if(timeout<0||(status&SUNXI_MMC_RINT_INTERRUPT_ERROR_BIT)) return -1;
}while(!(status&wait_bit));
return 0;
}
int mmc_trans_data_by_cpu(uint *data,int isw,int bytecnt){
 int timeout=0xfffff;
 uint status_bit=isw?SUNXI_MMC_STATUS_FIFO_FULL:SUNXI_MMC_STATUS_FIFO_EMPTY;
 mmc_offset->gctrl|=SUNXI_MMC_GCTRL_ACCESS_BY_AHB;
 
 for (int i = 0; i < (bytecnt >> 2); i++) {
  while (mmc_offset->status & status_bit) {
   if ((--timeout)<0) return -1;
  }

  if (!isw) data[i] = mmc_offset->fifo;
  else mmc_offset->fifo=data[i];
 }
 return 0;
}
int send_cmd(struct mmc_cmd *cmd){
 int error=0;
 uint cmdv=SUNXI_MMC_CMD_START;
 if(cmd->cmdidx==12) return 0;//probably autosent
 if(cmd->cmdidx==0) cmdv|=SUNXI_MMC_CMD_SEND_INIT_SEQ;
 if(cmd->rsptyp&MMC_RSP_PRESENT) cmdv|=SUNXI_MMC_CMD_RESP_EXPIRE;
 if(cmd->rsptyp&MMC_RSP_136) cmdv|=SUNXI_MMC_CMD_LONG_RESPONSE;
 if(cmd->rsptyp&MMC_RSP_CRC) cmdv|=SUNXI_MMC_CMD_CHK_RESPONSE_CRC;
 if(cmd->rsptyp&MMC_RSP_HASDATA){
  cmdv|=SUNXI_MMC_CMD_DATA_EXPIRE|SUNXI_MMC_CMD_WAIT_PRE_OVER;
  if(cmd->rsptyp&MMC_RSP_ISW) cmdv|=SUNXI_MMC_CMD_WRITE;
  if(cmd->blocks>1) cmdv|=SUNXI_MMC_CMD_AUTO_STOP;
  mmc_offset->blksz=cmd->blksz;
  mmc_offset->bytecnt=cmd->blksz*cmd->blocks;
 }
 mmc_offset->arg=cmd->arg;
 mmc_offset->cmd=cmdv|cmd->cmdidx;
 if(cmd->rsptyp&MMC_RSP_HASDATA){
  mmc_trans_data_by_cpu((uint*)cmd->data,!!(cmd->rsptyp&MMC_RSP_ISW),cmd->blksz*cmd->blocks);
 }
 mmc_rint_wait(0xfffff,SUNXI_MMC_RINT_COMMAND_DONE);
 if(cmd->rsptyp&MMC_RSP_HASDATA) mmc_rint_wait(0xffff,cmd->blocks>1?SUNXI_MMC_RINT_AUTO_COMMAND_DONE:SUNXI_MMC_RINT_DATA_OVER);
 if(cmd->rsptyp&MMC_RSP_BUSY){
  int timeout=0xffff;
  uint status;
  do{
   status=mmc_offset->status;
   if(timeout--<0){error=-1;goto out;}
  }while(status&SUNXI_MMC_STATUS_CARD_DATA_BUSY);
 }
 if(cmd->rsptyp&MMC_RSP_136){
  cmd->resp[0]=mmc_offset->resp3;
  cmd->resp[1]=mmc_offset->resp2;
  cmd->resp[2]=mmc_offset->resp1;
  cmd->resp[3]=mmc_offset->resp0;
 }else{
  cmd->resp[0]=mmc_offset->resp0;
 }
 out:
 if(error<0) mmc_offset->gctrl=SUNXI_MMC_GCTRL_RESET;
 mmc_offset->rint=0xffffffff;
 mmc_offset->gctrl|=SUNXI_MMC_GCTRL_FIFO_RESET;
 return error;
}

