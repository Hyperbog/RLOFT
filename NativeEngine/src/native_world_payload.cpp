#include "rloft_native/native_world_payload.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft_native {
namespace {
void require(bool condition, const char* why) {
    if(!condition) throw std::runtime_error(why);
}

std::uint16_t u16(const std::vector<std::uint8_t>& bytes,std::size_t at) {
    require(at<=bytes.size() && bytes.size()-at>=2,
            "original native payload word extends beyond source");
    return static_cast<std::uint16_t>(bytes[at]) |
           static_cast<std::uint16_t>(bytes[at+1])<<8U;
}

std::uint32_t u32(const std::vector<std::uint8_t>& bytes,std::size_t at) {
    require(at<=bytes.size() && bytes.size()-at>=4,
            "original native payload dword extends beyond source");
    return static_cast<std::uint32_t>(bytes[at]) |
           static_cast<std::uint32_t>(bytes[at+1])<<8U |
           static_cast<std::uint32_t>(bytes[at+2])<<16U |
           static_cast<std::uint32_t>(bytes[at+3])<<24U;
}

std::vector<std::uint8_t> bytes_between(
    const std::vector<std::uint8_t>& data,
    std::size_t start,std::size_t end) {
    require(start<=end && end<=data.size(),
            "original native payload section extends beyond source");
    return {data.begin()+static_cast<std::ptrdiff_t>(start),
            data.begin()+static_cast<std::ptrdiff_t>(end)};
}
}

std::uint16_t NativeLevelPayload::resource_lookup(
    unsigned bank,unsigned code) const {
    if(bank>=5U || code>=64U)
        throw std::out_of_range("original level resource table lookup");
    const auto offset=0x00AU+std::size_t(bank)*128U+
                      std::size_t(code)*2U;
    return static_cast<std::uint16_t>(original_header.at(offset)) |
           static_cast<std::uint16_t>(original_header.at(offset+1U))<<8U;
}

std::uint16_t NativeLevelPayload::cell(unsigned x,unsigned y) const {
    if(x>=width || y>=height)
        throw std::out_of_range("native world cell outside original grid");
    const auto position=std::size_t(y)*width+x;
    return cell_words.at(position);
}

std::uint8_t NativeLevelPayload::tile_code(unsigned x,unsigned y) const {
    return static_cast<std::uint8_t>(cell(x,y)&0x003FU);
}

const NativeLevelPayload& NativeWorldPayload::level(unsigned id) const {
    if(id>=levels.size() || levels[id].id!=id)
        throw std::out_of_range("native original world level ID not present");
    return levels.at(id);
}

NativeMutableLevel& NativeMutableWorld::level(unsigned id) {
    if(id>=levels.size() || levels[id].id!=id)
        throw std::out_of_range("native mutable world level ID not present");
    return levels.at(id);
}

NativeWorldPayload decode_native_world_payload(
    const std::vector<std::uint8_t>& original_levels_dat) {
    // Independent original archive size/offset/family/event/automap checks
    // are already centralized in the source-only full-world reader.
    const NativeLevelSystem validated=decode_native_levels(original_levels_dat);
    require(validated.levels.size()==32,
            "native original payload needs all 32 original levels");

    NativeWorldPayload world;
    world.levels.reserve(validated.levels.size());
    for(unsigned id=0;id<32;++id) {
        const auto start=std::size_t(u32(original_levels_dat,4U+4U*id));
        const auto end=id==31 ? original_levels_dat.size():
            std::size_t(u32(original_levels_dat,4U+4U*(id+1)));
        const auto record=bytes_between(original_levels_dat,start,end);
        const auto& source=validated.levels[id];

        std::array<std::size_t,7> cut{};
        for(unsigned j=0;j<7;++j)
            cut[j]=u32(record,1U+4U*j);

        NativeLevelPayload level;
        level.id=id;
        level.width=source.width;
        level.height=source.height;
        const auto header=bytes_between(
            record,cut[0],cut[0]+level.original_header.size());
        std::copy(header.begin(),header.end(),level.original_header.begin());

        // Keep the original 16-bit words, including every unclassified bit.
        const auto cell_start=cut[0]+0x2A4;
        level.cell_words.reserve(source.packed_cells);
        for(std::size_t n=0;n<source.packed_cells;++n)
            level.cell_words.push_back(u16(record,cell_start+n*2U));

        // Preserve every original ten-byte record and its owning column.
        auto cursor=cut[1];
        level.controls.reserve(source.control_records);
        for(unsigned col=0;col<source.width;++col) {
            const auto count=std::size_t(u16(record,cursor));
            cursor+=2U;
            for(std::size_t n=0;n<count;++n) {
                NativeOriginalControl control;
                control.column=col;
                const auto original=bytes_between(record,cursor,cursor+10U);
                std::copy(original.begin(),original.end(),control.bytes.begin());
                level.controls.push_back(control);
                cursor+=10U;
            }
        }
        require(cursor==cut[2] &&
                level.controls.size()==source.control_records,
                "native typed controls disagree with certified original");

        // Three compact record families retain all eleven bytes, even where
        // the historical meaning of a field is not independently established.
        for(std::size_t family=0;family<3;++family) {
            const auto count=(cut[family+3]-cut[family+2])/11U;
            level.families[family].reserve(count);
            for(std::size_t n=0;n<count;++n) {
                std::array<std::uint8_t,11> original{};
                const auto at=cut[family+2]+n*11U;
                const auto bytes=bytes_between(record,at,at+11U);
                std::copy(bytes.begin(),bytes.end(),original.begin());
                level.families[family].push_back(original);
            }
        }
        require(level.families[0].size()==source.family_a &&
                level.families[1].size()==source.family_b &&
                level.families[2].size()==source.family_c,
                "native typed family cardinality disagrees with certified original");

        // Preserve unused command slots verbatim; future native event VM
        // decides which bytes are consumed, without running original x86.
        for(unsigned slot=0;slot<50;++slot) {
            const auto at=cut[5]+std::size_t(slot)*26U;
            const auto bytes=bytes_between(record,at,at+26U);
            std::copy(bytes.begin(),bytes.end(),level.event_slots[slot].begin());
        }

        const auto map_start=cut[6]+4U;
        level.automap=bytes_between(record,map_start,
                                    map_start+source.packed_cells);
        const auto annotation_start=map_start+source.packed_cells+4U;
        level.annotation=bytes_between(record,annotation_start,record.size());
        require(level.automap.size()==source.packed_cells &&
                level.annotation.size()==source.annotation_bytes,
                "native typed world tail disagrees with certified original");
        world.total_cells+=level.cell_words.size();
        world.total_control_records+=level.controls.size();
        world.total_event_commands+=source.event_commands;
        world.levels.push_back(std::move(level));
    }
    require(world.total_cells==validated.map_cells &&
            world.total_control_records==validated.control_records &&
            world.total_event_commands==validated.active_event_commands,
            "native typed world totals disagree with original archive audit");
    return world;
}

NativeWorldPayload load_native_world_payload(const NativeMedia& original_media) {
    return decode_native_world_payload(original_media.read("LEVELS.DAT"));
}

NativeMutableWorld clone_native_mutable_world(const NativeWorldPayload& original) {
    NativeMutableWorld result;
    result.levels.reserve(original.levels.size());
    for(const auto& level:original.levels)
        result.levels.push_back({level.id,level.width,level.height,
                                 level.cell_words,level.automap});
    return result;
}
} // namespace rloft_native
