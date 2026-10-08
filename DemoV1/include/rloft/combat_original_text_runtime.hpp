#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

namespace rloft {

// Original CODE.2 3060C -> 303C8 uses a fixed ROW at DGROUP:2B684
// and advances a COLUMN at DGROUP:2B680 after EACH glyph.
// Global 2B67C is the current font record pointer; F56C selects
// normal (2939C) or shaded (293EC) glyph draw per character.
struct CombatOriginalTextState final {
    std::uint32_t row_2b684{};
    std::uint32_t column_2b680{};
    std::uint32_t font_record_2b67c{};
    std::uint32_t shader_f56c{};
};

struct CombatOriginalTextServices final {
    // 303CD pushes (font_style, 0) to 2F29C, even for empty text.
    std::function<std::uint32_t(std::uint32_t, std::uint32_t)>
        select_font_2f29c;
    // Re-read original font record DWORD+0 at each glyph call.
    std::function<std::uint32_t(std::uint32_t)> font_resource_pointer;
    // 30440 MOVZX BYTE [font_record + signed_char + 4] after draw.
    // Host caller must provide observed backing bytes, including
    // for negative signed CP437 glyph indexes when applicable.
    std::function<std::uint8_t(std::uint32_t, std::int32_t)>
        glyph_advance_width;
    std::function<void(std::int32_t, std::uint32_t, std::uint32_t,
                       std::uint32_t)> draw_normal_2939c;
    std::function<void(std::int32_t, std::uint32_t, std::uint32_t,
                       std::uint32_t, std::uint32_t)> draw_shaded_293ec;
};

// Executes the original glyph-service sequence and updates the
// captured original globals; does NOT produce pixels by itself.
// A missing NUL is an incomplete host memory snapshot: original
// 303C8 would continue reading potentially unrelated DOS memory.
class CombatOriginalTextRuntimeModel final {
public:
    static void draw_3060c_303c8(
        CombatOriginalTextState& state,
        std::uint32_t original_row,
        std::uint32_t original_column,
        const std::vector<std::uint8_t>& captured_c_string,
        std::uint32_t font_style,
        const CombatOriginalTextServices& services);

    // Connects 1C258's injected 3060C service to the exact 303C8
    // glyph walk. The state reference must outlive the callback;
    // the original font-service callbacks are retained by value.
    [[nodiscard]] static std::function<void(
        std::uint32_t, std::uint32_t,
        const std::vector<std::uint8_t>&, std::uint32_t)>
    bind_display_3060c(
        CombatOriginalTextState& original_font_state,
        CombatOriginalTextServices original_font_services);
};

} // namespace rloft
