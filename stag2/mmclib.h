#ifndef _MMCLIB_H
#define _MMCLIB_H
int init_mmc_dev(int num);
int shutdown_mmc_dev(int num);//SHUTS DOWN SD CARD TOO
int shutdown_mmc(int);//doesnt shut down sd card, only controller
int read_block(int blockpos,unsigned int* data);
int write_block(int blockpos,unsigned int* data);
#endif /*_MMCLIB_H*/
