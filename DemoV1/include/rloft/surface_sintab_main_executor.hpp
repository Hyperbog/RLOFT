#pragma once

#include <cstddef>

#include "rloft/renderer_tables.hpp"
#include "rloft/surface_shading.hpp"
#include "rloft/surface_texture.hpp"
#include "rloft/type7_vga_plane_masks.hpp"
#include "rloft/vga_planar_memory.hpp"
#include "rloft/view_sweep_runs.hpp"

namespace rloft {

struct SurfaceSintabMainReport final {
    std::size_t vga_calls{};
    std::size_t source_reads{};
    std::size_t indexed_writes{};
};

// Original CODE.2 3263C/FCD0=0, 328EE..3294A:
// regular surface SINTAB entry from the original scale lookup.
// Separate from the FCD0=1 prepass, which patches its RET and starts
// partway into generated code. All writes are transactional.
class SurfaceSintabMainExecutor final {
public:
    [[nodiscard]] static SurfaceSintabMainReport draw(
        const ViewSweepRun& run,
        const SurfaceTexture& texture,
        const RendererTables& tables,
        const SurfaceShadeTable& shades,
        const Type7VgaPlaneMaskModel& masks,
        unsigned video_page,
        VgaPlanarMemory& video);
};

} // namespace rloft
