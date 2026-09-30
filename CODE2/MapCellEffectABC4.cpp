#include "MapCellEffectABC4.h"
namespace ravenloft::game {
static void pos(MapCellEffectABC4State&s,int i,int&x,int&y){int v=s.table15A2C[i*7];x=v%s.width;y=v%s.height;}
void MapCellEffectABC4::set(MapCellEffectABC4State&s,const MapCellEffectABC4Ops&o,int i){int x,y;pos(s,i,x,y);o.play28110(s.resourceBase+0x49,0xff,0,1);auto*p=o.cell1E8C8(x,y);if(p&&p[0]==1)p[1]|=0x40;}
void MapCellEffectABC4::toggle(MapCellEffectABC4State&s,const MapCellEffectABC4Ops&o,int i){int x,y;pos(s,i,x,y);o.play28110(s.resourceBase+0x32,0xff,0,1);auto*p=o.cell1E8C8(x,y);if(!p)return;if(p[0]==1){if(p[1]&0x40){p[1]&=0xbf;return;}if(!p[6])return;int old=(p[1]>>7)&1;p[1]|=4;int v=o.update20C0C(p,x,y)&1;p[1]=std::uint8_t((p[1]&0xfb)|(v<<2));(void)old;}else if(p[0]==4){if(o.query20CF4(p[6])){o.dispatch02A8(std::int8_t(p[2]));p[1]&=0xfb;}else o.wait1C77C(o.alloc1C924(0x320));}}
}
