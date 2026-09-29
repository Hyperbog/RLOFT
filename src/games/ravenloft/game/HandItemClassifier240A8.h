#pragma once
#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/SaveGame.h"
#include <array>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {

struct HandItemClassifierContext240A8 {
    const std::array<data::CharacterRecord,4>* party = nullptr;
    std::uint8_t global258A8 = 0xFF;
};

// CODE.2 0x240A8..0x243DD.
class HandItemClassifier240A8 {
public:
    static int evaluate(std::size_t characterIndex,
                        int objectId,
                        int& classificationOut,
                        const data::ObjectDatabase& objects,
                        const HandItemClassifierContext240A8& context);
};

} // namespace ravenloft::game
