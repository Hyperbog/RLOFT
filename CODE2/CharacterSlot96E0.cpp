#include "CharacterSlot96E0.h"
namespace ravenloft::game {
static std::int16_t sw(const std::uint8_t*p){return std::int16_t(std::uint16_t(p[0])|(std::uint16_t(p[1])<<8));}
static void ww(std::uint8_t*p,std::int16_t v){p[0]=std::uint8_t(v);p[1]=std::uint8_t(std::uint16_t(v)>>8);}
int CharacterSlot96E0::run(CharacterSlot96E0State&s,const CharacterSlot96E0Ops&o,int first,int second,int a,int b,int chooseFirst,int character){
 auto*c=s.characters.data()+std::size_t(character)*0x248;int slot=0;for(;slot<12;++slot)if(sw(c+0x40+slot*2)==-1)break;
 if(slot<12){ww(c+0x40+slot*2,std::int16_t(chooseFirst?first:second));return chooseFirst?0:1;}
 if(chooseFirst){o.fallback2195C(0x80,first,a,s.playerX,s.playerY,1,0);return 0;}
 o.fallback2195C(0x80,second,b,s.playerX,s.playerY,1,0);return 1;
}}
