#include <cstdint>
namespace ravenloft::game {
struct GateD4E8Character{std::uint16_t stateF0;std::uint8_t flags212;std::uint8_t flags213;};
struct GateD4E8Target{std::uint8_t type1D;std::uint8_t flags3A;};
int gateD4E8(const GateD4E8Character&c,const GateD4E8Target&t){
 if(c.stateF0>=0x4000u)return 1;
 if(c.flags212&0x20u)return 1;
 if((c.flags213&2u) && t.type1D<=4u && (t.flags3A&0x20u))return 1;
 return 0;
}
}
