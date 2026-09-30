#include <array>
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct EquipmentPageState { std::array<std::int8_t,16> descriptor{}; };
struct EquipmentPageOps {
 std::function<void(int,int)> allocate;
 std::function<void(int)> drawEquipment;
 std::function<int()> selected;
 std::function<int()> mode;
 std::function<std::uint8_t&()> page;
 std::function<std::int32_t&()> stateFlag;
 std::function<void(int,int,int,int,int,int,int)> dispatch;
};
void setup10FAC(EquipmentPageState& s,const EquipmentPageOps& o,int index,std::uint8_t a,std::uint8_t b,std::uint8_t c){
 s.descriptor[index*4]=a;
 s.descriptor[index*4+1]=b;
 s.descriptor[index*4+3]=c;
 for(int p=0;p<4;++p) o.allocate(index*2+0x14+p,2);
 o.drawEquipment(index);
 for(int i=0;i<4;++i){
  const unsigned id=static_cast<unsigned>(static_cast<std::uint16_t>(o.selected()));
  if(id>=0xa7u&&id<=0xc9u) o.stateFlag()=0x3a;
  o.dispatch(o.page(),o.selected(),o.mode(),0xc4,0x50,100,0);
  o.page()^=1;
 }
}
}
