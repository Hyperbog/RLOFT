#pragma once

#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/SaveGame.h"
#include <array>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {

struct HeldObjectState1A344 {
    int objectId = -1;          // CODE.2 DS:8600
    std::uint8_t state = 0;     // CODE.2 DS:8602
};

// CODE.2 0xF390 recursively tests whether an object is, or contains, targetId.
// Container contents come from the loader-reconstructed DS:7330 descriptor arrays.
class ObjectContainsF390 {
public:
    static bool evaluate(int objectId,
                         int targetId,
                         const data::ObjectDatabase& objects,
                         const std::array<std::vector<std::int16_t>,data::kContainerDescriptorCount>& containerItems);
};

// CODE.2 0xF4B0: ownership predicate used by the opening quest-object pickup
// branch. The held/cursor object is tested first, followed by 21 WORD object
// slots in each non-0x8000 party record; nested containers recurse through F390.
class PartyOwnsObjectF4B0 {
public:
    static bool evaluate(int targetId,
                         const HeldObjectState1A344& held,
                         const std::array<data::CharacterRecord,4>& party,
                         const data::ObjectDatabase& objects,
                         const std::array<std::vector<std::int16_t>,data::kContainerDescriptorCount>& containerItems);
};

} // namespace ravenloft::game
