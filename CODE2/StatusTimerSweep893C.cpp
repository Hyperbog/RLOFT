#include "StatusTimerSweep893C.h"
namespace ravenloft::game {
static std::uint16_t rw(std::span<std::uint8_t>b,std::size_t p){return std::uint16_t(b[p])|(std::uint16_t(b[p+1])<<8);}
static std::uint32_t& rd(std::span<std::uint8_t>b,std::size_t p){return *reinterpret_cast<std::uint32_t*>(b.data()+p);}
void StatusTimerSweep893C::run(StatusTimerSweep893CState&s,const StatusTimerSweep893COps&o){
 auto now=o.time19CC4();
 for(int ch=0;ch<4;++ch){auto b=std::size_t(ch)*0x248;if(rw(s.characters,b+0x10)>=0x4000)continue;
  if(rd(s.characters,b+0x1c2))for(int i=0;i<9;++i){auto&t=rd(s.characters,b+0x1c6+i*4);if(t&&now>=std::int32_t(t)){rd(s.characters,b+0x1c2)^=(1u<<i);s.refreshFlags[ch]=1;t=0;o.probe86C4(ch,i);}}
  auto&special=rd(s.characters,b+0x1ee);if(special&&now>=std::int32_t(special)){o.call1202C(ch,1);special=0;if(o.call12098(ch)!=s.characters[b+0x32])special=std::uint32_t(o.time19CC4()+o.dice1C328(2,4,0,0)*0x2a30);}
  if(rd(s.characters,b+0x132))for(int i=0;i<0x1a;++i){auto t=rd(s.characters,b+0x136+i*4);if(std::int32_t(t)>0&&now>=std::int32_t(t))o.expire871C(ch,i);}
 }}
}
