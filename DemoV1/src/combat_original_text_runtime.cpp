#include "rloft/combat_original_text_runtime.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>

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

    // Bounded original-memory capture, not a DOS string truncation.
    std::size_t terminator=0U;
    while(terminator<captured_c_string.size() &&
          captured_c_string[terminator]!=0U)
        ++terminator;
    if(terminator==captured_c_string.size())
        throw std::invalid_argument("missing NUL in original 303C8 text capture");

    for(std::size_t i=0U;i<terminator;++i) {
        const auto raw=captured_c_string[i];
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
}

} // namespace rloft
