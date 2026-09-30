#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct EquipmentCycleOps{
 std::function<void(int)> drawEquipment;std::function<std::int16_t&()> selected;std::function<std::uint8_t&()> mode;std::function<std::uint8_t&()> page;
 std::function<void(int,int,int)> describeF790;std::function<void()> refresh22D70;std::function<void(int)> redrawPage;std::function<std::int32_t&()> stateFlag;
};
void cycle110B4(const EquipmentCycleOps&o,int ch){
 for(int i=3;i>=0;--i){o.redrawPage(ch);if(static_cast<unsigned>(static_cast<std::uint16_t>(o.selected())-0xa7)<=0x22u)o.stateFlag()=0x3a;o.page()^=1;}
}
void select112EC(const EquipmentCycleOps&o,std::int16_t item,std::uint8_t mode){
 o.selected()=item;o.mode()=mode;o.describeF790(item,mode,0);if(static_cast<unsigned>(static_cast<std::uint16_t>(item)-0xa7)<=0x22u)o.stateFlag()=0x3a;o.page()^=1;o.refresh22D70();
}
bool compatible113AC(int a,int b){
 if(a==0)return b!=5&&b!=3;
 if(a==1)return b!=1&&b!=6&&b!=2;
 if(a==2)return b!=2;
 return false;
}
}
