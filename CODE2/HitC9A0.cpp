#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct HitC9A0Ops{
 std::function<int(int,int,int,int)> dice1C328;
 std::function<int(int)> efa0;
 std::function<std::uint32_t(int)> characterFlags;
 std::function<void(int,int)> status871C;
};
int hitC9A0(const HitC9A0Ops&o,int ch,const std::uint8_t*attacker,const std::uint8_t*target){
 const int roll=o.dice1C328(1,0x14,0,0);
 int adjusted=roll;
 if(roll==0x14)return 1;
 if(roll==1)return 0;
 const int penalty=o.efa0(ch);
 const std::uint32_t flags=o.characterFlags(ch);
 if(flags&0x40u){o.status871C(ch,6);return 0;}
 if(flags&0x20u)adjusted=roll-2;
 if(flags&0x100u)--adjusted;
 if(flags&8u)adjusted-=2;
 int threshold=static_cast<int>(attacker[0x22])-penalty;
 if(target[8]&0x80)threshold+=4;
 return adjusted>=threshold;
}
}
