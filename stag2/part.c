#include "part.h"
#include "config.h"
#include "mbr.h"
//TODO:add GPT support
int scan_for_part(){
if(scan_mbr()) return 1;
return 0;
}
#define ENUM_TYPE_MBR 0
int curenumtyp=ENUM_TYPE_MBR;
#define ENUM_TYPE_COUNT 1
#define LAST_ENUM_TYPE (ENUM_TYPE_COUNT-1)
int reset_enumerate_part(void);
struct partition_desc* enumerate_part(){
 struct partition_desc* ptr=(struct partition_desc*)0;
 while(!ptr){
  if(curenumtyp>LAST_ENUM_TYPE){
   (void)reset_enumerate_part();
   return (struct partition_desc*)0;
  }
  switch(curenumtyp){
  case 0:
  ptr=enumerate_mbr();
  break;
  }
  if(!ptr){//do this until we reach the last provider or we get a non-null partition_desc*
   curenumtyp++;
  }
 }
 return ptr;
}
int reset_enumerate_part(){
 curenumtyp=0;
 reset_enumerate_mbr();
 return 0;
}
