#include "EntityMarkB6F4.h"
namespace ravenloft::game {
static std::uint16_t rw(const std::uint8_t*p,int n){return std::uint16_t(p[n])|(std::uint16_t(p[n+1])<<8);}
void EntityMarkB6F4::run(EntityMarkB6F4State&s){
 for(int i=0,off=0;i<s.entityCount;++i,off+=0x16){auto*e=s.entities+off;if(e[0x0b]&2)continue;auto*t=s.types+rw(e,0x0e)*0x45;if(t[0x3a]&0x20)e[9]|=2;}
}}
