#include "NearbyEntityB518.h"
#include <cstdlib>
namespace ravenloft::game {
static std::uint16_t rw(const std::uint8_t*p,int n){return std::uint16_t(p[n])|(std::uint16_t(p[n+1])<<8);}
static std::int16_t sw(const std::uint8_t*p,int n){return std::int16_t(rw(p,n));}
static std::int32_t rd(const std::uint8_t*p,int n){return std::int32_t(std::uint32_t(p[n])|(std::uint32_t(p[n+1])<<8)|(std::uint32_t(p[n+2])<<16)|(std::uint32_t(p[n+3])<<24));}
void NearbyEntityB518::run(NearbyEntityB518State&s,const NearbyEntityB518Ops&o,int ch,bool alt){
 if(s.characters[ch*0x248+0x133]&2)return;
 std::int32_t lr[5]{}; o.fill30D30(lr,0,0x14);
 int d20=o.dice1C328(1,0x14,0,0);
 int score=o.value22E40(ch,alt?1:3)-(alt?2:0);
 int gate=o.dice1C328(2,6,0,0);
 for(int i=0,off=0;i<s.entityCount;++i,off+=0x16){
  auto*e=s.entities+off; int cell=rw(e,0x0c); if(!o.cellFlag40(cell))continue;
  auto*t=s.types+rw(e,0x0e)*0x45; if(!(t[0x3a]&0x20))continue;
  if(std::abs(s.playerX-rd(e,0))>=0xa0000||std::abs(s.playerY-rd(e,4))>=0xa0000)continue;
  int level=t[0x1d],ix=12-level+score;
  if(!gate&&(level>=6||lr[level-1]==0))continue;
  int tv=o.table67B2(ix);
  if(tv<0){o.effectE284(e,t,d20,sw(e,0x12),0);--gate;if(tv==-2)lr[level-1]=o.dice1C328(2,4,0,0);}
  else {if(t[0x31]&4)d20-=4;if(tv<=d20){--gate;e[8]|=0x10;}}
 }
}}
