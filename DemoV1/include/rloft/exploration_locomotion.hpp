#pragma once

#include <cstdint>
#include <functional>

#include "rloft/exploration_motion.hpp"
#include "rloft/exploration_session.hpp"
#include "rloft/exploration_world.hpp"
#include "rloft/original_entity_collision.hpp"

namespace rloft {

enum class LocomotionAcceptance {
    NoTranslation,
    Full,
    YOnly,
    XOnly,
    Blocked,
};

struct AppliedExplorationMotion final {
    LocomotionAcceptance acceptance{LocomotionAcceptance::NoTranslation};
    std::uint32_t coordinate_a{};
    std::uint32_t coordinate_b{};
    std::uint32_t facing{};
    bool suppressed_side_effect{};
};

class ExplorationLocomotionModel final {
public:
    // Optional original post-cell entity footprint probe. Existing
    // standalone movement fixtures may omit it.
    using EntityProbe=std::function<
        OriginalEntityContact(std::uint32_t,std::uint32_t)>;
    static constexpr std::uint32_t kPartyCollisionHalfExtent = 0x5800U;

    // Applies CODE.2 2DBAC's movement-attempt order to the authoritative
    // session: full delta, Y-only, X-only, then restore. Heading has already
    // been updated by 2E120 and is committed even when translation is blocked.
    //
    // The live caller provides the original NPC and class-1 object
    // footprint probe, preserving 2C800/2CC88 collision rejection after
    // cell geometry. Authored transitions, NPC AI and event mutations
    // remain unimplemented and are reported as suppressed_side_effect.
    [[nodiscard]] static AppliedExplorationMotion apply(
        ExplorationSessionState& session,
        const ExplorationMotionStep& step,
        const EntityProbe& entity_probe = {});

private:
    enum class GeometryResult {
        Clear,
        Blocked,
        RequiresSideEffect,
    };

    [[nodiscard]] static GeometryResult party_geometry(
        const LevelState& level,
        std::uint32_t coordinate_a,
        std::uint32_t coordinate_b);

    [[nodiscard]] static const CellControlRecord* find_record(
        const LevelState& level,
        CellPosition cell) noexcept;

    [[nodiscard]] static const CellControlRecord* find_dynamic_record(
        const LevelState& level,
        CellPosition cell) noexcept;
};

}  // namespace rloft
