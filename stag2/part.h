#ifndef __PART_H
#define __PART_H
struct partition_desc{
unsigned char type;
unsigned int start;
unsigned int len;
};
int scan_for_part(void);
struct partition_desc* enumerate_part(void);//it may return a partition of any type(even type NONE)!!!
int reset_enumerate_part(void);
#define PART_TYP_NONE 0x00
#define PART_TYP_FAT12 0x01
#define PART_TYP_FAT16S 0x02
#define PART_TYP_FAT16L 0x03
#define PART_TYP_FAT16 0x04
#define PART_TYP_FAT32 0x05
#define PART_TYP_UEFI_SYS 0x06
#define PART_TYP_GPT_PROT 0x07
#endif /* __PART_H */
