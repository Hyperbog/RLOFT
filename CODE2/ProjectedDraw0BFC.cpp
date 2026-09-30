#include "ProjectedDraw0BFC.h"

#include <cstdint>

namespace ravenloft::game {

void ProjectedDraw0BFC::run(const ProjectedDrawState0BFC& state,
                            const ProjectedDrawOps0BFC& ops,
                            std::int32_t arg0,
                            std::int32_t arg1,
                            std::int32_t x,
                            std::int32_t y) {
    // 0x0BFF..0x0C1D.
    ops.call2975C(0x34, x - 2, y, state.drawContext, 0);

    // 0x0C20..0x0C3A.
    std::int32_t dx = 0;
    std::int32_t dy = 0;
    ops.call2DF2C(dx, dy, 0x10000, state.projectionData);

    // The stack locals are four WORDs. The original then repeatedly performs
    // DWORD additions at +8 and +0C, so preserve the resulting packed-word
    // behavior explicitly.
    std::uint32_t packedA =
        static_cast<std::uint16_t>(0) |
        (static_cast<std::uint32_t>(static_cast<std::uint16_t>(y)) << 16u);
    std::uint32_t packedB =
        static_cast<std::uint16_t>(x) |
        (static_cast<std::uint32_t>(static_cast<std::uint16_t>(0)) << 16u);

    for (std::int32_t i = 0; i < 8; ++i) {
        packedB += static_cast<std::uint32_t>(dy);
        packedA += static_cast<std::uint32_t>(dx);
    }

    const std::int16_t word0 =
        static_cast<std::int16_t>(packedA & 0xFFFFu);
    const std::int16_t word2 =
        static_cast<std::int16_t>((packedA >> 16u) & 0xFFFFu);
    const std::int16_t word4 =
        static_cast<std::int16_t>(packedB & 0xFFFFu);
    const std::int16_t word6 =
        static_cast<std::int16_t>((packedB >> 16u) & 0xFFFFu);

    // 0x0C74..0x0CAC tests WORD +6, then WORD +2, and bounds WORD +6 to
    // 0x18F. The call receives WORD +2 followed by WORD +6.
    if (word6 < 0 || word2 < 0 || word6 > 0x18F) {
        return;
    }

    (void)word0;
    (void)word4;
    ops.call29CEC(arg0, arg1, word2, word6, 0xFF);
}

} // namespace ravenloft::game
