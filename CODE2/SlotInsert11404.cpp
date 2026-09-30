#include <array>
#include <cstdint>
#include <functional>
namespace ravenloft::game {
static constexpr std::array<std::uint32_t,4> kTypeDispatch113F4={0x11466,0x11474,0x11482,0x11490};
struct SlotInsertOps{
 std::function<std::int16_t(int,int)> charSlot;std::function<std::uint8_t(int,int)> itemByte;std::function<int(int)> containerWeight;
 std::function<std::uint8_t(int)> groupMode,groupCount,groupCapacity;std::function<std::int16_t(int,int)> groupItem;std::function<void(int,int,std::int16_t)> setGroupItem;
 std::function<std::uint8_t&(int)> groupMask;std::function<void(std::uint8_t)> sound291B0;
};
bool insert11404(const SlotInsertOps&o,int ch,int slot,int item,std::uint8_t flag){
 const int current=o.charSlot(ch,slot);const int group=static_cast<std::int8_t>(o.itemByte(current,0x31));const int mode=o.groupMode(group);
 if(item!=-1&&mode>=3&&mode<=6){const int type=o.itemByte(item,0x2f);const int required[]={0x0d,0x0e,0x0c,0x0f};if(type==required[mode-3])return false;}
 if(mode<2&&item!=-1&&(o.itemByte(item,0x35)&3))return false;
 if(mode==4&&(item==0x114||item==0x115||item==0x128))return false;
 int pos=0;while(pos<o.groupCount(group)&&o.groupItem(group,pos)!=-1)++pos;if(pos==o.groupCount(group))return false;
 int load=o.containerWeight(group)-o.itemByte(current,0x30);if(item!=-1){if(o.itemByte(item,0x2f)==1)load+=o.containerWeight(static_cast<std::int8_t>(o.itemByte(item,0x31)));load+=o.itemByte(item,0x30);}
 if(load>o.groupCapacity(group)*2)return false;
 o.sound291B0(0);o.setGroupItem(group,pos,static_cast<std::int16_t>(item));const std::uint8_t bit=static_cast<std::uint8_t>(1u<<pos);o.groupMask(group)=static_cast<std::uint8_t>((o.groupMask(group)&~bit)|((flag?1u:0u)<<pos));return true;
}
}
