#include "EntityInteraction8CF4.h"
namespace ravenloft::game {
static std::uint16_t rw(const std::uint8_t*p,int o){return std::uint16_t(p[o])|(std::uint16_t(p[o+1])<<8);}
int EntityInteraction8CF4::run(EntityInteraction8CF4State&s,const EntityInteraction8CF4Ops&o,std::uint8_t*src,bool active){
 int result=1;int mode=1;if(active){unsigned owner=(rw(src,0x14)>>11)&7;if(owner==4)mode=3;else if(owner<s.actors.size()/0x248){auto*a=s.actors.data()+owner*0x248;for(int i=0;i<3;++i)if(std::int8_t(a[0x15+i])==4)mode=std::int16_t(rw(a,0x20+i*2));}}
 for(int i=0;i<s.entityCount;++i){auto*e=s.entities.data()+std::size_t(i)*0x16;if(e[0x0b]&2)continue;unsigned kind=rw(e,0x0e);if(kind>=0x22&&kind!=0x50)continue;
  int radius=0;if(kind*0x45+0x16<s.typeTable.size())radius=rw(s.typeTable.data()+kind*0x45,0x16);
  auto sx=*reinterpret_cast<std::uint32_t*>(src),sy=*reinterpret_cast<std::uint32_t*>(src+4),ex=*reinterpret_cast<std::uint32_t*>(e),ey=*reinterpret_cast<std::uint32_t*>(e+4);
  if(sy<ey-radius||sy>ey+radius||sx<ex-radius||sx>ex+radius)continue;
  if(active){bool allow=true;switch(rw(src,0x0e)){case 0:allow=(e[0x32%0x16]&0x21)!=0;break;case 1:allow=(e[0x32%0x16]&3)!=0;break;case 2:case 3:allow=(e[0x32%0x16]&9)!=0;break;case 4:allow=(e[0x32%0x16]&0x21)!=0;break;case 6:allow=(e[0x32%0x16]&1)!=0;break;default:break;}if(!allow){result=0;continue;}}
  if(kind*0x41+0x3b<=s.diceTable.size()){auto*d=s.diceTable.data()+kind*0x41;int damage=d[0x38]?o.dice1C328(d[0x38],d[0x39],d[0x3a],d[0x37]):0;if(damage)o.applyE284(e,i,damage,mode,active?1:0);}
 }
 return result;
}}
