#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct StatHelpersOps{std::function<std::uint8_t(int,int)> mapByte;std::function<std::int16_t(int,int)> word;std::function<std::int32_t(int,int)> dword;std::function<std::int8_t(int,int)> byte;std::function<int(int)> fe44;std::function<int(int,int)> f2d4;std::function<int(int,int)> probe24478;};
int itemTypeEEE0(const StatHelpersOps&o,int item){return item!=-1&&o.mapByte(item,0x2f)==4;}
int totalEF14(const StatHelpersOps&o,int ch){return o.word(ch,0xfc)+o.dword(ch,0x2ca)+o.dword(ch,0x1f3);}
int itemPenaltyEF34(const StatHelpersOps&o,int item){if(item==-1)return 0;const int t=o.mapByte(item,0x2f);return t>=2&&t<=8?-static_cast<int>(o.mapByte(item,0x37)):0;}
int itemBonusEF70(const StatHelpersOps&o,int item){return item==-1?0:static_cast<int>(o.byte(item,0x34))+itemPenaltyEF34(o,item);}
int combatEFA0(const StatHelpersOps&o,int ch){int v=o.byte(ch,0xfb);const int a=o.word(ch,0x140),b=o.word(ch,0x142);v+=itemBonusEF70(o,a)+itemBonusEF70(o,b);v+=static_cast<std::int8_t>(o.mapByte(o.mapByte(ch,0x115),0));const int c=o.word(ch,0x13c);if(c!=-1&&o.probe24478(ch,0))v+=itemBonusEF70(o,c);const int d=o.word(ch,0x13e);if(d!=-1&&o.probe24478(ch,1))v+=itemBonusEF70(o,d);const int m=o.fe44(ch);if(m==3)++v;if(m==4)v+=3;const auto flags=static_cast<std::uint32_t>(o.dword(ch,0x212));if((flags&1)&&v>6&&!itemTypeEEE0(o,ch))v=6;if(flags&0x80)v-=2;if(o.f2d4(ch,0x46))--v;return v;}
}
