#include "rloft/first_person_entity_projection.hpp"

#include <cassert>
#include <cstdint>

int main() {
    rloft::RendererTables tables{};
    tables.sine_q16_signmag.fill(0);
    tables.sine_q16_signmag[90] = 0x00008000U;
    tables.constants[2] = 160U;
    tables.constants[5] = 0U;
    tables.constants[6] = 100U;
    tables.constants[7] = 0x00100000U;

    rloft::ExplorationSessionState session{};
    session.coordinate_a = 0;
    session.coordinate_b = 0;
    session.facing = 0;

    const auto center =
        rloft::FirstPersonEntityProjectionModel::project(
            0x00020000U, 0U, session, tables);
    assert(center.visible);
    assert(center.camera_depth_16_16 == 0x0000E000U);
    assert(center.camera_lateral_16_16 == 0U);
    assert(center.screen_x == 160U);
    assert(center.projected_extent == 4681U);

    const auto right =
        rloft::FirstPersonEntityProjectionModel::project(
            0x00020000U, 0x00010000U, session, tables);
    assert(right.visible);
    assert(right.camera_depth_16_16 == 0x0000E000U);
    assert(right.camera_lateral_16_16 == 0x00008000U);
    assert(right.screen_x == 217U);
    assert(right.projected_extent == center.projected_extent);

    const auto near =
        rloft::FirstPersonEntityProjectionModel::project(
            0x0000E000U, 0U, session, tables);
    assert(!near.visible);
    assert(near.camera_depth_16_16 == 0x00005000U);
    assert(near.screen_x == 0U);
    assert(near.projected_extent == 0U);

    return 0;
}
