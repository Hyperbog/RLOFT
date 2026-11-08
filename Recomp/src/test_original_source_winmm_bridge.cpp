// No fake AIL, WinMM or sound-card success is allowed. This test never
// opens an audio device and does not embed original sound resources.
#include "original_source_winmm_bridge.hpp"
#include <iostream>
#include <stdexcept>
#include <vector>
int main() {
    try {
        rloft::source_audio::WinMMSourceSfx backend{};
        if(backend.status_playing() || backend.completions()!=0u)
            throw std::runtime_error("Uninitialized host adapter returned playing/done");
        bool rejected=false;
        try {
            backend.preflight({0u,0u,0u,0u});
        }catch(const std::runtime_error&) {rejected=true;}
        if(!rejected || backend.status_playing() || backend.completions()!=0u)
            throw std::runtime_error("Invalid sampled SFX was accepted by host device");
        rejected=false;
        try {backend.start_staged();}
        catch(const std::runtime_error&) {rejected=true;}
        if(!rejected || backend.status_playing() || backend.completions()!=0u)
            throw std::runtime_error("Unprepared source SFX was submitted or completed");
        std::cout<<"ORIGINAL_28168_WINMM_DRIVER_BOUNDARY_TEST_PASS"
                 <<" input_media=NONE"
                 <<" invalid_packet=REJECTED"
                 <<" unprepared_write=REJECTED"
                 <<" fabricated_playing=NONE"
                 <<" fabricated_completion=NONE"
                 <<" native_AIL=NOT_CERTIFIED\n";
        return 0;
    }catch(const std::exception& e){
        std::cerr<<"ORIGINAL_28168_WINMM_DRIVER_BOUNDARY_TEST_FAIL: "
                 <<e.what()<<"\n";
        return 1;
    }
}
