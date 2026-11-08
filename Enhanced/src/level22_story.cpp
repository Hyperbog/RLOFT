#include "rloft/level22_story.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace rloft {
namespace {

constexpr std::array<Level22Drop, 4> kDrops{{
    {0x6EU, Level22Story::kWitheredHeart},
    {0x78U, Level22Story::kFingerOfCommanding},
    {0x82U, Level22Story::kAssassinsParchment},
    {0x8CU, Level22Story::kLordDheltHolySymbol},
}};

constexpr std::array<std::uint16_t, 4> kQuestObjects{{
    Level22Story::kFingerOfCommanding,
    Level22Story::kWitheredHeart,
    Level22Story::kLordDheltHolySymbol,
    Level22Story::kAssassinsParchment,
}};

}  // namespace

const std::array<Level22Drop, 4>&
Level22Story::assassin_death_drops() noexcept {
    return kDrops;
}

const std::array<std::uint16_t, 4>&
Level22Story::quest_objects() noexcept {
    return kQuestObjects;
}

bool Level22Story::pickup_completes_set(
    std::uint16_t picked_object,
    const std::function<bool(std::uint16_t)>& has_object) {
    bool recognized = false;
    for (const auto object : kQuestObjects) {
        if (object == picked_object) {
            recognized = true;
            break;
        }
    }
    if (!recognized) return false;

    for (const auto object : kQuestObjects) {
        if (object == picked_object) continue;
        if (!has_object(object)) return false;
    }
    return true;
}

}  // namespace rloft
