#include "MapViewport34B0.h"
#include <algorithm>

namespace ravenloft::game {
void MapViewport34B0::run(MapViewport34B0State&s,const MapViewport34B0Ops&o){
 int y=s.viewY12740,x=s.viewX12744;
 const auto m=s.action5FCA;
 if(m&0x01){if(y>-0x28)y-=4;}
 else if(m&0x02){if(y<s.mapWidth12754-0x2B || y<-4)y+=4;}
 else if(m&0x04){if(x>-0x28)x-=4;}
 else if(m&0x08){if(x<s.mapHeight12756-0x2C || x<-4)x+=4;}
 else if(m&0x10)y=std::max(-0x28,y-0x28);
 else if(m&0x20)y=std::min<int>(s.mapWidth12754-0x2F,y+0x28);
 else if(m&0x40)x=std::max(-0x28,x-0x28);
 else if(m&0x80)x=std::min<int>(s.mapHeight12756-0x30,x+0x28);
 s.viewY12740=y; s.viewX12744=x;
 o.call291B0(s.valueC050);
 o.call139C(s.viewX12744,s.viewY12740,0x30,0x2F,1);
 o.call1A510(s.valueC050);
 s.valueC050^=1u;
}
} // namespace ravenloft::game
