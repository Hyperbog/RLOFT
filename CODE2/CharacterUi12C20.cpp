#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct StatusPanelOps{
 std::function<std::uint16_t(int)> status;
 std::function<void(int)> prepare,drawCharacter;
 std::function<void(int,int,int,int)> drawEntry;
 std::function<void(int,int)> drawSelected;
 std::function<int()> selectedItem;
 std::function<std::uint8_t()> selectedFlag;
 std::function<void()> refresh;
};
void status12C20(const StatusPanelOps&o,int ch){
 std::uint16_t s=static_cast<std::uint16_t>(o.status(ch)&0x1f00u);
 if(!s)return;
 o.prepare(ch);int row=0;
 for(std::uint16_t bit=0x100;bit<=0x1000;bit=static_cast<std::uint16_t>(bit<<1))
  if(s&bit)o.drawEntry(ch,row++,0x88+(row-1)*0x12,0x8c+(row-1)*0x12);
 o.drawSelected(o.selectedItem(),o.selectedFlag());o.drawCharacter(ch);o.refresh();
}
struct CharacterUiDispatchOps{
 std::function<bool(int)> blocked;
 std::function<bool(int,int,int)> container;
 std::function<void(int)> swapPanel,statsPanel,statusPanel;
 std::function<bool(int,int,int,int)> grid,equip;
 std::function<void()> release,refresh,post;
};
void dispatch12E70(const CharacterUiDispatchOps&o,int x,int y,int flags){
 int ch=y/0x50,localY=y-ch*0x50;if(o.blocked(ch))return;
 if(x>=0xdb&&x<=0x11f&&localY>=5&&localY<=0x4d&&o.container(ch,x,localY))return;
 if(x<0x10){o.swapPanel(ch);flags=0;}
 else if(x>=0x45&&x<=0x60&&localY>=0x39&&localY<=0x4d){o.grid(ch,x,localY,flags);return;}
 else if(x>=0x129&&x<=0x18c&&localY>=5&&localY<=0x4a&&o.grid(ch,x,localY,flags))return;
 else if(x>=0x6e&&x<=0x123&&localY>=2&&localY<=0x4d&&o.equip(ch,x,localY,flags))return;
 else if(x>=0x10&&x<=0x27&&localY>=0x44&&localY<=0x4d&&(flags&1))o.statsPanel(ch);
 else if(x>=0x2b&&x<=0x40&&localY>=0x39&&localY<=0x42&&(flags&1))o.statusPanel(ch);
 else if(x>=0x2b&&x<=0x40&&localY>=0x44&&localY<=0x4d&&(flags&1))o.swapPanel(ch);
 if(flags&2){o.release();o.refresh();o.post();}
}
}
