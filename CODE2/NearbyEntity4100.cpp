#include "NearbyEntity4100.h"
#include <cstdlib>
namespace ravenloft::game {
namespace { template<class T>T rd(const std::uint8_t*p){T v{};for(unsigned i=0;i<sizeof(T);++i)reinterpret_cast<std::uint8_t*>(&v)[i]=p[i];return v;} }
int NearbyEntity4100::run(NearbyEntity4100State&s,const NearbyEntity4100Ops&o){
 s.flag6688=0;
 if(s.mapC6C8==0x1D && s.playerX255D2>0x41)return 1;
 if(s.mapC6C8==0x0B && s.playerX255D2>=4&&s.playerX255D2<=7&&s.playerY255D6>=7&&s.playerY255D6<=0x0B)return 0;
 if(o.call4040()!=0){s.flag6688=1;return 0;}
 for(int i=0;i<s.count258A2;++i){
  const auto*p=s.entities14D78.data()+i*0x16;
  if((p[0x13]&2)!=0)continue;
  const auto gate=rd<std::uint16_t>(p+0x16);
  if(!o.call2ACDC(gate))continue;
  const auto cell=rd<std::uint16_t>(p+0x14);
  if(s.mapFlags198E0[cell]&0x40)return 1;
  const auto ey=rd<std::int16_t>(p+0x0A),ex=rd<std::int16_t>(p+0x0E);
  if(std::abs(int(ey)-s.playerY255D6)<0x0F&&std::abs(int(ex)-s.playerX255D2)<0x0F&&
     (s.mapFlags198E0[cell]&0x0400))return 1;
 }
 return 0;
}
}
