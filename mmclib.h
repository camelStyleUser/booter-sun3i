#ifndef _MMCLIB_H
#define _MMCLIB_H
int init_mmc_dev(int num);
int shutdown_mmc_dev(int num);
int read_block(int blockpos,unsigned int* data);
int write_block(int blockpos,unsigned int* data);
#endif /*_MMCLIB_H*/
