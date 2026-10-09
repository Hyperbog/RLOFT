#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "rloft/exploration_session.hpp"
#include "rloft/inventory_runtime.hpp"
#include "rloft/level_world_state.hpp"

namespace rloft {

// DEV-ONLY, source-neutral, read-only diagnostics. These are NOT
// original CODE.2 save bytes, checksums, or gameplay flags.
// A Level 22 preview uses them to detect unintended subsystem
// resets; future levels can use the identical inspector. No
// instrumentation writes party/world state or disk files.
struct DevLevelSnapshot final {
    std::uint32_t level_id{};
    std::uint32_t coordinate_a{};
    std::uint32_t coordinate_b{};
    std::uint32_t facing{};
    std::uint32_t time_of_day{};
    std::size_t active_actors{};
    std::size_t active_container_views{};
    std::size_t npc_count{};
    std::size_t placed_object_count{};
    std::size_t loose_item_count{};
    std::size_t set_story_flags{};

    std::uint64_t camera_hash{};
    std::uint64_t party_hash{};
    std::uint64_t inventory_hash{};
    std::uint64_t world_hash{};
    std::uint64_t flags_hash{};

    // Stable labels for regression reporting and host preview caption.
    // Digest is FNV-1a over explicit little-endian source values.
    // It is diagnostic only, NOT a secure or collision-free identity.
    [[nodiscard]] static std::string digest8(std::uint64_t digest);
};

// Separate domain hashes permit cross-level continuity tests to
// distinguish legitimate camera/world mutations from accidental
// party, inventory or story-state reconstruction.
struct DevLevelChange final {
    bool camera{};
    bool party{};
    bool inventory{};
    bool world{};
    bool story_flags{};
    [[nodiscard]] bool any() const noexcept {
        return camera || party || inventory || world || story_flags;
    }
};

class DevLevelDiagnostics final {
public:
    [[nodiscard]] static DevLevelSnapshot capture(
        const ExplorationSessionState& session,
        const LevelWorldState& world,
        const InventoryRuntimeState& inventory) noexcept;

    [[nodiscard]] static DevLevelChange compare(
        const DevLevelSnapshot& before,
        const DevLevelSnapshot& after) noexcept;
};

} // namespace rloft
