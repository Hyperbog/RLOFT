#include <array>
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct QuickSlotOps{
 std::function<std::uint8_t(int,int)> byte;std::function<std::uint8_t(int)> classLimit;std::function<void(int)> sound;std::function<void(int,int)> describe;
 std::function<std::int16_t&()> selected;std::function<std::uint8_t&()> mode;std::function<std::uint8_t&(int,int)> slotByte;
};
int quickSlot11630(const QuickSlotOps&o,int ch,int value){
 static constexpr std::array<int,6> threshold={0,10,20,30,40,50};int band=0,within=value;
 for(int i=0;i<6;++i)if(value<=threshold[i]){band=i;within=value-(i?threshold[i-1]:0);break;}
 int used=0;for(int i=0;i<10;++i)if((o.byte(ch,0x4a+band*10+i)&0x0f)!=0x0f)++used;
 if(used==o.classLimit(o.byte(ch,0x19)))return 0;
 auto&s=o.slotByte(ch,0x4a+band*10+within);if((s&0x0f)!=0x0f)return 0;s=0;o.sound(0xba);o.describe(o.selected(),o.mode());return 1;
}
}
