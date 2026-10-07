#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "rloft/exploration_state.hpp"
#include "rloft/game_source.hpp"

namespace rloft {

struct ExplorationSessionState final {
    static constexpr std::uint32_t kFreshLevelId = 22;
    static constexpr std::uint32_t kFreshCoordinateA = 0x00048000U;
    static constexpr std::uint32_t kFreshCoordinateB = 0x00048000U;
    static constexpr std::uint32_t kFreshFacing = 270;
    static constexpr std::uint32_t kFreshTimeOfDay = 0x000E0000U;

    std::uint32_t level_id{};
    std::uint32_t coordinate_a{};
    std::uint32_t coordinate_b{};
    std::uint32_t facing{};
    std::uint32_t time_of_day_16_16_hours{};
    std::array<std::uint8_t, 32> persistent_event_flags{};

    PartyImage party{};
    LevelState level{};
    bool level_from_direct_save{};

    [[nodiscard]] static ExplorationSessionState fresh_party(
        PartyImage party,
        LevelState level,
        bool level_from_direct_save = false);
};

class ExplorationSessionLoader final {
public:
    [[nodiscard]] static std::string standalone_level_filename(int level_id);

    // CODE.2 181B4 gives a mutable LEVnnn.DAT file precedence over the
    // corresponding compact record in LEVELS.DAT.
    [[nodiscard]] static LevelState load_level(
        const GameSource& source,
        int level_id,
        bool* used_direct_save = nullptr);

    // Normal CODE.2 fresh-party entry defaults from the startup path around
    // 20608..20773, after CODE.1 has produced the 0x920-byte CHARS image.
    [[nodiscard]] static ExplorationSessionState load_fresh_party(
        const GameSource& source,
        const std::vector<std::uint8_t>& chars);
};

}  // namespace rloft
