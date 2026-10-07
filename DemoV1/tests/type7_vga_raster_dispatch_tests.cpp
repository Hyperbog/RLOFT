#include "rloft/type7_vga_raster_dispatch.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

int main() {
    constexpr std::size_t start = 0x75F24U;
    constexpr std::size_t offsets[3] = {0xC668U, 0xC688U, 0xC6A8U};
    std::vector<std::uint8_t> bytes(start + 0xEF4DU + 1U);
    for (unsigned phase=0; phase<4U; ++phase) {
        for (unsigned count=1U; count<=7U; ++count) {
            for (unsigned n=0; n<count; ++n) {
                const auto x=phase+n;
                bytes[start+offsets[x/4U]+phase*8U+count] |=
                    static_cast<std::uint8_t>(1U << (x&3U));
            }
        }
    }
    const auto masks=rloft::Type7VgaPlaneMaskModel::from_code2(bytes);
    const auto grouped=rloft::Type7VgaRasterDispatchModel::build(
        masks,6,7U,0U,25U,120U,0xA1B2C3D4U,false);
    assert(grouped.calls.size()==3U);
    assert(grouped.calls[0].sequencer_mask==12U);
    assert(grouped.calls[1].sequencer_mask==15U);
    assert(grouped.calls[2].sequencer_mask==1U);
    assert(grouped.lookup_after_dispatch==0xA1B2D4C3U);
    const auto base=rloft::VgaPlanarMemory::kPhysicalBase+25U*80U+121U;
    for(unsigned i=0;i<3U;++i)
        assert(grouped.calls[i].physical_address==base+i);

    const auto single=rloft::Type7VgaRasterDispatchModel::build(
        masks,6,7U,1U,25U,120U,0xA1B2C3D4U,true);
    assert(single.calls.size()==7U);
    for(unsigned i=0;i<7U;++i) {
        const auto x=6U+i;
        assert(single.calls[i].sequencer_mask==(1U<<(x&3U)));
        assert(single.calls[i].ecx_lookup==
               (x&1U?0xA1B2D4C3U:0xA1B2C3D4U));
        assert(single.calls[i].physical_address==
               base+rloft::VgaPlanarMemory::kPageBytes+
               (x/4U)-1U);
    }
    assert(single.lookup_after_dispatch==0xA1B2C3D4U);
    const auto neg=rloft::Type7VgaRasterDispatchModel::build(
        masks,-5,1U,0U,0U,0U,0U,false);
    assert(neg.calls[0].physical_address==
           rloft::VgaPlanarMemory::kPhysicalBase-2U);
    bool failed=false;
    try {
        (void)rloft::Type7VgaRasterDispatchModel::build(
            masks,0,8U,0U,0U,0U,0U,false);
    } catch(const std::out_of_range&) {failed=true;}
    assert(failed);
}
