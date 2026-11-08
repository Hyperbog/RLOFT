#include "rloft/environment_projection.hpp"

#include <array>
#include <cassert>

int main() {
    rloft::EnvironmentProfile profile{};
    profile.res8_base = 8;
    profile.stored_record_count = 3;
    profile.placements.push_back(
        rloft::EnvironmentPlacement{0x18, 270, 64, 10});
    profile.placements.push_back(
        rloft::EnvironmentPlacement{24, 300, 32, 20});
    profile.placements.push_back(
        rloft::EnvironmentPlacement{});

    std::array<std::uint16_t, 32> rng{};
    rng[0] = 0;
    rng[1] = 0x7FFF;

    const auto commands =
        rloft::EnvironmentProjectionModel::build(
            profile, 270, 9, rng);

    assert(commands.size() == 34);
    assert(commands[0].kind ==
           rloft::EnvironmentDrawKind::RandomBackdropStrip);
    assert(commands[0].resource_id == 16);
    assert(commands[0].x == 32);
    assert(commands[0].y == 4);
    assert(commands[0].width == 8);
    assert(commands[1].resource_id == 22);

    const auto& first = commands[32];
    assert(first.kind ==
           rloft::EnvironmentDrawKind::ProfilePlacement);
    assert(first.resource_id == 32);
    assert(first.x == 128);
    assert(first.y == 10);
    assert(first.width == 64);
    assert(first.shade == 4);

    const auto& second = commands[33];
    assert(second.resource_id == 32);
    assert(second.y == 20);
    assert(second.width == 32);
    assert(second.shade == 8);

    // A centered zero-width marker lands at x=160.
    assert(rloft::EnvironmentProjectionModel::projected_x(
               270, 270, 0) == 160);

    // +36 degrees maps exactly 128 pixels left before width centering.
    assert(rloft::EnvironmentProjectionModel::projected_x(
               306, 270, 0) == 32);

    // Wrapping around 360 selects the visible equivalent.
    assert(rloft::EnvironmentProjectionModel::projected_x(
               0, 350, 0) ==
           rloft::EnvironmentProjectionModel::projected_x(
               360, 350, 0));
}
