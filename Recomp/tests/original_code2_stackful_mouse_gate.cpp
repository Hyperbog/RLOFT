// Original retail CODE.2 entry to the actual sequential DOS host boundaries.
// The mouse reset response here is an EXPLICIT TEST DEVICE, not a claim
// that a real DOS driver was detected on a user's PC.
#include "../src/canonical_code2_runtime_owner.hpp"
#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>
int main(int argc,char** argv){
    try{
        if(argc!=2)throw std::invalid_argument("usage: stackful_mouse_gate ORIGINAL_CODE.2");
        namespace owner=rloft::canonical_code2_runtime;
        std::unique_ptr<owner::Opaque,void(*)(owner::Opaque*)> guest(
            owner::start_stackful(argv[1]),&owner::destroy);
        const auto first=owner::inspect(guest.get());
        if(!first.source_353a0_executed ||
           !first.source_20608_executed ||
           !first.source_cpu_stack_resumable ||
           !first.source_keyboard_vectors_installed ||
           !first.source_sound_config_read ||
           !first.paused_at_original_mouse_reset ||
           first.pending_DOS_service_sequence!=1u ||
           first.host_dpmi_dos_blocks!=0u ||
           owner::pending_host_service(guest.get())!=1u)
            throw std::runtime_error("Original 20608 did not suspend at real INT33 reset");
        // A test host-provided virtual DOS mouse reset result allows
        // CODE.2's ACTUAL compiled source to continue; the source
        // DPMI 0100 allocation runs using the tracked original DOS heap.
        // Next unsupported service must be 33AA0 -> DPMI 0300 INT33.
        const auto next=owner::answer_mouse_reset(guest.get(),true,3u);
        const auto after=owner::inspect(guest.get());
        if(next!=3u || owner::pending_host_service(guest.get())!=3u ||
           after.memory_address!=first.memory_address ||
           after.heap_arena_header!=first.heap_arena_header ||
           after.host_dpmi_dos_blocks!=1u ||
           after.pending_DOS_service_sequence!=2u ||
           !after.source_cpu_stack_resumable)
            throw std::runtime_error(
                "Original mouse reset did not advance to tracked DPMI 0300 INT33");
        std::cout<<"ORIGINAL_CODE2_LIVE_STACKFUL_MOUSE_ADVANCE_PASS"
                 <<" source_entry=353A0_20608"
                 <<" mouse_reset_service=ACTUAL_SOURCE_INT33"
                 <<" initial_source_stack=PRESERVED"
                 <<" test_virtual_mouse_buttons=3"
                 <<" original_DPMI_0100_allocations=1"
                 <<" next_unsupported=ORIGINAL_33AA0_INT31_0300_MOUSE"
                 <<" suspended_guest_stack=PRESERVED"
                 <<" native_gameplay_frame=NOT_REACHED\n";
        return 0;
    }catch(const std::exception& ex) {
        std::cerr<<"ORIGINAL_CODE2_STACKFUL_MOUSE_GATE_FAIL: "<<ex.what()<<'\n';
        return 1;
    }
}
