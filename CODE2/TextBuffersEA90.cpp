#include <array>
#include <cstdint>
#include <cstring>
#include <functional>
namespace ravenloft::game {
struct TextBuffersEA90Ops{std::function<std::uint8_t*(int)> alloc;std::function<void(std::uint8_t*)> free;std::function<const char*(int)> source;std::function<void(char*,int)> render1C368;};
struct TextBuffersEA90{std::array<std::uint8_t*,17> p{};void init(const TextBuffersEA90Ops&o){for(auto&x:p){x=o.alloc(0x50);std::memset(x,0,0x50);}}void release(const TextBuffersEA90Ops&o){for(auto*x:p)o.free(x);}void load(const TextBuffersEA90Ops&o,int value){for(int i=0;i<16;++i){const char*s=o.source(i);std::strncpy(reinterpret_cast<char*>(p[i]),s,0x4f);p[i][0x4f]=0;}o.render1C368(reinterpret_cast<char*>(p[16]),value%0xc0+0x371);}};
}
