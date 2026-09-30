#include "SceneRender139C.h"

namespace ravenloft::game {

void SceneRender139C::run(const SceneRenderState139C& s,
                          const SceneRenderOps139C& o,
                          std::int32_t startX,
                          std::int32_t startY,
                          std::int32_t spanX,
                          std::int32_t spanY,
                          std::int32_t overlays) {
    o.call11A8();

    const std::int32_t endX = startX + spanX;
    const std::int32_t endY = startY + spanY;

    // First pass: base-cell drawing through 0x0B6C.
    std::int32_t screenY = 6;
    for (std::int32_t x = startX; x < endX; ++x, screenY += 8) {
        if (x < 0 || x >= s.width12756) {
            continue;
        }

        std::int32_t screenX = 4;
        for (std::int32_t y = startY; y < endY; ++y, screenX += 5) {
            if (y >= 0 && y < s.height12754) {
                o.call0B6C(x, y, screenY, screenX);
            }
        }
    }

    // Second pass: objects/entities through 0x0CB0. The original repeats
    // the same clipping and screen-coordinate construction.
    screenY = 6;
    for (std::int32_t x = startX; x < endX; ++x, screenY += 8) {
        if (x < 0 || x >= s.width12756) {
            continue;
        }

        std::int32_t screenX = 4;
        for (std::int32_t y = startY; y < endY; ++y, screenX += 5) {
            if (y >= 0 && y < s.height12754) {
                o.call0CB0(x, y, screenY, screenX);
            }
        }
    }

    if (s.drawText5FD9) {
        o.call1228(startX, startY);
    }

    if (overlays != 0) {
        if (!s.buffer6058Ready) {
            o.call1C368(s.buffer6058, s.value1274C + 0x26D);
        }
        o.call30508(0x26, 4, 0xEC, s.buffer6058, 1);
        o.call10C4();
    }
}

} // namespace ravenloft::game
