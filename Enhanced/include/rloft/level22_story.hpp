#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>

namespace rloft {

struct Level22Drop final {
    std::uint32_t screen_seed{};
    std::uint16_t object_id{};
};

struct Level22MistTransition final {
    static constexpr int kFrameCount = 22;
    static constexpr int kLoadFrame = 20;
    static constexpr std::uint16_t kFrameResourceOffset = 0x67;

    int target_level{23};
    std::uint32_t coordinate_a{0x00098000U};
    std::uint32_t coordinate_b{0x003A8000U};
    std::uint32_t facing{180U};

    std::array<std::uint16_t, 3> removed_after_scene{{
        0x87, 0x88, 0x74
    }};
};

class Level22Story final {
public:
    static constexpr int kLevelId = 22;
    static constexpr std::uint16_t kAssassinNpcType = 9;

    static constexpr std::uint16_t kFingerOfCommanding = 0x87;
    static constexpr std::uint16_t kWitheredHeart = 0x88;
    static constexpr std::uint16_t kLordDheltHolySymbol = 0x74;
    static constexpr std::uint16_t kAssassinsParchment = 0xE9;

    [[nodiscard]] static const std::array<Level22Drop, 4>&
    assassin_death_drops() noexcept;

    [[nodiscard]] static const std::array<std::uint16_t, 4>&
    quest_objects() noexcept;

    // Mirrors CODE.2 223F4 after the clicked family-C record has been
    // transferred to the held-item path. The just-picked object is not queried
    // through the possession callback; only the other three are.
    [[nodiscard]] static bool pickup_completes_set(
        std::uint16_t picked_object,
        const std::function<bool(std::uint16_t)>& has_object);

    [[nodiscard]] static constexpr Level22MistTransition
    mist_transition() noexcept {
        return {};
    }
};

}  // namespace rloft
