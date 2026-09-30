#include "SceneCell0CB0.h"

#include <cstddef>

namespace ravenloft::game {
namespace {
std::uint16_t r16(std::span<const std::uint8_t> s,std::size_t o) noexcept {
 return static_cast<std::uint16_t>(s[o])|(static_cast<std::uint16_t>(s[o+1])<<8u);
}
std::int32_t div3333(std::uint16_t v) noexcept {
 return static_cast<std::int32_t>(v)/0x3333;
}
std::int32_t div8192(std::uint16_t v) noexcept {
 // Matches the compiler's signed-division-by-8192 sequence for the
 // zero-extended WORD inputs used here.
 return static_cast<std::int32_t>(v)/0x2000;
}
}

void SceneCell0CB0::run(const SceneCellState0CB0& s,
                        const SceneCellOps0CB0& o,
                        std::int32_t cellX,std::int32_t cellY,
                        std::int32_t screenY,std::int32_t screenX) {
 const std::int32_t linear=cellX+static_cast<std::int32_t>(s.width12756)*cellY;
 if(s.map1760[static_cast<std::size_t>(linear)]==0xFFu) return;

 auto draw=[&](std::int32_t code,std::uint16_t rx,std::uint16_t ry) {
   o.call2975C(code,screenY+div8192(ry),screenX+div3333(rx),s.drawContext,0);
 };

 for(std::int32_t i=o.find0700(cellX,cellY,0);i!=-1;
     i=o.find0700(cellX,cellY,i+1)) {
   const std::size_t b=static_cast<std::size_t>(i)*0x16u;
   std::int32_t code;
   std::uint16_t rx,ry;
   if(s.alternateTable) {
     const std::uint16_t type=r16(s.table22230,b+0x16u);
     code=(type>=0x3Cu&&type<=0x43u)?0x35:(type==0xADu?0x31:0x38);
     rx=r16(s.table22230,b+0x08u); ry=r16(s.table22230,b+0x0Cu);
   } else {
     const std::uint16_t type=r16(s.table6048,b+0x0Eu);
     code=(type>=0x3Cu&&type<=0x43u)?0x35:(r16(s.table22230,b+0x16u)==0xADu?0x31:0x38);
     rx=r16(s.table6048,b); ry=r16(s.table6048,b+4u);
   }
   draw(code,rx,ry);
 }

 if(s.alternateTable) {
   for(std::int32_t i=o.find0794(cellX,cellY,0);i!=-1;
       i=o.find0794(cellX,cellY,i+1)) {
     const std::size_t b=static_cast<std::size_t>(i)*0x16u;
     draw(0x32,r16(s.table14D78,b+8u),r16(s.table14D78,b+0x0Cu));
   }
   for(std::int32_t i=o.find07E4(cellX,cellY,0);i!=-1;
       i=o.find07E4(cellX,cellY,i+1)) {
     const std::size_t b=static_cast<std::size_t>(i)*0x16u;
     draw(0x30,r16(s.table22230,b+8u),r16(s.table22230,b+0x0Cu));
   }
 }

 for(std::int32_t i=o.find0934(cellX,cellY,0);i!=-1;
     i=o.find0934(cellX,cellY,i+1)) {
   const std::size_t b=static_cast<std::size_t>(i)*0x16u;
   draw(0x37,r16(s.table14D78,b+8u),r16(s.table14D78,b+0x0Cu));
 }

 if(s.alternateTable) {
   for(std::int32_t i=o.find083C(cellX,cellY,0);i!=-1;
       i=o.find083C(cellX,cellY,i+1)) {
     const std::size_t b=static_cast<std::size_t>(i)*0x16u;
     draw(0x36,r16(s.table14D78,b+8u),r16(s.table14D78,b+0x0Cu));
   }
 }

 if(s.alternateTable && s.value255D2==cellX && s.value255D6==cellY) {
   o.call0BFC(screenY+div8192(s.value255D0),
              screenX+div3333(s.value255D4));
 }
}

} // namespace ravenloft::game
