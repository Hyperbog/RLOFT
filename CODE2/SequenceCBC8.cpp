#include <array>
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct SequenceCBC8Ops{
 std::function<void()> f1F1C8,f1EA78,f28168,f1A9F8,f2E608,f1EA04,f1AACC,f1A288;
 std::function<void(int)> refresh1F494,page291B0,wait27F24,delay205B4;
 std::function<void(int,int,int,int)> draw2E510;
 std::function<void(int,int)> event28C54,event2E614;
 std::function<int()> poll1A1F4;
 std::function<void()> transitionCADC;
 std::function<std::uint32_t()> base258DC;
 std::function<std::uint32_t()> value258D0;
};
void sequenceCBC8(const SequenceCBC8Ops&o,std::uint8_t page){
 static constexpr std::array<int,9> k={1,0,0,1,0,2,3,4,5};
 o.f1F1C8();o.f1EA78();o.refresh1F494(1);o.f28168();o.f1A9F8();o.page291B0(page);o.refresh1F494(0);
 for(int i=0;i<9;++i){o.f2E608();o.page291B0(page);o.draw2E510(4,0x20,static_cast<int>(o.base258DC()+0x5d+k[i]),0);o.f1EA04();if(i==0)o.event28C54(0x11,1);o.f1EA78();
  if(i==6){o.page291B0(page);o.draw2E510(4,0x20,static_cast<int>(o.base258DC()+0x5d+k[i+1]),0);o.f1EA04();o.f1AACC();o.wait27F24(static_cast<int>(o.value258D0()+1));++i;o.transitionCADC();o.f1A288();}
  else o.event2E614(0x12,0);
 }
 while(o.poll1A1F4()!=0){} while(o.poll1A1F4()==0){} o.delay205B4(0x0c);
}
}
