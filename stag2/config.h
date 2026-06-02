//#define CONFIG_MEM_LOG
#ifndef __ASSEMBLER__
typedef unsigned int uint;//i just like it this way
#endif
#define CONFIG_PRIO_MMC0
#define CONFIG_NO_BACKUP_MMC
#define CORE_PLL_FREQ 240 //in MHz, conditions are in stage1's head.S
//#define CONFIG_EDGE_OPTIM //remove edge-case handling
//#define CONFIG_CONCISE_LOGS //makes logs even more concise
