#include "rloft/options_menu.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {
namespace {

constexpr std::array<std::string_view, OptionsMenuModel::kControlCount>
kLabels{{
    "Double Click Speed",
    "Floors",
    "Ceiling",
    "Sky",
    "Sprite Dithering",
    "Distancing",
    "Sound",
    "Music",
    "Palette Intensity",
    "Step Movement",
}};

std::uint8_t clamp_u8(
    int value,
    int low,
    int high) noexcept {
    if (value < low) value = low;
    if (value > high) value = high;
    return static_cast<std::uint8_t>(value);
}

std::size_t raw_index(OptionsAction action) noexcept {
    switch (action) {
        case OptionsAction::DoubleClickSpeed: return 0;
        case OptionsAction::Floors: return 1;
        case OptionsAction::Ceiling: return 2;
        case OptionsAction::Sky: return 3;
        case OptionsAction::SpriteDithering: return 4;
        case OptionsAction::Distancing: return 5;
        case OptionsAction::Sound: return 6;
        case OptionsAction::Music: return 7;
        case OptionsAction::PaletteIntensity: return 9;
        case OptionsAction::StepMovement: return 10;
    }
    return 0;
}

void xor_toggle(
    OptionsState& state,
    OptionsAction action) noexcept {
    const auto index = raw_index(action);
    state.raw[index] =
        static_cast<std::uint8_t>(
            state.raw[index] ^ 1U);
}

void apply_step_snap(
    ExplorationSessionState& session) noexcept {
    // CODE.2 20994:
    //   low16(A)=8000h; low16(B)=8000h
    //   q=facing/45; r=(q+1)%8; facing=(r/2)*90
    session.coordinate_a =
        (session.coordinate_a & 0xFFFF0000U) |
        0x00008000U;
    session.coordinate_b =
        (session.coordinate_b & 0xFFFF0000U) |
        0x00008000U;

    const auto q =
        static_cast<std::uint32_t>(
            static_cast<std::int32_t>(
                session.facing) / 45);
    const auto r = (q + 1U) % 8U;
    session.facing = (r / 2U) * 90U;
}

}  // namespace

OptionsState OptionsState::from_bytes(
    const std::vector<std::uint8_t>& bytes) {
    if (bytes.size() != kSerializedSize) {
        throw std::invalid_argument(
            "GAME.OPT must contain exactly 14 bytes");
    }

    OptionsState out{};
    std::copy(
        bytes.begin(),
        bytes.end(),
        out.raw.begin());
    return out;
}

std::string_view OptionsMenuModel::label(
    OptionsAction action) noexcept {
    const auto index =
        static_cast<std::size_t>(action);
    return index < kLabels.size()
        ? kLabels[index]
        : std::string_view{};
}

std::optional<OptionsAction> OptionsMenuModel::action_at(
    int row,
    int column) noexcept {
    if (row < kPanelMinRow) {
        return std::nullopt;
    }

    // x86 IDIV and C++ signed integer division both truncate toward zero.
    const int adjusted =
        column - kFirstColumn;
    const int remainder =
        adjusted % kColumnPitch;
    const int action =
        adjusted / kColumnPitch;

    // The encoded test is only "remainder >= 25". Negative remainders from
    // signed division therefore pass exactly as they do in CODE.2.
    if (remainder >= kActiveColumnWidth ||
        action < 0 ||
        action >= kControlCount) {
        return std::nullopt;
    }

    return static_cast<OptionsAction>(action);
}

std::uint8_t OptionsMenuModel::double_click_value(
    int pointer_row) noexcept {
    const int value =
        (pointer_row - kSliderOriginRow) / 10;
    return clamp_u8(value, 0, 9);
}

std::uint8_t OptionsMenuModel::distancing_value(
    int pointer_row) noexcept {
    return clamp_u8(
        pointer_row - kSliderOriginRow,
        0,
        0x80);
}

std::uint8_t OptionsMenuModel::palette_intensity_value(
    int pointer_row) noexcept {
    const int value =
        (pointer_row - kSliderOriginRow) / 10;
    return clamp_u8(value, 0, 0x0E);
}

std::uint8_t OptionsMenuModel::render_threshold(
    std::uint8_t distancing,
    std::int16_t level_floor) noexcept {
    // CODE.2 06444:
    //   factor = 80h - int16(header_floor)
    //   numer  = factor*C02A + 40h
    //   q      = numer/80h
    //   C02D   = 80h - q
    const auto factor =
        static_cast<std::int32_t>(0x80) -
        static_cast<std::int32_t>(level_floor);
    const auto numerator =
        factor *
            static_cast<std::int32_t>(distancing) +
        0x40;
    const auto quotient =
        numerator / 0x80;
    return static_cast<std::uint8_t>(
        0x80 - quotient);
}

OptionsApplyResult OptionsMenuModel::apply(
    OptionsState& state,
    ExplorationSessionState& session,
    OptionsAction action,
    int pointer_row) noexcept {
    OptionsApplyResult result{};

    switch (action) {
        case OptionsAction::DoubleClickSpeed: {
            const auto value =
                double_click_value(pointer_row);
            result.changed =
                state.raw[0] != value;
            state.raw[0] = value;
            break;
        }

        case OptionsAction::Floors:
        case OptionsAction::Ceiling:
        case OptionsAction::Sky:
        case OptionsAction::SpriteDithering:
            if (pointer_row < kToggleMaxRowExclusive) {
                xor_toggle(state, action);
                result.changed = true;
                result.renderer_changed = true;
            }
            break;

        case OptionsAction::Distancing: {
            const auto value =
                distancing_value(pointer_row);
            const auto threshold =
                render_threshold(
                    value,
                    session.level.render_threshold_floor);
            result.changed =
                state.raw[5] != value ||
                state.raw[8] != threshold;
            state.raw[5] = value;
            state.raw[8] = threshold;
            result.renderer_changed =
                result.changed;
            break;
        }

        case OptionsAction::Sound:
            if (pointer_row < kToggleMaxRowExclusive) {
                xor_toggle(state, action);
                result.changed = true;
                result.sound_enable_changed = true;
            }
            break;

        case OptionsAction::Music:
            if (pointer_row < kToggleMaxRowExclusive) {
                xor_toggle(state, action);
                result.changed = true;
                result.music_enable_changed = true;
                result.start_level_music =
                    state.music();
            }
            break;

        case OptionsAction::PaletteIntensity: {
            const auto value =
                palette_intensity_value(
                    pointer_row);
            result.changed =
                state.raw[9] != value;
            state.raw[9] = value;
            result.palette_changed =
                result.changed;
            break;
        }

        case OptionsAction::StepMovement:
            if (pointer_row < kToggleMaxRowExclusive) {
                xor_toggle(state, action);
                result.changed = true;
                result.step_movement_changed = true;
                if (state.step_movement()) {
                    apply_step_snap(session);
                    result.position_or_facing_snapped = true;
                    result.renderer_changed = true;
                }
            }
            break;
    }

    return result;
}

}  // namespace rloft
