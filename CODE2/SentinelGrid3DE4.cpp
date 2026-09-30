#include "SentinelGrid3DE4.h"
namespace ravenloft::game {
namespace {
void apply(std::span<std::uint8_t>b,std::size_t base,const std::array<std::int32_t,8>&cnt,
           const std::array<std::array<std::uint8_t,8>,8>&tab,int level){
 for(int row=0;row<8;++row){
   const bool on=tab[level][row]!=0;
   for(int i=0;i<cnt[row];++i){
     auto&p=b[base+row*10+i];
     if(on){if((p&0x0F)==0x0F)p=0;}
     else {if((p&0x0F)!=0x0F)p|=0xFF;}
   }
 }
}
}
void SentinelGrid3DE4::apply3DE4(SentinelGrid3DE4State&s,int obj,int level){
 const auto base=static_cast<std::size_t>(obj)*0x248;
 s.objectBytes[base+0x219A2]=0x11;
 apply(s.objectBytes,base+0x2199A,s.counts6420,s.flags62B0,level-1);
}
void SentinelGrid3DE4::apply3EA4(SentinelGrid3DE4State&s,int obj,int level){
 const auto base=static_cast<std::size_t>(obj)*0x248;
 s.objectBytes[base+0x219A1]=0x11;
 if(level-1>=3)s.objectBytes[base+0x219A2]=0x11;
 apply(s.objectBytes,base+0x2199A,s.counts6420,s.flags6350,level-1);
}
}
