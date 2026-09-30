#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct DamageCD44Ops{
 std::function<std::int32_t& (int,int)> dword;
 std::function<std::uint16_t& (int,int)> word;
 std::function<std::uint8_t& (int,int)> byte;
 std::function<std::uint8_t& (int)> redraw;
 std::function<std::int32_t()> clock19CC4;
 std::function<void(int)> clearAD24;
 std::function<int(int)> lookup14A60;
 std::function<int(int)> probe1C1DC;
 std::function<void()> partyGateCCF4;
 std::function<bool(int)> specialActive;
 std::function<int(int)> specialOwner;
 std::function<void(int)> clearSpecial;
 std::function<void(int)> setGlobalBF44;
};
void damageCD44(const DamageCD44Ops&o,int ch,int damage){
 o.dword(ch,0x292)=damage;o.dword(ch,0x286)=o.clock19CC4()+0x36;
 auto&v=o.dword(ch,0x1f7);if(v){v-=damage;if(v<=0){v=0;o.byte(ch,0x212)&=0xfe;}}
 auto absorb=[&](int off){auto&x=o.dword(ch,off);if(!x)return;if(damage>=x){damage-=x;x=0;}else{x-=damage;damage=0;}};
 absorb(0x2ca);absorb(0x1f3);
 o.word(ch,0xfc)=static_cast<std::uint16_t>(o.word(ch,0xfc)-static_cast<std::uint16_t>(damage));o.redraw(ch)=1;
 if(static_cast<std::int16_t>(o.word(ch,0xfc))<=0){o.word(ch,0xfc)=0;o.byte(ch,0xf1)|=0x40;o.byte(ch,0xf1)&=0xe3;o.clearAD24(ch);
  const int half=(ch-(ch>>31))/2;if(o.specialActive(half)&&o.specialOwner(half)==ch){o.redraw(half*2)=1;o.redraw(half*2+1)=1;o.clearSpecial(half);}
  if(ch==o.lookup14A60(0x578)&&o.probe1C1DC(0x46)==0)o.setGlobalBF44(0x6a);
 }
 o.partyGateCCF4();
}
}
