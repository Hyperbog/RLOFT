#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct LevelLossD1E0Ops{
 std::function<std::int8_t(int,int)> classCode;
 std::function<std::int16_t&(int,int)> level;
 std::function<std::int32_t&(int,int)> experience;
 std::function<std::uint32_t()> random30CA2;
 std::function<std::uint16_t(int,int,int)> xpTable;
 std::function<void(int,int)> normalize3A78,normalize3C04;
 std::function<void(int)> redraw;
 std::function<void(int,int,int)> reduceD0B0;
};
void levelLossD1E0(const LevelLossD1E0Ops&o,int ch){
 int slot=0;const auto c0=o.classCode(ch,0),c1=o.classCode(ch,1);
 if(c0==-1)return;
 if(c1==-1){slot=static_cast<int>((o.random30CA2()*2u)>>15);if(o.level(ch,1)>o.level(ch,0))slot=1;if(o.level(ch,0)>o.level(ch,1))slot=0;}
 else{slot=static_cast<int>((o.random30CA2()*3u)>>15);const auto a=o.level(ch,0),b=o.level(ch,1),c=o.level(ch,2);if(c>a&&c>b)slot=2;if(b>a&&b>c)slot=1;if(a>b&&a>c)slot=0;}
 auto&lv=o.level(ch,slot);const int old=lv;if(old<=1){lv=0;o.redraw(ch);return;}--lv;
 const int cls=o.classCode(ch,slot);const auto lo=o.xpTable(cls,lv,0),hi=o.xpTable(cls,lv,1);o.experience(ch,slot)=static_cast<std::int32_t>(lo*1000u+((hi-lo)*1000u)/2u);
 for(int s=0;s<3;++s){const int cc=o.classCode(ch,s);if(cc==4)o.normalize3A78(ch,o.level(ch,s));if(cc==3)o.normalize3C04(ch,o.level(ch,s));}
 o.redraw(ch);o.reduceD0B0(ch,cls,old);
}
}
