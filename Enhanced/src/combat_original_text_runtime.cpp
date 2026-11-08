#include "rloft/combat_original_text_runtime.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>

namespace rloft {

void CombatOriginalTextRuntimeModel::draw_3060c_303c8(
    CombatOriginalTextState& state,
    std::uint32_t original_row,
    std::uint32_t original_column,
    const std::vector<std::uint8_t>& captured_c_string,
    std::uint32_t font_style,
    const CombatOriginalTextServices& services) {
    if(!services.select_font_2f29c)
        throw std::invalid_argument("missing original 2F29C font selector");

    // 3060C:30614/30621 writes row/column globals before
    // entering 303C8. 303C8 calls 2F29C even for empty text.
    state.row_2b684=original_row;
    state.column_2b680=original_column;
    state.font_record_2b67c=services.select_font_2f29c(font_style,0U);

    // Test each byte only when 303C8 would reach it. Earlier
    // renderer callbacks can mutate future source-memory bytes;
    // an eager whole-string copy or NUL scan would be observable.
    for(std::size_t i=0U;i<captured_c_string.size();++i) {
        const auto raw=captured_c_string[i];
        if(raw==0U) return;
        // 30413/30434 and 30446 use MOVSX BYTE; unsigned char
        // normalization changes the glyph selected for CP437 >=80h.
        const auto glyph=raw<0x80U
            ? static_cast<std::int32_t>(raw)
            : static_cast<std::int32_t>(raw)-0x100;

        if(!services.font_resource_pointer ||
           !services.glyph_advance_width)
            throw std::invalid_argument(
                "missing original font resource/width memory reader");

        // Original 303F1 tests the mutable F56C DWORD *per glyph*.
        // Only the actually selected sprite routine is required.
        if(state.shader_f56c!=0U) {
            if(!services.draw_shaded_293ec)
                throw std::invalid_argument("missing 293EC shaded glyph renderer");
            const auto tint=state.shader_f56c;
            const auto resource=services.font_resource_pointer(
                state.font_record_2b67c);
            services.draw_shaded_293ec(
                glyph,state.row_2b684,state.column_2b680,
                resource,tint);
        } else {
            if(!services.draw_normal_2939c)
                throw std::invalid_argument("missing 2939C glyph renderer");
            const auto resource=services.font_resource_pointer(
                state.font_record_2b67c);
            services.draw_normal_2939c(
                glyph,state.row_2b684,state.column_2b680,resource);
        }

        // 30440 re-reads font_record_2B67C AFTER the glyph
        // render service returns; width is an unsigned byte
        // at font_record + (signed glyph) + 4.
        const auto advance=services.glyph_advance_width(
            state.font_record_2b67c,glyph);
        // x86 3044F ADD DWORD does not saturate at the screen edge.
        state.column_2b680+=advance;
    }
    // The host snapshot ended before the first live NUL; the DOS
    // executable would keep reading outside the captured memory.
    throw std::invalid_argument(
        "missing NUL in original 303C8 text capture");
}

std::function<void(
    std::uint32_t, std::uint32_t,
    const std::vector<std::uint8_t>&, std::uint32_t)>
CombatOriginalTextRuntimeModel::bind_display_3060c(
    CombatOriginalTextState& original_font_state,
    CombatOriginalTextServices original_font_services) {
    return [&original_font_state,
            font_services=std::move(original_font_services)](
                std::uint32_t row, std::uint32_t column,
                const std::vector<std::uint8_t>& text,
                std::uint32_t font_style) {
        CombatOriginalTextRuntimeModel::draw_3060c_303c8(
            original_font_state,row,column,text,font_style,font_services);
    };
}

} // namespace rloft
