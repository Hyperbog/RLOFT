#pragma once

#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/ResourceStore.h"
#include "games/ravenloft/data/TrigTable.h"
#include "games/ravenloft/data/WorkLevel.h"
#include "games/ravenloft/game/OpeningSequence.h"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>

namespace ravenloft::game {

// Opening-relevant render record fields consumed by CODE.2 0x223F4.
// The original record is 0x16 bytes; these are the fields used by the
// cursor-selection loop at 0x22413..0x224E6 for category 2 (ground objects).
struct OpeningGroundObjectHitRecord223F4 {
    std::uint16_t drawOrder=0;   // original render record +0x0A
    std::size_t sourceIndex=0;   // original render record +0x06
    std::uint16_t scale8_8=0;    // depth-sort key used by 0x2C294
    std::int32_t left=0;         // +0x0E
    std::int32_t right=0;        // +0x10
    std::int32_t top=0;          // +0x12
    std::int32_t bottom=0;       // +0x14
};

// Reconstructs the opening ground-object portion of the previous-frame render
// list built by CODE.2 0x2BDF4 and consumed by 0x223F4. It uses the already
// reconstructed 0x35798 projection, 0x2B2F4 normal 8/4-view selector and
// 0x21BF0 sprite rectangle setup. The object builder's literal +0x20 Y bias at
// 0x2BFC1/0x2BFC9 is preserved.
class OpeningGroundObjectHit223F4 {
public:
    static std::vector<OpeningGroundObjectHitRecord223F4> build(
        std::span<const data::RuntimePlacement22> runtimeObjects,
        const data::ObjectDatabase& objects,
        const data::ResourceStore& resources,
        const data::TrigTable& trig,
        const PartyState& party);

    // CODE.2 0x223FB..0x224E6 scans draw-order values from count-1 down to 0
    // and uses inclusive rectangle comparisons. The object branch requires
    // click bit 0 at 0x224EF. Returns the runtime-object source index supplied
    // to 0x21704, or nullopt if no ground object is selected.
    static std::optional<std::size_t> select(
        std::int32_t cursorX,
        std::int32_t cursorY,
        std::uint32_t clickFlags,
        std::span<const OpeningGroundObjectHitRecord223F4> records);
};

} // namespace ravenloft::game
