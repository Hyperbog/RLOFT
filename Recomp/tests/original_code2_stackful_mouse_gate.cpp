// Original retail CODE.2 entry to the actual sequential DOS host boundaries.
// The mouse reset response here is an EXPLICIT TEST DEVICE, not a claim
// that a real DOS driver was detected on a user's PC.
#include "../src/canonical_code2_runtime_owner.hpp"
#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
int main(int argc,char** argv){
    try{
        if(argc!=2 && !(argc==3 && std::string(argv[2])=="--through-host-disk"))
            throw std::invalid_argument("usage: stackful_mouse_gate ORIGINAL_CODE.2 [--through-host-disk]");
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
        // A virtual DOS mouse's explicit 33h response is written back
        // to the source real-mode frame, then original source 33B44
        // should release tracked 0101 memory and request INT33
        // horizontal mouse range (function 7), then vertical (8).
        const auto rangeX=owner::answer_real_mode_mouse(
            guest.get(),1u,3u,0u,0u);
        const auto after0300=owner::inspect(guest.get());
        if(rangeX!=2u || owner::pending_host_service(guest.get())!=2u ||
           after0300.host_dpmi_dos_blocks!=0u ||
           after0300.original_dpmi_0101_releases!=1u ||
           after0300.original_mouse_range_calls!=1u ||
           after0300.pending_DOS_service_sequence!=3u ||
           after0300.memory_address!=first.memory_address)
            throw std::runtime_error(
                "Original DPMI 0300 did not release 0101 and request mouse X range");
        std::cout<<"ORIGINAL_CODE2_DPMI_0300_RESUMED_PASS"
                 <<" original_DPMI_0101_release=1"
                 <<" source_INT33_range7=PENDING"
                 <<" source_stack=RETAINED\\n";
        const auto rangeY=owner::answer_mouse_range(guest.get(),7u);
        const auto afterRangeX=owner::inspect(guest.get());
        if(rangeY!=2u || owner::pending_host_service(guest.get())!=2u ||
           afterRangeX.original_mouse_range_calls!=2u ||
           afterRangeX.pending_DOS_service_sequence!=4u)
            throw std::runtime_error("Original source mouse Y range not reached");
        const auto disk=owner::answer_mouse_range(guest.get(),8u);
        const auto afterRangeY=owner::inspect(guest.get());
        if(disk!=4u || owner::pending_host_service(guest.get())!=4u ||
           afterRangeY.pending_DOS_service_sequence!=5u ||
           afterRangeY.host_dpmi_dos_blocks!=0u ||
           afterRangeY.memory_address!=first.memory_address ||
           !afterRangeY.source_cpu_stack_resumable)
            throw std::runtime_error(
                "Original startup did not reach DOS 21h AH36 after range X/Y");
        std::cout<<"ORIGINAL_CODE2_SOURCE_MOUSE_RANGE_TO_DISK_PASS"
                 <<" DPMI0300=EXPLICIT_FRAME_WRITEBACK"
                 <<" original_DPMI0101=RELEASED"
                 <<" original_INT33_range7=EXECUTED"
                 <<" original_INT33_range8=EXECUTED"
                 <<" next_unsupported=DOS_INT21_AH36"
                 <<" source_stack=PRESERVED"
                 <<" original_game_loop=NOT_REACHED\\n";
        std::cout<<"ORIGINAL_CODE2_LIVE_STACKFUL_MOUSE_ADVANCE_PASS"
                 <<" source_entry=353A0_20608"
                 <<" mouse_reset_service=ACTUAL_SOURCE_INT33"
                 <<" initial_source_stack=PRESERVED"
                 <<" test_virtual_mouse_buttons=3"
                 <<" original_DPMI_0100_allocations=1"
                 <<" next_unsupported=ORIGINAL_33AA0_INT31_0300_MOUSE"
                 <<" suspended_guest_stack=PRESERVED"
                 <<" native_gameplay_frame=NOT_REACHED\n";
        if(argc==3) {
            // Deliberately execute actual original CODE2 beyond the
            // previously unsupported AH36 callback using OBSERVED host
            // volume capacity and available space, converted to DOS 16-
            // bit FAT-style geometry. This MAY encounter another real
            // unavailable host callback. Never mark gameplay ready.
            std::cout<<"ORIGINAL_CODE2_AH36_HOST_ADVANCE_BEGIN"
                     <<" input_volume=OWNER_ORIGINAL_INSTALL"
                     <<" source_35286=ORIGINAL_CODE2"<<std::endl;
            const auto nextService=owner::answer_host_disk_free(guest.get());
            if(!nextService)
                throw std::runtime_error(
                    "Original CODE2 source returned after AH36 without gameplay acceptance");
            std::cout<<"ORIGINAL_CODE2_AFTER_AH36_NEW_SERVICE"
                     <<" service="<<nextService
                     <<" source_stack=PRESERVED"
                     <<" original_game_loop=NOT_VERIFIED\n";
        }
        return 0;
    }catch(const std::exception& ex) {
        std::cerr<<"ORIGINAL_CODE2_STACKFUL_MOUSE_GATE_FAIL: "<<ex.what()<<'\n';
        return 1;
    }
}
