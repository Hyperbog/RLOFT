#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct ReduceD0B0Ops{std::function<int(int)> averageCF88;std::function<int(int,int,int,int)> dice;std::function<std::uint8_t(int)> type17;std::function<std::int8_t(int)> base6821;std::function<std::int8_t(int,int)> table67FA;std::function<std::int16_t&(int,int)> word;};
void reduceD0B0(const ReduceD0B0Ops&o,int ch,int cls,int level){
 const int cap=o.averageCF88(ch);
 const int roll=o.dice(1,o.base6821(cls),0,0);
 const int adj=o.table67FA(o.type17(ch),cls/3);
 int amount=0;
 switch(cls){case 0:amount=level<10?roll+adj:adj+3;break;case 1:amount=level<11?roll+adj:adj+1;break;default:amount=level<10?roll+adj:adj+2;break;}
 if(amount<1)amount=1;
 auto&max=o.word(ch,0xfe);max=static_cast<std::int16_t>(max-amount);if(max<1)max=1;if(max>cap)max=static_cast<std::int16_t>(cap);
 auto&cur=o.word(ch,0xfc);if(cur>max)cur=max;
}
}
