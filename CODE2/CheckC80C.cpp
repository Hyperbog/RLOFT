#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct CheckC80COps{
 std::function<std::uint8_t(int)> saveByte37;
 std::function<int(int)> rankC7C4;
 std::function<int(int)> modeFE44;
 std::function<std::uint8_t(int,int)> characterByte;
 std::function<std::int16_t(int,int)> characterWord;
 std::function<int(int,int,int,int)> dice1C328;
};
int checkC80C(const CheckC80COps&o,int ch,const std::uint8_t*target,const std::int8_t*meta,int saveIndex,bool subtractRank){
 if(target[8]&8)return 1;
 int b=saveIndex==-1?0:o.saveByte37(saveIndex);
 if(subtractRank)b-=o.rankC7C4(ch);
 const int mode=o.modeFE44(ch);
 if(mode==2)++b; else if(mode==3)b+=2; else if(mode==4)b+=mode;
 int threshold=o.characterByte(ch,0x1ea)-meta[0x21]+b;
 if(target[8]&0x80)threshold-=4;
 int bonus=0;
 const auto flags=o.characterByte(ch,0x213);
 if(flags&1)++bonus;if(flags&8)++bonus;
 if(o.characterWord(ch,0x148)==0x54)bonus+=3;
 const int roll=o.dice1C328(1,0x14,bonus,0);
 if(roll>=0x14)return 1;
 if(roll==1)return 0;
 return roll>=threshold;
}
}
