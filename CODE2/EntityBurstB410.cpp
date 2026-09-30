#include "EntityBurstB410.h"
namespace ravenloft::game {
static std::uint16_t rw(const std::uint8_t*p,int n){return std::uint16_t(p[n])|(std::uint16_t(p[n+1])<<8);}
static void ww(std::uint8_t*p,int n,unsigned v){p[n]=v;p[n+1]=v>>8;}
static void wd(std::uint8_t*p,int n,const std::uint8_t*q){for(int i=0;i<4;++i)p[n+i]=q[i];}
void EntityBurstB410::run(EntityBurstB410State&s,const EntityBurstB410Ops&o,int idx){
 auto*d=s.defs+idx*0x16;auto*q=s.records+s.activeCount*0x16;ww(q,0x0e,0x0a);wd(q,4,d+4);wd(q,0,d);ww(q,0x0c,rw(d,0x0c));++s.activeCount;
 int roll=o.dice1C328(6,8,0,0),type=rw(d,0x0e);auto*t=s.types+type*0x45;if(!(t[0x32]&0x21))o.effectE284(d,t,idx,roll,0);
 o.play28110(s.resourceBase+0x20,0xff,0,1);o.refresh1F1C8();o.update1EA78();o.finish28168();o.refresh1F1C8();o.update1EA78();
 o.play28110(s.resourceBase+0x21,0xff,0,1);for(int i=0;i<3;++i){o.finish28168();o.refresh1F1C8();o.update1EA78();}
 o.remove8C88(s.activeCount-1);--s.activeCount;
}}
