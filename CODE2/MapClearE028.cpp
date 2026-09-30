#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct MapClearE028Entity{std::int16_t y2,x6;std::uint16_t flags14;};
struct MapClearE028Ops{std::function<std::int16_t()> width;std::function<std::uint16_t&(int)> cell;std::function<void(int)> event1C124;};
void mapClearE028(const MapClearE028Ops&o,const MapClearE028Entity&e){
 int dir=(e.flags14>>13)&7;int x=e.x6,y=e.y2;switch((8-dir+4)%8){case 0:++x;break;case 1:++x;--y;break;case 2:--y;break;case 3:--x;--y;break;case 4:--x;break;case 5:--x;++y;break;case 6:++y;break;case 7:++x;++y;break;}
 o.event1C124(0xb0);o.cell(y*o.width()+x)&=0xffc0u;
}
struct CountE0BCOps{std::function<std::int16_t()> count;std::function<std::uint8_t(int)> flags;std::function<std::uint16_t(int)> type;};
int countE0BC(const CountE0BCOps&o){int n=0;for(int i=0;i<o.count();++i)if((o.flags(i)&2u)==0&&o.type(i)==0x18u)++n;return n!=1;}
}
