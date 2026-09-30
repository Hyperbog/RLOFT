#include <cstdint>
#include <functional>
#include <vector>
namespace ravenloft::game {
struct TransitionCADCRecord{std::int32_t x=0;std::int32_t delay=0;std::uint8_t done=0;};
struct TransitionCADCOps{
 std::function<void*(int)> alloc3104C;
 std::function<void(void*)> free310B0;
 std::function<std::uint32_t()> random30CA2;
 std::function<void(int)> page291B0;
 std::function<void(int,int,int,int,int,int,int)> draw2A078;
};
void transitionCADC(const TransitionCADCOps&o,std::uint8_t page){
 auto*mem=o.alloc3104C(0xe10); if(!mem)return;
 auto*r=static_cast<TransitionCADCRecord*>(mem);
 o.page291B0(page);
 for(int i=0;i<0x190;++i){r[i].x=0;r[i].delay=static_cast<std::int32_t>(((o.random30CA2()*15u)>>15)+10u);r[i].done=0;}
 int finished=0;
 while(finished!=0x190){finished=0;for(int i=0;i<0x190;++i){if(r[i].done){++finished;continue;}if(r[i].delay){--r[i].delay;continue;}r[i].delay=static_cast<std::int32_t>((o.random30CA2()*5u)>>15);o.page291B0(page);o.draw2A078(page^1,i,r[i].x,i,r[i].x,1,4);o.page291B0(page^1);r[i].x+=4;if(r[i].x>=0x140){r[i].done=1;++finished;}}}
 o.free310B0(mem);
}
}
