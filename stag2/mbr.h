#ifndef __MBR_H
#define __MBR_H
#include "part.h"
int scan_mbr(void);
struct partition_desc* enumerate_mbr(void);
int reset_enumerate_mbr(void);
#endif /*__MBR_H*/
