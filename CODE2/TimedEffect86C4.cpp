#include "TimedEffect86C4.h"
namespace ravenloft::game {
static std::uint16_t w(std::span<std::uint8_t>b,std::size_t p){return std::uint16_t(b[p])|(std::uint16_t(b[p+1])<<8);}
void TimedEffect86C4::run(TimedEffect86C4State&s,const TimedEffect86C4Ops&o,int ch,int effect){
 auto b=std::size_t(ch)*0x248;if(w(s.characters,b+0x5c)==s.table67CA[effect])o.call24A9C(effect,0);
 if(w(s.characters,b+0x5e)==s.table67CA[effect])o.call24A9C(effect,1);
}}
