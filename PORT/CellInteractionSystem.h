#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

// Runtime 10-byte cell-interaction record used by CODE.2 0x20C0C/0x20B40.
// The reconstruction intentionally exposes only fields proven by those routines;
// the remaining bytes stay opaque until their semantics are mapped elsewhere.
struct CellInteractionRecord {
    std::uint8_t type = 0;       // +0, terminator when zero in the per-cell list
    std::uint8_t flags = 0;      // +1, 0x04 enabled; 0x40 inhibits activation; low 2 bits select transition behavior
    std::uint8_t opaque2 = 0;    // +2
    std::uint8_t opaque3 = 0;    // +3
    std::uint8_t cellY = 0;      // +4, proven by 0x1E8C8 / 0x20A24
    std::uint8_t cellX = 0;      // +5, proven by 0x1E8C8 / 0x20A24
    std::uint8_t frame = 0;      // +6, animated state/frame
    std::int8_t frameDelta = 0;  // +7, signed per-tick delta (-1, 0, +1)
    std::uint8_t opaque8 = 0;    // +8
    std::uint8_t opaque9 = 0;    // +9
};
static_assert(sizeof(CellInteractionRecord) == 10);
static_assert(offsetof(CellInteractionRecord, cellY) == 4);
static_assert(offsetof(CellInteractionRecord, cellX) == 5);

struct CellInteractionCallbacks {
    // CODE.2 0x20C0C checks the center of the supplied cell, represented as
    // 16.16 fixed-point X/Y = integer cell + 0x8000. The two NPC callbacks
    // correspond exactly to CODE.2 0x2C800 and 0x2C8E4; the third is 0x2C9DC.
    std::function<bool(std::int32_t xFixed, std::int32_t yFixed)> npcOccupied;
    std::function<bool(std::int32_t xFixed, std::int32_t yFixed)> filteredNpcOccupied2C8E4;
    std::function<bool(std::int32_t xFixed, std::int32_t yFixed)> partyOccupied;

    // DS:CF50, incremented once for each transition started by 0x20C0C.
    std::uint8_t* activeTransitionCount = nullptr;

    // Exact dependency on CODE.2 0x20A24 after a successful state transition.
    // That routine performs interaction sound selection/spatialization; it is
    // reconstructed separately by CellInteractionSoundSystem.
    std::function<void(CellInteractionRecord&)> transitionSideEffect20A24;
};

class CellInteractionSystem {
public:
    // Direct reconstruction of CODE.2 0x20C0C.
    // Returns 1 when a transition was initiated, 0 when rejected.
    static int activate(int cellX,
                        int cellY,
                        CellInteractionRecord& record,
                        const CellInteractionCallbacks& callbacks);
};

} // namespace ravenloft::game
