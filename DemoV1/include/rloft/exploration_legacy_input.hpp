#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "rloft/exploration_motion.hpp"

namespace rloft {

class ExplorationLegacyInputController final {
public:
    static constexpr std::size_t kAxisCount = 6;

    // Raw IBM PC set-1 make scan codes consumed by CODE.2 IRQ-9
    // 308F8/307F0. The translation table maps these to 0x147..0x151.
    static constexpr std::uint8_t kHome = 0x47;
    static constexpr std::uint8_t kUp = 0x48;
    static constexpr std::uint8_t kPageUp = 0x49;
    static constexpr std::uint8_t kLeft = 0x4B;
    static constexpr std::uint8_t kCenter = 0x4C;
    static constexpr std::uint8_t kRight = 0x4D;
    static constexpr std::uint8_t kEnd = 0x4F;
    static constexpr std::uint8_t kDown = 0x50;
    static constexpr std::uint8_t kPageDown = 0x51;

    static constexpr std::array<std::int16_t, kAxisCount> kLimits{{
        160, 80, 160, 160, 80, 80,
    }};
    static constexpr std::array<std::int16_t, kAxisCount> kGain{{
        10, 15, 10, 20, 10, 10,
    }};
    static constexpr std::array<std::int16_t, kAxisCount> kDecay{{
        25, 80, 20, 20, 25, 25,
    }};

    // Mirrors 307F0/30748's ordered held-scan queue. Pressing an already-held
    // scan moves it to the newest position, just as the original does.
    void press(std::uint8_t scan_code);
    void release(std::uint8_t scan_code);
    void clear() noexcept;

    [[nodiscard]] bool held(std::uint8_t scan_code) const noexcept;
    [[nodiscard]] const std::array<std::int16_t, kAxisCount>& axes() const noexcept {
        return axes_;
    }
    [[nodiscard]] const std::vector<std::uint8_t>& held_order() const noexcept {
        return held_order_;
    }

    // One keyboard-only iteration of CODE.2 1FC14 from key selection through
    // C6F0/C6EC/C6E8 generation. Mouse-produced C6 values are deliberately a
    // separate future input provider.
    [[nodiscard]] ExplorationMotionInputs tick(bool grid_mode);

private:
    void select(std::int16_t index, std::int16_t direction) noexcept;
    void process_current_scan() noexcept;
    void apply_selected_axis() noexcept;
    void decay_axes() noexcept;
    void clamp_axis(std::size_t index) noexcept;

    [[nodiscard]] static std::int32_t scale_cwde(
        std::int32_t value,
        std::uint32_t factor) noexcept;

    std::array<std::int16_t, kAxisCount> axes_{};
    std::vector<std::uint8_t> held_order_;
    std::int16_t selected_{-1};
    std::int16_t direction_{1};
};

}  // namespace rloft
