#pragma once
// Ravenloft RES type-6: EH envelope with type_header_size=0, followed by
// a separate 2-byte decoded length and LZSS column
// texture payload, 128 bytes per vertical source column. Read-only importer.
//
// Exact format evidence: Resources/RES/TYPE6_RENDERER.md; source CODE.2
// 33647..33664 and 338C8..3395A. No retail assets embedded.
#include "original_res_image_archive.hpp"
#include <cstdint>
#include <stdexcept>
#include <vector>
namespace rloft::original_res {
struct Texture6 {
    std::uint32_t archive_entry=0;
    std::uint16_t decoded_bytes=0;
    std::uint16_t columns=0;
    std::vector<Byte> column_major;
};
inline Texture6 decode_type6(const std::vector<Byte>& file,std::uint32_t entry_id) {
    if(file.size()<8u)throw std::runtime_error("short original RES archive");
    const auto count=le32(file.data());
    if(!count || count>200000u || entry_id>=count ||
       std::uint64_t(4u)+4ull*count>file.size())
        throw std::runtime_error("invalid RES directory/entry");
    const std::uint64_t min_offset=4u+4ull*count;
    if(le32(file.data()+4)!=min_offset)
        throw std::runtime_error("invalid RES first entry");
    std::uint32_t prev=static_cast<std::uint32_t>(min_offset);
    for(unsigned i=0;i<count;++i) {
        const auto off=le32(file.data()+4u+4u*i);
        if(off<prev || off>file.size())
            throw std::runtime_error("invalid RES directory offset order");
        prev=off;
    }
    const auto begin=le32(file.data()+4u+4u*entry_id);
    const auto end=entry_id+1<count ? le32(file.data()+4u+4u*(entry_id+1)):
        static_cast<std::uint32_t>(file.size());
    if(end-begin<8u)throw std::runtime_error("short RES6 entry");
    const Byte* e=file.data()+begin;
    if(e[0]!='E' || e[1]!='H' || e[2]!=6u || le16(e+3)!=0u)
        throw std::runtime_error("not an EH type-6 zero-size typed-header record");
    const auto decoded=le16(e+5);
    if(decoded!=4096u && decoded!=8192u)
        throw std::runtime_error("unsupported RES6 source texture dimensions");
    Texture6 t;
    t.archive_entry=entry_id;
    t.decoded_bytes=decoded;
    t.columns=static_cast<std::uint16_t>(decoded/128u);
    t.column_major=lzss(e+7u,end-begin-7u,decoded);
    if(t.column_major.size()!=decoded)
        throw std::runtime_error("invalid RES6 decompressed texture length");
    return t;
}
inline Texture6 load_type6(const std::string& filename,std::uint32_t entry) {
    return decode_type6(read_only(filename),entry);
}
inline const Byte* select_column(const Texture6& tex,std::uint32_t column) {
    if(column>=tex.columns || tex.column_major.size()!=tex.decoded_bytes)
        throw std::out_of_range("RES6 source column invalid");
    return tex.column_major.data()+column*128u;
}
} // namespace rloft::original_res
