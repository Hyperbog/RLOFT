#include "PackedConsume44D0.h"
namespace ravenloft::game {
namespace {std::uint16_t w(std::span<std::uint8_t>b,std::size_t p){return std::uint16_t(b[p])|(std::uint16_t(b[p+1])<<8);}}
int PackedConsume44D0::run(PackedConsume44D0State&s,const PackedConsume44D0Ops&o,int object,int row,int col,int*timer,int*count,int kind,int timeBase){
 const auto cell=static_cast<std::size_t>(object)*0x248+row*10+col+0x2199A;
 auto &v=s.objects[cell]; if((v&15)==15 || (v&0xF0)==0)return 0;
 int result=0;
 for(int ch=0;ch<4;++ch){
  const auto b=static_cast<std::size_t>(ch)*0x248;
  const auto status=w(s.objects,b+0x218F0)&0xC000;
  if(status!=0)continue;
  const auto cur=w(s.objects,b+0x218FC),max=w(s.objects,b+0x218FE);
  if(cur==max)continue;
  while((v&0xF0)!=0 && w(s.objects,b+0x218FC)<w(s.objects,b+0x218FE)){
   o.call9D90(kind,ch,object); s.flagsCF5A[ch]=1; o.call1F494(1);
   const auto hi=std::uint8_t((v>>4)-1)&15; v=(v&15)|(hi<<4);
   if(kind!=0x57 && *timer==0){*timer=timeBase+0xF0;result=1;}
  }
  if(w(s.objects,b+0x218FC)==w(s.objects,b+0x218FE))--*count;
 }
 return result;
}
}
