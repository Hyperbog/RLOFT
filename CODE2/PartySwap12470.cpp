#include <cstdint>
namespace ravenloft::game {
std::uint16_t remapOwner12470(std::uint16_t w,int a,int b){
 unsigned owner=(w>>11)&7u;
 if(owner==static_cast<unsigned>(a&7)) return static_cast<std::uint16_t>((w&0xc7ffu)|((b&7)<<11));
 if(owner==static_cast<unsigned>(b&7)) return static_cast<std::uint16_t>((w&0xc7ffu)|((a&7)<<11));
 return w;
}
}
