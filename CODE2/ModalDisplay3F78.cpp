#include "ModalDisplay3F78.h"
namespace ravenloft::game {
void ModalDisplay3F78::run(ModalDisplay3F78State&s,const ModalDisplay3F78Ops&o,int a,int b){
 o.call291B0(static_cast<std::uint8_t>(s.valueC050));
 o.call1C40C(s.valueC050,0x50,0x50,0xF0,0xA0);
 o.call30508(0xC4,0x50,0xA0,a,1);
 o.call30508(0xCE,0x50,0xA0,b,1);
 o.call1A510(s.valueC050); reinterpret_cast<std::uint8_t*>(&s.valueC050)[0]^=1;
 while(o.call1A1F4()!=0){}
 while(o.call1A1F4()==0 && o.call30B40()==0){}
 while(o.call1A1F4()!=0 || o.call30B40()!=0){}
 o.call1A5B0(s.valueC050,s.value8600,s.value8602);
 reinterpret_cast<std::uint8_t*>(&s.valueC050)[0]^=1;
}
}
