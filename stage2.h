#ifndef STAGE2_H
#define STAGE2_H
//really should have been called stage2ldr but whatever
#define CURR_DESC_VER 0x00
#define DESC_MAGIC (0xDEADFEEDD00DEA70ULL)
#pragma pack(push,1)
struct stage2_desc_sector{
unsigned long long magic;//0x000-0x007: 0xDEADFEEDD00DEA70
unsigned int blockpos;//0x008-0x00b:block position of start of stage2 image
unsigned int blocksize;//0x00c-0x00f:size of stage2 image in blocks
unsigned int codeadler32;//0x010-0x013:adler32 of second stage image
char pad[0x1e7];//0x14-0x1fa
unsigned char desc_ver;//0x1fb:version of descriptor(currently 0x00)
unsigned int metaadler32;//0x1fc-0x1ff:adler32 checksum, set to 0x00000000 then the checksum is computed over the modified sector and checked if it matches
};
#pragma pack(pop)
void stage2(void);
#endif
