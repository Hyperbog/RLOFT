#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_original_text_runtime.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

int main() {
    using namespace rloft;
    CombatOriginalTextState state{};
    state.shader_f56c=0U;
    std::vector<std::string> calls{};
    std::vector<std::uint8_t> text{'A','B',0U};
    CombatOriginalTextServices svc{};

    svc.select_font_2f29c=[&](std::uint32_t style,std::uint32_t variant) {
        calls.emplace_back("font");
        assert(style==1U && variant==0U);
        // 3060C writes the latest caller-supplied row/column before this call.
        return 0x1000U;
    };
    svc.font_resource_pointer=[&](std::uint32_t record) {
        calls.emplace_back("resource");
        assert(record==0x1000U || record==0x2000U);
        return record+4U;
    };
    svc.glyph_advance_width=[&](std::uint32_t record,std::int32_t glyph) {
        calls.emplace_back("width");
        assert(record==0x1000U || record==0x2000U);
        assert(glyph=='A' || glyph=='B' || glyph==-23);
        return static_cast<std::uint8_t>(glyph=='A'?5U:7U);
    };
    svc.draw_normal_2939c=[&](std::int32_t glyph,
                                std::uint32_t row,std::uint32_t column,
                                std::uint32_t resource) {
        calls.emplace_back("normal");
        assert(row==263U);
        if(glyph=='A') {
            assert(column==34U && resource==0x1004U);
            // Original font-record pointer and sprite shading mode
            // may change within the 2939C callback.
            state.font_record_2b67c=0x2000U;
            state.shader_f56c=9U;
        } else assert(false && "expected 293EC for second glyph");
    };
    svc.draw_shaded_293ec=[&](std::int32_t glyph,
                                std::uint32_t row,std::uint32_t column,
                                std::uint32_t resource,std::uint32_t shade) {
        calls.emplace_back("shade");
        assert(glyph=='B' && row==263U && column==39U);
        assert(resource==0x2004U && shade==9U);
    };

    CombatOriginalTextRuntimeModel::draw_3060c_303c8(
        state,263U,34U,text,1U,svc);
    assert((calls==std::vector<std::string>{
        "font","resource","normal","width",
        "resource","shade","width"
    }));
    assert(state.font_record_2b67c==0x2000U);
    assert(state.column_2b680==46U && state.row_2b684==263U);

    // A zero first byte still invokes 2F29C once but never requests
    // a font sprite, a width read, or a renderer service.
    calls.clear();
    text={0U};
    CombatOriginalTextRuntimeModel::draw_3060c_303c8(
        state,100U,10U,text,1U,svc);
    assert((calls==std::vector<std::string>{"font"}));
    assert(state.column_2b680==10U);
    assert(state.font_record_2b67c==0x1000U);

    // Original uses SIGNED MOVSX BYTE: DOS 0xE9 => glyph -23.
    text={0xE9U,0U};
    state.shader_f56c=0U;
    svc.draw_normal_2939c=[&](std::int32_t glyph,
                                std::uint32_t row,std::uint32_t col,
                                std::uint32_t resource) {
        assert(glyph==-23 && row==263U && col==0xFFFFFFFEU);
        assert(resource==0x1004U);
    };
    calls.clear();
    CombatOriginalTextRuntimeModel::draw_3060c_303c8(
        state,263U,0xFFFFFFFEU,text,1U,svc);
    assert((calls==std::vector<std::string>{
        "font","resource","width"
    }));
    assert(state.column_2b680==5U); // 0xFFFFFFFE + 7 wraps

    // Original observes text memory again AFTER a callback:
    // an intervening glyph draw can shorten the C string.
    text={'A','B','C',0U};
    state.shader_f56c=0U;
    svc.draw_normal_2939c=[&](std::int32_t glyph,
                                std::uint32_t,std::uint32_t,
                                std::uint32_t) {
        assert(glyph=='A');
        text[1]=0U;
    };
    calls.clear();
    CombatOriginalTextRuntimeModel::draw_3060c_303c8(
        state,263U,34U,text,1U,svc);
    assert((calls==std::vector<std::string>{
        "font","resource","width"
    }));
    assert(state.column_2b680==39U);

    // Incomplete capture is unsupported; do not produce a fictional
    // default NUL or silently stop at end-of-buffer.
    text={'A'};
    svc.draw_normal_2939c=[&](std::int32_t,
                                std::uint32_t,std::uint32_t,
                                std::uint32_t) {};
    bool rejected=false;
    try {
        CombatOriginalTextRuntimeModel::draw_3060c_303c8(
            state,0U,0U,text,1U,svc);
    } catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected);
    text={0U};
    CombatOriginalTextServices no_font{};
    rejected=false;
    try {
        CombatOriginalTextRuntimeModel::draw_3060c_303c8(
            state,0U,0U,text,1U,no_font);
    } catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected);
}
