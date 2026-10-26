#pragma once
// Shared signed 32-bit DOS file seek arithmetic for owner-private source
// transports. No file I/O, guest mutation, global state or allocation.
// DOS origin: 0=SET, 1=CUR, 2=END. The original displacement is encoded
// as a u32 with signed two's-complement semantics. Do not interpret that
// input via implementation-defined unsigned -> signed casts.
#include <cstdint>
#include <limits>
namespace rloft { namespace dos_seek {
inline bool compute(std::uint32_t displacement,std::uint32_t origin,
                    std::uint32_t cursor,std::uint64_t file_size,
                    std::uint64_t limit,std::uint32_t& result) noexcept {
    if(origin>2u || file_size>limit ||
       limit>std::numeric_limits<std::uint32_t>::max() ||
       cursor>limit)
        return false;
    const std::int64_t delta=
        displacement<0x80000000u?
        static_cast<std::int64_t>(displacement):
        static_cast<std::int64_t>(displacement)-0x100000000LL;
    const std::int64_t base=origin==0u?0:
        origin==1u?static_cast<std::int64_t>(cursor):
                   static_cast<std::int64_t>(file_size);
    const std::int64_t next=base+delta;
    if(next<0 || next>static_cast<std::int64_t>(limit))
        return false;
    result=static_cast<std::uint32_t>(next);
    return true;
}
}} // namespace rloft::dos_seek
