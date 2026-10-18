#pragma once
// Read-only original LEVELS.DAT archive -> CODE.2 guest header/cell grid.
// Source evidence: CODE.2 181B4 and 334C4; DatFiles/LEVELS.DAT/FORMAT.md.
// Source has 32 offset-indexed level records. This is NOT a world renderer.
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
namespace rloft::original_levels {
using Byte=std::uint8_t;
inline std::uint16_t le16(const Byte* p) noexcept {
    return std::uint16_t(p[0]) | (std::uint16_t(p[1])<<8);
}
inline std::uint32_t le32(const Byte* p) noexcept {
    return std::uint32_t(le16(p)) | (std::uint32_t(le16(p+2))<<16);
}
struct LevelGrid {
    std::uint32_t level=0;
    std::uint16_t width=0,height=0;
    std::array<Byte,0x2A4> header{};
    std::vector<std::uint16_t> cells;
};
inline LevelGrid decode_grid(const std::vector<Byte>& bytes,std::uint32_t level) {
    if(bytes.size()<8u || bytes.size()>16u*1024u*1024u)
        throw std::runtime_error("Invalid LEVELS.DAT archive length");
    const std::uint32_t count=le32(bytes.data());
    if(count!=32u || level>=count || bytes.size()<4u+4u*count)
        throw std::runtime_error("LEVELS.DAT index/count invalid");
    const std::uint32_t base=4u+4u*count;
    std::uint32_t prev=base;
    for(std::uint32_t i=0;i<count;++i) {
        const std::uint32_t off=le32(bytes.data()+4u+4u*i);
        if((i==0 && off!=base)||off<prev||off>=bytes.size())
            throw std::runtime_error("LEVELS.DAT offsets invalid");
        prev=off;
    }
    const std::uint32_t first=le32(bytes.data()+4u+4u*level);
    const std::uint32_t last=level+1u<count?
        le32(bytes.data()+4u+4u*(level+1u)):static_cast<std::uint32_t>(bytes.size());
    if(last<=first || last-first<0x1Du+0x2A4u)
        throw std::runtime_error("Truncated original level header");
    const Byte* record=bytes.data()+first;
    std::uint32_t previous=0x1Du;
    for(unsigned i=0;i<7u;++i) {
        const std::uint32_t b=le32(record+1u+4u*i);
        if((i==0 && b!=0x1Du)||b<previous || b>last-first)
            throw std::runtime_error("Original level boundary invalid");
        previous=b;
    }
    const Byte* source=record+0x1Du;
    const auto w=static_cast<std::int16_t>(le16(source+1u));
    const auto h=static_cast<std::int16_t>(le16(source+3u));
    if(w<=0||h<=0||w>128||h>128)
        throw std::runtime_error("Original level dimensions invalid");
    const std::uint32_t count_cells=std::uint32_t(w)*std::uint32_t(h);
    const std::uint32_t gridend=0x1Du+0x2A4u+2u*count_cells;
    if(gridend!=le32(record+5u) || gridend>last-first)
        throw std::runtime_error("Original level grid boundary inconsistent");
    LevelGrid out{};
    out.level=level;out.width=static_cast<std::uint16_t>(w);
    out.height=static_cast<std::uint16_t>(h);
    for(std::size_t i=0;i<out.header.size();++i)out.header[i]=source[i];
    const Byte* grid=source+0x2A4u;
    out.cells.reserve(count_cells);
    for(std::uint32_t i=0;i<count_cells;++i)
        out.cells.push_back(le16(grid+i*2u));
    return out;
}
// Read-only standalone LEVnnn.DAT direct-save representation. This is
// intentionally distinct from the compact archive entry format: 181B4
// selects a different branch, with expanded actors and two column lists.
inline LevelGrid decode_saved_grid(const std::vector<Byte>& bytes,
                                   std::uint32_t level) {
    if(level>=32u || bytes.size()<0x1Du+0x2A4u+4u ||
       bytes.size()>16u*1024u*1024u)
        throw std::runtime_error("Invalid standalone LEV length/index");
    std::uint32_t prev=0x1Du;
    std::array<std::uint32_t,7u> section{};
    for(unsigned i=0u;i<section.size();++i) {
        const auto off=le32(bytes.data()+1u+4u*i);
        if((i==0u && off!=0x1Du) || off<prev || off>bytes.size())
            throw std::runtime_error("Invalid LEV section boundary");
        section[i]=off;prev=off;
    }
    const Byte* src=bytes.data()+0x1Du;
    const auto width=static_cast<std::int16_t>(le16(src+1u));
    const auto height=static_cast<std::int16_t>(le16(src+3u));
    if(width<=0 || height<=0 || width>128 || height>128)
        throw std::runtime_error("Invalid LEV grid dimensions");
    const auto cells=static_cast<std::uint32_t>(width)*
                     static_cast<std::uint32_t>(height);
    if(section[1]!=0x1Du+0x2A4u+cells*2u ||
       section[6]-section[5]!=0x514u ||
       bytes.size()-section[6]<cells+8u)
        throw std::runtime_error("Invalid standalone LEV grid/events/automap bounds");
    // Source 181B4 seeks forward four bytes after the 0x514-byte
    // script block, *then* reads the width*height-byte automap.
    const std::uint32_t automap=section[6]+4u;
    const auto annotation=le32(bytes.data()+automap+cells);
    if(annotation>=0x7FFu || bytes.size()-automap-cells-4u!=annotation)
        throw std::runtime_error("Invalid standalone LEV annotation tail");
    LevelGrid grid{};
    grid.level=level;
    grid.width=static_cast<std::uint16_t>(width);
    grid.height=static_cast<std::uint16_t>(height);
    for(std::size_t i=0;i<grid.header.size();++i)grid.header[i]=src[i];
    grid.cells.reserve(cells);
    const Byte* packed=src+0x2A4u;
    for(std::uint32_t i=0u;i<cells;++i)grid.cells.push_back(le16(packed+2u*i));
    return grid;
}
inline std::vector<Byte> read_archive(const std::string& filename) {
    std::ifstream in(filename,std::ios::binary|std::ios::ate);
    if(!in)throw std::runtime_error("Cannot open original LEVELS.DAT read-only");
    const auto size=in.tellg();
    if(size<0 || size>16*1024*1024)
        throw std::runtime_error("Original LEVELS.DAT exceeds bounded size");
    in.seekg(0);
    std::vector<Byte> bytes(static_cast<std::size_t>(size));
    if(!bytes.empty())in.read(reinterpret_cast<char*>(bytes.data()),
                              static_cast<std::streamsize>(bytes.size()));
    if(!in)throw std::runtime_error("Truncated original LEVELS.DAT read");
    return bytes;
}
} // namespace rloft::original_levels
