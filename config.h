#define DRAM_FREQ 80//DRAM FREQUENCY IN MHz
#define CONFIG_MEM_LOG
//MAKE SURE TO DEFINE CONFIG_NO2NDSTAGE IF YOU DO NOT WANT IT TO LOAD A SECONDARY BOOTLOADER AND INSTEAD BRANCH TO FEL
//#define CONFIG_NO2NDSTAGE
#ifndef ASSEMBLY
typedef unsigned int uint;//i just like it this way
#endif
#define CONFIG_PRIO_MMC0
#define CONFIG_NO_BACKUP_MMC
#define CONFIG_EDGE_OPTIM //remove edge-case handling
#define CONFIG_CONCISE_LOGS //makes logs even more concise
