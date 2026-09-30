#include "NearbyEntityAED8.h"
#include <cstdlib>
namespace ravenloft::game {
static std::uint16_t rw(const std::uint8_t*p,int n){return std::uint16_t(p[n])|(std::uint16_t(p[n+1])<<8);}
static std::int16_t sw(const std::uint8_t*p,int n){return std::int16_t(rw(p,n));}
void NearbyEntityAED8::run(NearbyEntityAED8State&s,const NearbyEntityAED8Ops&o,int ch,bool alt){
 if(s.characters[ch*0x248+0x133]&2)return;
 int budget=o.dice1C328(4,0x14,0,0);int d20=o.dice1C328(1,0x14,0,0);int score=o.value22E40(ch,alt?1:3)-(alt?2:0);int gate=o.dice1C328(2,6,0,0);std::array<int,5> rolls{};
 for(int i=0;i<s.entityCount&&budget>0;++i){auto*e=s.entities+i*0x16;unsigned cell=rw(e,0x0c),type=rw(e,0x0e);if(!o.cellFlag40(cell))continue;auto*m=s.typeTable+type*0x45;if(!(m[0x3a]&0x20))continue;
   if(std::abs(s.playerX-*reinterpret_cast<std::int32_t*>(e))>=0xa0000||std::abs(s.playerY-*reinterpret_cast<std::int32_t*>(e+4))>=0xa0000)continue;
   int level=m[0x1d],index=12-level+score;if(gate==0){if(level>=6||level<=0||rolls[level-1]==0)continue;}
   int tv=o.table67B2(index);
   if(tv<0){o.effectE284(e,m,d20,sw(e,0x12),0);--budget;if(tv==-2&&level>0&&level<=5)rolls[level-1]=o.dice1C328(2,4,0,0);}
   else {if(m[0x31]&4)d20-=4;if(tv<=d20){--budget;e[8]|=0x10;}}
 }
}}
