#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "rloft/type7_microcode_plan.hpp"
#include "rloft/type7_vga_raster_dispatch.hpp"
#include "rloft/world_scene_render_plan.hpp"

namespace rloft {

// One original CODE.2 36B14 invocation: the VGA mask pass sequence applies
// to *every descriptor* in the source column's zero-terminated display list.
// 369D8 loops descriptors internally; 36B14 swaps lookup bytes (mode 0)
// only after that entire descriptor list has been drawn.
struct Type7VgaSceneRun final {
    std::uint16_t source_x{};
    std::size_t source_column_index{};
    std::size_t viewport_x{};
    std::uint16_t width{};
    std::vector<Type7MicrocodeDraw> descriptors;
    Type7VgaRasterDispatch dispatch{};
};

struct Type7VgaSceneTrace final {
    std::vector<Type7VgaSceneRun> runs;
    std::uint32_t final_lookup_word{};
};

class Type7VgaSceneTraceModel final {
public:
    // Trace immutable original CODE.2/SINTAB execution inputs, without
    // calling compiled x86 or changing a framebuffer. video_page and the
    // code2_0xc029 flag are passed explicitly; the original shipped
    // DGROUP initial byte for 0xC029 is one, but the game can toggle it.
    [[nodiscard]] static Type7VgaSceneTrace build(
        const WorldSceneRenderPlan& plan,
        const Type7SceneShape& shape,
        const RendererTables& tables,
        const Type7VgaPlaneMaskModel& masks,
        std::uint8_t video_page,
        bool individual_columns_mode);
};

} // namespace rloft
