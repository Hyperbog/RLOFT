#include "rloft_native/native_source_scene_compositor.hpp"
#include "rloft_native/native_lzss.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft_native {
namespace {
void require(bool valid,const char* reason){
    if(!valid)throw std::runtime_error(reason);
}
}
NativeSceneCompositeResult compose_native_source_scene_rgb6(
    NativeWorldRuntime& world,
    const NativeWorldMaterialCatalogue& materials,
    const NativeResArchive& res4,
    const NativeResArchive& res3,
    const NativeSintabPrograms& sintab,
    const NativeType7VgaMasks& masks,
    const NativeOriginalLevelPaletteTable& palettes,
    const std::vector<std::uint8_t>& colors,
    unsigned level,unsigned page,
    const std::vector<NativeSceneDrawCommand>& commands,
    NativeIndexedVgaPlanes& video,
    unsigned output_height){
    require(level<32 && page<=1 && !commands.empty() &&
            commands.size()<=320 && (output_height==200 || output_height==400),
            "original-source scene compositing parameters out of bounds");
    require(materials.levels.size()==32 && sintab.blocks.size()==127 &&
            res3.entry_count()>0 && res4.entry_count()>0 &&
            masks.original_code2_sha256==palettes.original_code2_sha256,
            "original-world scene source archives or CODE.2 state incomplete");

    // Stage BOTH mutable reachability and VGA planes. The original
    // world contains stateful 0x0440 visibility writes; a late sprite
    // exception cannot publish visibility from an earlier wall.
    NativeWorldRuntime candidate_world(world);
    NativeIndexedVgaPlanes candidate_video(video);
    NativeSceneCompositeResult report;
    report.ordered_commands=commands.size();
    for(const auto& cmd:commands){
        if(cmd.kind==NativeSceneDrawKind::Wall){
            const auto& w=cmd.wall;
            require(w.viewport_column<320U && w.resource_word_slot<2U,
                    "source original wall viewport/resource slot invalid");
            const auto record=emit_native_code2_wall_record(
                candidate_world,materials,level,w.cell_column,w.cell_row,
                w.path,w.original_phase,w.original_sign_sentinel,
                w.caller_depth,w.caller_screen_position,w.source_neighbor);
            const auto draw=draw_native_code2_wall_record_projected_35730(
                candidate_world,materials,res4,record,w.projection,
                w.resource_word_slot,w.viewport_column,
                w.original_palette_shade,page,w.working_region_top,
                candidate_video,w.source_neighbor);
            ++report.wall_commands;
            report.texture_source_reads+=draw.raster.raster.source_texel_reads;
            report.indexed_plane_writes+=draw.raster.raster.plane_writes;
        }else if(cmd.kind==NativeSceneDrawKind::Sprite){
            const auto& s=cmd.sprite;
            require(s.original_res3_id<res3.entry_count() &&
                    s.original_sintab_scale<127U &&
                    s.destination_width>0 && s.destination_width<=7U,
                    "source original EH7 sprite resource/scale/run invalid");
            const auto bytes=res3.entry(s.original_res3_id);
            const auto shape=decode_native_type7_scene_shape(bytes);
            require(shape.structure_verified && bytes.size()>=7,
                    "original EH7 geometry source corrupted");
            const auto uncompressed=std::size_t(bytes[5]) |
                                     (std::size_t(bytes[6])<<8U);
            const auto body=native_lzss4k_decode(
                bytes.data()+7,bytes.size()-7,uncompressed);
            require(body.consumed==bytes.size()-7,
                    "original EH7 LZSS has trailing or truncated source");
            const auto dispatch=plan_native_type7_vga_dispatch(
                masks,s.viewport_start_x,s.destination_width,
                static_cast<std::uint8_t>(page),
                s.renderer_row,s.renderer_row_byte_offset,
                s.original_ecx_lookup,s.individual_columns_mode);
            const auto rendered=draw_native_type7_vga_dispatch(
                sintab,shape,s.original_sintab_scale,
                s.source_column,body.bytes,s.source_base,
                dispatch,candidate_video);
            ++report.sprite_commands;
            report.total_eh7_descriptors+=rendered.descriptor_invocations;
            report.eh7_source_reads+=rendered.indexed_source_reads;
            report.indexed_plane_writes+=rendered.individual_plane_writes;
        }else{
            throw std::runtime_error("unknown original-source scene draw tag");
        }
    }
    report.final_four_plane_sha256=candidate_video.planes_sha256();
    // The original DAC6 conversion is included in the *same* atomic
    // operation. Bad source palettes cannot alter the caller's world.
    report.scanout=scanout_native_world_vga_rgb6(
        candidate_video,palettes,colors,level,page,320U,output_height);
    world=std::move(candidate_world);
    video=std::move(candidate_video);
    return report;
}
} // namespace rloft_native
