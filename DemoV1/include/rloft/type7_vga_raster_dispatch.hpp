#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "rloft/type7_vga_plane_masks.hpp"
#include "rloft/vga_planar_memory.hpp"

namespace rloft {

// One 369C0 generated-rasterizer call after 36B14/369D8 has chosen the
// original sequencer write mask and VGA byte address. The SINTAB entrypoint,
// source pointer and entry/stop descriptors are supplied separately by
// Type7MicrocodeDraw. This call describes registers and memory, not pixels.
struct Type7VgaRasterCall final {
    std::int32_t destination_column{};
    std::uint32_t physical_address{};
    std::uint32_t ecx_lookup{};
    std::uint8_t sequencer_mask{};
    std::uint8_t pass_index{};
};

struct Type7VgaRasterDispatch final {
    std::vector<Type7VgaRasterCall> calls;
    std::uint32_t lookup_after_dispatch{};
};

class Type7VgaRasterDispatchModel final {
public:
    // CODE.2 36B14 distinguishes dgroup C029==0 (one grouped 369D8 call;
    // then swap the low two bytes of global 105E7) and C029!=0 (one
    // 369D8 call per destination column; swap argument bytes on odd X).
    // CODE.2 369D8 computes the four-plane VGA address and up to three
    // sequencer mask passes. The generated-code body is not executed here.
    [[nodiscard]] static Type7VgaRasterDispatch build(
        const Type7VgaPlaneMaskModel& masks,
        std::int32_t destination_column,
        std::uint8_t run_width,
        std::uint8_t video_page,
        std::uint32_t renderer_phase,
        std::uint32_t renderer_row_base,
        std::uint32_t ecx_lookup,
        bool individual_columns_mode);
};

} // namespace rloft
