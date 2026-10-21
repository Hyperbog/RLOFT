// Host-ordered original-source startup checkpoint. DOES NOT claim the
// original DOS CODE.1 main 508C invoked CODE.2. The real program handoff
// remains unmapped: CODE.1 FC98 enters a host stop callback here.
//
// This combines two *independently authenticated executable modules*
// in one native process: CODE.1 CC1C/CD97/FCE8/FC98 source constructors,
// followed by CODE.2 INIT/INFO/RES and scene-selected COLORS→DAC in one
// CODE.2 guest session. No SAVE.DAT, synthetic palette bytes or sprites.
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include "original_media_identity.hpp"
#include "original_r_exe_supervisor.hpp"
#include "original_r_exe_native_session.hpp"
#include "original_code2_guest_session.hpp"
#include <memory>
#include <fstream>
#include <iterator>
#include <cstdint>

int original_code1_startup_main(int argc,char** argv);
// The CODE.2 guest is now a persistent module port, not a disposable CLI stage.

namespace {
int run_stage(int (*entry)(int,char**),std::vector<std::string> text) {
    std::vector<char*> argv;
    argv.reserve(text.size());
    for(auto& value:text)argv.push_back(value.data());
    return entry(static_cast<int>(argv.size()),argv.data());
}
}

int main(int argc,char** argv) {
    try {
        if(argc!=5 || std::string(argv[1])!="--root" ||
           std::string(argv[3])!="--cd-root")
            throw std::invalid_argument(
                "usage: rloft_code1_code2_staged_boot --root ORIGINAL_RLOFT --cd-root ORIGINAL_CD_RLOFT");
        const std::filesystem::path root(argv[2]),cd(argv[4]);
        const auto code1=(root/"CODE.1").string();
        const auto code2=(root/"CODE.2").string();
        if(!std::filesystem::is_regular_file(code1) ||
           !std::filesystem::is_regular_file(code2) ||
           !std::filesystem::is_regular_file(root/"COLORS"))
            throw std::runtime_error("CODE.1/CODE.2/COLORS not present at original installation");
        // The original 16-bit R.EXE supervisor launches CODE.2 FIRST,
        // then CODE.1 only when CODE.2 actually exits with status 10.
        // Never model an imagined CODE.1 -> CODE.2 direct CPU jump.
        const auto r_exe=root/"R.EXE";
        std::ifstream rfile(r_exe,std::ios::binary);
        if(!rfile)throw std::runtime_error("Missing original R.EXE supervisor");
        const std::vector<std::uint8_t> supervisor_bytes{
            std::istreambuf_iterator<char>(rfile),
            std::istreambuf_iterator<char>()};
        if(supervisor_bytes.size()!=12368u ||
           rloft_recomp::sha256_hex(supervisor_bytes)!=
           "a6d5874dc91fb068a3ed0a07290547c310ee7f4f50be4d8176c4788f251631b0")
            throw std::runtime_error("Original R.EXE supervisor SHA-256 mismatch");
        rloft::original_supervisor::Machine owner_supervisor;
        if(owner_supervisor.next_module()!=rloft::original_supervisor::Module::Code2)
            throw std::runtime_error("R.EXE source protocol no longer starts CODE.2");
        std::cout<<"RLOFT_ORIGINAL_SUPERVISOR_R_EXE_SHA256=AUTHENTIC"
                 <<" INITIAL_CHILD=CODE2\n";
        std::cout<<"RLOFT_STAGED_BOOT_STAGE=CODE2_ORIGINAL_INIT_INFO_11_RES_PALETTE\n";
        // Genuine CODE2 guest memory, original INIT+INFO+11 RES archives
        // and palette remain alive on the same source session across
        // consecutive host dispatcher iterations. No synthetic DOS exit.
        namespace runtime=rloft::native_module_runtime;
        namespace owner=rloft::original_code2_session;
        struct PersistentCode2 final:runtime::ModulePort {
            std::unique_ptr<owner::Opaque,void(*)(owner::Opaque*)> game;
            owner::Evidence initial{};
            explicit PersistentCode2(const std::filesystem::path& r,
                                     const std::filesystem::path& d)
                :game(owner::create(r,d,5u),&owner::destroy) {
                std::cerr<<"RLOFT_STAGED_TRACE=CANONICAL_RESOURCE_GUEST_CONSTRUCTED\n"<<std::flush;
                initial=owner::inspect(game.get());
                if(!initial.initialized || initial.resource_archives!=0u ||
                   initial.vga_port_writes!=0u ||
                   !initial.original_20608_executed_to_host_dependency ||
                   initial.original_352ee_getenv_calls==0u ||
                   initial.original_game_loop_connected)
                    throw std::runtime_error("Original CODE.2 persistent guest startup differs");
            }
            runtime::ChildStep step() override {
                owner::dispatch_pending(game.get());
                const auto now=owner::inspect(game.get());
                if(now.memory_address!=initial.memory_address ||
                   now.source_state_fingerprint!=initial.source_state_fingerprint ||
                   now.resource_cache_base!=initial.resource_cache_base ||
                   now.resource_archives!=initial.resource_archives ||
                   now.selected_palette!=initial.selected_palette ||
                   now.vga_port_writes!=initial.vga_port_writes ||
                   now.dispatch_calls==0u || now.original_game_loop_connected)
                    throw std::runtime_error("CODE.2 resource/palette guest state lost on Pending");
                if(now.dispatch_calls==3u) {
                    std::cerr<<"RLOFT_STAGED_TRACE=THIRD_STEP_ORIGINAL_SOURCE_ADVANCE_BEGIN\n"<<std::flush;
                    // After two unchanged Pending dispatches, resume
                    // the SAME source stack through 2F468 and ACTUAL
                    // original 290F4 / 29930 planar VGA configuration,
                    // then stop at the next 20294 / 2E590 dependency.
                    if(owner::test_advance_through_original_video_mode(game.get())!=7u)
                        throw std::runtime_error("CODE2 did not run source VGA initializer");
                    const auto progressed=owner::inspect(game.get());
                    if(progressed.memory_address!=initial.memory_address ||
                       progressed.resource_archives!=11u ||
                       progressed.selected_palette!=3u ||
                       progressed.vga_port_writes!=769u ||
                       progressed.original_game_loop_connected)
                        throw std::runtime_error(
                            "Original CODE2 lazy RES and VGA state diverged");
                }
                return {runtime::Progress::Pending,0};
            }
        };
        runtime::SupervisorSession session(
            [&](rloft::original_supervisor::Module which,
                const runtime::CharsHandoff& chars)
                ->std::unique_ptr<runtime::ModulePort>{
                if(which!=rloft::original_supervisor::Module::Code2 ||
                   chars.available())return nullptr;
                return std::make_unique<PersistentCode2>(root,cd);
            });
        std::cerr<<"RLOFT_STAGED_TRACE=SUPERVISOR_FIRST_STEP_BEGIN\n"<<std::flush;
        const auto first=session.step_once();
        std::cerr<<"RLOFT_STAGED_TRACE=SUPERVISOR_FIRST_STEP_DONE\n"<<std::flush;
        const auto second=session.step_once();
        std::cerr<<"RLOFT_STAGED_TRACE=SUPERVISOR_SECOND_STEP_DONE\n"<<std::flush;
        if(first.current!=rloft::original_supervisor::Module::Code2 ||
           first.launches!=1u || first.transitions!=0u ||
           second.launches!=1u || second.transitions!=0u ||
           second.steps!=2u || !second.running ||
           second.chars_available || second.stopped)
            throw std::runtime_error("Original R.EXE invented CODE2 exit or relaunched guest");
        std::cerr<<"RLOFT_STAGED_TRACE=SUPERVISOR_THIRD_STEP_BEGIN\n"<<std::flush;
        const auto third=session.step_once();
        std::cerr<<"RLOFT_STAGED_TRACE=SUPERVISOR_THIRD_STEP_DONE\n"<<std::flush;
        if(third.current!=rloft::original_supervisor::Module::Code2 ||
           third.launches!=1u || third.transitions!=0u ||
           third.steps!=3u || third.stopped || !third.running)
            throw std::runtime_error("CODE2 virtual host-service progress restarted the DOS child");
        std::cout<<"RLOFT_CODE2_SAME_GUEST_PERSISTENCE_PASS"
                 <<" source_init=16D04"
                 <<" source_archives=11"
                 <<" palette_bank=3"
                 <<" DAC_writes=769"
                 <<" original_main_20608=EXECUTED_TO_2F2EC"
                 <<" original_352EE_getenv=COMPILED"
                 <<" pending_steps=2"
                 <<" relaunches=0"
                 <<" exit_status=NOT_OBSERVED"
                 <<" game_loop=NOT_CONNECTED\n";
        // No genuine CODE2 status10: CODE1 cannot be launched as an
        // actual supervisor child. The independent constructor probe
        // below is a separate diagnostic only.
        std::cout<<"RLOFT_ORIGINAL_SUPERVISOR_STOP="
                 <<"CODE2_PROCESS_EXIT_STATUS_NOT_OBSERVED"
                 <<" NEXT_CODE1_CHILD=NOT_AUTHORIZED\n";
        // Additional independent bounded CODE.1 source diagnostic: it
        // may be useful to the developer but is not an R.EXE child action.
        std::cout<<"RLOFT_STAGED_BOOT_INDEPENDENT_DIAGNOSTIC="
                 <<"CODE1_CC1C_CD97_FCE8_CTORS_NOT_SUPERVISOR_CHILD\n";
        if(run_stage(&original_code1_startup_main,
                     {"rloft_code1_startup_gate",
                      "--extender-host-resize-to-cd97-constructors",code1})!=0)
            throw std::runtime_error("Independent original CODE.1 constructors failed");
        std::cout<<"RLOFT_STAGED_BOOT_HANDOFF=HOST_ORDERED_NOT_DOS_GUEST\n";
        std::cout<<"RLOFT_CODE1_CODE2_SOURCE_BOOT_PIPELINE_PASS"
                 <<" code1=CC1C_CD97_FCE8_FC98"
                 <<" code2=16D04_2E768_2E7F0_325B0_32150_29CA0"
                 <<" media=OWNER_AUTHENTICATED"
                 <<" handoff=HOST_ORDERED"
                 <<" source_vga_palette=COMPLETE"
                 <<" gameplay_boot=NOT_ACCEPTED"
                 <<" original_CPU_call_ancestry=NOT_VERIFIED\n";
        return 0;
    }catch(const std::exception& e){
        std::cerr<<"RLOFT_CODE1_CODE2_STAGED_BOOT_FAIL: "<<e.what()<<"\n";
        return 1;
    }
}
