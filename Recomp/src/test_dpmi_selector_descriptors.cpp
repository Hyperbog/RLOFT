// DPMI 0006 descriptor table contract: independent of game assets.
#include "dpmi_selector_descriptors.hpp"
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>

using rloft_recomp::DpmiSelectorDescriptors;
static void check(bool condition,const char* message) {
    if(!condition)throw std::runtime_error(message);
}
int main() {
    try {
        DpmiSelectorDescriptors table;
        check(table.size()==0u,"descriptor table was preseeded");
        const auto invalid=table.get_base(0x60u);
        check(invalid.carry && invalid.ax==0x8022u,
              "unallocated selector accepted by 0006");
        check(!table.install(0u,0x00100000u,0x100u),
              "null descriptor accepted");
        check(!table.install(0x60u,0x00100000u,0u),
              "zero-sized descriptor accepted");
        check(!table.install(0x60u,0xffffffffu,2u),
              "32-bit linear range overflow accepted");
        check(table.size()==0u,"rejections installed a descriptor");
        check(table.install(0x60u,0x0002d600u,5u*16u),
              "DPMI 0100-style selector registration failed");
        check(table.install(0x17u,0x01000000u,0x1000u),
              "LE code-selector registration failed");
        check(!table.install(0x60u,0x01000000u,1u),
              "duplicate selector identity was replaced");
        const auto base=table.get_base(0x60u);
        check(!base.carry && base.ax==0u &&
              base.cx==0x0002u && base.dx==0xd600u &&
              base.base()==0x0002d600u,
              "DPMI 0006 must split protected linear base into CX:DX");
        const auto program=table.get_base(0x17u);
        check(!program.carry && program.base()==0x01000000u,
              "LE program selector returned wrong base");
        check(table.check(0x60u,0u,80u) &&
              table.check(0x60u,79u,1u) &&
              table.check(0x60u,80u,0u) &&
              !table.check(0x60u,80u,1u) &&
              !table.check(0x60u,0u,81u) &&
              !table.check(0x60u,0xffffffffu,1u) &&
              !table.check(0x68u,0u,1u),
              "descriptor bounds or unmapped-selector check failed");
        check(table.retire(0x60u),"selector release failed");
        check(!table.retire(0x60u),"stale selector was released twice");
        check(table.get_base(0x60u).carry &&
              !table.check(0x60u,0u,1u),
              "retired selector remained accessible");
        check(table.install(0x68u,0x0002d600u,80u) &&
              table.get_base(0x68u).base()==0x2d600u &&
              table.get_base(0x60u).carry,
              "reallocated base revived retired selector");
        check(table.install(0x70u,0xfffffff0u,16u) &&
              table.check(0x70u,15u,1u) &&
              !table.check(0x70u,16u,1u),
              "last representable 32-bit range rejected");
        check(table.retire(0x17u) && table.retire(0x68u) &&
              table.retire(0x70u) && table.size()==0u,
              "descriptors leaked after shutdown");
        std::cout<<"DPMI_INT31_0006_DESCRIPTOR_BASE=PASS\n"
                    "SELECTOR_BASE_CX_DX=PASS\n"
                    "SELECTOR_INVALIDATION_AND_BOUNDS=PASS\n"
                    "ORIGINAL_DOS_PARITY=NO\n";
        return 0;
    }catch(const std::exception& e) {
        std::cerr<<"DPMI_0006_SELECTOR_TEST_FAIL: "<<e.what()<<'\n';
        return 1;
    }
}
