#include "rloft/combat_original_sprite_runtime.hpp"

#include <cstdint>
#include <stdexcept>

namespace rloft {
namespace {
void original_refresh_2e758(
    const CombatOriginalSpriteRefreshState& state,
    const CombatOriginalSpriteServices& services) {
    // 2E758 only invokes the deferred original F0C8 callback when
    // the pointer is nonzero; the primitive may change it first.
    if(state.refresh_callback_pointer_f0c8==0U) return;
    if(!services.invoke_live_f0c8)
        throw std::invalid_argument(
            "missing observed original F0C8 sprite refresh callback");
    services.invoke_live_f0c8();
}
} // namespace

void CombatOriginalSpriteRuntimeModel::draw_normal_2939c(
    CombatOriginalSpriteRefreshState& state,
    std::int32_t frame,
    std::uint32_t row,
    std::uint32_t column,
    std::uint32_t resource,
    const CombatOriginalSpriteServices& services) {
    // 2939C uses an unsigned pointer-zero test, branching to
    // the shared early RET at 2939A. No refresh is called.
    if(resource==0U) return;
    if(!services.raster_2a380)
        throw std::invalid_argument("missing CODE.2 2A380 sprite rasterizer");
    services.raster_2a380(frame,row,column,resource);
    // 293BB is a tail JMP to 2E758, not a second drawing pass.
    original_refresh_2e758(state,services);
}

void CombatOriginalSpriteRuntimeModel::draw_shaded_293ec(
    CombatOriginalSpriteRefreshState& state,
    std::int32_t frame,
    std::uint32_t row,
    std::uint32_t column,
    std::uint32_t resource,
    std::uint32_t shade,
    const CombatOriginalSpriteServices& services) {
    // 293EC and 2939C share early-RET and deferred refresh rules.
    if(resource==0U) return;
    if(!services.raster_shaded_2a584)
        throw std::invalid_argument("missing CODE.2 2A584 shaded sprite rasterizer");
    services.raster_shaded_2a584(frame,row,column,resource,shade);
    // 2940F tail JMPs to exactly the same 2E758 helper.
    original_refresh_2e758(state,services);
}

} // namespace rloft
