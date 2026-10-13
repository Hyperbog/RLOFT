#include "rloft_native/native_scene_frame_ranges.hpp"
#include "rloft_native/native_sha256.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace rloft_native {
namespace {
void require(bool ok,const char* reason) {
    if(!ok)throw std::runtime_error(reason);
}
void append16(std::vector<std::uint8_t>& out,std::size_t n) {
    require(n<=65535U,"scene authored frame ledger 16-bit overflow");
    out.push_back(static_cast<std::uint8_t>(n));
    out.push_back(static_cast<std::uint8_t>(n>>8U));
}
void append32(std::vector<std::uint8_t>& out,std::size_t n) {
    require(n<=0xffffffffULL,"scene authored frame ledger 32-bit overflow");
    for(unsigned i=0;i<4;++i)
        out.push_back(static_cast<std::uint8_t>(n>>(i*8U)));
}
std::vector<std::uint8_t> encode_level(
    unsigned id,const std::vector<NativeAuthoredFrameRange>& records) {
    std::vector<std::uint8_t> bytes;
    bytes.reserve(6U+records.size()*22U);
    append16(bytes,id);
    append32(bytes,records.size());
    for(const auto& r:records) {
        require(r.level==id && r.source_class<=2U,
                "authored frame-range record level/class corrupted");
        bytes.push_back(static_cast<std::uint8_t>(r.source_class));
        append32(bytes,r.compact_record_index);
        append16(bytes,r.original_type_id);
        append16(bytes,r.runtime_type_id);
        append32(bytes,r.first_absolute_res3_id);
        append32(bytes,r.resource_slot_count);
        bytes.push_back(r.state_variant_count);
        bytes.push_back(r.source_shape_stride);
        bytes.push_back(r.npc_animation_stride);
        bytes.push_back(r.npc_cycle_divisor);
        bytes.push_back(r.npc_animation_span);
    }
    return bytes;
}
}
NativeAuthoredFrameRangeDirectory derive_native_authored_frame_ranges(
    const NativeAuthoredSceneDirectory& authored,
    const NativeSceneDefinitionDirectory& defs) {
    require(authored.levels.size()==32 &&
            authored.resolved_source_bases_sha256.size()==64,
            "original scene authored ledger incomplete");
    NativeAuthoredFrameRangeDirectory out;
    out.source_resolved_instances_sha256=authored.resolved_source_bases_sha256;
    out.levels.reserve(32);
    out.per_level_frame_ranges_sha256.reserve(32);
    static constexpr char label[]="RLOFT-native-authored-frame-ranges-v1";
    std::vector<std::uint8_t> all(label,label+sizeof(label)-1U);
    append32(all,32);
    for(unsigned level=0;level<32;++level) {
        std::vector<NativeAuthoredFrameRange> records;
        records.reserve(authored.levels[level].size());
        for(const auto& instance:authored.levels[level]) {
            require(instance.level==level && instance.source_class<=2,
                    "original authored frame instance corrupted");
            NativeAuthoredFrameRange r;
            r.level=level;
            r.source_class=instance.source_class;
            r.compact_record_index=instance.compact_record_index;
            r.original_type_id=instance.original_type_id;
            r.runtime_type_id=instance.runtime_type_id;
            r.first_absolute_res3_id=instance.absolute_res3_base;
            if(r.source_class==0) {
                require(r.runtime_type_id<defs.npc_definitions.size(),
                        "NPC source definition outside original table");
                const auto& n=defs.npc_definitions[r.runtime_type_id];
                require(n.absolute_shape_base==r.first_absolute_res3_id,
                        "NPC source base mismatch against authenticated instance");
                r.resource_slot_count=1;
                r.source_shape_stride=n.animation_stride;
                r.state_variant_count=1; // base-only; NOT NPC frame count
                r.npc_animation_stride=n.animation_stride;
                r.npc_cycle_divisor=n.cycle_divisor;
                r.npc_animation_span=n.animation_span;
                ++out.npc_source_metadata_records;
            }else if(r.source_class==1) {
                const auto d=defs.init_scene_descriptor(1,r.runtime_type_id);
                require(d.shape_stride>0 && d.variant_count>0 &&
                        defs.banks.absolute_id(1,d.shape_base_local)==
                            r.first_absolute_res3_id,
                        "INIT class-one source base/stride invalid");
                r.resource_slot_count=
                    std::size_t(d.shape_stride)*d.variant_count;
                r.source_shape_stride=d.shape_stride;
                r.state_variant_count=d.variant_count;
                ++out.init_state_subranges;
            }else{
                require(r.runtime_type_id<defs.object_definitions.size(),
                        "OBJECTS source type outside original table");
                const auto& d=defs.object_definitions[r.runtime_type_id];
                require(d.absolute_shape_base==r.first_absolute_res3_id,
                        "OBJECTS source base mismatch against authenticated instance");
                // Zero-stride base-only OBJECTS definitions are valid.
                r.resource_slot_count=std::max(std::size_t(1),
                                                std::size_t(d.shape_stride));
                r.source_shape_stride=d.shape_stride;
                r.state_variant_count=1; // live selector NOT reconstructed
                ++out.loose_item_subranges;
            }
            require(r.resource_slot_count>0 &&
                    defs.banks.contains(r.source_class,
                        r.first_absolute_res3_id-
                            defs.banks.starts[r.source_class]) &&
                    r.first_absolute_res3_id>=defs.banks.starts[r.source_class] &&
                    r.resource_slot_count<=
                        defs.banks.starts[r.source_class]+
                        defs.banks.lengths[r.source_class]-
                        r.first_absolute_res3_id,
                    "source-authored frame range crosses RES3 class bank");
            ++out.instances;
            out.bounded_resource_slots+=r.resource_slot_count;
            records.push_back(r);
        }
        const auto row=encode_level(level,records);
        out.per_level_frame_ranges_sha256.push_back(
            native_sha256_hex(row));
        all.insert(all.end(),row.begin(),row.end());
        out.levels.push_back(std::move(records));
    }
    out.frame_ranges_sha256=native_sha256_hex(all);
    return out;
}
std::size_t select_native_authored_res3_frame(
    const NativeAuthoredFrameRange& r,
    const NativeRes3SceneBanks& banks,
    unsigned variant,unsigned subframe) {
    require(r.source_class<=2 && r.state_variant_count>0 &&
            r.resource_slot_count>0 &&
            variant<r.state_variant_count,
            "source frame variant selector outside original definition");
    if(r.source_class==0)
        require(variant==0 && subframe==0,
                "NPC live animation frame selection not yet reconstructed");
    else if(r.source_class==1)
        require(r.source_shape_stride>0 &&
                subframe<r.source_shape_stride,
                "INIT perspective subframe exceeds authenticated stride");
    else
        require((r.source_shape_stride==0 && subframe==0) ||
                (r.source_shape_stride>0 &&
                 subframe<r.source_shape_stride),
                "OBJECTS perspective subframe exceeds authenticated stride");
    const auto offset=std::size_t(variant)*r.source_shape_stride+subframe;
    require(offset<r.resource_slot_count &&
            r.first_absolute_res3_id>=banks.starts[r.source_class] &&
            r.first_absolute_res3_id+offset<
                banks.starts[r.source_class]+banks.lengths[r.source_class],
            "selected original RES3 frame outside owning source allocation");
    return r.first_absolute_res3_id+offset;
}
NativeAuthoredBaseShapeEvidence probe_native_authored_base_eh7(
    const NativeAuthoredFrameRange& range,
    const NativeRes3SceneBanks& banks,
    const NativeResArchive& original_res3) {
    require(banks.res3_entries==original_res3.entry_count() &&
            banks.res3_entries>0,
            "original RES3 base probe bank/archive cardinality mismatch");
    const auto id=select_native_authored_res3_frame(range,banks,0U,0U);
    require(id<original_res3.entry_count(),
            "source-authored base EH7 frame outside original archive");
    const auto shape=decode_native_type7_scene_shape(original_res3.entry(id));
    require(shape.structure_verified &&
            shape.source_width>=64U &&
            shape.source_width<=128U,
            "source-authored original base frame has invalid EH7 geometry");
    NativeAuthoredBaseShapeEvidence result;
    result.absolute_res3_id=id;
    result.source_width=shape.source_width;
    result.x_min=shape.x_min;
    result.x_max=shape.x_max;
    result.y_min=shape.y_min;
    result.y_max=shape.y_max;
    result.raster_descriptors=shape.total_descriptors;
    result.decoded_body_bytes=shape.decoded_body_bytes;
    result.original_entry_sha256=shape.original_entry_sha256;
    result.original_body_sha256=shape.decoded_body_sha256;
    return result;
}

std::string native_authored_frame_ranges_json(
    const NativeAuthoredFrameRangeDirectory& dir) {
    std::ostringstream out;
    out<<"{\"schema\":\"rloft-native-authored-scene-frame-ranges/v1\","
       <<"\"authored_instance_count\":"<<dir.instances
       <<",\"bounded_resource_slots\":"<<dir.bounded_resource_slots
       <<",\"npc_metadata_instances\":"<<dir.npc_source_metadata_records
       <<",\"init_state_subranges\":"<<dir.init_state_subranges
       <<",\"loose_item_subranges\":"<<dir.loose_item_subranges
       <<",\"original_instance_source_sha256\":\""
       <<dir.source_resolved_instances_sha256
       <<"\",\"frame_ranges_sha256\":\""<<dir.frame_ranges_sha256
       <<"\",\"per_level_frame_ranges_sha256\":[";
    for(std::size_t i=0;i<dir.per_level_frame_ranges_sha256.size();++i) {
        if(i)out<<",";
        out<<"\""<<dir.per_level_frame_ranges_sha256[i]<<"\"";
    }
    out<<"],\"original_view_subframe_selection_verified\":false,"
       <<"\"original_npc_animation_frame_verified\":false,"
       <<"\"original_dos_depth_order_verified\":false,"
       <<"\"original_full_frame_parity_verified\":false}\n";
    return out.str();
}
} // namespace rloft_native
