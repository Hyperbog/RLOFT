#include "SlotNameInput75CC.h"
#include <cstring>
namespace ravenloft::game {
bool SlotNameInput75CC::run(const SlotNameInput75CCOps&o,std::uint32_t c050,std::uint32_t surface,int slot,const char*initial,char*out){
 o.call291B0(std::uint8_t(c050));char buf[50]{};std::strcpy(buf,initial);int y=slot*0x25+0x1d;bool first=true,dirty=true;
 for(;;){if(dirty){o.restore2A380(surface,0x64,y,0);o.draw3060C(y,0x64,buf,1);o.cursor30354(0x7c,1);o.present1A8C8(c050,y,0x64,0x0a,0xd8);dirty=false;}
  int k=o.key30AEC();if(k==0x1b)return false;if(k==0x0d){std::strcpy(out,buf);return true;}if(k==8){auto n=std::strlen(buf);if(n){buf[n-1]=0;dirty=true;}first=false;continue;}
  auto n=std::strlen(buf);if(n>=0x31||o.width30288(buf,1)>=0xd2){first=false;continue;}if(first)o.call30D30(buf,0,0x50);
  auto cls=o.charClass165C((std::uint8_t(k)+1)&0xff);if((cls&0xe0)||(cls&2)||(cls&4)){buf[n]=char(k);buf[n+1]=0;dirty=true;}first=false;
 }
}}
