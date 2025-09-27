#define CONFIG_MEM_LOG
#ifndef ASSEMBLY
typedef unsigned int uint;//i just like it this way
#endif
#define CONFIG_PRIO_MMC0
#define CONFIG_NO_BACKUP_MMC
#define CONFIG_EDGE_OPTIM //remove edge-case handling
#define CONFIG_CONCISE_LOGS //makes logs even more concise
