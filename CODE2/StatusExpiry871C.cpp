#include "StatusExpiry871C.h"
namespace ravenloft::game {
static std::uint32_t& dw(std::span<std::uint8_t>b,std::size_t p){return *reinterpret_cast<std::uint32_t*>(b.data()+p);}
void StatusExpiry871C::apply(StatusExpiry871CState&s,const StatusExpiry871COps&o,int ch,int st){
 auto b=std::size_t(ch)*0x248;
 dw(s.characters,b+0x132)^=(1u<<std::uint8_t(st));
 dw(s.characters,b+0x136+st*4)=0;
 s.refreshFlags[ch]=1;
 o.notify(ch,st);
 switch(st){
 case 0:case 1:case 10:o.call31F38(0);break;
 case 6:--dw(s.characters,b+0x1f2);break;
 case 11:dw(s.characters,b+0x1ea)=0;break;
 case 12:o.call1202C(ch,-std::int32_t(dw(s.characters,b+0x123)));break;
 case 13:s.characters[b+0x35]-=s.characters[b+0x127];break;
 case 17:--s.globalFD08;break;
 case 18:--s.globalF03C;break;
 case 20:s.refreshFlags={1,1,1,1};--s.global6DAE;break;
 case 21:s.refreshFlags={1,1,1,1};for(int i=0;i<4;++i)s.characters[i*0x248+0x134]&=0xdf;break;
 case 23:case 24:o.call1202C(ch,-std::int32_t(dw(s.characters,b+0x1be)));break;
 case 25:--s.globalF03E;break;
 default:break;
 }
}}
