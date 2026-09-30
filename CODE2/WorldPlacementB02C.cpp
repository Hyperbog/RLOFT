#include "WorldPlacementB02C.h"
namespace ravenloft::game {
static std::uint16_t rw(const std::uint8_t*p,int n){return std::uint16_t(p[n])|(std::uint16_t(p[n+1])<<8);}
static std::int16_t sw(const std::uint8_t*p,int n){return std::int16_t(rw(p,n));}
static void ww(std::uint8_t*p,int n,unsigned v){p[n]=std::uint8_t(v);p[n+1]=std::uint8_t(v>>8);}
static void wd(std::uint8_t*p,int n,std::int32_t v){p[n]=v;p[n+1]=v>>8;p[n+2]=v>>16;p[n+3]=v>>24;}
void WorldPlacementB02C::run(WorldPlacementB02CState&s,const WorldPlacementB02COps&o,int first,int last,int steps,int mask,int unique,int kind,int res){
 auto saved=s.c02f;s.c02f=0;
 for(int a=first;a<=last;++a){int dx=0,dy=0;auto r=o.random30CA2();int dir=s.facing+a;while(dir<0)dir+=0x168;while(dir>=0x168)dir-=0x168;o.vector2DF2C(dx,dy,0xa000,dir);
   int x=0,y=0; // callers provide the original starting coordinates through the first candidate record path.
   while(o.occupied2C9DC(x,y)){x+=dx;y+=dy;}
   if(std::int16_t(y>>16)>s.height||std::int16_t(x>>16)>s.width)break;
   for(int n=0;n<=steps;++n){x+=dx;y+=dy;int cy=std::int16_t(y>>16),cx=std::int16_t(x>>16);if(cy>=s.height||cx>=s.width)break;int cell=cy*s.width+cx;auto flags=s.cells[cell*2];
     bool blocked=false;if(flags&0x80)blocked=o.cellProbe1E930(cell)!=0;else blocked=(flags&0x3f)!=0;if(blocked)break;
     int slot=s.activeCount;if(unique){for(slot=0;slot<s.activeCount;++slot)if(rw(s.records+slot*0x16,0x0c)==cell)break;}if(slot!=s.activeCount)continue;
     auto*q=s.records+slot*0x16;wd(q,0,x);wd(q,4,y);if(unique){ww(q,0,0x8000);ww(q,4,0x8000);}ww(q,0x0c,cell);ww(q,0x0e,kind);auto bits=unsigned((r*3)>>15)&7;ww(q,0x14,(rw(q,0x14)&0xfe3f)|(bits<<6));if(kind==8){auto rr=o.random30CA2();ww(q,0x14,(rw(q,0x14)&0xfe3f)|(((rr*3)>>15&7)<<6));}++s.activeCount;
     for(int i=0;i<s.entityDefs;++i){auto*d=s.defs+i*0x16;if(d[0x0b]&2||rw(d,0x0c)!=cell||!(d[0x0b]&0x10)||!sw(d,0x12))continue;auto*t=s.types+rw(d,0x0e)*0x45;if(kind==0x0b){if(rw(d,0x0e)!=8||sw(d,0x12)>10)continue;d[0x12]=std::uint8_t(sw(d,0x12)-2);o.effectE284(d,t,i,0,0);}else if(!(mask&*reinterpret_cast<const std::uint32_t*>(t+0x31)))o.effectE284(d,t,i,mask,0);}
   }
 }
 o.play28110((kind==8||kind==0)?s.resourceB+0x19:s.resourceA+res,0xff,0,1);o.refresh1F1C8();o.update1EA78();o.finish28168();
 for(int i=0;i<4;++i){o.refresh1F1C8();o.update1EA78();}
 for(;;){int i=0;for(;i<s.activeCount;++i)if(rw(s.records+i*0x16,0x0e)==kind){o.remove8C88(i);break;}if(i==s.activeCount)break;}
 s.c02f=saved;
}}
