#include "rloft/original_ground_planar_fill_replay.hpp"
#include <stdexcept>

namespace rloft {

OriginalGroundPlanarFillReport OriginalGroundPlanarFillReplay::replay_flat_es(
    VgaPlanarMemory& video,
    const SurfaceFillTable& original_fe94,
    OriginalGroundFillStrip strip,
    std::uint8_t base_color,
    unsigned original_c050_page) {
    // 1F1D6: FDBC=A0000+([C050]<<15), full guest DWORD.
    // The 320x400 planar aperture only supports C050=0 or 1.
    if(original_c050_page>1U)
        throw std::out_of_range("CODE.2 C050 outside double-buffer aperture");
    const auto fdbc=VgaPlanarMemory::kPhysicalBase+
        (static_cast<std::uint32_t>(original_c050_page)<<15U);
    const bool upper=strip==OriginalGroundFillStrip::Upper33E28;
    auto edi=fdbc+(upper?0x148U:0x50F8U);
    const auto first=edi;

    // 33DE0/33E28: OUT 03C4h,0F02h; mask all 4 VGA planes.
    video.write_sequencer_raw_mask_02(0x0FU);
    for(std::size_t row=0;row<kRows;++row) {
        // Original EBX=128..1; AL=byte(base-[FE94+EBX]).
        const auto color=static_cast<std::uint8_t>(
            base_color-original_fe94.subtract[kRows-row]);
        // ECX=40h, REP STOSB ES:[EDI]. Physical interpretation
        // here is conditional on an independently verified flat ES.
        for(std::uint32_t byte=0;byte<kBytesPerRow;++byte)
            video.write(edi+byte,0x0FU,color);
        if(row+1U<kRows) {
            // REP advances 40h; upper adds 10h, lower subtracts 90h.
            edi=upper ? edi+kStride : edi-kStride;
        }
    }
    return OriginalGroundPlanarFillReport{
        fdbc,first,edi,edi+kBytesPerRow-1U,
        kRows*kBytesPerRow,kRows*kBytesPerRow*4U,0x0FU
    };
}

} // namespace rloft
