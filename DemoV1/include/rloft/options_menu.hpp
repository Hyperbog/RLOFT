#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>

#include "rloft/exploration_session.hpp"

namespace rloft {

enum class OptionsAction : std::uint8_t {
    DoubleClickSpeed = 0,
    Floors = 1,
    Ceiling = 2,
    Sky = 3,
    SpriteDithering = 4,
    Distancing = 5,
    Sound = 6,
    Music = 7,
    PaletteIntensity = 8,
    StepMovement = 9,
};

struct OptionsState final {
    static constexpr std::size_t kSerializedSize = 14;

    // Exact CODE.2 C025..C032 defaults. 05DD0/05E38 persist exactly these
    // fourteen bytes as GAME.OPT.
    std::array<std::uint8_t, kSerializedSize> raw{{
        9, 1, 1, 1, 1, 0x80, 1, 1, 0, 0, 0, 0, 0, 0,
    }};

    [[nodiscard]] static OptionsState defaults() noexcept {
        return OptionsState{};
    }

    [[nodiscard]] static OptionsState from_bytes(
        const std::vector<std::uint8_t>& bytes);

    [[nodiscard]] std::vector<std::uint8_t> bytes() const {
        return std::vector<std::uint8_t>(
            raw.begin(), raw.end());
    }

    [[nodiscard]] std::uint8_t double_click_speed() const noexcept {
        return raw[0];
    }
    [[nodiscard]] bool floors() const noexcept {
        return raw[1] != 0;
    }
    [[nodiscard]] bool ceiling() const noexcept {
        return raw[2] != 0;
    }
    [[nodiscard]] bool sky() const noexcept {
        return raw[3] != 0;
    }
    [[nodiscard]] bool sprite_dithering() const noexcept {
        return raw[4] != 0;
    }
    [[nodiscard]] std::uint8_t distancing() const noexcept {
        return raw[5];
    }
    [[nodiscard]] bool sound() const noexcept {
        return raw[6] != 0;
    }
    [[nodiscard]] bool music() const noexcept {
        return raw[7] != 0;
    }
    [[nodiscard]] std::uint8_t render_threshold() const noexcept {
        return raw[8];
    }
    [[nodiscard]] std::uint8_t palette_intensity() const noexcept {
        return raw[9];
    }
    [[nodiscard]] bool step_movement() const noexcept {
        return raw[10] != 0;
    }
};

struct OptionsApplyResult final {
    bool changed{};
    bool renderer_changed{};
    bool palette_changed{};
    bool sound_enable_changed{};
    bool music_enable_changed{};
    bool start_level_music{};
    bool step_movement_changed{};
    bool position_or_facing_snapped{};
};

class OptionsMenuModel final {
public:
    static constexpr int kControlCount = 10;
    static constexpr int kPanelMinRow = 0x78;
    static constexpr int kToggleMaxRowExclusive = 0xA8;
    static constexpr int kFirstColumn = 0x0F;
    static constexpr int kColumnPitch = 0x1E;
    static constexpr int kActiveColumnWidth = 0x19;
    static constexpr int kSliderOriginRow = 0x90;

    [[nodiscard]] static std::string_view label(
        OptionsAction action) noexcept;

    // CODE.2 06548 signed IDIV/remainder hit test. row is the vertical mouse
    // coordinate (9E33), column is horizontal (9E31).
    [[nodiscard]] static std::optional<OptionsAction> action_at(
        int row,
        int column) noexcept;

    // Exact slider transfer functions used by 062AC / 06444 / 06354.
    [[nodiscard]] static std::uint8_t double_click_value(
        int pointer_row) noexcept;
    [[nodiscard]] static std::uint8_t distancing_value(
        int pointer_row) noexcept;
    [[nodiscard]] static std::uint8_t palette_intensity_value(
        int pointer_row) noexcept;

    // 06444 / 181B4 live threshold calculation. The level loader has already
    // normalized serialized zero header floors to one.
    [[nodiscard]] static std::uint8_t render_threshold(
        std::uint8_t distancing,
        std::int16_t level_floor) noexcept;

    // Apply one current pointer sample for the already-selected action.
    // Slider actions are intended to be called repeatedly while the primary
    // button is held, matching 062AC/06444/06354.
    static OptionsApplyResult apply(
        OptionsState& state,
        ExplorationSessionState& session,
        OptionsAction action,
        int pointer_row) noexcept;
};

}  // namespace rloft
