#include "rloft/combat_original_clipped_raster_binding.hpp"
#include <cstdint>
#include <stdexcept>
#include <utility>
namespace rloft {
CombatOriginalSpriteClipServices CombatOriginalClippedRasterBinder::bind_29414(
    VgaPlanarMemory& vga,
    const CombatOriginalSpriteVideoState& video,
    const CombatOriginalDgroupCapture& dgroup,
    CombatOriginalClippedRasterState& machine,
    std::function<std::optional<CombatOriginalSpriteMemory>(std::uint32_t)> capture,
    CombatOriginalSpriteClipServices existing,
    CombatOriginalSpriteMachineState* mutable_code_image) {
    existing.clipped_sprite_2a6b0=[&vga,&video,&dgroup,&machine,
                                  mutable_code_image,
                                  capture=std::move(capture)](
        std::uint32_t frame,std::uint32_t row,std::uint32_t column,
        std::uint32_t scratch,std::uint32_t resource) {
        if(!capture)
            throw std::logic_error("missing original CODE.2 clipped sprite capture");
        const auto sprite=capture(resource);
        if(!sprite || sprite->resource_header_address!=resource)
            throw std::logic_error("uncaptured original 2A6B0 sprite resource");
        if(!CombatOriginalClippedRasterModel::draw_2a6b0(
                vga,*sprite,dgroup,frame,row,column,scratch,video,machine,
                mutable_code_image))
            throw std::logic_error("uncaptured original 2A6B0 VGA/source/clip bytes");
    };
    return existing;
}
} // namespace rloft
