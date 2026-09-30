#include "CharacterEffect9D90.h"
namespace ravenloft::game {
static std::uint16_t rw(const std::uint8_t*p,int n){return std::uint16_t(p[n])|(std::uint16_t(p[n+1])<<8);}
static void ww(std::uint8_t*p,int n,std::uint16_t v){p[n]=std::uint8_t(v);p[n+1]=std::uint8_t(v>>8);}
static std::uint32_t& dw(std::uint8_t*p,int n){return *reinterpret_cast<std::uint32_t*>(p+n);}
int CharacterEffect9D90::run(CharacterEffect9D90State&s,const CharacterEffect9D90Ops&o,int effect,int ch,int target){
 auto*c=s.characters+ch*0x248;auto*t=s.characters+target*0x248;int ok=1;
 auto heal=[&](int n){int v=std::int16_t(rw(c,0x1c))+n,m=std::int16_t(rw(c,0x1e));if(v>m)v=m;ww(c,0x1c,std::uint16_t(v));};
 switch(effect){
 case -1:c[0x11]&=0xf3;heal(o.dice(1,4,0,8));break;
 case 0x51:{int n=o.dice(1,8,0,0);if(t[0x133]&0x80){n=8;t[0x133]&=0x7f;}heal(n);break;}
 case 0x57:{int n=o.value22E40(1,target)*2;heal(n);break;}
 case 0x64:if(!(rw(c,0x10)&0x0400))return ok;c[0x11]&=0xfb;break;
 case 0x68:if(!(rw(c,0x10)&0x1000))return ok;c[0x11]&=0xef;break;
 case 0x6e:{int n=o.dice(2,8,0,1);if(t[0x133]&0x80){n=0x10;t[0x133]&=0x7f;}heal(n);break;}
 case 0x71:if(!(rw(c,0x10)&0x0800))return ok;c[0x11]&=0xf7;break;
 case 0x78:{int n=o.dice(3,8,0,3);if(t[0x133]&0x80){n=0x1b;t[0x133]&=0x7f;}heal(n);break;}
 case 0x7b:{
   if(!(rw(c,0x10)&0x4000)){o.message(c,0);return 0;}
   if(rw(c,0x10)&0x8000)return ok;
   if(c[0x14]==5){o.message(c,2);return 0;}
   if(rw(c,0x20)==0||rw(c,0x22)==0||rw(c,0x24)==0){o.message(c,1);return 0;}
   if(c[0x37]&&int(c[0x37])-int(c[0x36])<5){ww(c,0x1c,1);c[0x11]&=0xbf;--c[0x37];o.message(c,3);return 1;}
   o.message(c,1);return 0;
 }
 case 0x82:{auto max=rw(c,0x1e);ww(c,0x1c,max);auto st=rw(c,0x10);st&=0x01c0;ww(c,0x10,st);c[0x17a]=1;break;}
 default:break;
 }
 return ok;
}}
