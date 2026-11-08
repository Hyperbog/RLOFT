#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "rloft/sintab_pixel_microcode.hpp"
#include "rloft/type7_vga_scene_trace.hpp"
#include "rloft/type7_rasterizer_patch.hpp"

namespace rloft {

struct Type7PixelDrawReport final {
    std::size_t descriptor_invocations{};
    std::size_t vga_passes{};
    std::size_t source_reads{};
    std::size_t indexed_writes{};
};

// Executes the original CODE.2 369D8 / 369C0 descriptor loop on a private
// generated-code copy, applying transient stop-RET and mode patches and
// writing the original AL/AH color bytes into the planar VGA framebuffer.
// A complete frame needs one call per ordered scene shape.
class Type7ScenePixelExecutor final {
public:
    [[nodiscard]] static Type7PixelDrawReport draw(
        const WorldSceneRenderPlan& plan,
        const Type7VgaSceneTrace& trace,
        const std::vector<std::uint8_t>& decoded_res3_body,
        const RendererTables& tables,
        VgaPlanarMemory& video);
};

} // namespace rloft
