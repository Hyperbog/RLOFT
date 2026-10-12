#include "rloft_native/native_world_materials.hpp"
#include "rloft_native/native_sha256.hpp"
#include "rloft_native/native_type6_texture.hpp"
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace rloft_native {
namespace {
void require(bool value,const char* reason) {if(!value)throw std::runtime_error(reason);}
void put16(std::vector<std::uint8_t>& v,unsigned n) {
    v.push_back(static_cast<std::uint8_t>(n));v.push_back(static_cast<std::uint8_t>(n>>8));
}
std::vector<std::uint8_t> level_source(const NativeLevelPayload& m) {
    std::vector<std::uint8_t> bytes;
    bytes.reserve(642);
    put16(bytes,m.id);
    for(unsigned code=0;code<64;++code)
        for(unsigned bank=0;bank<5;++bank)
            put16(bytes,m.resource_lookup(bank,code));
    return bytes;
}
std::string world_digest(const NativeWorldPayload& source) {
    std::vector<std::uint8_t> bytes{
        'R','L','O','F','T','-','L','E','V','E','L','S','-','R','E','S','4','-','v','1'};
    put16(bytes,static_cast<unsigned>(source.levels.size()));
    for(const auto& level:source.levels) {
        const auto b=level_source(level);
        bytes.insert(bytes.end(),b.begin(),b.end());
    }
    return native_sha256_hex(bytes);
}
}
NativeWorldMaterialCatalogue decode_native_world_material_catalogue(
    const NativeWorldPayload& world,const NativeResArchive& archive,
    unsigned wide_count) {
    require(world.levels.size()==32 && wide_count>0 &&
            wide_count<archive.entry_count(),"invalid original RES4 bank count");
    NativeWorldMaterialCatalogue out;
    out.wide_texture_count=wide_count;
    out.candidate_slots=32U*64U*4U;
    std::vector<bool> visited(archive.entry_count(),false);
    bool first=true;
    for(unsigned i=0;i<32;++i) {
        const auto& source=world.level(i);
        require(source.id==i && source.width>0 && source.height>0,
                "invalid level material header");
        NativeWorldMaterialLevel level;
        level.level=i;
        level.original_lookup_sha256=native_sha256_hex(level_source(source));
        for(unsigned code=0;code<64;++code) {
            for(unsigned bank=0;bank<5;++bank)
                level.source_lookups[code][bank]=source.resource_lookup(bank,code);
            const auto flag=level.source_lookups[code][4];
            if(first) {out.original_fifth_bank_word=flag;first=false;}
            require(flag==out.original_fifth_bank_word,
                    "original fifth header bank is not a uniform marker");
            for(unsigned bank=0;bank<4;++bank) {
                const unsigned id=level.source_lookups[code][bank];
                require(id<wide_count,"LEVELS material outside wide texture archive bank");
                out.nonzero_candidate_slots+=id!=0U;
                if(visited[id])continue;
                visited[id]=true;
                const auto tex=decode_native_type6_texture(archive.entry(id));
                require(tex.width==64 && tex.height==128 && tex.frames==1,
                        "mapped original material is not wide EH6 texture");
                ++out.unique_wide_textures;
            }
        }
        out.levels.push_back(std::move(level));
    }
    out.original_lookup_sha256=world_digest(world);
    return out;
}
NativeWorldSurfaceDraw draw_native_world_surface_column(
    const NativeWorldRuntime& world,
    const NativeWorldMaterialCatalogue& materials,
    const NativeResArchive& archive,
    unsigned level,unsigned column,unsigned row,
    unsigned bank,unsigned source_column,unsigned x,unsigned height,
    std::uint8_t shade,unsigned page,unsigned working_top,
    NativeIndexedVgaPlanes& video,
    std::optional<NativeCardinal> neighbor) {
    require(bank<4 && level<32 && materials.levels.size()==32 &&
            materials.wide_texture_count<archive.entry_count() &&
            materials.original_lookup_sha256==world_digest(world.original()),
            "world material source evidence or bank mismatch");
    const auto entry=native_scene_resource_candidates(world,level,column,row,neighbor);
    require(entry.effective_code<64 &&
            materials.levels[level].source_lookups[entry.effective_code][bank]==
            entry.raw_lookup[bank],"original material lookup mismatch");
    const unsigned id=entry.candidate_resource_ids[bank];
    require(id<materials.wide_texture_count,"state-selected RES4 texture outside wide bank");
    const auto tex=decode_native_type6_texture(archive.entry(id));
    require(tex.width==64 && tex.height==128,"selected material is not 64x128");
    auto pixel=draw_native_type6_columns(
        {{&tex,source_column,x,height,shade,page,working_top}},video);
    NativeWorldSurfaceDraw result;
    result.level=level;result.column=column;result.row=row;
    result.lookup_bank=bank;
    result.original_tile_code=entry.original_code;
    result.effective_tile_code=entry.effective_code;
    result.selected_original_res4_id=static_cast<std::uint16_t>(id);
    result.original_variant_offset=entry.selected_variant_offset;
    result.raster=std::move(pixel);
    return result;
}
} // namespace rloft_native
