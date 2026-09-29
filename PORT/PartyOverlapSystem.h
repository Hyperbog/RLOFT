#pragma once

#include <cstdint>

namespace ravenloft::game {

class PartyOverlapSystem {
public:
    // Direct reconstruction of CODE.2 0x2C9DC. Argument order mirrors the
    // original routine: candidate Y, candidate X, then current party X/Y state.
    static bool overlaps(std::int32_t candidateYFixed,
                         std::int32_t candidateXFixed,
                         std::int32_t partyXFixed,
                         std::int32_t partyYFixed);
};

} // namespace ravenloft::game
