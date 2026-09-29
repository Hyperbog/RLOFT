#pragma once

#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/SaveGame.h"
#include "games/ravenloft/data/WorkLevel.h"
#include "games/ravenloft/game/GroundObjectPickup21704.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace ravenloft::game {

struct OpeningGroundPickupResult223F4 {
    bool handled=false;
    bool picked=false;
    bool startMistTransition=false;
    std::uint16_t objectId=0xFFFFu;
};

// Reconstruction of the object-pickup branch at CODE.2 0x224E6..0x22713
// inside the 0x223F4 interaction dispatcher. Screen hit-testing/UI text are
// upstream presentation boundaries; this component begins with the selected
// runtime-object index and original click-flags value.
class OpeningGroundPickup223F4 {
public:
    static OpeningGroundPickupResult223F4 execute(
        std::size_t objectIndex,
        std::uint32_t clickFlags,
        std::vector<data::RuntimePlacement22>& runtimeObjects,
        HeldObjectState1A344& held,
        std::array<data::CharacterRecord,4>& party,
        const data::ObjectDatabase& objects,
        const std::array<std::vector<std::int16_t>,data::kContainerDescriptorCount>& containerItems,
        const GroundObjectPickupContext21704& pickupContext={});
};

} // namespace ravenloft::game
