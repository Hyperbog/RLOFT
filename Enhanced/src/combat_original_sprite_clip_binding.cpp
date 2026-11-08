#include "rloft/combat_original_sprite_clip_binding.hpp"

#include <cstdint>
#include <utility>

namespace rloft {

CombatOriginalOverlayDrawServices CombatOriginalSpriteClipBinder::bind_29804(
    CombatOriginalSpriteClipGlobals& globals,
    CombatOriginalDgroupCapture& dgroup,
    CombatOriginalSpriteClipServices raster,
    CombatOriginalOverlayDrawServices draw) {
    draw.draw_29414=[
        &globals,&dgroup,raster=std::move(raster)
    ](std::int32_t mode,std::int32_t row,std::int32_t column,
      std::uint32_t resource,std::uint32_t scratch) {
        (void)CombatOriginalSpriteClipModel::draw_29414(
            globals,dgroup,
            static_cast<std::uint32_t>(mode),
            static_cast<std::uint32_t>(row),
            static_cast<std::uint32_t>(column),
            resource,scratch,raster);
    };
    return draw;
}

} // namespace rloft
