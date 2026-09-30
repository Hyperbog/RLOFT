#include "PackedGrid3A78.h"
namespace ravenloft::game {
namespace {
void pass(std::span<std::uint8_t>b,std::size_t base,const std::array<std::uint8_t,8>&lim,
          const std::array<std::int32_t,8>&cnt){
 for(int row=0;row<8;++row){
   unsigned lowSum=0,highSum=0;
   for(int i=0;i<cnt[row];++i){
     auto&p=b[base+row*10+i]; const unsigned lo=p&15;
     if(lo!=15){
       if(lowSum+lo>lim[row])p=(p&0xF0)|((lim[row]-lowSum)&15);
       lowSum+=p&15;
       unsigned hi=p>>4;
       if(hi>(p&15))p=(p&15)|((p&15)<<4);
       hi=p>>4;
       if(highSum+hi>lim[row])p=(p&15)|(((lim[row]-highSum)&15)<<4);
       highSum+=p>>4;
     }
   }
 }
}
}
void PackedGrid3A78::normalize3A78(PackedGrid3A78State&s,const PackedGrid3A78Ops&o,int obj,int level){
 o.call5BDC(1,obj,level-1,0x6210);
 pass(s.objectBytes,static_cast<std::size_t>(obj)*0x248+0x2194A,s.limits6208,s.counts6440);
}
void PackedGrid3A78::normalize3C04(PackedGrid3A78State&s,const PackedGrid3A78Ops&o,int obj,int level){
 const auto ob=static_cast<std::size_t>(obj)*0x248;
 const std::int8_t type=static_cast<std::int8_t>(s.objectBytes[ob+0x218F5]);
 o.call5BDC(0,obj,level-1,type==1?0x6350:0x62B0);
 if(type==1){s.objectBytes[ob+0x219A1]=0x11;if(level-1<3)s.objectBytes[ob+0x219A2]=0xFF;}
 else s.objectBytes[ob+0x219A2]=0x11;
 pass(s.objectBytes,ob+0x2199A,s.limits6208,s.counts6440);
}
}
