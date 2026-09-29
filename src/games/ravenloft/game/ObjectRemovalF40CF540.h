#pragma once

#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/SaveGame.h"
#include "games/ravenloft/game/ObjectOwnershipF390F4B0.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

namespace ravenloft::game {

// CODE.2 0xF40C recursively removes the first matching target object from a
// category-1 container descriptor. The routine clears the descriptor's slot
// occupancy bit and returns immediately after the first removal.
class ContainerRemoveF40C {
public:
    static bool execute(int objectId,
                        int targetId,
                        const data::ObjectDatabase& objects,
                        std::array<std::vector<std::int16_t>,data::kContainerDescriptorCount>& containerItems,
                        std::array<std::uint8_t,data::kContainerDescriptorCount>& containerState);
};

struct ObjectRemoveContextF540 {
    // Original 0xF540 calls 0xF238 after clearing a direct party slot. The slot
    // is already -1 at that point, so 0xF238 performs no bit-state mutation;
    // retain the boundary for provenance/observation without inventing UI work.
    std::function<void(std::size_t,std::size_t,int)> slotBoundaryF238;
};

// CODE.2 0xF540 removes the first occurrence of targetId from the held/cursor
// object or from the four active characters' 21 object slots. Nested category-1
// containers delegate to converted 0xF40C. The bool return is a reconstruction
// convenience; the original routine itself returns void.
class ObjectRemoveF540 {
public:
    static bool execute(int targetId,
                        HeldObjectState1A344& held,
                        std::array<data::CharacterRecord,4>& party,
                        const data::ObjectDatabase& objects,
                        std::array<std::vector<std::int16_t>,data::kContainerDescriptorCount>& containerItems,
                        std::array<std::uint8_t,data::kContainerDescriptorCount>& containerState,
                        const ObjectRemoveContextF540& context={});
};

} // namespace ravenloft::game
