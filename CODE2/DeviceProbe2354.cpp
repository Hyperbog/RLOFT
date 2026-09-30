#include "DeviceProbe2354.h"

namespace ravenloft::game {

std::int32_t DeviceProbe2354::run(const DeviceProbe2354Ops& o) {
    std::array<std::uint8_t,0x1C> block{};
    block[0]=0x00;
    block[1]=0x02; // WORD 0x0200 at the structure head.
    // The assembly also explicitly clears the WORD corresponding to
    // original [esp+0x0C] before 0x30D52.
    block[0x0C]=0;
    block[0x0D]=0;

    o.call30D52(0x17,block);

    const std::uint8_t flags=block[1];
    if((flags&0x80u)==0) return 0;
    if((flags&0x20u)!=0) return 0;
    if((flags&0x10u)==0) return 0;
    if((flags&0x08u)!=0) return 0;
    return 1;
}

} // namespace ravenloft::game
