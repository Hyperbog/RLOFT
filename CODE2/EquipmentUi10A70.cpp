#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct EquipmentUiOps{
 std::function<std::uint8_t(int)> groupMode,groupCount;std::function<std::int16_t(int,int)> groupItem;std::function<std::int16_t(int,int)> charSlot;
 std::function<std::uint8_t(int,int)> itemByte;std::function<std::int16_t(int,int)> itemWord;std::function<int(int)> specialF168;
 std::function<void(int,int,int,int)> drawF308;std::function<void(int,int,int,int,int)> rect29B40;std::function<std::uint32_t()> base7F6C;
};
void group10A70(const EquipmentUiOps&o,int group,int page){
 const int n=o.groupCount(group);const int mode=o.groupMode(group);for(int i=0;i<n;++i){int x=0xdb+(i/mode)*0x22,y=page+o.itemByte(group,3)+(i%mode)*0x12;o.rect29B40(x,y,0x20,0x10,0);int item=o.groupItem(group,i);if(item!=-1)o.drawF308(x,y,o.itemWord(item,0x2d),item);}
}
void equipment10BBC(const EquipmentUiOps&o,int ch){
 const int side=(o.itemByte(ch,0xf3)+5);(void)side;
 // Original tests signed slot words before drawing each equipment position.
 const int offs[]={0x46,0x40,0x44,0x42,0x48,0x38,0x3a,0x3c,0x3e};
 for(int k:offs){int item=o.charSlot(ch,k);if(item<0)continue;int image=o.itemWord(item,0x2d);if((k==0x3c||k==0x3e)&&o.specialF168(item))++image;o.drawF308(0x72,o.base7F6C(),image,item);}
}
}
