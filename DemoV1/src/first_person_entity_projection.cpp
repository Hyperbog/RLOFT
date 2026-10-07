#include "rloft/first_person_entity_projection.hpp"

#include "rloft/fixed_projection.hpp"
#include "rloft/view_sweep_setup.hpp"

#include <cstdint>

namespace rloft {

FirstPersonEntityProjection
FirstPersonEntityProjectionModel::project(
    std::uint32_t world_y_16_16,
    std::uint32_t world_x_16_16,
    const ExplorationSessionState& session,
    const RendererTables& tables) {
    const auto basis =
        ViewSweepSetupModel::camera_basis(
            session, tables);

    const auto delta_y =
        world_y_16_16 - basis.origin_b;
    const auto delta_x =
        world_x_16_16 - basis.origin_a;

    // CODE.2 357BB..357E4:
    // depth = dy*sin(facing+90) - dx*sin(facing) - 0x2000.
    const auto depth =
        FixedProjection::multiply_signed_factor(
            delta_y, basis.sine_quarter) -
        FixedProjection::multiply_signed_factor(
            delta_x, basis.sine_forward) -
        0x00002000U;

    // CODE.2 357E7..3580F:
    // lateral = dy*sin(facing) + dx*sin(facing+90).
    const auto lateral =
        FixedProjection::multiply_signed_factor(
            delta_y, basis.sine_forward) +
        FixedProjection::multiply_signed_factor(
            delta_x, basis.sine_quarter);

    FirstPersonEntityProjection out{};
    out.camera_depth_16_16 = depth;
    out.camera_lateral_16_16 = lateral;

    // CODE.2 checks the signed high word first, then the low word. Points
    // behind the camera, and positive depths below 0x5800, are rejected.
    const auto depth_high =
        static_cast<std::int16_t>(depth >> 16U);
    const auto depth_low =
        static_cast<std::uint16_t>(depth);
    if (depth_high < 0 ||
        (depth_high == 0 &&
         depth_low < 0x5800U)) {
        return out;
    }

    const auto signed_depth =
        static_cast<std::int32_t>(depth);
    const auto signed_lateral =
        static_cast<std::int32_t>(lateral);

    // CODE.2 35834..3584D uses signed IMUL/IDIV and stores only AX into
    // the 16-bit scene X coordinate.
    const auto screen_delta =
        static_cast<std::int32_t>(
            (static_cast<std::int64_t>(
                 static_cast<std::int32_t>(
                     tables.c6cc())) *
             static_cast<std::int64_t>(
                 signed_lateral)) /
            static_cast<std::int64_t>(
                signed_depth));

    out.screen_x =
        static_cast<std::uint16_t>(
            static_cast<std::uint16_t>(
                tables.c6d0()) +
            static_cast<std::uint16_t>(
                screen_delta));

    // CODE.2 35850..3585E arithmetic-shifts depth by eight and performs a
    // signed division of the SINTAB depth numerator by that value.
    const auto depth_divisor =
        signed_depth >> 8;
    const auto extent =
        static_cast<std::int32_t>(
            static_cast<std::int32_t>(
                tables.depth_numerator()) /
            depth_divisor);

    out.projected_extent =
        static_cast<std::uint16_t>(extent);
    out.visible = true;
    return out;
}

}  // namespace rloft
