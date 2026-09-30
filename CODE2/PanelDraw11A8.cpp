#include "PanelDraw11A8.h"

namespace ravenloft::game {

void PanelDraw11A8::run(const PanelDrawState11A8& s,
                        const PanelDrawOps11A8& o) {
    o.call296C0(6, 4, 0x189, 0xEF);
    o.call291B0(s.valueC050);

    for (std::int32_t x = 0; x != 0x120; x += 0x60) {
        o.call2978C(0, 0, x, s.context3E, 0);
        o.call2978C(0, 0xC8, x, s.context3E, 0);
    }

    o.call296C0(0, 0, 0x18F, 0x13F);
}

} // namespace ravenloft::game
