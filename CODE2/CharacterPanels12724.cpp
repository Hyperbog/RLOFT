#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct Panel12724Ops{
 std::function<std::uint32_t(int)> mask;
 std::function<void(int)> prepare,drawCharacter;
 std::function<void(int,int,int,int)> drawText;
 std::function<void(int,int)> drawSelected;
 std::function<int()> selectedItem;
 std::function<std::uint8_t()> selectedFlag;
 std::function<void()> refresh;
};
void panel12724(const Panel12724Ops&o,int ch){
 if(o.mask(ch)==0)return;
 o.prepare(ch);
 int row=0;
 for(std::uint32_t bit=1;bit<=0x1000000u;bit<<=1)
  if(o.mask(ch)&bit)o.drawText(ch,row++,0x88+(row-1)*0x12,0x8c+(row-1)*0x12);
 o.drawSelected(o.selectedItem(),o.selectedFlag());
 o.drawCharacter(ch);
 o.refresh();
}
struct Stats12988Ops{
 std::function<void(int)> prepare,drawCharacter;
 std::function<int(int)> a,b,c;
 std::function<void(int,int,int)> line;
 std::function<void(int,int)> drawSelected;
 std::function<int()> selectedItem;
 std::function<std::uint8_t()> selectedFlag;
 std::function<void()> refresh;
};
void stats12988(const Stats12988Ops&o,int ch){
 o.prepare(ch);
 o.line(0x266,o.c(ch),0x88);
 o.line(0x267,o.a(ch),0x96);
 o.line(0x268+o.b(ch),0,0xa4);
 o.drawSelected(o.selectedItem(),o.selectedFlag());
 o.drawCharacter(ch);
 o.refresh();
}
}
