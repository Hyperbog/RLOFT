#pragma once

#include "games/ravenloft/data/WorkLevel.h"
#include <cstdint>
#include <span>

namespace ravenloft::game {

// 22-byte render-list facts consumed by CODE.2 0xDACC. 0xDACC reads the
// previous render buffer after 0x2C294 has assigned sort ordinals at +0x0A
// and re-biased the 8.8 scale word at +0x02 by 0x7D00.
struct PlayerTargetRecordDACC {
    std::uint16_t screenX = 0;       // render record +0x00
    std::uint16_t biasedScale = 0;   // render record +0x02 (scale + 0x7D00)
    std::uint16_t category = 0;      // render record +0x08; 0 = NPC
    std::uint16_t actorIndex = 0;    // render record +0x06
    std::uint16_t sortRank = 0;      // render record +0x0A
};

class PlayerTargetSelectorDACC {
public:
    // Direct reconstruction of CODE.2 0xDACC..0xDB93. mode 0 searches the
    // left/center half (screenX <= 0x80), mode 1 the right half (> 0x80).
    static int select(int mode,
                      std::span<const PlayerTargetRecordDACC> records,
                      std::span<const data::RuntimePlacement22> actors);
};

class PlayerAutoTargetDB94 {
public:
    // Direct reconstruction of CODE.2 0xDB94..0xDBD2. Even character indices
    // prefer DACC mode 0 then 1; odd indices prefer mode 1 then 0.
    static int select(std::size_t characterIndex,
                      std::span<const PlayerTargetRecordDACC> records,
                      std::span<const data::RuntimePlacement22> actors);
};

} // namespace ravenloft::game
