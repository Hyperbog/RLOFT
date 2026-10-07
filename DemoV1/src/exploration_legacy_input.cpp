#include "rloft/exploration_legacy_input.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>

namespace rloft {

void ExplorationLegacyInputController::select(
    std::int16_t index,
    std::int16_t direction) noexcept {
    selected_ = index;
    direction_ = direction;
}

bool ExplorationLegacyInputController::held(
    std::uint8_t scan_code) const noexcept {
    return std::find(
               held_order_.begin(),
               held_order_.end(),
               scan_code) != held_order_.end();
}

void ExplorationLegacyInputController::press(
    std::uint8_t scan_code) {
    if (scan_code < kHome ||
        scan_code > kPageDown) {
        return;
    }

    // CODE.2 307F0 removes an existing scan first, then appends it.
    release(scan_code);
    held_order_.push_back(scan_code);

    // IRQ-side selection performed immediately by 307F0. 1FC14 normally
    // selects again from the translated key; preserving this here matters for
    // scans whose switch entry is intentionally absent.
    switch (scan_code) {
        case kHome: select(1, -1); break;
        case kUp: select(3, -1); break;
        case kPageUp: select(1, 1); break;
        case kLeft: select(2, -1); break;
        case kRight: select(2, 1); break;
        case kDown: select(0, 1); break;
        default: break;
    }
}

void ExplorationLegacyInputController::release(
    std::uint8_t scan_code) {
    const auto it = std::find(
        held_order_.begin(),
        held_order_.end(),
        scan_code);
    if (it != held_order_.end()) {
        held_order_.erase(it);
    }
}

void ExplorationLegacyInputController::clear() noexcept {
    axes_.fill(0);
    held_order_.clear();
    selected_ = -1;
    direction_ = 1;
}

void ExplorationLegacyInputController::process_current_scan() noexcept {
    if (held_order_.empty()) {
        return;
    }

    const auto scan = held_order_.back();
    switch (scan) {
        case kHome:
            if (held(kUp)) select(4, -1);
            else if (held(kDown)) select(5, -1);
            else select(1, -1);
            break;

        case kUp:
            if (held(kHome)) select(4, -1);
            else if (held(kPageUp)) select(4, 1);
            else select(0, -1);
            break;

        case kPageUp:
            if (held(kUp)) select(4, 1);
            else if (held(kDown)) select(5, 1);
            else select(1, 1);
            break;

        case kLeft:
            select(2, -1);
            break;

        case kCenter:
            axes_.fill(0);
            selected_ = -1;
            break;

        case kRight:
            select(2, 1);
            break;

        case kEnd:
            select(2, -1);
            break;

        case kDown:
            if (held(kHome)) select(5, -1);
            else if (held(kPageUp)) select(5, 1);
            else select(0, 1);
            break;

        case kPageDown:
            select(2, 1);
            break;

        default:
            break;
    }
}

void ExplorationLegacyInputController::clamp_axis(
    std::size_t index) noexcept {
    const auto limit = kLimits[index];
    if (axes_[index] > limit) {
        axes_[index] = limit;
    } else if (axes_[index] < -limit) {
        axes_[index] =
            static_cast<std::int16_t>(-limit);
    }
}

void ExplorationLegacyInputController::apply_selected_axis() noexcept {
    if (selected_ < 0 ||
        selected_ >= static_cast<std::int16_t>(kAxisCount)) {
        return;
    }

    const auto index =
        static_cast<std::size_t>(selected_);

    // The two diagonal controller axes are coupled into C000/C002 after their
    // own update. The odd C008 initialization is preserved from 1FC14's
    // partial-register arithmetic; with the shipped +/-1 direction it reduces
    // to the low 16 bits of abs(C000) * that signed-low-word pattern.
    if (selected_ == 4 && axes_[4] == 0) {
        const auto source = axes_[0];
        const std::uint32_t high =
            source < 0 ? 0xFFFF0000U : 0U;
        const auto edx =
            high |
            static_cast<std::uint16_t>(direction_);
        const auto magnitude =
            static_cast<std::uint32_t>(
                source < 0 ? -source : source);
        axes_[4] =
            static_cast<std::int16_t>(
                static_cast<std::uint16_t>(
                    magnitude * edx));
    } else if (selected_ == 5 && axes_[5] == 0) {
        const auto source = axes_[5];
        const std::uint32_t high =
            source < 0 ? 0xFFFF0000U : 0U;
        const auto edx =
            high |
            static_cast<std::uint16_t>(direction_);
        const auto magnitude =
            static_cast<std::uint32_t>(
                source < 0 ? -source : source);
        axes_[5] =
            static_cast<std::int16_t>(
                static_cast<std::uint16_t>(
                    magnitude * edx));
    }

    const auto delta =
        static_cast<std::int32_t>(direction_) *
        static_cast<std::int32_t>(kGain[index]);
    axes_[index] =
        static_cast<std::int16_t>(
            static_cast<std::uint16_t>(
                static_cast<std::uint16_t>(axes_[index]) +
                static_cast<std::uint16_t>(delta)));
    clamp_axis(index);

    if (selected_ == 4) {
        const auto raw = axes_[4];
        const auto magnitude =
            static_cast<std::int16_t>(
                raw < 0 ? -raw : raw);
        axes_[0] =
            static_cast<std::int16_t>(-magnitude);
        axes_[1] = raw;
        clamp_axis(0);
        clamp_axis(1);
    } else if (selected_ == 5) {
        const auto raw = axes_[5];
        const auto magnitude =
            static_cast<std::int16_t>(
                raw < 0 ? -raw : raw);
        axes_[0] = magnitude;
        axes_[1] = raw;
        clamp_axis(0);
        clamp_axis(1);
    }
}

void ExplorationLegacyInputController::decay_axes() noexcept {
    const std::size_t first =
        (selected_ == 4 || selected_ == 5)
            ? 2U
            : 0U;

    // Encoded loop is i < 5: C00A (axis 5) is not in this decay pass.
    for (std::size_t i = first; i < 5U; ++i) {
        const auto value = axes_[i];

        if (static_cast<std::int16_t>(i) == selected_ &&
            static_cast<std::int32_t>(direction_) *
                    static_cast<std::int32_t>(value) >=
                0) {
            continue;
        }

        if (value > 0) {
            const auto next =
                static_cast<std::int16_t>(
                    value - kDecay[i]);
            axes_[i] = next < 0 ? 0 : next;
        } else if (value < 0) {
            const auto next =
                static_cast<std::int16_t>(
                    value + kDecay[i]);
            axes_[i] = next > 0 ? 0 : next;
        }
    }
}

std::int32_t ExplorationLegacyInputController::scale_cwde(
    std::int32_t value,
    std::uint32_t factor) noexcept {
    const auto raw =
        static_cast<std::uint32_t>(value);
    const auto shifted =
        (raw * factor) >> 3U;
    return static_cast<std::int16_t>(
        static_cast<std::uint16_t>(shifted));
}

ExplorationMotionInputs ExplorationLegacyInputController::tick(
    bool grid_mode) {
    // 1FC14 clears only C6E8/C6EC/C6F0. The six controller axes persist.
    selected_ = -1;
    process_current_scan();
    apply_selected_axis();
    decay_axes();

    ExplorationMotionInputs out{};
    out.orthogonal_input = axes_[2];
    out.angular_input =
        scale_cwde(axes_[1], 0x14U);
    out.velocity =
        scale_cwde(
            static_cast<std::int32_t>(axes_[3]) +
                static_cast<std::int32_t>(axes_[0]),
            0x14U);
    out.grid_mode = grid_mode;

    // 1FC14 resets the transient selected index after producing C6 inputs.
    selected_ = -1;
    return out;
}

}  // namespace rloft
