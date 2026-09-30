#include "NearbySpawn9368.h"
namespace ravenloft::game {
static std::uint16_t rw(const std::uint8_t*p,int o){return std::uint16_t(p[o])|(std::uint16_t(p[o+1])<<8);}
static std::int16_t sw(const std::uint8_t*p,int o){return std::int16_t(rw(p,o));}
void NearbySpawn9368::run(NearbySpawn9368State&s,const NearbySpawn9368Ops&o,const std::uint8_t*e){
 int dy=sw(e,2)-s.playerY;if(dy<0)dy=-dy;int dx=sw(e,6)-s.playerX;if(dx<0)dx=-dx;int code=-1;
 switch(rw(e,0x0e)){case 0:code=0x1c;break;case 1:code=0x37;break;case 2:code=0x1e;break;case 6:code=0x39;break;default:break;}
 auto flags=rw(e,0x14)&0x3800;if(flags>=0x2000&&!o.ownerFlag198E0(rw(e,0x0c))&&(dy>=10||dx>=10))return;
 if(code!=-1)o.spawn28110(s.resource258D4+code,0xff,1,1);
}}
