#pragma once
#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/SaveGame.h"
#include <cstdint>
#include <functional>

namespace ravenloft::game {

// Small routines called by CODE.2 0xEFA0. Kept separate to preserve the
// original callgraph for the later complete source reconstruction.
class CharacterDefenseHelpers {
public:
    // CODE.2 0xEEE0: body slot (+0x60) contains an object with category +0x2F == 4.
    static int bodyIsArmorEEE0(const data::CharacterRecord& character,
                               const data::ObjectDatabase& objects);

    // CODE.2 0xEF34: for object categories 2..8, returns -(BYTE +0x37), else 0.
    static int itemAcMagicAdjustmentEF34(int objectId,
                                         const data::ObjectDatabase& objects);

    // CODE.2 0xF2D4: object id is in either ring slot +0x58/+0x5A.
    static int ringSlotContainsF2D4(const data::CharacterRecord& character,
                                    int objectId);

    // CODE.2 0xFD84. The recursive container-weight routine 0xF5F8 is kept as
    // an explicit dependency until its container runtime state is converted.
    static int carriedWeightFD84(const data::CharacterRecord& character,
                                 const data::ObjectDatabase& objects,
                                 const std::function<int(int containerIndex)>& containerWeightF5F8);

    // CODE.2 0xFE44, using the exact DS:7E8C 22x5 threshold table.
    static int encumbranceTierFE44(const data::CharacterRecord& character,
                                   const data::ObjectDatabase& objects,
                                   const std::function<int(int containerIndex)>& containerWeightF5F8);
};

} // namespace ravenloft::game
