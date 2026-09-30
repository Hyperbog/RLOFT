#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct InventoryGridOps{
 std::function<std::int16_t&(int,int)> slot;std::function<std::uint32_t&(int)> flags;std::function<int(int)> groupWeight;std::function<std::uint8_t(int)> itemWeight,itemType;
 std::function<std::int8_t(int)> itemGroup;std::function<std::uint8_t(int)> capacity;std::function<void(int)> redraw;std::function<void(int,int)> describe;std::function<std::uint8_t&()> page;
};
bool grid11C3C(const InventoryGridOps&o,int ch,int cell,int incoming,std::uint8_t incomingFlag,int group){
 int old=o.slot(ch,cell);int load=o.groupWeight(group);
 if(old!=-1){if(o.itemType(old)==1)load-=o.groupWeight(o.itemGroup(old));load-=o.itemWeight(old);}
 if(incoming!=-1){if(o.itemType(incoming)==1)load+=o.groupWeight(o.itemGroup(incoming));load+=o.itemWeight(incoming);}
 if(load>o.capacity(group)*2)return true;
 const std::uint32_t bit=1u<<(cell&31);o.slot(ch,cell)=static_cast<std::int16_t>(incoming);o.flags(ch)=(o.flags(ch)&~bit)|(incomingFlag?bit:0);o.redraw(ch);o.describe(old,(o.flags(ch)&bit)?1:0);o.page()^=1;return true;
}
bool gridClick11FB0(int pageState,int x,int y,int flags,const std::function<void(int,int,int)>&move){
 if((flags&2)||pageState)return false;int col=(x-0x129)/0x22;col%=0x20;int row=(y-5)/0x12;row%=0x10;move(col*4+row,1,0);return true;
}
}
