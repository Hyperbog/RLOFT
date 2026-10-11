#pragma once

// Renderer-neutral diagnostics contract, NOT original DOS rendering or a GPU backend.
// No backend headers or host graphics types may cross this boundary.
#include <array>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace rloft_native {

struct IndexedFrame final {
    static constexpr unsigned width = 320;
    static constexpr unsigned height = 400;
    std::vector<std::uint8_t> pixels;
    std::array<std::uint8_t, 768> dac6{};

    void validate() const {
        if (pixels.size() != width * height)
            throw std::invalid_argument("original indexed frame must have 128000 pixels");
        for (const auto channel : dac6)
            if (channel > 63)
                throw std::invalid_argument("VGA DAC channel is not 6-bit");
    }
};

struct FrameIdentity final {
    std::uint64_t pixels_fnv1a64{};
    std::uint64_t palette_fnv1a64{};
};

inline std::uint64_t fnv1a64(const std::uint8_t* bytes, std::size_t count) {
    std::uint64_t hash = UINT64_C(14695981039346656037);
    for (std::size_t i = 0; i < count; ++i) {
        hash ^= bytes[i];
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

inline FrameIdentity frame_identity(const IndexedFrame& frame) {
    frame.validate();
    return {fnv1a64(frame.pixels.data(), frame.pixels.size()),
            fnv1a64(frame.dac6.data(), frame.dac6.size())};
}

// Contract only: a future SDL_GPU/bgfx/Godot backend receives the SAME
// immutable indexed pixels and palette; GPU display hashes require readback.
class IndexedPresenter {
public:
    virtual ~IndexedPresenter() = default;
    virtual void present(const IndexedFrame& source) = 0;
};

struct OutputRect final { unsigned x{}, y{}, width{}, height{}; };

// Original VGA 320x400 is displayed in a 4:3 viewport. This is display
// geometry only; the source indexed framebuffer remains 320x400.
inline OutputRect fit_original_aspect(unsigned window_width, unsigned window_height) {
    if (window_width == 0 || window_height == 0)
        throw std::invalid_argument("nonzero window size required");
    const std::uint64_t w = window_width, h = window_height;
    const unsigned view_width = (w * 3 <= h * 4)
        ? window_width : static_cast<unsigned>(h * 4 / 3);
    const unsigned view_height = (w * 3 <= h * 4)
        ? static_cast<unsigned>(w * 3 / 4) : window_height;
    return {(window_width - view_width) / 2, (window_height - view_height) / 2,
            view_width, view_height};
}

} // namespace rloft_native
