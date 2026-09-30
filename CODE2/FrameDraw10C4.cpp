#include "FrameDraw10C4.h"

namespace ravenloft::game {

void FrameDraw10C4::run(const FrameDrawState10C4& s,
                        const FrameDrawOps10C4& o) {
    // Preserve the original eight calls and their order exactly.
    o.call2975C(0,   0,     0x67, s.context3C, 0);
    o.call2975C(1,   0x170, 0x67, s.context3C, 0);
    o.call2975C(0,   0xA4,  0,    s.context3D, 0);
    o.call2975C(1,   0xA4,  0xE4, s.context3D, 0);
    o.call2975C(0,   8,     0x75, s.context36, 0);
    o.call2975C(1,   0x17A, 0x75, s.context36, 0);
    o.call2975C(2,   0xC0,  4,    s.context36, 0);
    o.call2975C(3,   0xC0,  0xE9, s.context36, 0);
}

} // namespace ravenloft::game
