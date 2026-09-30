#include <cstdint>
namespace ravenloft::game {
struct GateC910Save { std::uint8_t byte2f; std::uint8_t byte37; };
int gateC910(int value,std::uint32_t flags,int saveIndex,const GateC910Save*save){
 int kind=9;
 if(saveIndex!=-1){kind=save->byte2f;if(save->byte37!=0)kind=11;}
 if(kind<10){if(kind==9 && (static_cast<std::uint8_t>(flags>>8)&0x80))value=0;}
 else if(kind==10){if(static_cast<std::uint8_t>(flags>>8)&0x40)value=0;}
 else if(kind==11){
  if((flags&0x20000u) && save->byte37>=0xfeu)value=0;
  if((flags&0x40000u) && save->byte37<0xfeu)value=0;
 }
 return value<1?1:value;
}
}
