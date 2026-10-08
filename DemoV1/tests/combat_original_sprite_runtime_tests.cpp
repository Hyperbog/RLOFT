#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_original_sprite_runtime.hpp"
#include "rloft/combat_original_text_runtime.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

int main() {
    using namespace rloft;
    CombatOriginalSpriteRefreshState state{};
    CombatOriginalSpriteServices svc{};
    std::vector<std::string> trace;

    svc.raster_2a380=[&](std::int32_t frame,std::uint32_t row,
                           std::uint32_t column,std::uint32_t resource) {
        trace.emplace_back("raster");
        assert(frame==-23 && row==263U && column==34U && resource==0xBEEFU);
        // 2E758 reads F0C8 only after this raster call returns.
        state.refresh_callback_pointer_f0c8=0xFA12U;
    };
    svc.invoke_live_f0c8=[&]() {
        trace.emplace_back("refresh");
        assert(state.refresh_callback_pointer_f0c8==0xFA12U);
    };
    // An initially disabled F0C8 can become enabled DURING 2A380.
    CombatOriginalSpriteRuntimeModel::draw_normal_2939c(
        state,-23,263U,34U,0xBEEFU,svc);
    assert((trace==std::vector<std::string>{"raster","refresh"}));

    // Zero resource returns BEFORE the rasterizer and 2E758.
    trace.clear();
    CombatOriginalSpriteRuntimeModel::draw_normal_2939c(
        state,-23,263U,34U,0U,svc);
    assert(trace.empty());
    CombatOriginalSpriteRuntimeModel::draw_shaded_293ec(
        state,-23,263U,34U,0U,12U,svc);
    assert(trace.empty());

    svc.raster_shaded_2a584=[&](
        std::int32_t frame,std::uint32_t row,std::uint32_t col,
        std::uint32_t resource,std::uint32_t shade) {
        assert(frame==-23 && row==263U && col==34U);
        assert(resource==0xBEEFU && shade==12U);
        trace.emplace_back("shaded");
        state.refresh_callback_pointer_f0c8=0U;
    };
    // Raster can also REMOVE the deferred refresh before 2E758.
    trace.clear();
    CombatOriginalSpriteRuntimeModel::draw_shaded_293ec(
        state,-23,263U,34U,0xBEEFU,12U,svc);
    assert((trace==std::vector<std::string>{"shaded"}));

    // Original font traversal can forward the signed glyph and
    // row/column to the source-backed 2939C dispatcher unchanged.
    CombatOriginalTextState font{};
    CombatOriginalTextServices draw_text{};
    draw_text.select_font_2f29c=[](std::uint32_t,std::uint32_t) {
        return 0x1000U;
    };
    draw_text.font_resource_pointer=[](std::uint32_t record) {
        assert(record==0x1000U);return 0xBEEFU;
    };
    draw_text.glyph_advance_width=[](
        std::uint32_t,int) {return static_cast<std::uint8_t>(6U);};
    draw_text.draw_normal_2939c=[&](
        std::int32_t f,std::uint32_t r,std::uint32_t c,std::uint32_t ptr) {
        CombatOriginalSpriteRuntimeModel::draw_normal_2939c(
            state,f,r,c,ptr,svc);
    };
    trace.clear();
    font.shader_f56c=0U;
    CombatOriginalTextRuntimeModel::draw_3060c_303c8(
        font,263U,34U,{0xE9U,0U},1U,draw_text);
    assert((trace==std::vector<std::string>{"raster","refresh"}));
    assert(font.column_2b680==40U);

    // Nonzero F0C8 with no callback and actual raster must fail
    // closed. Zero resources do not demand an uncalled service.
    state.refresh_callback_pointer_f0c8=1U;
    svc.invoke_live_f0c8={};
    bool rejected=false;
    try {
        CombatOriginalSpriteRuntimeModel::draw_normal_2939c(
            state,-23,263U,34U,0xBEEFU,svc);
    } catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected);
    svc.raster_2a380={};
    CombatOriginalSpriteRuntimeModel::draw_normal_2939c(
        state,-23,263U,34U,0U,svc);
    rejected=false;
    try {
        CombatOriginalSpriteRuntimeModel::draw_normal_2939c(
            state,-23,263U,34U,0xBEEFU,svc);
    } catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected);
}
