#ifndef __FAT32_H
#define __FAT32_H
struct fat32_file{
 unsigned int clust_num;
 unsigned int size;
 #define FAT32_ATTR_READ_ONLY 0x01
 #define FAT32_ATTR_HIDDEN 0x02
 #define FAT32_ATTR_SYSTEM 0x04
 #define FAT32_ATTR_VOLUME_ID 0x08
 #define FAT32_ATTR_DIRECTORY 0x10
 #define FAT32_ATTR_ARCHIVE 0x20
 unsigned char attr;
};
struct fat32_file_handle{
struct fat32_file file;
unsigned int cur_clust_num;
unsigned int cur_loc;
};
#define FAT32_SEEK_FORWARD 0x00
#define FAT32_SEEK_BACK 0x01
#define FAT32_SEEK_SET 0x02
int fat32_seek(struct fat32_file_handle* handle,unsigned int seek_distance,unsigned char seek_flags);
int fat32_get_root_dir(struct fat32_file* file);
int fat32_traverse(struct fat32_file* curdir,struct fat32_file* next,unsigned char* name);
int fat32_get_file_handle(struct fat32_file* file,struct fat32_file_handle* handle);
unsigned int fat32_read(struct fat32_file_handle* handle,unsigned char* data,unsigned int read_size);


int find_fat32_vol_label(unsigned char [11]);
int find_fat32_vol_id(unsigned int);
#endif /* __FAT32_H */
