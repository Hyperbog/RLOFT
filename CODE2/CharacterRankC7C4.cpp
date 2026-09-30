#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct CharacterRankC7C4Ops {
 std::function<std::uint8_t(int)> classCode;
 std::function<std::uint8_t(int)> classValue;
 std::function<std::int8_t(int)> classBase;
};
int characterRankC7C4(const CharacterRankC7C4Ops&o,int ch){
 const int cls=o.classCode(ch);
 const int value=o.classValue(ch);
 if(cls!=0x12)return o.classBase(cls);
 if(value==0)return o.classBase(cls);
 if(value<=0x32)return 1;
 if(value<=0x63)return 2;
 return 3;
}
}
