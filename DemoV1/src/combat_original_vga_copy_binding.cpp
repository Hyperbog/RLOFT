#include "rloft/combat_original_vga_copy_binding.hpp"

#include <cstdint>
#include <stdexcept>

namespace rloft {

CombatOriginalCompositeServices
CombatOriginalVgaCopyBinder::bind_direct_copy(
    VgaPlanarMemory& original_vga_planes,
    CombatOriginalVgaCopyState& original_video_state) {
    CombatOriginalCompositeServices services{};
    services.copy_rect_2a078=[
        &original_vga_planes,&original_video_state
    ](std::uint32_t destination_page,
      std::uint32_t source_row,std::uint32_t source_column,
      std::uint32_t destination_row,std::uint32_t destination_column,
      std::uint32_t row_count,std::uint32_t column_count) {
        const CombatOriginalVgaCopyRect rect{
            destination_page,source_row,source_column,
            destination_row,destination_column,row_count,column_count};
        const auto copied=CombatOriginalVgaCopyModel::copy_2a078(
            original_vga_planes,original_video_state,rect);
        if(!copied)
            throw std::logic_error(
                "2A078 original VGA source or destination outside captured aperture");
    };
    // These three are not supplied here: 1A288 and 1AB20
    // require their own DOS overlay behavior, and 30CD4
    // requires the original backing DGROUP byte image.
    return services;
}

CombatOriginalCompositeServices
CombatOriginalVgaCopyBinder::bind_copy_and_backing(
    VgaPlanarMemory& original_vga_planes,
    CombatOriginalVgaCopyState& original_video_state,
    CombatOriginalDgroupCapture& original_dgroup) {
    auto services=bind_direct_copy(
        original_vga_planes,original_video_state);
    services.move_overlay_bytes_30cd4=[
        &original_dgroup
    ](std::uint32_t destination,std::uint32_t source,
      std::uint32_t count) {
        const auto moved=CombatOriginalDgroupMoveModel::move_30cd4(
            original_dgroup,destination,source,count);
        if(!moved)
            throw std::logic_error(
                "30CD4 backing-byte move outside captured original DGROUP");
    };
    return services;
}

} // namespace rloft
