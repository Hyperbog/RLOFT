#include "TextDraw1228.h"

#include <cstddef>

namespace ravenloft::game {
namespace {
std::int16_t s16(const std::uint8_t* p,std::size_t o) noexcept {
 return static_cast<std::int16_t>(
   static_cast<std::uint16_t>(p[o]) |
   (static_cast<std::uint16_t>(p[o+1])<<8u));
}
}

void TextDraw1228::run(const TextDrawState1228& s,
                       const TextDrawOps1228& o,
                       std::int32_t arg0,
                       std::int32_t arg1) {
    const std::int32_t left = arg0 * 8;
    const std::int32_t right = left + 0x184;
    const std::int32_t top = arg1 * 5;
    const std::int32_t bottom = top + 0xEC;

    const std::uint8_t* record = o.call2F29C(1, 0);
    o.call296C0(6, 4, 0x189, 0xEF);

    while (s16(record, 4) != 0) {
        std::int32_t x = s16(record, 0);
        std::int32_t y = s16(record, 2);
        const std::int32_t count = s16(record, 4);

        // 0x302CC's return value is used as the vertical extent.
        const std::int32_t extent = o.call302CC(record, 1, count);

        const bool xVisible = x >= left && x <= right;
        const bool yVisible =
            (y >= top && y <= bottom) ||
            (y + extent >= top && y + extent <= bottom);

        if (xVisible && yVisible) {
            std::int32_t drawX = x - left;
            std::int32_t drawY = y - top;
            const std::int32_t baseX = drawX;

            for (std::int32_t i = 0; i < count; ++i) {
                const std::uint8_t ch = record[6 + i];
                if (ch == 0x0D) {
                    drawX = baseX + 0x0C;
                    drawY = y - top;
                } else {
                    o.call29804(ch, drawX, drawY, 0x5760);
                    drawY += s.glyphTable5760[
                        static_cast<std::size_t>(ch) + 4u];
                }
            }
        }

        record += 6 + count;
    }

    o.call296C0(0, 0, 0x18F, 0x13F);
}

} // namespace ravenloft::game
