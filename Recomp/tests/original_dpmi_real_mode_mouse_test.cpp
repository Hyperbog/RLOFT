// Media-free DPMI 0300 ABI regression. The original CODE.2 frame is
// 0x32 bytes. Validate bounded little-endian register writeback while
// preserving the real-mode flags and ES selector/segment fields.
#include "../src/original_dpmi_real_mode_mouse.hpp"
#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>

namespace rm=rloft::dpmi_real_mode_mouse;
static void require(bool good,const char* msg){
    if(!good)throw std::runtime_error(msg);
}
int main(){
    try {
        constexpr std::size_t at=0x30u;
        std::array<std::uint8_t,at+rm::kFrameBytes+3u> ram{};
        auto* frame=ram.data()+at;
        rm::store32(frame+rm::kOffsetEAX,0x53C1u);
        rm::store32(frame+rm::kOffsetEBX,0xAABBCCDDu);
        rm::store32(frame+rm::kOffsetECX,0x12345678u);
        rm::store32(frame+rm::kOffsetEDX,0xDEADBEEFu);
        frame[rm::kOffsetFlags]=0xADu;
        frame[rm::kOffsetFlags+1u]=0x55u;
        frame[rm::kOffsetES]=0x60u;
        frame[rm::kOffsetES+1u]=0x2Du;
        frame[0]=0xF7u;
        const auto original=rm::before(ram.data(),ram.size(),at,0x2D60u);
        require(original.eax==0x53C1u && original.ebx==0xAABBCCDDu &&
                original.ecx==0x12345678u && original.edx==0xDEADBEEFu,
                "Original DPMI register read mismatch");
        rm::apply(ram.data(),ram.size(),at,0x2D60u,
                  {1u,3u,0x01234567u,0xABCDEFFEu});
        const auto returned=rm::before(ram.data(),ram.size(),at,0x2D60u);
        require(returned.eax==1u && returned.ebx==3u &&
                returned.ecx==0x01234567u && returned.edx==0xABCDEFFEu,
                "DPMI real-mode mouse register writeback mismatch");
        require(frame[rm::kOffsetFlags]==0xADu &&
                frame[rm::kOffsetFlags+1u]==0x55u &&
                frame[0]==0xF7u && frame[rm::kOffsetES]==0x60u &&
                frame[rm::kOffsetES+1u]==0x2Du,
                "DPMI frame modified unrelated registers/flags");
        const auto copy=ram;
        bool bad_segment=false;
        try {
            rm::apply(ram.data(),ram.size(),at,0x2D61u,{2u,4u,0u,0u});
        }catch(const std::runtime_error&){bad_segment=true;}
        require(bad_segment && ram==copy,
                "DPMI wrong-segment response mutated guest frame");
        bool bad_length=false;
        try{
            rm::apply(ram.data(),at+rm::kFrameBytes-1u,at,0x2D60u,
                      {2u,4u,0u,0u});
        }catch(const std::out_of_range&){bad_length=true;}
        require(bad_length && ram==copy,
                "DPMI truncated response mutated guest frame");
        std::cout<<"ORIGINAL_DPMI_0300_REGISTER_FRAME_PASS"
                 <<" little_endian=YES"
                 <<" eax_ebx_ecx_edx=EXACT"
                 <<" segment_and_flags=UNCHANGED"
                 <<" out_of_bounds=REJECTED"
                 <<" virtual_mouse_results=TEST_ONLY\n";
        return 0;
    }catch(const std::exception& e){
        std::cerr<<"ORIGINAL_DPMI_0300_REGISTER_FRAME_FAIL: "<<e.what()<<"\n";
        return 1;
    }
}
