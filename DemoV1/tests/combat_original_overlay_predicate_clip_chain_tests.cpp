#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_original_overlay.hpp"
#include "rloft/combat_original_overlay_probe.hpp"
#include "rloft/combat_original_overlay_predicates.hpp"
#include "rloft/combat_original_overlay_draw.hpp"
#include "rloft/combat_original_sprite_clip_binding.hpp"

#include <cassert>
#include <cstdint>
#include <optional>
#include <unordered_map>
#include <vector>

int main() {
    using namespace rloft;
    CombatOriginalCompositeState screen{};
    screen.enabled_85fc=1U;
    screen.overlay_reference_count_9e37=0;
    screen.active_page_9e39=0U;
    screen.overlay_row_9e31=10;
    screen.overlay_column_9e33=20;
    CombatOriginalOverlayGlobals overlay{screen};
    overlay.overlay_mode_a6c0=1U;

    CombatOriginalOverlayProbeState scene{};
    scene.active_scene_page_c050=0U;
    std::unordered_map<std::uint32_t,std::uint16_t> words;
    const auto record=0x27148U+0x2260U;
    words[0xF03AU]=1U;  // original C050 XOR 1 = page one
    words[record+8U]=0U;
    words[record+6U]=0U;
    words[0x14D8EU]=0x3BU;
    words[record+0xEU]=10U;
    words[record+0x10U]=50U;
    words[record+0x12U]=20U;
    words[record+0x14U]=30U;
    CombatOriginalOverlayProbeServices raw{};
    raw.original_word=[&](std::uint32_t at)
        ->std::optional<std::uint16_t> {
        const auto found=words.find(at);
        if(found==words.end())return std::nullopt;
        return found->second;
    };
    std::vector<char> trace;
    std::unordered_map<std::uint32_t,std::uint32_t> bits{
        {0x22208U,0U},{0x22204U,1U<<21U}};
    CombatOriginalObjectTypeBitServices world{};
    world.original_dgroup_dword=[&](std::uint32_t at)
        ->std::optional<std::uint32_t> {
        if(at==0x22208U)trace.push_back('G');
        if(at==0x22204U)trace.push_back('B');
        const auto found=bits.find(at);
        if(found==bits.end())return std::nullopt;
        return found->second;
    };
    auto predicates=CombatOriginalOverlayPredicatesModel::bind_2ae30(
        world,raw);
    CombatOriginalOverlayDrawState depth{};
    CombatOriginalSpriteClipGlobals clip{};
    clip.viewport_first_row_ef55=0U;
    clip.viewport_last_row_ef59=319U;
    clip.viewport_first_column_ef5d=0U;
    clip.viewport_last_column_ef61=399U;
    CombatOriginalDgroupCapture record_bytes{};
    record_bytes.captured_base_dgroup_offset=0xEF74U;
    record_bytes.bytes.assign(24U,0x00U);
    CombatOriginalSpriteClipServices draw_raster{};
    draw_raster.refresh_2e758=[&](){
        trace.push_back('R');
        assert(depth.nested_record_depth_efd4==1U);
    };
    draw_raster.read_sprite_word=[&](std::uint32_t address)
        ->std::optional<std::uint16_t> {
        assert(address==0x1004U);return 2U;
    };
    draw_raster.read_sprite_byte=[&](std::uint32_t address)
        ->std::optional<std::uint8_t> {
        assert(address==0x1006U);return 1U;
    };
    draw_raster.full_sprite_2a380=[&](
        std::uint32_t mode,std::uint32_t row,
        std::uint32_t column,std::uint32_t resource) {
        trace.push_back('F');
        assert(mode==1U&&row==10U&&column==20U);
        assert(resource==0x1000U);
        assert(depth.nested_record_depth_efd4==1U);
    };
    auto draw_services=CombatOriginalSpriteClipBinder::bind_29804(
        clip,record_bytes,draw_raster,CombatOriginalOverlayDrawServices{});
    CombatOriginalOverlayServices services{};
    services.push_page_2e590=[&](std::uint8_t page){
        trace.push_back('P');assert(page==0U);
    };
    services.position_1a3b8=[&](CombatOriginalOverlayGlobals&){
        trace.push_back('M');
    };
    services.capture_292bc=[&](
        std::uint32_t,std::int32_t,std::int32_t,
        std::uint32_t,std::uint32_t) {
        trace.push_back('C');
    };
    services.pop_page_2e5d4=[&](){trace.push_back('Q');};
    services=CombatOriginalOverlayProbeModel::bind_1a288(
        scene,predicates,services);
    services=CombatOriginalOverlayDrawModel::bind_1a288(
        depth,draw_services,services);

    CombatOriginalOverlayModel::suspend_1a288(overlay,services);
    assert((trace==std::vector<char>{
        'P','M','C','G','B','R','F','Q'
    }));
    assert(depth.nested_record_depth_efd4==0U);
    assert(screen.overlay_reference_count_9e37==1);
    assert(clip.output_row_ef69==10U);
    assert(clip.output_column_ef6b==20U);
    assert(clip.output_rows_ef6d==2U);
    assert(clip.output_columns_ef6f==8U);

    // Turning on original world-state bit 0x48 suppresses
    // the 2AD18 special case, preventing the source class-2
    // sprite from being drawn. The scan still returns 0.
    trace.clear();
    bits[0x22208U]=1U<<8U;
    screen.overlay_reference_count_9e37=0;
    CombatOriginalOverlayModel::suspend_1a288(overlay,services);
    assert((trace==std::vector<char>{'P','M','C','G','Q'}));
    assert(screen.overlay_reference_count_9e37==1);
    assert(depth.nested_record_depth_efd4==0U);
}
