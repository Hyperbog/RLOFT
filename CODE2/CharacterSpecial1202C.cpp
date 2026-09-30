#include <cstdint>
namespace ravenloft::game {
struct CharacterSpecialState{std::int16_t slot48;std::uint8_t value13;std::uint8_t value15;std::int32_t alternate96;};
int add1202C(CharacterSpecialState&s,int amount){
 int cur=s.slot48==0x54?s.alternate96:s.value13;int applied=amount;if(cur+amount>20)applied=20-cur;
 if(s.slot48==0x54)s.alternate96+=applied;else s.value13=static_cast<std::uint8_t>(s.value13+applied);return applied;
}
int get12098(const CharacterSpecialState&s){return s.slot48==0x54?s.alternate96:s.value13;}
void swap120BC(CharacterSpecialState&s,int oldType,int newType){
 if(oldType==0x54)s.value13=static_cast<std::uint8_t>(s.alternate96);
 if(newType==0x54){s.alternate96=s.value13;s.value13=20;}
 if(oldType==0x53)--s.value15;if(newType==0x53)++s.value15;
}
bool special12128(int id){return id==0x10||id==0x5f||id==0x0c||id==0xec||id==0xee;}
}
