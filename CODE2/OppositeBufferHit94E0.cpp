#include "OppositeBufferHit94E0.h"
namespace ravenloft::game {
static std::uint16_t rw(const std::uint8_t*p,int o){return std::uint16_t(p[o])|(std::uint16_t(p[o+1])<<8);}
int OppositeBufferHit94E0::run(OppositeBufferHit94E0State&s,const OppositeBufferHit94E0Ops&o,int x,int y){
 unsigned page=(s.c050^1u)&1u;int ordinal=int(s.counts[page])-1;
 for(;ordinal>=0;--ordinal){int physical=0;for(;physical<int(s.counts[page]);++physical){auto*p=s.records.data()+page*0x2260+physical*0x16;if(rw(p,0x0a)==ordinal)break;}if(physical>=int(s.counts[page]))continue;
  auto*p=s.records.data()+page*0x2260+physical*0x16;if(rw(p,8)!=0)continue;unsigned type=rw(p,6);if(type*0x16+0x10>s.typeTable.size())continue;if(!o.probe2ACDC(rw(s.typeTable.data()+type*0x16,0x0e)))continue;if(!o.probe21BF0(p,0))continue;
  if(x<int(rw(p,0x0e))||x>=int(rw(p,0x10))||y<int(rw(p,0x12))||y>=int(rw(p,0x14)))continue;return int(type);
 }return -1;
}}
