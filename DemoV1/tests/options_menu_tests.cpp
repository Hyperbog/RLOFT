#include "rloft/options_menu.hpp"

#include <array>
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

int main() {
    using rloft::OptionsAction;
    using rloft::OptionsMenuModel;
    using rloft::OptionsState;

    const auto defaults =
        OptionsState::defaults();
    const std::array<std::uint8_t, 14> expected{{
        9, 1, 1, 1, 1, 0x80, 1, 1,
        0, 0, 0, 0, 0, 0,
    }};
    assert(defaults.raw == expected);

    const auto serialized = defaults.bytes();
    const auto decoded =
        OptionsState::from_bytes(serialized);
    assert(decoded.raw == expected);

    bool rejected = false;
    try {
        (void)OptionsState::from_bytes(
            std::vector<std::uint8_t>(13));
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    assert(rejected);

    assert(OptionsMenuModel::label(
               OptionsAction::DoubleClickSpeed) ==
           "Double Click Speed");
    assert(OptionsMenuModel::label(
               OptionsAction::SpriteDithering) ==
           "Sprite Dithering");
    assert(OptionsMenuModel::label(
               OptionsAction::StepMovement) ==
           "Step Movement");

    // 06548 control geometry: x=15 + action*30, width 25, y>=120.
    assert(!OptionsMenuModel::action_at(
                119, 15).has_value());
    assert(OptionsMenuModel::action_at(
               120, 15) ==
           OptionsAction::DoubleClickSpeed);
    assert(OptionsMenuModel::action_at(
               120, 39) ==
           OptionsAction::DoubleClickSpeed);
    assert(!OptionsMenuModel::action_at(
                120, 40).has_value());
    assert(OptionsMenuModel::action_at(
               120, 45) ==
           OptionsAction::Floors);
    assert(OptionsMenuModel::action_at(
               120, 285) ==
           OptionsAction::StepMovement);
    assert(!OptionsMenuModel::action_at(
                120, 310).has_value());

    // Preserve signed IDIV behavior: adjusted=-1 truncates action to zero and
    // remainder=-1, which passes the encoded remainder>=25 rejection test.
    assert(OptionsMenuModel::action_at(
               120, 14) ==
           OptionsAction::DoubleClickSpeed);

    assert(OptionsMenuModel::double_click_value(0) == 0);
    assert(OptionsMenuModel::double_click_value(144) == 0);
    assert(OptionsMenuModel::double_click_value(234) == 9);
    assert(OptionsMenuModel::double_click_value(999) == 9);

    assert(OptionsMenuModel::distancing_value(0) == 0);
    assert(OptionsMenuModel::distancing_value(144) == 0);
    assert(OptionsMenuModel::distancing_value(272) == 128);
    assert(OptionsMenuModel::distancing_value(999) == 128);

    assert(OptionsMenuModel::palette_intensity_value(144) == 0);
    assert(OptionsMenuModel::palette_intensity_value(284) == 14);
    assert(OptionsMenuModel::palette_intensity_value(999) == 14);

    // Level 22's live normalized header floor is 1.
    assert(OptionsMenuModel::render_threshold(0, 1) == 128);
    assert(OptionsMenuModel::render_threshold(128, 1) == 1);

    rloft::ExplorationSessionState session{};
    session.coordinate_a = 0x0004ABCDU;
    session.coordinate_b = 0x00051234U;
    session.facing = 315;
    session.level.render_threshold_floor = 1;

    auto state = OptionsState::defaults();

    // Toggles only accept the upper control band.
    auto result = OptionsMenuModel::apply(
        state, session,
        OptionsAction::Floors,
        0xA7);
    assert(result.changed);
    assert(result.renderer_changed);
    assert(!state.floors());

    result = OptionsMenuModel::apply(
        state, session,
        OptionsAction::Ceiling,
        0xA8);
    assert(!result.changed);
    assert(state.ceiling());

    // Distancing updates both C02A and derived C02D.
    result = OptionsMenuModel::apply(
        state, session,
        OptionsAction::Distancing,
        144);
    assert(result.changed);
    assert(result.renderer_changed);
    assert(state.distancing() == 0);
    assert(state.render_threshold() == 128);

    // Sound/music report their required subsystem side effects.
    result = OptionsMenuModel::apply(
        state, session,
        OptionsAction::Sound,
        120);
    assert(result.sound_enable_changed);
    assert(!state.sound());

    result = OptionsMenuModel::apply(
        state, session,
        OptionsAction::Music,
        120);
    assert(result.music_enable_changed);
    assert(!result.start_level_music);
    assert(!state.music());

    result = OptionsMenuModel::apply(
        state, session,
        OptionsAction::Music,
        120);
    assert(result.music_enable_changed);
    assert(result.start_level_music);
    assert(state.music());

    // 20994 enabling Step Movement centers position and cardinalizes facing.
    assert(!state.step_movement());
    result = OptionsMenuModel::apply(
        state, session,
        OptionsAction::StepMovement,
        120);
    assert(result.step_movement_changed);
    assert(result.position_or_facing_snapped);
    assert(state.step_movement());
    assert(session.coordinate_a == 0x00048000U);
    assert(session.coordinate_b == 0x00058000U);
    assert(session.facing == 0);

    // Disabling does not snap again.
    session.coordinate_a = 0x0004A000U;
    session.coordinate_b = 0x0005B000U;
    session.facing = 123;
    result = OptionsMenuModel::apply(
        state, session,
        OptionsAction::StepMovement,
        120);
    assert(result.step_movement_changed);
    assert(!result.position_or_facing_snapped);
    assert(!state.step_movement());
    assert(session.coordinate_a == 0x0004A000U);
    assert(session.coordinate_b == 0x0005B000U);
    assert(session.facing == 123);
}
