#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_original_overlay_position.hpp"
#include "rloft/combat_original_overlay_probe.hpp"
#include "rloft/combat_original_overlay_draw.hpp"

#include <cassert>
#include <cstdint>
#include <optional>
#include <unordered_map>
#include <vector>

int main(){
    using namespace rloft;
    CombatOriginalCompositeState composite{};
    composite.enabled_85fc=1U;
    composite.overlay_reference_count_9e37=0;
    composite.active_page_9e39=0U;
    CombatOriginalOverlayGlobals overlay{composite};
    overlay.overlay_mode_a6c0=1U;

    CombatOriginalOverlayPositionState mouse{};
    mouse.clamp_mode_a6be=1U;
    mouse.row_a6b4=1;mouse.column_a6b6=1;mouse.third_word_a6b8=5;
    CombatOriginalOverlayProbeState probe{};
    probe.active_scene_page_c050=0U;
    CombatOriginalOverlayDrawState draw{};

    std::vector<int> trace;
    CombatOriginalOverlayPositionServices move{};
    move.pointer_move_33c90=[&](std::int32_t row,std::int32_t column){
        trace.push_back(1);
        assert(row==0x124&&column==0x7e);
        // First mode stores its clamped WORDs BEFORE 33C90.
        assert(mouse.row_a6b4==row&&mouse.column_a6b6==column);
    };
    std::unordered_map<std::uint32_t,std::uint16_t> words;
    const auto record=0x27148U+0x2260U;
    words[0xF03AU]=1U;
    words[record+8U]=0U;words[record+6U]=2U;
    words[record+2U]=0x7EC8U;
    words[0x14D8EU+2U*0x16U]=0x50U;
    words[record+0xEU]=0x120U;words[record+0x10U]=0x130U;
    words[record+0x12U]=0x7DU;words[record+0x14U]=0x80U;
    CombatOriginalOverlayProbeServices lookup{};
    lookup.original_word=[&](std::uint32_t address)
      ->std::optional<std::uint16_t>{
        const auto at=words.find(address);
        if(at==words.end())return std::nullopt;
        return at->second;
    };
    lookup.predicate_2ad18=[&](std::uint32_t type)
      ->std::optional<bool>{
        trace.push_back(4);assert(type==0x50U);return true;
    };
    lookup.predicate_2ade0=[&](std::uint32_t)
      ->std::optional<bool>{
        assert(false&&"2AD18 true must short-circuit 2ADE0");
        return false;
    };
    CombatOriginalOverlayDrawServices drawsvc{};
    drawsvc.draw_29414=[&](
        std::int32_t mode,std::int32_t row,std::int32_t col,
        std::uint32_t sprite,std::uint32_t scratch) {
        trace.push_back(5);
        assert(mode==1&&row==0x124&&col==0x7e);
        assert(sprite==0x880DU && scratch==0xEF74U);
        assert(draw.nested_record_depth_efd4==1U);
    };
    CombatOriginalOverlayServices base{};
    base.push_page_2e590=[&](std::uint8_t page){
        trace.push_back(0);assert(page==0U);
    };
    base.pop_page_2e5d4=[&](){trace.push_back(6);};
    base.capture_292bc=[&](
        std::uint32_t pointer,std::int32_t row,std::int32_t col,
        std::uint32_t rows,std::uint32_t columns){
        trace.push_back(2);
        assert(pointer==0x9E3AU&&row==0x124&&col==0x7e);
        assert(rows==0x20U&&columns==0x10U);
        assert(composite.overlay_third_word_9e35==5);
    };
    base.restore_2932c=[&](
        std::uint32_t pointer,std::int32_t row,std::int32_t col,
        std::uint32_t rows,std::uint32_t columns){
        trace.push_back(8);
        assert(pointer==0x9E3AU&&row==0x124&&col==0x7e);
        assert(rows==0x20U&&columns==0x10U);
    };
    auto linked=CombatOriginalOverlayPositionModel::bind_1a288(
        mouse,move,base);
    linked=CombatOriginalOverlayProbeModel::bind_1a288(
        probe,lookup,linked);
    linked=CombatOriginalOverlayDrawModel::bind_1a288(
        draw,drawsvc,linked);
    CombatOriginalOverlayModel::suspend_1a288(overlay,linked);
    assert(composite.overlay_reference_count_9e37==1);
    assert(composite.overlay_row_9e31==0x124);
    assert(composite.overlay_column_9e33==0x7e);
    assert(draw.nested_record_depth_efd4==0U);
    assert((trace==std::vector<int>{0,1,2,4,5,6}));
    trace.clear();
    CombatOriginalOverlayModel::restore_1ab20(
        overlay,0x124,0x7e,linked);
    assert(composite.overlay_reference_count_9e37==0);
    assert((trace==std::vector<int>{0,8,6}));
}
