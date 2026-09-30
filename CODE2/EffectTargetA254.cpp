#include "EffectTargetA254.h"
namespace ravenloft::game {
static std::uint16_t rw(const std::uint8_t*p,int n){return std::uint16_t(p[n])|(std::uint16_t(p[n+1])<<8);}
static std::uint32_t& dw(std::uint8_t*p,int n){return *reinterpret_cast<std::uint32_t*>(p+n);}
int EffectTargetA254::run(EffectTargetA254State&s,const EffectTargetA254Ops&o,int mask,int effect,int descriptor,int duration,int resourceOffset,int effectArg,int force,int mult){
 for(auto&v:s.refresh)v=1;auto old1=s.d681,old2=s.d682;s.d681=s.d682=0;o.ui1F494(1);o.prompt();o.begin();
 int result=1;const int delta=duration*mult;
 for(;;){while(o.input()){}while(!o.input()){}if(o.insidePartyRect&&!(o.insidePartyRect(o.mouseX(),o.mouseY())))continue;
  int x=o.mouseX(),y=o.mouseY(),ch=-1;
  if(x>=0x114&&x<=0x193){if(y>=4&&y<=0x37)ch=0;else if(y>=0x39&&y<=0x6c)ch=1;else if(y>=0xd7&&y<=0x10a)ch=2;else if(y>=0x10c&&y<=0x13f)ch=3;}
  if(ch<0)continue;auto*c=s.characters+ch*0x248;auto status=rw(c,0x10);
  bool eligible=true;
  if(effect==0x7b)eligible=(status&0x8000)==0;
  else if(effect==0x68)eligible=(status&0x1000)!=0;
  else if(effect==0x71||effect==0x5d)eligible=(status&0x0800)!=0;
  else if(effect==0x64)eligible=(status&0x0400)!=0;
  else eligible=status<0x4000;
  if(!eligible)continue;
  if(resourceOffset>=0)o.playResource(s.resourceBase+resourceOffset,0xff,0,1);
  if(descriptor>=0){o.render9B4C(ch,descriptor,0);if(!force){dw(c,0x132)|=std::uint32_t(mask);if(duration){dw(c,0x136+effect*4)=std::uint32_t(o.clock()+delta);}
    if(mask==0x40)dw(c,0x1f2)+=o.dice(1,4,0,(o.value22E40(4,effectArg)-(o.value22E40(4,effectArg)>>31))/2);
    if(mask==0x800)dw(c,0x1ea)+=o.dice(1,8,0,0);
   }else result=o.effect9D90(effect,ch,effectArg);
  }else result=o.effect9D90(effect,ch,effectArg);
  if(effect==0){dw(c,0x136)=0;dw(c,0x117)=std::uint32_t(o.value22E40(4,ch)+8);}
  break;
 }
 o.end();s.d681=old1?2:0;s.d682=old2?2:0;for(auto&v:s.refresh)v=1;o.ui1F494(1);o.finish();return result;
}}
