#pragma once
#include <cstdint>
#include <map>
#include <vector>
#include "rloft/exploration_session.hpp"
#include "rloft/inventory_runtime.hpp"
#include "rloft/level_continuity.hpp"
#include "rloft/level_world_state.hpp"
namespace rloft {
// Native V1 progress save: independent from the original game's SAVE.DAT.
struct EnhancedGameSnapshot final {
    std::uint64_t original_media_id{};
    ExplorationSessionState session{};
    LevelWorldState world{};
    InventoryRuntimeState inventory{};
    std::map<int,LevelContinuityEntry> visited_levels;
    std::uint32_t random_state{};
    std::uint32_t original_story_selector{};
};
class EnhancedGameSaveCodec final {
public:
    [[nodiscard]] static std::vector<std::uint8_t> encode(const EnhancedGameSnapshot&);
    [[nodiscard]] static EnhancedGameSnapshot decode(const std::vector<std::uint8_t>&);
};
}
