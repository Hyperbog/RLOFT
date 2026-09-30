#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct InventoryMoveOps{
 std::function<std::int16_t&(int,int)> slot;std::function<std::uint32_t&(int)> slotFlags;std::function<int(int,int,int,std::uint8_t)> insert;
 std::function<void(int)> redraw;std::function<void(int,int)> describe;std::function<void(std::uint8_t)> sound;std::function<std::uint8_t&()> page;
 std::function<int(int,int)> checkF2D4;std::function<int(int,int)> class50DC;std::function<void(int,int)> effect3A78;std::function<std::uint8_t&(int)> flags21A14;
};
void move117A0(const InventoryMoveOps&o,int ch,int slotIndex,int selected,std::uint8_t selectedFlag){
 int old=o.slot(ch,slotIndex);const std::uint32_t bit=1u<<(slotIndex&31);const std::uint8_t oldFlag=(o.slotFlags(ch)&bit)?1:0;
 if(old==0xa6&&selected>=0xa7&&selected<=0xc9){o.sound(0);return;}
 if(old==0x35&&selected>=0x61&&selected<=0x64){o.sound(static_cast<std::uint8_t>(selected-0x61));return;}
 if(selected!=-1&&old!=-1&&o.insert(ch,slotIndex,selected,selectedFlag)){o.redraw(ch);o.describe(-1,0);o.page()^=1;return;}
 o.slot(ch,slotIndex)=static_cast<std::int16_t>(selected);o.slotFlags(ch)=(o.slotFlags(ch)&~bit)|(selectedFlag?bit:0);o.redraw(ch);o.describe(old,oldFlag);
 if(!o.checkF2D4(ch,0x48)&&o.class50DC(1,ch)){int k=o.class50DC(1,ch);o.effect3A78(ch,k);}
 if(o.slot(ch,0x3c)==0x67||o.slot(ch,0x3e)==0x67)o.flags21A14(ch)|=0x40;else o.flags21A14(ch)&=0xbf;
}
}
