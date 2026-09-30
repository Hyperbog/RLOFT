#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct CharacterSheetOps{
 std::function<std::int8_t(int,int)> sbyte;std::function<std::uint8_t(int,int)> byte;std::function<std::int16_t(int,int)> word;std::function<std::int32_t(int,int)> dword;
 std::function<void(int,int,int,int)> text305A8;std::function<void(int,int,int,int)> itemF308;std::function<int(int)> combatEFA0;std::function<int(int)> format2E6F0;
 std::function<int(int)> format2E70C;std::function<int(int)> measure30288;std::function<std::uint32_t()> base7F6C;
};
void sheet10158(const CharacterSheetOps&o,int ch){
 int active=0;for(int i=0;i<3;++i)if(o.sbyte(ch,0x15+i)>=0)++active;
 int y=0x145-((active*11-(active*11>>31))>>1);
 for(int i=0;i<3;++i){const int v=o.sbyte(ch,0x15+i);if(v<0)continue;o.text305A8(y,o.base7F6C()+0x22,v+0x15,1);o.text305A8(y,o.base7F6C()+0x22,o.format2E70C(o.dword(ch,0x26+i*4)),1);y+=11;}
 const int hp=o.word(ch,0x1c)+o.dword(ch,0x1ea)+o.dword(ch,0x113),max=o.word(ch,0x1e);int pct=((hp*5)<<4)+(max>>1);pct/=max;
 (void)pct;(void)o.combatEFA0(ch);
}
}
