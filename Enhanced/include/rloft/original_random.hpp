#pragma once

#include <cstdint>

namespace rloft {

class OriginalRandom final {
public:
    static constexpr std::uint32_t kShippedInitialState = 1U;

    explicit OriginalRandom(
        std::uint32_t state = kShippedInitialState) noexcept
        : state_(state) {}

    [[nodiscard]] std::uint32_t state() const noexcept {
        return state_;
    }

    void seed(std::uint32_t value) noexcept {
        state_ = value;
    }

    // CODE.2 30CA2.
    [[nodiscard]] std::uint16_t next15() noexcept;

    // CODE.2 1C328. Per-die bonus is added after every die; final bonus is
    // added once. The result is clamped to a minimum of one.
    [[nodiscard]] std::uint16_t dice(
        std::uint8_t count,
        std::uint8_t sides,
        std::int16_t per_die_bonus = 0,
        std::int16_t final_bonus = 0) noexcept;

private:
    std::uint32_t state_{};
};

}  // namespace rloft
