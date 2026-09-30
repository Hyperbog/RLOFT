#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct CheckCEE4Ops{std::function<std::int8_t(int)> classCode;std::function<std::int16_t(int)> level;std::function<std::int32_t(int,int)> table6888;std::function<int(int,int,int,int)> dice;};
int checkCEE4(const CheckCEE4Ops&o,int ch){const int cls=o.classCode(ch),lv=o.level(ch);const int target=o.table6888(cls,lv);return o.dice(1,20,0,0)>=target;}
struct CheckCF2COps{std::function<std::int8_t(int)> classCode;std::function<std::int16_t(int)> level;std::function<std::int32_t(int,int)> table6AB0;std::function<int(int,int,int,int)> dice;};
int checkCF2C(const CheckCF2COps&o,int ch,int modifier){const int cls=o.classCode(ch),lv=o.level(ch);const int target=o.table6AB0(cls,lv)+modifier;return o.dice(1,20,0,0)>=target;}
}
