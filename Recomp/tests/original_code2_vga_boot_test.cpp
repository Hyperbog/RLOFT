// Standalone media-free native execution of canonical 290F4 -> 29930
// VGA planar 320x400 source setup against the existing bounded VGA host.
#include "../../Code2/Code2.cpp"
#include "../src/original_code2_video_boot.hpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>
int main(){
    try{
        std::vector<std::uint8_t> memory(2u*1024u*1024u);
        code2::Machine machine{};
        machine.data=memory.data();
        code2::store_le16(memory.data()+0xEF47u,0u);
        rloft::graphics::OriginalVgaHost video{};
        rloft::source_video_boot::Bridge bridge(machine,video,memory.size());
        bridge.run_original_290f4(3u);
        auto check=[&](bool ok,const char* why){
            if(!ok)throw std::runtime_error(why);
        };
        check(bridge.source_mode_calls==1u &&
              bridge.bios_changes==1u &&
              video.bios_mode()==0x13u &&
              video.repeat_scanlines()==1u &&
              memory[0xEF3Cu]==3u &&
              code2::load_le16(memory.data()+0xEF3Fu)==400u &&
              code2::load_le16(memory.data()+0xEF41u)==320u &&
              code2::load_le32(memory.data()+0xEFFDu)==400u &&
              code2::load_le32(memory.data()+0xF005u)==0x800u,
              "Original 290F4/29930 source video layout diverged");
        check(video.crtc_start()==0u && !bridge.invalid_access &&
              !video.unsupported_read(),
              "Original CRTC/VGA initialization unsupported");
        for(unsigned i=0;i<768u;++i)
            memory[0xC054u+i]=static_cast<std::uint8_t>(i&63u);
        bridge.stream_original_29234(0xC054u);
        check(bridge.source_palette_stream_calls==1u &&
              video.palette(0).r==0u &&
              video.palette(0).g==1u &&
              video.palette(0).b==2u &&
              video.palette(255).r==61u &&
              video.palette(255).g==62u &&
              video.palette(255).b==63u,
              "Original 29234/29CA0 did not stream full 768 RGB6 bytes");
        // Original source video shutdown route requests the DOS text mode.
        bridge.run_original_290f4(0xFFu);
        check(video.bios_mode()==3u && memory[0xEF3Cu]==0xFFu,
              "Original CODE2 290F4 exit video mode diverged");
        std::cout<<"SOURCE_CODE2_290F4_FULL_VGA_BOOT_PASS"
                 <<" original_functions=290F4_29930_2AB98_2ABF4_2AC10_29FEC_29FFC_296C0"
                 <<" original_320x400_source=YES"
                 <<" BIOS_mode_13=EXECUTED"
                 <<" original_29234_DAC=768_RGB6"
                 <<" text_mode_exit=EXECUTED"
                 <<" original_1FC14_loop=NOT_REACHED\n";
        return 0;
    }catch(const std::exception& e){
        std::cerr<<"SOURCE_CODE2_290F4_VGA_BOOT_FAIL: "<<e.what()<<'\n';
        return 1;
    }
}
