#pragma once

#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/WorkLevel.h"
#include <cstdint>

namespace ravenloft::game {

class FilteredNpcOverlapSystem {
public:
    FilteredNpcOverlapSystem(const data::WorkLevelSnapshot& work,
                             const data::NpcDatabase& npcs)
        : work_(work), npcs_(npcs) {}

    // Direct reconstruction of CODE.2 0x2C8E4. Argument order mirrors the
    // original routine: candidate Y, candidate X, mover id to exclude.
    bool overlaps(std::int32_t candidateYFixed,
                  std::int32_t candidateXFixed,
                  int moverId) const;

private:
    const data::WorkLevelSnapshot& work_;
    const data::NpcDatabase& npcs_;
};

} // namespace ravenloft::game
