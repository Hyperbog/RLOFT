#pragma once
// Read-only original RES archive/type-1 wrapper. No proprietary assets embedded.
// Original-byte contract: Resources/RES/FORMAT.md and CODE.2 2FD9C / 37700.
// This validates/decodes a bounded input; CODE.2 2A20C performs the final
// guest image layout conversion and 2975C/2F29C/2A380 render it.
#include <array>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>
namespace rloft::original_res {
using Byte=std::uint8_t;
struct Image {
    std::uint32_t archive_entry=0;
    std::uint16_t frames=0, rows=0;
    Byte width_units=0, tag=0, method=0, reorder=0;
    std::vector<Byte> decoded;
    std::uint32_t frame_size() const noexcept {
        return std::uint32_t(rows)*width_units*8u;
    }
};
inline std::uint16_t le16(const Byte* p) noexcept {
    return std::uint16_t(p[0])|(std::uint16_t(p[1])<<8);
}
inline std::uint32_t le32(const Byte* p) noexcept {
    return std::uint32_t(le16(p))|(std::uint32_t(le16(p+2))<<16);
}
inline std::vector<Byte> read_only(const std::string& filename) {
    std::ifstream in(filename,std::ios::binary);
    if(!in) throw std::runtime_error("Cannot open original RES archive read-only");
    in.seekg(0,std::ios::end);
    const auto end=in.tellg();
    if(end<0 || static_cast<std::uint64_t>(end)>64u*1024u*1024u)
        throw std::runtime_error("RES archive exceeds 64 MiB limit");
    in.seekg(0,std::ios::beg);
    std::vector<Byte> bytes(static_cast<std::size_t>(end));
    if(!bytes.empty()) in.read(reinterpret_cast<char*>(bytes.data()),
                              static_cast<std::streamsize>(bytes.size()));
    if(!in && !bytes.empty()) throw std::runtime_error("Truncated RES archive read");
    return bytes;
}
inline std::vector<Byte> lzss(const Byte* src,std::size_t size,std::size_t expected) {
    // CODE.2 37700 defines dictionary bytes [0,0xFEE) as FE. The final
    // 18 bytes are undefined until written: fail if a backref observes them.
    std::array<int,4096> dictionary{};
    dictionary.fill(-1);
    for(unsigned i=0;i<0xFEEu;++i) dictionary[i]=0xFE;
    std::vector<Byte> output;
    output.reserve(expected);
    std::size_t cursor=0;
    unsigned window_pos=0xFEE, flags=0;
    while(output.size()<expected) {
        flags>>=1u;
        if((flags&0x100u)==0) {
            if(cursor>=size) throw std::runtime_error("RES LZSS control EOF");
            flags=unsigned(src[cursor++])|0xFF00u;
        }
        if(flags&1u) {
            if(cursor>=size) throw std::runtime_error("RES LZSS literal EOF");
            const Byte x=src[cursor++];
            output.push_back(x);
            dictionary[window_pos]=x;
            window_pos=(window_pos+1u)&4095u;
        } else {
            if(size-cursor<2u) throw std::runtime_error("RES LZSS backref EOF");
            const unsigned lo=src[cursor++],hi=src[cursor++];
            const unsigned offset=lo|((hi&0xF0u)<<4u);
            const unsigned count=(hi&15u)+3u;
            for(unsigned j=0;j<count && output.size()<expected;++j) {
                const unsigned from=(offset+j)&4095u;
                const int x=dictionary[from];
                if(x<0) throw std::runtime_error("RES LZSS read of uninitialized dictionary");
                output.push_back(static_cast<Byte>(x));
                dictionary[window_pos]=x;
                window_pos=(window_pos+1u)&4095u;
            }
        }
    }
    if(cursor!=size) throw std::runtime_error("RES LZSS payload not exhausted exactly");
    return output;
}
inline Image decode_type1(const std::vector<Byte>& file,std::uint32_t entry_id) {
    if(file.size()<8u) throw std::runtime_error("Short RES archive");
    const std::uint32_t count=le32(file.data());
    if(count==0 || count>200000u || entry_id>=count ||
       std::uint64_t(count)*4u+4u>file.size())
        throw std::runtime_error("RES archive directory invalid");
    const std::size_t table_end=4u+std::size_t(count)*4u;
    if(le32(file.data()+4u)!=table_end)
        throw std::runtime_error("Invalid RES archive first offset");
    std::uint32_t previous=static_cast<std::uint32_t>(table_end);
    for(std::uint32_t i=0;i<count;++i) {
        const auto v=le32(file.data()+4u+std::size_t(i)*4u);
        if(v<previous || v>file.size()) throw std::runtime_error("Invalid RES directory offsets");
        previous=v;
    }
    const auto start=le32(file.data()+4u+std::size_t(entry_id)*4u);
    const auto stop=entry_id+1u<count ?
        le32(file.data()+4u+std::size_t(entry_id+1u)*4u):
        static_cast<std::uint32_t>(file.size());
    if(stop-start<13u) throw std::runtime_error("RES entry is empty/too short for type 1");
    const Byte* const e=file.data()+start;
    if(e[0]!='E'||e[1]!='H'||e[2]!=1u||le16(e+3)!=8u)
        throw std::runtime_error("RES image requires EH type 1 header size 8");
    const auto raw_frames=static_cast<std::int16_t>(le16(e+5));
    const auto raw_rows=static_cast<std::int16_t>(le16(e+7));
    Image image;
    image.archive_entry=entry_id;
    if(raw_frames<=0 || raw_rows<=0 || raw_frames>255 || raw_rows>400)
        throw std::runtime_error("RES raster frame/row count invalid");
    image.frames=static_cast<std::uint16_t>(raw_frames);
    image.rows=static_cast<std::uint16_t>(raw_rows);
    image.width_units=e[9];image.tag=e[10];image.method=e[11];image.reorder=e[12];
    if(image.width_units==0 || image.width_units>40u)
        throw std::runtime_error("RES type-1 plane width outside framebuffer");
    const std::uint64_t expected=std::uint64_t(image.frames)*image.rows*image.width_units*8u;
    if(expected>2u*1024u*1024u) throw std::runtime_error("RES decoded image >2 MiB");
    const Byte* src=e+13;
    const std::size_t size=stop-start-13u;
    if(image.method==0u) {
        if(size!=expected) throw std::runtime_error("RES raw raster payload length mismatch");
        image.decoded.assign(src,src+size);
    } else if(image.method==3u) {
        image.decoded=lzss(src,size,static_cast<std::size_t>(expected));
    } else {
        // Method 1 exists in CODE.2 but not this supplied corpus. Better to
        // reject until that exact original path is wired than claim support.
        throw std::runtime_error("RES type-1 compression method unsupported");
    }
    return image;
}
inline Image load_type1(const std::string& filename,std::uint32_t entry) {
    return decode_type1(read_only(filename),entry);
}
} // namespace rloft::original_res
