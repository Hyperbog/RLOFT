#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct CharacterPanelOps{
 std::function<int(int)> tierFE44,combatEFA0;std::function<std::int16_t(int,int)> word;std::function<std::int32_t(int,int)> dword;std::function<std::uint32_t(int,int)> flags;
 std::function<void(int,int,int,int,int)> image2975C;std::function<void(int,int,int,int,int,int)> sprite298F0;std::function<void(int,int,int,int,int)> text30508;
 std::function<int(int)> portrait1F3FC;std::function<std::uint32_t()> base7F6C;
};
void panelFED0(const CharacterPanelOps&o,int ch){
 const int tier=o.tierFE44(ch);o.image2975C(tier,o.base7F6C()+0x44,0x10,0x11,2);o.image2975C(0,o.base7F6C()+0x38,0x10,0x10,2);
 o.text30508(0x16,o.base7F6C()+0x38,0x0e,o.combatEFA0(ch),0);
}
void panelFF3C(const CharacterPanelOps&o,int ch){
 const int portrait=o.portrait1F3FC(ch);o.sprite298F0(0,0x10,o.base7F6C()+3,1,portrait,0xcf70+(portrait<<8));
 if((o.flags(ch,0xf0)&0x4000u)==0){const int hp=o.word(ch,0xfc)+o.dword(ch,0x2ca)+o.dword(ch,0x1f3),max=o.word(ch,0xfe);int pct=((hp*5)<<4)+(max>>1);pct/=max;if(pct>0x50)pct=0x50;o.image2975C(pct,o.base7F6C()+0x2f,pct>0x14?0x43:0x57,4,0);}
 panelFED0(o,ch);if(o.flags(ch,0x212))o.image2975C(0,o.base7F6C()+0x44,0x2a,0x13,2);if(o.flags(ch,0xf0)&0x1f00u)o.image2975C(0,o.base7F6C()+0x38,0x2a,0x12,2);
}
}
