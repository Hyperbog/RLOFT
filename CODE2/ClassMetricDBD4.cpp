#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct ClassMetricDBD4Ops{std::function<std::int8_t(int,int)> cls;std::function<std::int16_t(int,int)> level;std::function<std::uint8_t(int)> div6828,mul6827;};
int classMetricDBD4(const ClassMetricDBD4Ops&o,int ch){
 int count=1;if(o.cls(ch,1)!=-1)count=2;if(o.cls(ch,2)!=-1)++count;
 int best=0,slot=0;for(int i=0;i<count;++i){const int v=o.level(ch,i);if(v>best){best=v;slot=i;}}
 const int c=o.cls(ch,slot);return 20-(best/o.div6828(c))*o.mul6827(c);
}
struct RestoreDC7COps{std::function<std::int8_t(int,int)> cls;std::function<std::uint8_t(int)> type17,base6821;std::function<std::int8_t(int,int)> adj67FA;std::function<int(int,int,int,int)> dice;std::function<std::int16_t&(int,int)> word;};
void restoreDC7C(const RestoreDC7COps&o,int ch,int slot,int level){
 const int c=o.cls(ch,slot);int roll=o.dice(1,o.base6821(c),0,0);if(roll<1)roll=1;const int adj=o.adj67FA(o.type17(ch),c/3);
 int add;switch(c){case 0:add=level<10?roll+adj:adj+3;break;case 1:add=level<11?roll+adj:adj+1;break;default:add=level<10?roll+adj:adj+2;break;}if(add<1)add=1;
 if(o.cls(ch,1)!=-1)add/=o.cls(ch,2)!=-1?3:2;
 o.word(ch,0xfe)=static_cast<std::int16_t>(o.word(ch,0xfe)+add);o.word(ch,0xfc)=static_cast<std::int16_t>(o.word(ch,0xfc)+add);
}
}
