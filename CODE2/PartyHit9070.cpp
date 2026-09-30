#include "PartyHit9070.h"
namespace ravenloft::game {
static std::uint16_t rw(const std::uint8_t*p,int o){return std::uint16_t(p[o])|(std::uint16_t(p[o+1])<<8);}
static std::int16_t sw(const std::uint8_t*p,int o){return std::int16_t(rw(p,o));}
void PartyHit9070::run(PartyHit9070State&s,const PartyHit9070Ops&o,std::uint8_t*e,bool active){
 int px=s.playerX,py=s.playerY,scale=1;unsigned kind=rw(e,0x0e);
 if(active){unsigned owner=(rw(e,0x14)>>11)&7;if(owner==4)scale=3;else if(owner==7)scale=5;else if(owner<s.characters.size()/0x248){auto*c=s.characters.data()+owner*0x248;for(int i=0;i<3;++i)if(std::int8_t(c[0x15+i])==4)scale=sw(c,0x20+i*2);}}
 const std::uint32_t r=0x8fa0u;auto ex=*reinterpret_cast<std::uint32_t*>(e),ey=*reinterpret_cast<std::uint32_t*>(e+4);
 if(ey<std::uint32_t(py-r)||ey>std::uint32_t(py+r)||ex<std::uint32_t(px-r)||ex>std::uint32_t(px+r))return;
 const std::uint8_t*d=(kind*0x41+0x3a<s.attackTable.size())?s.attackTable.data()+kind*0x41:nullptr;
 for(int ch=0;ch<4;++ch){auto*c=s.characters.data()+std::size_t(ch)*0x248;if((rw(c,0x10)&0xf240)!=0)continue;if(o.active14A60(0x510)==ch)continue;
  int damage=0,mode=0;
  if(active&&d){damage=o.dice1C328(d[0x38],d[0x39],d[0x3a],0);if(d[3])damage*=scale;
   if(kind==0){if(o.traitF2D4(ch,0x44)||(c[0x134]&8)){damage=o.dice1C328(d[0x38],int(d[0x39])-2,d[0x3a],0)*scale;mode=2;}}
   else if(kind==1){if(sw(c,0x5c)==0x67||sw(c,0x5e)==0x67){damage=o.dice1C328(d[0x38],int(d[0x39])-2,d[0x3a],0)*scale;mode=2;}else mode=0;}
   if(o.resistCF2C(ch,mode)){std::int32_t x=damage;damage=(x-(x>>31))>>1;}if(damage)o.damageCD44(ch,damage);
  }else if(!active){if(d&&d[0x38])damage=o.dice1C328(d[0x38],d[0x39],d[0x3a],0);else damage=o.dice1C328(1,2,0,0);o.damageCD44(ch,damage);e[0x15]&=0xfd;}
 }
}}
