#pragma once

#include <cstddef>
#include <cstdint>

#include "rloft/renderer_tables.hpp"
#include "rloft/surface_prepass.hpp"
#include "rloft/surface_texture.hpp"
#include "rloft/type7_vga_plane_masks.hpp"
#include "rloft/vga_planar_memory.hpp"

namespace rloft {

struct SurfaceSintabPrepassReport final {
    std::size_t vga_calls{};
    std::size_t source_reads{};
    std::size_t indexed_writes{};
};

// CODE.2 32620/3263C FCD0=1: original RES4 texture columns through
// the *original* SINTAB code bytes and planar VGA sequencer masks.
// This is a standalone execution boundary, not yet a claim of full
// original-frame wall rendering or an enabled presenter path.
class SurfaceSintabPrepassExecutor final {
public:
    [[nodiscard]] static SurfaceSintabPrepassReport draw(
        const SurfacePrepassPlan& plan,
        const SurfaceTexture& texture,
        const RendererTables& tables,
        const Type7VgaPlaneMaskModel& masks,
        unsigned video_page,
        VgaPlanarMemory& video);
};

} // namespace rloft
