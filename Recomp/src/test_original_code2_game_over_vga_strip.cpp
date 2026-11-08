// Execute original 0CADC/2A078 against the real planar VGA host.
// Distinct source/destination pages and VGA latches, not observer stubs.
#include "../../Code2/Code2.cpp"
#include "original_code2_video_boot.hpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>
int main() {
    try {
        std::vector<std::uint8_t> memory(2u*1024u*1024u,0u);
        code2::Machine machine{};machine.data=memory.data();
        rloft::graphics::OriginalVgaHost v{};
        v.bios_set_mode(3u);
        rloft::source_video_boot::Bridge bridge(machine,v,memory.size());
        bridge.run_original_290f4(3u);
        const auto check=[](bool value,const char* error) {
            if(!value)throw std::runtime_error(error);
        };
        check(v.bios_mode()==0x13u &&
              memory[0xEFF9u]!=0u, "authentic source VGA initialization missing");
        const std::uint32_t row=5u,column=12u;
        const std::uint32_t source_addr=row*80u+column/4u;
        const std::uint32_t dest_addr=source_addr+0x8000u;
        check(v.write_indexed_port(0x3C4,0x0F02u),"VGA plane masks");
        check(v.write_indexed_port(0x3CE,0x0005u),"VGA mode 0");
        check(v.write_aperture(static_cast<std::uint16_t>(source_addr),0xA5u),
              "source planar pixel write");
        check(v.write_aperture(static_cast<std::uint16_t>(dest_addr),0x13u),
              "destination planar pixel write");
        check(v.write_indexed_port(0x3CE,0x0004u),"VGA read plane");
        check(v.read_aperture(static_cast<std::uint16_t>(dest_addr))==0x13u,
              "source/destination test must be independent");
        const std::uint8_t prior=memory[0xEFE9u];
        bridge.run_original_2a078_game_over_strip(1u,row,column);
        check(memory[0xEFE9u]==prior,"original source page mapping mutated");
        check(v.read_aperture(static_cast<std::uint16_t>(dest_addr))==0xA5u &&
              v.read_aperture(static_cast<std::uint16_t>(source_addr))==0xA5u,
              "original 2A078 source VGA latch copy incorrect");
        check(!bridge.invalid_access && !v.unsupported_read(),
              "source VGA reported unsupported register access");
        bool invalid_rejected=false;
        try {bridge.run_original_2a078_game_over_strip(1u,400u,0u);}
        catch(const std::invalid_argument&) {invalid_rejected=true;}
        check(invalid_rejected,"out-of-VGA strip must be rejected");
        std::cout<<"ORIGINAL_0CADC_2A078_RETAINED_VGA_STRIP_PASS"
                 <<" source_latches=YES target_page=1"
                 <<" real_VGA=YES live_game_over=NOT_ENABLED\n";
        return 0;
    }catch(const std::exception& e) {
        std::cerr<<"FAIL "<<e.what()<<"\n";return 1;
    }
}
