#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct InventoryGraphOps{
 std::function<std::uint16_t()> global6DAE; std::function<std::uint8_t(int,int)> itemByte; std::function<std::int16_t(int,int)> slotWord;
 std::function<std::uint8_t(int)> listCount; std::function<std::int16_t(int,int)> listItem; std::function<void(int,int,int,int,int,int)> draw298F0;
 std::function<void(int,int,int,int,int)> image2975C; std::function<void(int,int)> setSlotBit; std::function<void(int,int)> detach1A344;
};
bool pairF2D4(std::int16_t,std::int16_t,int);
void drawF308(const InventoryGraphOps&o,int a,int b,int item){if(o.global6DAE()&&(o.itemByte(item,0x37)||o.slotWord(item,0x32)))o.draw298F0(0,a,b,1,0xd470);else o.image2975C(0,a,b,1,0);}
bool simpleF368(std::uint8_t v){return v!=2&&v!=3&&v!=4;}
bool containsF390(const InventoryGraphOps&o,int item,int target){if(static_cast<std::int16_t>(item)==static_cast<std::int16_t>(target))return true;if(o.itemByte(item,0x2f)!=1)return false;const int row=static_cast<std::int8_t>(o.itemByte(item,0x31));for(int i=0;i<o.listCount(row);++i){const auto x=o.listItem(row,i);if(x!=-1&&containsF390(o,x,target))return true;}return false;}
bool removeF40C(const InventoryGraphOps&o,int item,int target){if(o.itemByte(item,0x2f)!=1)return false;const int row=static_cast<std::int8_t>(o.itemByte(item,0x31));for(int i=0;i<o.listCount(row);++i){const auto x=o.listItem(row,i);if(x==-1)continue;if(x==target){o.setSlotBit(row,i);return true;}if(removeF40C(o,x,target))return true;}return false;}
bool anywhereF4B0(const InventoryGraphOps&o,int target,std::int16_t globalItem){if(globalItem!=-1&&containsF390(o,globalItem,target))return true;for(int ch=0;ch<4;++ch)for(int s=0;s<21;++s){auto x=o.slotWord(ch,s);if(x!=-1&&containsF390(o,x,target))return true;}return false;}
void removeAnywhereF540(const InventoryGraphOps&o,int target,std::int16_t globalItem){if(globalItem==target){o.detach1A344(-1,0);return;}if(globalItem!=-1&&removeF40C(o,globalItem,target))return;for(int ch=0;ch<4;++ch)for(int s=0;s<21;++s){auto x=o.slotWord(ch,s);if(x==target){o.setSlotBit(ch,s);return;}if(x!=-1&&removeF40C(o,x,target))return;}}
int weightF5F8(const InventoryGraphOps&o,int row){if(row==20)return o.itemByte(0,0x1480);int sum=0;for(int i=0;i<o.listCount(row);++i){int item=o.listItem(row,i);if(item==-1)continue;sum+=o.itemByte(item,0x30);if(o.itemByte(item,0x2f)==1)sum+=weightF5F8(o,static_cast<std::int8_t>(o.itemByte(item,0x31)));}return sum;}
}
