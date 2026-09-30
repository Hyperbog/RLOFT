#include "SceneRefresh1F6C.h"

namespace ravenloft::game {
namespace {
std::int32_t scale2(std::uint32_t v) noexcept {
    return static_cast<std::int32_t>((v*2u)>>15u);
}
}

void SceneRefresh1F6C::run(SceneRefreshState1F6C& s,
                           const SceneRefreshOps1F6C& o,
                           std::int32_t overlayFlag) {
    o.call291B0(s.valueC050);
    o.call290D0(s.value12750);

    for(std::int32_t x=8;x<0xB8;x+=8) {
        std::uint32_t v=o.call30CA2(0,x,o.contextAt258DC(0x35),0);
        o.call2975C(scale2(v)+9,0,x,o.contextAt258DC(0x35),0);

        v=o.call30CA2(0x180,x,o.contextAt258DC(0x35),0);
        o.call2975C(scale2(v)+5,0x180,x,o.contextAt258DC(0x35),0);
    }

    for(std::int32_t x=0x10;x<0x180;x+=0x10) {
        std::uint32_t v=o.call30CA2(x,0,o.contextAt258DC(0x35),0);
        o.call2975C(scale2(v)+0x0C,x,0,o.contextAt258DC(0x35),0);

        v=o.call30CA2(x,0xF0,o.contextAt258DC(0x35),0);
        o.call2975C(scale2(v)+0x12,x,0xF0,o.contextAt258DC(0x35),0);
    }

    o.call139C(s.value12744,s.value12740,0x30,0x2F,overlayFlag);
    o.call1A510(s.valueC050);
    s.valueC050^=1u;
}

} // namespace ravenloft::game
