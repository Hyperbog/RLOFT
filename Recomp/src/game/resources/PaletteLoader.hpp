#pragma once
// Original CODE.2 object-1 325B0 palette-bank reader, recovered file/guest
// contract. This bounded C++ source reconstruction is NOT yet installed in
// canonical Code2.cpp and is NOT proof that the original CPU executed this
// call site at the captured palette transition.
//
// Caller must include canonical Code2.cpp before including this header.
// The authentic source opens COLORS, seeks to 1 + bank*0x300, reads
// count*3 bytes into guest_palette+3*first, closes, then invokes 32150.
// Read-only, validated host callbacks stand in for the original DOS I/O ABI.
#include <cstdint>
#include <stdexcept>
#include <limits>

namespace rloft { namespace original_code2_palette {
using u8=std::uint8_t;
using u32=std::uint32_t;
struct ReadOnlyFile {
    void* opaque=nullptr;
    bool (*open)(void*,const char*)=nullptr;
    bool (*seek_absolute)(void*,u32)=nullptr;
    bool (*read_exact)(void*,u8*,u32)=nullptr;
    void (*close)(void*)=nullptr;
};
struct LoadWitness {
    u32 palette_index=0;
    u32 offset=0;
    u32 count=0;
    u32 destination=0;
    bool brightness_source_executed=false;
};
inline LoadWitness function_325b0(code2::Machine& machine,
                                  ReadOnlyFile& file,
                                  u32 palette_dest,u32 first_color,
                                  u32 bank,u32 color_count,
                                  u32 guest_memory_extent) {
    if(!machine.data || !file.open || !file.seek_absolute ||
       !file.read_exact || !file.close)
        throw std::invalid_argument("Original 325B0 palette requires guest and DOS file callbacks");
    if(bank>=15u || first_color>=256u || color_count==0u ||
       color_count>256u-first_color)
        throw std::out_of_range("Original 325B0 palette range exceeds COLORS bank");
    const u32 start=palette_dest+3u*first_color;
    const u32 extent=3u*color_count;
    if(palette_dest>guest_memory_extent ||
       std::uint64_t(palette_dest)+768u>guest_memory_extent ||
       std::uint64_t(start)+extent>guest_memory_extent)
        throw std::out_of_range("Original 325B0 guest destination outside RAM");
    const u32 offset=1u+bank*768u;
    if(!file.open(file.opaque,"COLORS"))
        throw std::runtime_error("Original 325B0 open COLORS failed");
    bool closed=false;
    try {
        if(!file.seek_absolute(file.opaque,offset) ||
           !file.read_exact(file.opaque,machine.data+start,extent))
            throw std::runtime_error("Original 325B0 seek/read COLORS failed");
        file.close(file.opaque);closed=true;
    }catch(...) {
        if(!closed)file.close(file.opaque);
        throw;
    }
    // The original 32150 is already reconstructed in the canonical CODE.2
    // compilation unit. Run it over the entire destination palette, as
    // the original 325B0 source does after a (potentially partial) load.
    code2::function_32150(machine,palette_dest);
    return {bank,offset,color_count,palette_dest,true};
}
}} // namespace rloft::original_code2_palette
