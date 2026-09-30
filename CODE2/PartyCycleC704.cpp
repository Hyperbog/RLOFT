#include <array>
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct PartyCycleC704State {
 std::uint8_t cursor=0;
 std::array<std::uint8_t,4> redraw{};
};
struct PartyCycleC704Ops {
 std::function<std::uint16_t(int)> characterWordF0;
 std::function<std::int16_t(int,int)> characterWord13C;
 std::function<int(int,int)> probe24478;
 std::function<int(int)> probe23E58;
 std::function<int(int)> probe23F50;
};
void cyclePartySlotC704(PartyCycleC704State&s,const PartyCycleC704Ops&o){
 const std::uint8_t old=s.cursor;
 for(;;){
  s.cursor=static_cast<std::uint8_t>((s.cursor+1u)&7u);
  const int slot=static_cast<int>(s.cursor>>1);
  const int side=static_cast<int>(s.cursor&1u);
  bool accept=false;
  if(o.characterWordF0(slot)<0x2000u && o.probe24478(slot,side)!=0){
   const int v=o.characterWord13C(slot,side);
   if(v==-1 || (o.probe23E58(v)==0 && o.probe23F50(v)==0))accept=true;
  }
  if(accept || s.cursor==old)break;
 }
 s.redraw[s.cursor>>1]=1;
 s.redraw[old>>1]=1;
}
}
