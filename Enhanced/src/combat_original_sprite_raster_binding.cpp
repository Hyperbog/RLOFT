#include "rloft/combat_original_sprite_raster_binding.hpp"

#include <cstdint>
#include <stdexcept>
#include <utility>

namespace rloft {

CombatOriginalSpriteServices CombatOriginalSpriteRasterBinder::bind(
    VgaPlanarMemory& vga,
    CombatOriginalSpriteMachineState& machine,
    const CombatOriginalSpriteVideoState& video_registers,
    CombatOriginalSpriteMemoryReaders readers,
    std::function<void()> original_refresh) {
    CombatOriginalSpriteServices out{};
    out.raster_2a380=[
        &vga,&machine,&video_registers,
        get_source=readers.capture_sprite_at_pointer
    ](std::int32_t frame,std::uint32_t row,
      std::uint32_t col,std::uint32_t resource) {
        if(!get_source) throw std::logic_error(
            "missing original DOS sprite resource capture");
        const auto original=get_source(resource);
        if(!original || original->resource_header_address!=resource)
            throw std::logic_error(
                "unresolved original sprite header pointer");
        auto video=video_registers;
        video.row=row;
        video.column=col;
        const auto drawn=CombatOriginalSpriteRasterModel::draw_2a380(
            vga,*original,static_cast<std::uint32_t>(frame),
            video,&machine);
        if(!drawn) throw std::logic_error(
            "unsupported original 2A380 sprite/source/VGA capture");
    };
    out.raster_shaded_2a584=[
        &vga,&machine,&video_registers,
        get_source=std::move(readers.capture_sprite_at_pointer),
        get_shader=std::move(readers.capture_shader_at_pointer)
    ](std::int32_t frame,std::uint32_t row,
      std::uint32_t col,std::uint32_t resource,std::uint32_t shade) {
        // Source 2A590/2A595 stores the lookup pointer BEFORE
        // 2AB38 and any resource reads. The capture callbacks may
        // observe this original code memory, so apply it first.
        // The subsequent bounded raster rechecks the same patched
        // operands but does not change their already stored value.
        if(!CombatOriginalSpriteCodePatchModel::patch_2a584(
                machine,shade))
            throw std::logic_error(
                "uncaptured or unrecognized CODE.2 shade operand sites");
        if(!get_source || !get_shader) throw std::logic_error(
            "missing original DOS sprite or shader table capture");
        const auto original=get_source(resource);
        const auto lookup=get_shader(shade);
        if(!original || original->resource_header_address!=resource ||
           !lookup || lookup->original_lookup_pointer!=shade)
            throw std::logic_error(
                "unresolved DOS sprite or shader memory pointer");
        auto video=video_registers;
        video.row=row;
        video.column=col;
        const auto drawn=CombatOriginalSpriteRasterModel::draw_2a584(
            vga,*original,static_cast<std::uint32_t>(frame),
            video,*lookup,&machine);
        if(!drawn) throw std::logic_error(
            "unsupported original 2A584 sprite/shader/VGA capture");
    };
    out.invoke_live_f0c8=std::move(original_refresh);
    return out;
}

} // namespace rloft
