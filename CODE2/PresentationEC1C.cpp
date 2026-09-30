#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct PresentationEC1COps{std::function<std::uint8_t&()> page;std::function<void(int)> setPage,wait27F24,delay28DCC,delay205B4;std::function<void(std::uint32_t,int,int,int,int)> image2975C;std::function<std::uint32_t()> res258DC,res258D0;std::function<void()> initEA90,freeEA90,poll1A1F4,poll30B40,frame2E608,wait28EF0,cleanup28CE8,cleanup20294;std::function<void(int,int)> textEB3C,event2E614;};
void presentationEC1C(const PresentationEC1COps&o){
 o.setPage(o.page());o.image2975C(o.res258DC()+0x65,0,0,0,0);o.image2975C(o.res258DC()+0x66,0xc8,0,0,0);o.page()^=1;o.setPage(o.page());o.image2975C(o.res258DC()+0x65,0,0,0,0);o.image2975C(o.res258DC()+0x66,0xc8,0,0,0);o.page()^=1;o.wait27F24(o.res258D0()+0x11);o.initEA90();
 for(int frame=0;frame<0xc00;++frame){o.frame2E608();o.setPage(o.page());o.image2975C(o.res258DC()+0x65,0,0,0,0);o.image2975C(o.res258DC()+0x66,0xc8,0,0,0);o.textEB3C(frame,1);o.event2E614(1,0);}
 o.freeEA90();o.delay28DCC(0x9c4);o.wait28EF0();o.cleanup28CE8();o.cleanup20294();o.delay205B4(0);
}
}
