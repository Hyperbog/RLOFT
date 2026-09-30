#include "SaveSlotList7070.h"
#include <cstdio>
#include <cstring>
namespace ravenloft::game {
void SaveSlotList7070::build(SaveSlotList7070State&s,const SaveSlotList7070Ops&o,int baseX,int baseY,int arg2){
 int x=0,y=0x15,dx=0,dy=0x1d;int label=0x6461;
 for(int i=0;i<10;++i,label+=0x32,x+=2,y+=0x25,dx+=4,dy+=0x25){
  char name[16]{};std::snprintf(name,sizeof(name),"%02d",i+1);int f=o.open312A0(name);
  if(f==-1){if(s.emptyLabel)std::strcpy(reinterpret_cast<char*>(std::uintptr_t(label)),s.emptyLabel);}
  else{o.read312EC(reinterpret_cast<void*>(std::uintptr_t(label)),0x32,f);o.close312DC(f);}
  s.surfaces[i]=o.alloc30070(baseX,0x64,0x0f,0xd8,0xff);
  o.draw2975C(x,baseY,0x14,arg2,0);o.draw3060C(baseY,0x64,label,1);
 }
}
void SaveSlotList719C::destroy(SaveSlotList719CState&s,const SaveSlotList719COps&o){for(auto p:s.surfaces)o.free30040(p);}
}
