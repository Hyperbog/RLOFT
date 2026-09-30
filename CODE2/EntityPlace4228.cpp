#include "EntityPlace4228.h"
#include <cstdlib>
#include <cstring>
namespace ravenloft::game {
namespace {
template<class T>T rd(const std::uint8_t*p){T v;std::memcpy(&v,p,sizeof v);return v;}
template<class T>void wr(std::uint8_t*p,T v){std::memcpy(p,&v,sizeof v);}
}
int EntityPlace4228::run(EntityPlace4228State&s,const EntityPlace4228Ops&o){
 for(int cat=0;cat<3;++cat){
  const auto target=s.targets258AA[cat];
  for(int i=0;i<s.entityCount258A2;++i){
   auto*p=s.entities14D78.data()+i*0x16;
   if((p[0x13]&2)||(rd<std::uint16_t>(p+0x14)&0x40)||rd<std::uint16_t>(p+0x16)!=static_cast<std::uint16_t>(target))continue;
   const auto ex=rd<std::int16_t>(p+0x0E),ey=rd<std::int16_t>(p+0x0A);
   bool found=false;
   if(std::abs(int(ex)-static_cast<std::int16_t>(s.playerX255D0>>16))>10 &&
      std::abs(int(ey)-static_cast<std::int16_t>(s.playerY255D4>>16))>10){
    const auto oldA=rd<std::int32_t>(p+8),oldB=rd<std::int32_t>(p+12);
    wr(p+12,s.playerX255D0); wr(p+8,s.playerY255D4);
    auto tryPos=[&](int dx,int dy){wr(p+0x0E,std::int16_t(ex+dx));wr(p+0x0A,std::int16_t(ey+dy));
      return o.call2D5D0(rd<std::int32_t>(p+8),rd<std::int32_t>(p+12),i)!=0;};
    found=tryPos(0,1)||tryPos(0,-1)||tryPos(-1,0)||tryPos(1,0);
    if(!found){wr(p+8,oldA);wr(p+12,oldB);}
   }
   if(found){p[0x13]=(p[0x13]&0xCF)|0x10;return 1;}
  }
 }
 return 0;
}
}
