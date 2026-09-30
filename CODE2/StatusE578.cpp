#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct StatusE578Ops{std::function<std::uint8_t&(int,int)> byte;std::function<std::int32_t&(int,int)> dword;std::function<std::uint8_t&(int)> redraw;};
void statusE578(const StatusE578Ops&o,int ch){for(int i=0;i<4;++i)if(o.byte(i,0x212)&0x20u){o.redraw(i)=1;o.byte(i,0x212)&=0xdfu;}if(o.byte(ch,0x213)&2u){o.dword(ch,0x23a)=0;o.byte(ch,0x213)&=0xfdu;o.redraw(ch)=1;}}
}
