#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>

namespace rloft {

struct AssassinDrop final {
    std::uint16_t screen_seed{};
    std::uint16_t object_id{};
};

struct AssassinRelicTransition final {
    static constexpr std::uint16_t kAssassinNpcType = 9;

    // CODE.2 0E284: NPC type 9 scatters these four family-C items at death.
    static constexpr std::array<AssassinDrop, 4> kDrops{{
        {0x006E, 0x0088}, // WITHERED HEART OF MORTAL ARDOR
        {0x0078, 0x0087}, // FINGER OF COMMANDING
        {0x0082, 0x00E9}, // ASSASSIN'S PARCHMENT
        {0x008C, 0x0074}, // LORD DHELT'S HOLY SYMBOL
    }};

    static constexpr std::array<std::uint16_t, 4> kRelics{{
        0x0087, 0x0088, 0x0074, 0x00E9,
    }};

    static constexpr std::int16_t kStagingColumn = 0x17;
    static constexpr std::int16_t kStagingRow = 7;
    static constexpr std::uint16_t kStagingFraction = 0x8000;
    static constexpr std::uint32_t kStagingFacing = 0x00D5;

    static constexpr std::uint32_t kEffectMode = 2;
    static constexpr std::uint32_t kEffectResource = 0x21;
    static constexpr std::uint32_t kEffectVariant = 7;

    static constexpr std::size_t kTransitionFrameCount = 0x16;
    static constexpr std::uint16_t kFirstFrameResourceOffset = 0x67;
    static constexpr std::size_t kLevelChangeFrame = 0x14;

    static constexpr std::uint32_t kDestinationLevel = 0x17;
    static constexpr std::uint32_t kDestinationCoordinateA = 0x00098000U;
    static constexpr std::uint32_t kDestinationCoordinateB = 0x003A8000U;
    static constexpr std::uint32_t kDestinationFacing = 0x00B4;

    // CODE.2 consumes these after the transition. The parchment survives.
    static constexpr std::array<std::uint16_t, 3> kConsumedAfterTransition{{
        0x0087, 0x0088, 0x0074,
    }};

    [[nodiscard]] static bool is_relic(
        std::uint16_t object_id) noexcept;

    // The pickup path checks the other three relics only, so whichever relic
    // is collected last is the apparent trigger.
    [[nodiscard]] static bool completes_on_pickup(
        std::uint16_t picked_object_id,
        const std::function<bool(std::uint16_t)>& party_contains);

    [[nodiscard]] static bool consumed_after_transition(
        std::uint16_t object_id) noexcept;
};

}  // namespace rloft
