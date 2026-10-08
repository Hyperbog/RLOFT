#include "rloft/combat_original_overlay_backing_clip_binding.hpp"

#include <cstdint>
#include <utility>

namespace rloft {

CombatOriginalOverlayServices CombatOriginalOverlayBackingClipBinder::bind(
    CombatOriginalOverlayClipLimits& bounds,
    CombatOriginalOverlayBackingServices backend,
    CombatOriginalOverlayServices overlays) {
    overlays.capture_292bc=[
        &bounds,backend
    ](std::uint32_t pointer,std::int32_t row,std::int32_t col,
      std::uint32_t row_count,std::uint32_t column_count) {
        // 292BC reads the ORIGINAL signed-bit-pattern coordinates,
        // not a normalized Windows row/column pair.
        const CombatOriginalOverlayBackingRequest req{
            pointer,static_cast<std::uint32_t>(row),
            static_cast<std::uint32_t>(col),row_count,column_count};
        (void)CombatOriginalOverlayClipModel::capture_292bc(
            bounds,req,backend);
    };
    overlays.restore_2932c=[
        &bounds,backend
    ](std::uint32_t pointer,std::int32_t row,std::int32_t col,
      std::uint32_t row_count,std::uint32_t column_count) {
        const CombatOriginalOverlayBackingRequest req{
            pointer,static_cast<std::uint32_t>(row),
            static_cast<std::uint32_t>(col),row_count,column_count};
        (void)CombatOriginalOverlayClipModel::restore_2932c(
            bounds,req,backend);
    };
    return overlays;
}

} // namespace rloft
