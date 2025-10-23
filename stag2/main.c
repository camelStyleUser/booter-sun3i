#include "config.h"
#include "slib.h"
#include "mmclib.h"
#include "part.h"
#include "fat32.h"
char* old_sp;
char stack[2048];
#ifdef CONFIG_MEM_LOG
char memlog[2048]={0xfe,0x01,0xfe,0x01,0xfe,0x01,0xfe,0x01};
char* memlogcurchar=memlog+4;
#endif
int main(int dram_size){
struct fat32_file root;
struct fat32_file child;
struct fat32_file_handle handle;
puts_nonl("received dram size from stage 1:");
print_dec(dram_size);
puts("MiB");
#ifdef CONFIG_PRIO_MMC0
init_mmc_dev(0);
#else
init_mmc_dev(2);
#endif
scan_for_part();
unsigned char label[11];
for(int i=0;i<5;i++){
label[i]="BOOT"[i];
}
for(int i=5;i<11;i++){
label[i]=0;
}
find_fat32_vol_label(label);
fat32_get_root_dir(&root);
if(fat32_traverse(&root,&child,(unsigned char*)"DATA.TXT")){
 puts("could not find DATA.TXT :(");
 goto err;
}
fat32_get_file_handle(&child,&handle);
unsigned int size=child.size;
unsigned char buffer[65];
while(size){
if(fat32_read(&handle,buffer,(size<64)?size:64)<((size<64)?size:64)){
 puts("not enough bytes");
 goto err;
}
if(size<64){
for(int i=size;i<64;i++){
buffer[i]=0;
}
size=0;
}else size-=64;
buffer[64]=0;
puts_nonl((char*)buffer);
}
puts_nonl("\nRead ");
print_dec(child.size);
puts(" bytes.");
err:
#ifdef CONFIG_PRIO_MMC0
shutdown_mmc(0);
#else
shutdown_mmc(2);
#endif
return 0;
}
