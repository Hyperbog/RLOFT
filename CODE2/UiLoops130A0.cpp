#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct UiLoopOps{
 std::function<int()> poll;
 std::function<std::int16_t()> mouseY;
 std::function<int()> maxY;
 std::function<void(std::uint8_t)> page;
 std::function<void(int,int)> select;
 std::function<void(int,int,int,int,int)> draw;
 std::function<void(int,int)> action;
};
void loop130A0(const UiLoopOps&o){
 int last=-1,pos=0;
 for(;;){int ev=o.poll();if(!ev){pos=o.mouseY();if(pos>o.maxY())pos=o.maxY();if(pos==last)continue;o.page(0);o.select(pos,0);o.draw(0x62,0,0x10,0x140,pos);last=pos;continue;}if(ev&1)o.action(pos,0);return;}
}
struct TimedSelectionOps{
 std::function<int()> input;
 std::function<std::uint32_t()> ticks;
 std::function<std::int16_t()> item;
 std::function<std::uint8_t()> flag;
 std::function<void(int,int,int,int,int,int,int)> invoke;
 std::function<void(int,int)> redrawSelection;
 std::function<void()> refresh;
};
void timed13118(const TimedSelectionOps&o,int x,int y){
 std::uint32_t deadline=0;bool armed=false;
 for(;;){int k=o.input();if(k==0x44&&o.item()!=-1)o.invoke(0x80,o.item(),o.flag(),x,y,1,1);
  if(armed&&o.ticks()>deadline){deadline=0;armed=false;o.redrawSelection(o.item(),o.flag());}
  o.refresh();
 }
}
}
