#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "rloft/renderer_tables.hpp"

namespace rloft {

struct MicroRasterizerSegment final {
    std::size_t index{};
    std::uint16_t patch_offset{};
    std::uint16_t next_patch_offset{};
    std::uint16_t scale_step{};
    std::size_t code_begin{};
    std::size_t code_end{};

    [[nodiscard]] bool empty() const noexcept {
        return code_begin == code_end;
    }
};

class MicroRasterizerLayout final {
public:
    static constexpr std::size_t kSegmentCount = 128;

    [[nodiscard]] static std::vector<MicroRasterizerSegment> segments(
        const RendererScaleBlock& block);

    [[nodiscard]] static MicroRasterizerSegment segment(
        const RendererScaleBlock& block,
        std::size_t index);
};

}  // namespace rloft
