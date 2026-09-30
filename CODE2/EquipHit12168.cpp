#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct EquipHitOps{
 std::function<std::int16_t(int,int)> slot;std::function<std::uint8_t(int)> itemType;std::function<int(int)> special;std::function<void(int,int,int)> move;
};
int choose12180(const EquipHitOps&o,int ch,int x,int selected){
 int dst=-1;if(selected!=-1){int t=o.itemType(selected);switch(t){case 3:dst=0x11;break;case 4:dst=0x10;break;case 5:dst=0x10;break;case 6:dst=0x12;break;case 7:dst=0x14;break;case 8:dst=x<=0x28?0x0c:0x0d;break;default:dst=x<=0x28?(o.special(selected)?0x13:0x0e):0x0f;break;}}
 if(dst<0)return 0;int old=o.slot(ch,dst);if(old>=0x7e&&old<=0x86)return 0;o.move(ch,dst,1);return 1;
}
}
