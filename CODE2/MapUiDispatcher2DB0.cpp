#include "MapUiDispatcher2DB0.h"
#include <algorithm>

namespace ravenloft::game {
namespace {
void redraw(MapUiDispatcher2DB0State&s,const MapUiDispatcher2DB0Ops&o){
 o.call291B0(s.valueC050); o.call139C(s.viewX12744,s.viewY12740,0x30,0x2F,1);
 o.call1A510(s.valueC050); s.valueC050^=1u;
}
void center(MapUiDispatcher2DB0State&s){
 if(s.currentMap5FCB){
   s.viewX12744=std::max(-0x28,static_cast<int>(s.playerX255D2)-0x18);
   s.viewY12740=std::max(-0x28,static_cast<int>(s.playerY255D6)-0x18);
 } else s.viewX12744=s.viewY12740=-4;
}
}

void MapUiDispatcher2DB0::run(MapUiDispatcher2DB0State&s,const MapUiDispatcher2DB0Ops&o,
                              std::int32_t x,std::int32_t y,std::uint8_t flags){
 int action=-1;
 for(int i=0;i<16 && s.hit[i][0]!=-1;++i)
   if(x>=s.hit[i][1]&&x<=s.hit[i][3]&&y>=s.hit[i][2]&&y<=s.hit[i][4]){action=i;break;}
 if(action<0){if((flags&2)==0)return; action=11;}
 else if(flags&2) action=(action>=0&&action<=3)?action+12:11;
 if(action>15)return;

 if(action<=3){s.action5FCA=static_cast<std::uint8_t>(1u<<action);return;}
 if(action>=12){s.action5FCA=static_cast<std::uint8_t>(1u<<(action-8));return;}

 switch(action){
 case 4:
   if(!s.enabled5FD9||!s.currentMap5FCB)return;
   o.drawAction(4); o.call207C(); o.drawAction(0); return;
 case 5:
   if(!s.enabled5FD9||!s.currentMap5FCB)return;
   o.drawAction(5); o.call21D8(); o.drawAction(0); return;
 case 6:
   do {
     s.mapIndex1274C=(s.mapIndex1274C+1)&0x1F;
     s.currentMap5FCB=(s.mapIndex1274C==s.currentMapC6C8);
     center(s);
   } while(o.call2AE8(s.mapIndex1274C)==0);
   redraw(s); return;
 case 7:
   do {
     if(--s.mapIndex1274C<0)s.mapIndex1274C=0x1F;
     s.currentMap5FCB=(s.mapIndex1274C==s.currentMapC6C8);
     center(s);
   } while(o.call2AE8(s.mapIndex1274C)==0);
   redraw(s); return;
 case 8:
   s.enabled5FD9^=1u; o.drawAction(8); redraw(s); return;
 case 9:
   if(!s.currentMap5FCB)return;
   o.drawAction(9); o.call26E8(0xF764,0,0); o.drawAction(8); return;
 case 10:
   if(!s.currentMap5FCB)return;
   o.drawAction(10); o.call2A34(s.currentMapC6C8); o.drawAction(8); return;
 case 11:
   o.drawAction(11); s.action5FCA=0; o.call1F8FC(); o.call1AE30(); return;
 default:return;
 }
}

} // namespace ravenloft::game
