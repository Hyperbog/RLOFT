#include "rloft_native/native_scene_instances.hpp"
#include "rloft_native/native_sha256.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace rloft_native {
namespace {
void require(bool ok,const char* reason) {
    if(!ok) throw std::runtime_error(reason);
}
std::uint16_t word(const std::array<std::uint8_t,11>& bytes,
                   unsigned offset) {
    return std::uint16_t(bytes.at(offset)) |
           (std::uint16_t(bytes.at(offset+1U))<<8U);
}
std::int16_t signed_word(const std::array<std::uint8_t,11>& bytes,
                          unsigned offset) {
    const auto value=word(bytes,offset);
    return value<32768U?static_cast<std::int16_t>(value):
        static_cast<std::int16_t>(static_cast<std::int32_t>(value)-65536);
}
void add16(std::vector<std::uint8_t>& out,unsigned value) {
    out.push_back(static_cast<std::uint8_t>(value));
    out.push_back(static_cast<std::uint8_t>(value>>8U));
}
void add32(std::vector<std::uint8_t>& out,std::size_t value) {
    require(value<=0xFFFFFFFFULL,"scene source ledger exceeds 32-bit bound");
    for(unsigned k=0;k<4;++k)
        out.push_back(static_cast<std::uint8_t>(value>>(8U*k)));
}
std::string compact_original_sha(const NativeWorldPayload& world) {
    static constexpr char label[]="RLOFT-compact-world-scene-families-v1";
    std::vector<std::uint8_t> bytes(label,label+sizeof(label)-1U);
    add32(bytes,world.levels.size());
    for(const auto& level:world.levels) {
        add16(bytes,level.id);add16(bytes,level.width);
        add16(bytes,level.height);
        for(const auto& family:level.families) {
            add32(bytes,family.size());
            for(const auto& record:family)
                bytes.insert(bytes.end(),record.begin(),record.end());
        }
    }
    return native_sha256_hex(bytes);
}
}

NativeAuthoredSceneDirectory derive_native_authored_scene_instances(
    const NativeWorldPayload& world,
    const NativeSceneDefinitionDirectory& defs) {
    require(world.levels.size()==32,"native scene ledger needs 32 original levels");
    NativeAuthoredSceneDirectory result;
    result.original_compact_families_sha256=compact_original_sha(world);
    result.levels.reserve(world.levels.size());
    for(unsigned level_id=0;level_id<32;++level_id) {
        const auto& source=world.level(level_id);
        require(source.id==level_id && source.width>0 && source.height>0,
                "native scene authored source map invalid");
        std::vector<NativeAuthoredSceneInstance> list;
        const std::size_t total=source.families[0].size()+
                                source.families[1].size()+
                                source.families[2].size();
        list.reserve(total);
        for(unsigned cls=0;cls<3;++cls) {
            const auto& records=source.families[cls];
            for(std::size_t ordinal=0;ordinal<records.size();++ordinal) {
                const auto& record=records[ordinal];
                NativeAuthoredSceneInstance out;
                out.level=level_id;
                out.source_class=cls;
                out.compact_record_index=ordinal;
                out.original_type_id=word(record,0);
                out.runtime_type_id=out.original_type_id;
                const auto x_cell=signed_word(record,2);
                const auto y_cell=signed_word(record,6);
                require(x_cell>=0 && y_cell>=0 &&
                        x_cell<static_cast<int>(source.width) &&
                        y_cell<static_cast<int>(source.height),
                        "original compact authored scene cell outside map");
                out.cell_column=static_cast<unsigned>(x_cell);
                out.cell_row=static_cast<unsigned>(y_cell);
                const auto xfraction=word(record,4);
                const auto yfraction=word(record,8);
                out.x_position_16_16=static_cast<std::int32_t>(
                    std::int64_t(x_cell)*65536+xfraction);
                out.y_position_16_16=static_cast<std::int32_t>(
                    std::int64_t(y_cell)*65536+yfraction);
                out.compact_flags=record[10];
                if(cls==0) {
                    // CODE.2 loader maps archive byte +0x0A low three
                    // bits into facing; other bits are retained verbatim.
                    out.compact_facing_0_to_7=record[10]&7U;
                    out.absolute_res3_base=defs.npc_shape_base(
                        out.runtime_type_id);
                    ++result.original_npc_instances;
                } else if(cls==1) {
                    // Class-1 type ID indexes 175 INIT descriptors.
                    // No inferred angle/view subframe is selected.
                    out.absolute_res3_base=defs.init_base_variant(
                        1,out.runtime_type_id,0);
                    ++result.original_placed_instances;
                } else {
                    // Original CODE.2 archive loader maps legacy family-C
                    // type 0x54 to current OBJECTS.DAT 0x58.
                    if(out.runtime_type_id==0x54U) {
                        out.runtime_type_id=0x58U;
                        ++result.original_legacy_object_aliases;
                    }
                    out.absolute_res3_base=defs.object_shape_base(
                        out.runtime_type_id);
                    ++result.original_loose_instances;
                }
                require(defs.banks.contains(cls,
                        out.absolute_res3_base-defs.banks.starts[cls]),
                        "scene instance source base exceeds original RES3 namespace");
                list.push_back(out);
            }
        }
        result.levels.push_back(std::move(list));
    }
    return result;
}

std::vector<NativeAuthoredSceneInstance>
native_marked_scene_instance_candidates(
    const NativeWorldRuntime& runtime,
    const NativeAuthoredSceneDirectory& directory,
    unsigned level,std::size_t limit) {
    require(level<32 && directory.levels.size()==32 &&
            runtime.original().levels.size()==32,
            "scene instance original level scope mismatch");
    require(directory.original_compact_families_sha256==
            compact_original_sha(runtime.original()),
            "scene instance ledger belongs to a different original world");
    require(limit<=32768U,"scene instance candidate list limit too large");
    std::vector<NativeAuthoredSceneInstance> visible;
    visible.reserve(std::min(limit,directory.levels[level].size()));
    for(const auto& item:directory.levels[level]) {
        require(item.level==level,"scene instance level metadata corrupted");
        if(!runtime.cell(level,item.cell_column,item.cell_row).transient_visible)
            continue;
        require(visible.size()<limit,
                "scene instance candidate list exceeds caller budget");
        visible.push_back(item);
    }
    return visible;
}
} // namespace rloft_native
