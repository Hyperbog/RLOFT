#include "EntityCreate8A80.h"
#include <cstring>
namespace ravenloft::game {
static void ww(std::uint8_t*p,int o,std::uint16_t v){p[o]=std::uint8_t(v);p[o+1]=std::uint8_t(v>>8);}
static void wd(std::uint8_t*p,int o,std::uint32_t v){std::memcpy(p+o,&v,4);}
std::uint8_t* EntityCreate8A80::run(EntityCreate8A80State&s,const EntityCreate8A80Ops&o,int type,int x,int y,int id,int orient,int owner,int resource){
 o.initE578(resource);if(s.count>=0x96)return nullptr;auto*p=s.entities.data()+std::size_t(s.count)*0x16;wd(p,0,x);wd(p,4,y);wd(p,8,id);
 std::int32_t packed=owner+std::int16_t(std::uint16_t(orient)*std::uint16_t(s.scale25619));ww(p,0x0c,std::uint16_t(packed));ww(p,0x0e,std::uint16_t(type));ww(p,0x10,0);
 auto flags=std::uint16_t((std::uint16_t(p[0x14])|(std::uint16_t(p[0x15])<<8))&0xc43f);flags|=std::uint16_t((resource&7)<<11);ww(p,0x14,flags);
 if(type==8)ww(p,0x12,std::uint16_t(((o.random30CA2()*10u)>>15)+0x14));++s.count;p[0x14]&=0xf9;
 int angle=orient+0x16;if(angle>=0x168)angle-=0x168;if(angle<0)angle+=0x168;int facing=(8-angle/0x2d)&7;p[0x14]&=0xc7;p[0x14]|=std::uint8_t(facing<<3);
 if(type!=8&&o.probe2C9DC(x,y))o.adjust22E90(p,0);
 if((p[0x14]&6)!=0){o.call19CCC(0);auto old=s.bf38;s.bf38=0;o.call1F1C8();o.call1EA04();o.call1EA78();s.bf38=old;o.call19CF4();}
 if(owner>=0&&owner<4){wd(s.entities.data()+std::size_t(owner)*0x248,0x14a,0);wd(s.entities.data()+std::size_t(owner)*0x248,0x15a,0);}
 return p;
}}
