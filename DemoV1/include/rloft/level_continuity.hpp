#pragma once

#include <cstddef>
#include <cstdint>
#include <map>
#include <optional>
#include <utility>

#include "rloft/exploration_session.hpp"
#include "rloft/level_world_state.hpp"

namespace rloft {

// A target must come from a separately verified original event/exit.
// This model does not synthesize portals, entry positions or scripts.
struct LevelTransitionLocation final {
    int level_id{};
    std::uint32_t coordinate_a{};
    std::uint32_t coordinate_b{};
    std::uint32_t facing{};
};

struct LevelContinuityEntry final {
    LevelState level{};
    LevelWorldState world{};
    bool from_direct_save{};
};

// In-memory mutable level snapshots. The live party, inventory, persistent
// event flags, global clocks and RNG remain owned by ExplorationRuntime.
// This store is NOT an original SAVE.DAT writer or an authored exit router.
class LevelContinuityStore final {
public:
    [[nodiscard]] std::optional<LevelContinuityEntry> recall(
        int level_id) const;
    [[nodiscard]] const std::map<int,LevelContinuityEntry>& snapshots() const noexcept {
        return snapshots_;
    }
    void restore_snapshots(std::map<int,LevelContinuityEntry> entries) {
        snapshots_=std::move(entries);
    }
    [[nodiscard]] std::size_t stored_count() const noexcept {
        return snapshots_.size();
    }

    // Atomic stage-and-swap: invalid target/geometry or failed allocations
    // leave current session/world and saved levels unchanged.
    void enter(
        ExplorationSessionState& session,
        LevelWorldState& current_world,
        const LevelTransitionLocation& target,
        LevelContinuityEntry destination);

private:
    std::map<int, LevelContinuityEntry> snapshots_;
};

}  // namespace rloft
