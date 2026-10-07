#pragma once

#include <cstdint>

#include "rloft/exploration_motion.hpp"
#include "rloft/exploration_session.hpp"
#include "rloft/exploration_world.hpp"

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
    static constexpr std::uint32_t kPartyCollisionHalfExtent = 0x5800U;

    // Applies CODE.2 2DBAC's movement-attempt order to the authoritative
    // session: full delta, Y-only, X-only, then restore. Heading has already
    // been updated by 2E120 and is committed even when translation is blocked.
    //
    // This Demo-v1 boundary deliberately does not activate the excluded
    // NPC/world-object/event side effects that follow the static geometry
    // checks in 2D5D0/2DBAC. Geometry that would invoke a transition is
    // reported via suppressed_side_effect and is not executed.
    [[nodiscard]] static AppliedExplorationMotion apply(
        ExplorationSessionState& session,
        const ExplorationMotionStep& step);

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
