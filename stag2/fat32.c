#include "mmclib.h"
#include "uart.h"
#include "config.h"
#include "part.h"
#include "fat32.h"
#pragma pack(push,1)
struct fat32_vbr{
unsigned char jmp[3];
unsigned char oemname[8];
unsigned short bytes_per_sec;
unsigned char sec_per_clust;
unsigned short resv_sec_cnt;
unsigned char num_fat;
unsigned short root_ent_cnt;
unsigned short tot_sec16;
unsigned char media;
unsigned short fat_sz16;
unsigned short sec_per_trk;
unsigned short num_heads;
unsigned int hidd_sect;
unsigned int tot_sec32;
unsigned int fat_sz32;
unsigned short flags;
unsigned char ver_min;
unsigned char ver_maj;
unsigned int root_clust;
unsigned short fsinfo;
unsigned short bk_boot_sec;
unsigned char resv1[12];
unsigned char drv_num;
unsigned char resv2;
unsigned char ext_boot_sig;
unsigned int vol_id;
unsigned char vol_label[11];
unsigned char file_system_type_str[8];
unsigned char bootcode[420];
unsigned short vbr_sig;
};
struct fat32_dir_entry{
unsigned char name[11];
unsigned char attr;
unsigned char nt_res_flags;
unsigned char crt_time_tenth;
unsigned short crt_time;
unsigned short crt_date;
unsigned short last_acc_date;
unsigned short first_clust_hi;
unsigned short write_time;
unsigned short write_date;
unsigned short first_clust_lo;
unsigned int file_size;
};
#pragma pack(pop)
struct {
unsigned int start;
unsigned int first_fat_sector;
unsigned int resv_sect;
unsigned int tot_sec;
unsigned int fat_sz;
unsigned char sec_per_clust;
unsigned char num_fats;
unsigned short flags;
unsigned int root_clust;
unsigned int first_data_sector;
} partition_data;

struct fat32_dir_sector{
struct fat32_dir_entry entries[16];
};
//this is a bootloader, so no FSInfo is really needed
unsigned char buf[512];
struct partition_desc* partition; // one partition at a time
int init_fat32_part(void){
 if(read_block(partition->start,(unsigned int*)buf)) return 1;
 struct fat32_vbr* vbr=(struct fat32_vbr*)buf;
 partition_data.start=partition->start;
 partition_data.resv_sect=vbr->resv_sec_cnt;
 partition_data.first_fat_sector=partition_data.start+partition_data.resv_sect;
 partition_data.tot_sec=vbr->tot_sec32;
 partition_data.fat_sz=vbr->fat_sz32;
 partition_data.num_fats=vbr->num_fat;
 partition_data.flags=vbr->flags;
 partition_data.sec_per_clust=vbr->sec_per_clust;
 partition_data.root_clust=vbr->root_clust;
 partition_data.first_data_sector=partition->start+vbr->resv_sec_cnt+(vbr->fat_sz32*vbr->num_fat);
 return 0;
}
int fat32_get_root_dir(struct fat32_file* file){
file->clust_num=partition_data.root_clust&0x0FFFFFFF;
file->size=0;
file->attr=FAT32_ATTR_DIRECTORY;
return 0;
}
unsigned char fat32_toupper(unsigned char in){
 return (in>=0x61&&in<=0x7A)?in&0x5f:in;
}
int fat32_traverse(struct fat32_file* curdir,struct fat32_file* next,unsigned char* name){
 if(!(curdir->attr&FAT32_ATTR_DIRECTORY)) return 1;
 unsigned char n8_3[11];//no LFN is supported currently(do you expect me to have a whole unicode decoder in a bootloader?)
 for(int i=0;i<11;i++){
  n8_3[i]=(unsigned char)' ';
 }
 int state=0;
 char haddot=0;
 while(*name){
  if((*name)==(unsigned char)'.'){
   state=8;
   haddot=1;
   goto cont_get_sfn;
  }
  n8_3[state]=fat32_toupper(*name);
  if(state>=8&&(!haddot)) return 1;
  state++;
  cont_get_sfn:
  name++;
 }
 unsigned int clust=curdir->clust_num;
 unsigned int cur_data_sec=partition_data.first_data_sector+((clust-2)*partition_data.sec_per_clust);
 struct fat32_dir_sector* dir_sec=(struct fat32_dir_sector*)buf;
 char good=1;
 while((clust>=0x00000002)&&(clust<=0x0FFFFFF6)){
  for(int i=0;i<partition_data.sec_per_clust;i++){
   read_block(cur_data_sec+i,(unsigned int*)buf);
   for(int j=0;j<32;j++){
    if(dir_sec->entries[j].name[0]==0) goto no_results;
    if(dir_sec->entries[j].attr&FAT32_ATTR_VOLUME_ID) continue;
    good=1;
    for(int k=0;k<11;k++){
     good&=(n8_3[k]==dir_sec->entries[j].name[k]);
    }
    if(!good) continue;
    next->size=dir_sec->entries[j].file_size;
    next->clust_num=((dir_sec->entries[j].first_clust_hi&0x0FFF)<<16)|(dir_sec->entries[j].first_clust_lo);
    next->attr=dir_sec->entries[j].attr&0x3F;
    return 0;
   }
  }
  read_block(partition_data.start+partition_data.resv_sect+(clust/128),(unsigned int*)buf); //MMC block size is 512 bytes
  clust=((unsigned int*)buf)[clust%128]&0x0FFFFFFF;
  cur_data_sec=partition_data.first_data_sector+((clust-2)*partition_data.sec_per_clust);
 }
 if(clust<0x0FFFFFF8) return 3;
 no_results:
 return 2;
}
int fat32_get_file_handle(struct fat32_file* file,struct fat32_file_handle* handle){
 if(file->attr&FAT32_ATTR_DIRECTORY) return 1;
 handle->file.size=file->size;
 handle->file.clust_num=file->clust_num;
 handle->file.attr=file->attr;
 handle->cur_clust_num=file->clust_num;
 handle->cur_loc=0;
 return 0;
}
unsigned int fat32_read(struct fat32_file_handle* handle,unsigned char* data,unsigned int read_size){
 unsigned int sector;
 unsigned int out_index=0;
 unsigned int sector_last;
 unsigned int obtained;
 unsigned int tmp;
 while(read_size){
  sector=partition_data.first_data_sector+((handle->cur_clust_num-2)*partition_data.sec_per_clust)+((handle->cur_loc/512)%partition_data.sec_per_clust);
  read_block(sector,(unsigned int*)buf);
  sector_last=(handle->cur_loc%512)+read_size;
  sector_last=sector_last>512?512:sector_last;
  for(int i=handle->cur_loc%512;i<sector_last;i++){
   data[out_index++]=buf[i];
  }
  obtained=sector_last-(handle->cur_loc%512);
  read_size-=obtained;
  tmp=handle->cur_loc%(512*partition_data.sec_per_clust);
  handle->cur_loc+=obtained;
  if(handle->cur_loc%(512*partition_data.sec_per_clust)<tmp){
   read_block(partition_data.start+partition_data.resv_sect+(handle->cur_clust_num/128),(unsigned int*)buf); //MMC block size is 512 bytes
   if((((unsigned int*)buf)[handle->cur_clust_num%128]&0x0FFFFFFF)>=0x0FFFFFF7&&(read_size!=0)){
    return out_index;
   }
   handle->cur_clust_num=((unsigned int*)buf)[handle->cur_clust_num%128]&0x0FFFFFFF;
  }
 }
 return out_index;
}
int fat32_seek(struct fat32_file_handle* handle,unsigned int seek_distance,unsigned char seek_flags){
 unsigned int tmp;
 switch(seek_flags){
 case FAT32_SEEK_FORWARD:
 for(int i=0;i<(seek_distance/(512*partition_data.sec_per_clust));i++){
  read_block(partition_data.start+partition_data.resv_sect+(handle->cur_clust_num/128),(unsigned int*)buf); //MMC block size is 512 bytes
  if((((unsigned int*)buf)[handle->cur_clust_num%128]&0x0FFFFFFF)>=0x0FFFFFF7){
   return 1;
  }
  handle->cur_clust_num=((unsigned int*)buf)[handle->cur_clust_num%128]&0x0FFFFFFF;
  handle->cur_loc+=512*partition_data.sec_per_clust;
 }
 tmp=handle->cur_loc%(512*partition_data.sec_per_clust);
 handle->cur_loc+=seek_distance%(512*partition_data.sec_per_clust);
 if(handle->cur_loc%(512*partition_data.sec_per_clust)<tmp){
  read_block(partition_data.start+partition_data.resv_sect+(handle->cur_clust_num/128),(unsigned int*)buf); //MMC block size is 512 bytes
  if((((unsigned int*)buf)[handle->cur_clust_num%128]&0x0FFFFFFF)>=0x0FFFFFF7){
   return 1;
  }
  handle->cur_clust_num=((unsigned int*)buf)[handle->cur_clust_num%128]&0x0FFFFFFF;
 }
 break;
 case FAT32_SEEK_BACK:
 seek_distance=handle->cur_loc-seek_distance;
 /* FALLTHRU */
 case FAT32_SEEK_SET:
 handle->cur_clust_num=handle->file.clust_num;
 for(int i=0;i<(seek_distance/(512*partition_data.sec_per_clust));i++){
  read_block(partition_data.start+partition_data.resv_sect+(handle->cur_clust_num/128),(unsigned int*)buf); //MMC block size is 512 bytes
  if((((unsigned int*)buf)[handle->cur_clust_num%128]&0x0FFFFFFF)>=0x0FFFFFF7){
   return 1;
  }
  handle->cur_clust_num=((unsigned int*)buf)[handle->cur_clust_num%128]&0x0FFFFFFF;
  handle->cur_loc+=512*partition_data.sec_per_clust;
 }
 handle->cur_loc+=seek_distance%(512*partition_data.sec_per_clust);
 break;
 }
 return 2;
}
int find_fat32_vol_label(unsigned char name[11]){
 for(int i=10;i>-1;i--){
  if(name[i]!=0) break;
  name[i]=' ';
 }
 struct partition_desc* ptr=(struct partition_desc*)0;
 int good=0;
 struct fat32_vbr* vbr=(struct fat32_vbr*)buf;
 do{
  good=1;
  ptr=enumerate_part();
  if(ptr->type!=PART_TYP_FAT32) continue;
  if(read_block(ptr->start,(unsigned int*)buf)) return 1; // something is seriously wrong if i cannot read a sector
  if(vbr->vbr_sig!=0xaa55) continue;
  for(int i=0;i<11;i++){
   good&=(vbr->vol_label[i]==name[i]);
  } //TODO: some OSes do not change the vol_label in the vbr, so check root dir
  if(good){
   partition=ptr;
   return init_fat32_part();
  }else continue;
 }while(ptr);
 return 1;
}
int find_fat32_vol_id(unsigned int vol_id){
 struct partition_desc* ptr=(struct partition_desc*)0;
 struct fat32_vbr* vbr=(struct fat32_vbr*)buf;
 do{
  ptr=enumerate_part();
  if(ptr->type!=PART_TYP_FAT32) continue;
  read_block(ptr->start,(unsigned int*)buf);
  if(vbr->vbr_sig!=0xaa55) continue;
  if(vbr->vol_id==vol_id){
   partition=ptr;
   return init_fat32_part();
  }else continue;
 }while(ptr);
 return 1;
}
