#include "StatusClearAD24.h"
namespace ravenloft::game {
static std::uint16_t rw(const std::uint8_t*p,int n){return std::uint16_t(p[n])|(std::uint16_t(p[n+1])<<8);}
static std::uint32_t& dw(std::uint8_t*p,int n){return *reinterpret_cast<std::uint32_t*>(p+n);}
void StatusClearAD24::run(StatusClearAD24State&s,const StatusClearAD24Ops&o,int which){
 int first=which==-1?0:which,last=which==-1?4:which+1;
 for(int ch=first;ch<last;++ch){auto*c=s.characters+ch*0x248;if(rw(c,0x10)&0x8000)continue;for(int i=0;i<26;++i)if(dw(c,0x132)&(1u<<i))o.expire871C(ch,i);}
 for(int ch=first;ch<last;++ch){auto*c=s.characters+ch*0x248;if(rw(c,0x10)&0x8000)continue;for(int i=0;i<9;++i){auto m=1u<<i;if(dw(c,0x1c2)&m){dw(c,0x1c2)^=m;dw(c,0x1c6+i*4)=0;o.probe86C4(ch,i);}}}
 o.sweep893C();
}}
