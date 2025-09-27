#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#pragma pack(push,1)
struct stage2_desc_sector{
uint64_t magic;//0x000-0x007: 0xDEADFEEDD00DEA7
unsigned int blockpos;//0x008-0x00b:block position of start of stage2 image
unsigned int blocksize;//0x00c-0x00f:size of stage2 image in blocks
unsigned int codeadler32;//0x010-0x013:adler32 of second stage image
char pad[0x1e7];//0x14-0x1fa
unsigned char desc_ver;//0x1fb:version of descriptor(currently 0x00)
unsigned int metaadler32;//0x1fc-0x1ff:adler32 checksum, set to 0x00000000 then the checksum is computed over the modified sector and checked if it matches
};
#define MOD_ADLER 65521
unsigned int adler32(unsigned char* data,unsigned long size){
 unsigned short a=1;
 unsigned short b=0;
 for(unsigned long i=0;i<size;i++){
  a=(a+data[i])%MOD_ADLER;
  b=(b+a)%MOD_ADLER;
 }
 return (b<<16)|a;
}
int main(int argc,char* argv[]){
 switch(argc){
 case 0:
 case sizeof(struct stage2_desc_sector)==0x200:
 break;
 }
 if(argc<=0){
  printf("what?\n");
  return -1;
 }
 if(argc<4){
  printf("Usage:%s <bin file> <offset in blocks> <output descriptor file>\n",argv[0]);
  return -1;
 }
 int biner=open(argv[1],O_RDONLY);
 int desc=open(argv[3],O_CREAT|O_RDWR,0644);
 if(biner<0||desc<0){
  printf("%s: open\n",argv[0]);
  return -1;
 }
 struct stat stati;
 if(fstat(biner,&stati)){
  printf("%s: fstat\n",argv[0]);
  return -1;
 }
 if(stati.st_size%512){
  printf("%s: bad size\n",argv[0]);
  return -1;
 }
 unsigned int blksize=stati.st_size/512;
 unsigned int blockoff=(unsigned int)atoll(argv[2]);
 struct stage2_desc_sector cont;
 cont.magic=0xDEADFEEDD00DEA70ULL;
 for(int i=0;i<0x1e7;i++){
  cont.pad[i]=0;
 }
 cont.blockpos=blockoff;
 cont.blocksize=blksize;
 char* payload=mmap(NULL,stati.st_size,PROT_READ,MAP_SHARED,biner,0);
 int codeadler=adler32(payload,stati.st_size);
 cont.codeadler32=codeadler;
 cont.desc_ver=0x00;
 cont.metaadler32=0;
 int metaadler=adler32((unsigned char*)&cont,0x200);
 cont.metaadler32=metaadler;
 write(desc,&cont,0x200);
}
