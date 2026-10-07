#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

#include "rloft/exploration_session.hpp"
#include "rloft/level_world_state.hpp"
#include "rloft/npc_catalog.hpp"
#include "rloft/renderer_tables.hpp"

namespace rloft {

// Stable portion of CODE.2's 0x16-byte generated scene record after NPC
// projection/frame selection. The four trailing interaction-bound words are
// deliberately excluded until helper 21BF0 is translated.
struct NpcSceneEmission final {
    std::uint16_t screen_x{};
    std::uint16_t projected_extent{};
    std::uint16_t resource_id{};
    std::uint16_t source_index{};
    std::uint16_t source_class{};  // CODE.2 class 0 = NPC.
    std::uint16_t render_scale_lookup_bias{};
};

class NpcSceneModel final {
public:
    static constexpr std::size_t kMaxSceneRecords = 0x190U;

    using Random15 = std::function<std::uint16_t()>;

    // CODE.2 2B6D0..2BB91, stopping immediately before the 21BF0
    // interaction-bound calculation. Visibility, projection, threshold,
    // animation/directional frame selection, source identity, and scale bias
    // are preserved exactly for the supplied-build NPC definitions.
    [[nodiscard]] static std::vector<NpcSceneEmission> build(
        const LevelWorldState& world,
        const ExplorationSessionState& session,
        const NpcCatalog& catalog,
        const RendererTables& tables,
        std::uint8_t render_threshold,
        const Random15& shared_random15 = {});
};

}  // namespace rloft
