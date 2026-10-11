#pragma once

// Benchmark-only, deterministic CPU reference for future backend pixel readback.
// Host-defined VGA DAC6->RGBA8 conversion, NOT a DOS gamma/CRT parity oracle.
#include "rloft_native/frame_contract.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft_native {

inline std::uint8_t diagnostic_dac6_to_u8(std::uint8_t value) {
    // Rounded linear host expansion, specified so all GPU spikes use one rule.
    return static_cast<std::uint8_t>((unsigned(value) * 255U + 31U) / 63U);
}

class CpuReferencePresenter final : public IndexedPresenter {
public:
    void present(const IndexedFrame& source) override {
        source.validate();
        rgba_.resize(source.pixels.size() * 4U);
        for (std::size_t i = 0; i < source.pixels.size(); ++i) {
            const auto palette = static_cast<std::size_t>(source.pixels[i]) * 3U;
            rgba_[i * 4U + 0U] = diagnostic_dac6_to_u8(source.dac6[palette]);
            rgba_[i * 4U + 1U] = diagnostic_dac6_to_u8(source.dac6[palette + 1U]);
            rgba_[i * 4U + 2U] = diagnostic_dac6_to_u8(source.dac6[palette + 2U]);
            rgba_[i * 4U + 3U] = 255U;
        }
    }

    const std::vector<std::uint8_t>& rgba() const noexcept { return rgba_; }
    std::uint64_t rgba_fnv1a64() const {
        return fnv1a64(rgba_.data(), rgba_.size());
    }
private:
    std::vector<std::uint8_t> rgba_;
};

} // namespace rloft_native
