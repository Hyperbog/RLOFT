#include "CharacterState9790.h"
namespace ravenloft::game {
static int sw(const std::uint8_t*p,int n){return std::int16_t(std::uint16_t(p[n])|(std::uint16_t(p[n+1])<<8));}
static void ww(std::uint8_t*p,int n,int v){p[n]=std::uint8_t(v);p[n+1]=std::uint8_t(unsigned(v)>>8);}
static std::uint32_t& dw(std::uint8_t*p,int n){return *reinterpret_cast<std::uint32_t*>(p+n);}
int CharacterState9790::run(std::uint8_t*base,std::uint32_t screen,int resourceBase,const CharacterState9790Ops&o,
 int kind,int value,int mask,int timerIndex,int timerScale,int duration,int resourceOffset,int ch){
 auto*c=base+ch*0x248;int left=sw(c,0x5c),right=sw(c,0x5e);
 int ql=o.querySlot(ch,0x0e)&0xff,qr=o.querySlot(ch,0x0f)&0xff;
 if(mask!=-1&&(dw(c,0x1c2)&std::uint32_t(mask)))return 0;
 int vl=(left<0x7e||left>0x86),vr=(right<0x7e||right>0x86);if(!vl&&!vr)return 0;
 int side=0;
 if(kind==0x80){if(!vl||!vr)return 0;if(vl&&left!=-1)o.preserveSlot(left,right,ql,qr,1,ch);if(vr&&right!=-1)o.preserveSlot(left,right,ql,qr,0,ch);side=1;}
 else if(left==-1)side=0;else if(right==-1)side=1;else side=o.preserveSlot(left,right,ql,qr,vl,ch);
 if(kind==0x80){ww(c,0x5c,value);ww(c,0x5e,value);o.updateSlot(ch,0x0e,0);o.updateSlot(ch,0x0f,0);}
 else if(kind==0xa5){if(vl){ww(c,0x5c,value);o.updateSlot(ch,0x0e,0);}if(vr){ww(c,0x5e,value);o.updateSlot(ch,0x0f,0);}}
 else {ww(c,0x5c+side*2,kind);o.updateSlot(ch,side+0x0e,0);}
 o.refreshPair(ch,side,-1);o.refreshPair(ch,side^1,-1);
 if(resourceOffset>=0){o.playResource(resourceBase+resourceOffset,0xff,0,1);o.finishResource();}
 int column=(ch%2)*0x35,row=(ch/2)*0xd3,y=row+column+4+side*0x21;
 o.present(screen,0x114,y,0x20,0x10);o.present(screen,0x114,row+column+4+(side^1)*0x21,0x20,0x10);
 if(mask!=-1){dw(c,0x1c2)|=std::uint32_t(mask);dw(c,0x1c6+timerIndex*4)=std::uint32_t(o.clock()+timerScale*duration);}
 return 1;
}}
