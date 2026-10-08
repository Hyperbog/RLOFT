#include "rloft/level22_viewport_target.hpp"

#include "rloft/level22_story.hpp"

#include <cstddef>
#include <optional>
#include <stdexcept>

namespace rloft {

std::optional<std::size_t>
Level22ViewportTargetModel::assassin_index(
    const std::optional<WorldSceneTarget>& target,
    const LevelWorldState& world) {
    if (!target || target->source_class != 0U) {
        return std::nullopt;
    }
    const auto index = static_cast<std::size_t>(
        target->source_index);
    if (index >= world.npcs.size()) {
        throw std::out_of_range(
            "viewport NPC target no longer refers to a live record");
    }
    if (world.npcs[index].npc_type_id !=
        Level22Story::kAssassinNpcType) {
        return std::nullopt;
    }
    return index;
}

std::optional<std::size_t>
Level22ViewportTargetModel::loose_item_index(
    const std::optional<WorldSceneTarget>& target,
    const LevelWorldState& world) {
    if (!target || target->source_class != 2U) {
        return std::nullopt;
    }
    const auto index = static_cast<std::size_t>(
        target->source_index);
    if (index >= world.loose_items.size()) {
        throw std::out_of_range(
            "viewport loose-item target no longer refers to a live record");
    }
    return index;
}

} // namespace rloft
