#include "mmclib.h"
#include "config.h"
#include "mbr.h"
#pragma pack(push,1)
struct mbr_part_entry{
unsigned char status;
unsigned char chsstart[3];
unsigned char type;
unsigned char chsend[3];
unsigned int lbastart;
unsigned int len;
};
struct mbr_boot_sector{
char bootstrap[446];
struct mbr_part_entry entries[4];
unsigned short sig;
};
#pragma pack(pop)
unsigned char mbr_typ_to_btr_typ[256]={
[0x01]=PART_TYP_FAT12,
[0x04]=PART_TYP_FAT16S,
[0x06]=PART_TYP_FAT16L,
[0x0B]=PART_TYP_FAT32,
[0x0C]=PART_TYP_FAT32,
[0x0E]=PART_TYP_FAT16,
[0xEF]=PART_TYP_UEFI_SYS,
[0xEE]=PART_TYP_GPT_PROT
};
struct partition_desc descs[4];
struct mbr_boot_sector boot_sec;
int scan_mbr(){
 if(read_block(0,(unsigned int*)&boot_sec)) return -1;
 for(int i=0;i<4;i++){
  descs[i].type=mbr_typ_to_btr_typ[boot_sec.entries[i].type];
  descs[i].start=boot_sec.entries[i].lbastart;
  descs[i].len=boot_sec.entries[i].len;
 }
 return 0;
}
int curmbrpart=0;
int reset_enumerate_mbr(void);
struct partition_desc* enumerate_mbr(){
 if(curmbrpart>=4){
  (void)reset_enumerate_mbr();
  return (struct partition_desc*)0;
 }
 return &descs[curmbrpart++];
}
int reset_enumerate_mbr(){
 curmbrpart=0;
 return 0;
}
