#include "rloft_native/native_code2_wall_emission.hpp"

#include <cstdint>
#include <stdexcept>

namespace rloft_native {
namespace {
void require(bool ok,const char* message) {
    if(!ok)throw std::runtime_error(message);
}
}

NativeCode2WallRecord emit_native_code2_wall_record(
    NativeWorldRuntime& world,
    const NativeWorldMaterialCatalogue& materials,
    unsigned level,unsigned column,unsigned row,
    NativeCode2WallPath path,
    std::uint32_t phase,
    bool sentinel,
    std::uint32_t depth,
    std::uint32_t screen_position,
    std::optional<NativeCardinal> neighbor) {
    require(level<32U && materials.levels.size()==32U &&
            materials.wide_texture_count>0,
            "original CODE.2 wall emission material catalogue invalid");
    const auto current=world.cell(level,column,row);
    const auto source=native_scene_resource_candidates(
        world,level,column,row,neighbor);
    unsigned first_bank=0,second_bank=0;
    bool reflect=false;
    bool increment=false;
    switch(path) {
        case NativeCode2WallPath::Banks3And2_32F8C:
            first_bank=3;
            second_bank=2;
            reflect=sentinel;
            increment=sentinel;
            break;
        case NativeCode2WallPath::Banks1And0_33124:
            first_bank=1;
            second_bank=0;
            reflect=!sentinel;
            increment=sentinel;
            break;
        default:
            throw std::runtime_error("original CODE.2 wall emission path invalid");
    }
    const auto source_code=unsigned(source.effective_code);
    require(source_code<64U &&
            source.raw_lookup[first_bank]==
                materials.levels[level].source_lookups[source_code][first_bank] &&
            source.raw_lookup[second_bank]==
                materials.levels[level].source_lookups[source_code][second_bank],
            "CODE.2 wall original source bank mismatch");
    const unsigned first=source.candidate_resource_ids[first_bank];
    const unsigned second=source.candidate_resource_ids[second_bank];
    require(first<materials.wide_texture_count &&
            second<materials.wide_texture_count,
            "CODE.2 wall source ID exceeds RES4 wide source bank");
    const auto original_selector=static_cast<std::uint16_t>(
        (phase>>4U)&0x0FC0U);
    const auto effective_selector=static_cast<std::uint16_t>(
        reflect?0x0FC0U-original_selector:original_selector);
    require(effective_selector<=0x0FC0U &&
            (effective_selector&0x3FU)==0U,
            "CODE.2 source 64-column selector arithmetic invalid");
    NativeCode2WallRecord out;
    out.level=level;
    out.column=column;
    out.row=row;
    out.path=path;
    out.original_cell_code=current.code;
    out.effective_cell_code=source.effective_code;
    out.original_shade_class=
        static_cast<std::uint8_t>((current.word&0xC000U)>>14U);
    out.original_state_variant_offset=source.selected_variant_offset;
    out.special_neighbor_substitution=source.direction_neighbor_substituted;
    out.reflected_source_column=reflect;
    out.sentinel_counter_increment=increment;
    out.unreflected_column_selector=original_selector;
    out.selected_column_selector=effective_selector;
    out.source_texture_column=effective_selector/64U;
    out.first_source_bank=first_bank;
    out.second_source_bank=second_bank;
    out.first_original_res4_id=first;
    out.second_original_res4_id=second;
    out.caller_depth=depth;
    out.caller_screen_position=screen_position;
    // Both original 32FDF/3304F and 33173/331DA paths OR 0x0440.
    // Only mark after every material/selector check succeeds.
    world.mark_reached_by_view(level,column,row);
    return out;
}

NativeWorldSurfaceDraw draw_native_code2_wall_record_column(
    const NativeWorldRuntime& world,
    const NativeWorldMaterialCatalogue& materials,
    const NativeResArchive& res4,
    const NativeCode2WallRecord& r,
    unsigned slot,
    unsigned viewport_x,unsigned height,
    std::uint8_t shade,unsigned page,unsigned working_top,
    NativeIndexedVgaPlanes& pixels,
    std::optional<NativeCardinal> neighbor) {
    require(slot<2U && r.level<32U &&
            r.source_texture_column<64U &&
            !r.original_complete_frame_verified,
            "CODE.2 wall selected raster word/column invalid");
    unsigned bank=0;
    unsigned chosen=0;
    if(slot==0U) {
        bank=r.first_source_bank;
        chosen=r.first_original_res4_id;
    } else {
        bank=r.second_source_bank;
        chosen=r.second_original_res4_id;
    }
    require(bank<4U && chosen<materials.wide_texture_count,
            "CODE.2 wall selected source ID outside wide archive");
    const auto current=native_scene_resource_candidates(
        world,r.level,r.column,r.row,neighbor);
    require(current.effective_code==r.effective_cell_code &&
            current.candidate_resource_ids[bank]==chosen &&
            ((bank==3U || bank==2U)==
                (r.path==NativeCode2WallPath::Banks3And2_32F8C)),
            "CODE.2 wall selection no longer matches mutable source");
    return draw_native_world_surface_column(
        world,materials,res4,r.level,r.column,r.row,bank,
        r.source_texture_column,viewport_x,height,shade,page,working_top,
        pixels,neighbor);
}
} // namespace rloft_native
