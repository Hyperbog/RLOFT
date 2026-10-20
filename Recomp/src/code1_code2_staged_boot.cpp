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

int original_code1_startup_main(int argc,char** argv);
int original_code2_resource_main(int argc,char** argv);

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
        std::cout<<"RLOFT_STAGED_BOOT_STAGE=CODE1_ORIGINAL_CC1C_CD97_CTORS\n";
        if(run_stage(&original_code1_startup_main,
                     {"rloft_code1_startup_gate",
                      "--extender-host-resize-to-cd97-constructors",code1})!=0)
            throw std::runtime_error("Original CODE.1 source runtime boundary failed");
        // A guest CPU transfer from original CODE.1 main to CODE.2 is
        // not reconstructed. Fail closed at the seam as a HOST-ORDERED
        // transition; this is not continuous original execution.
        std::cout<<"RLOFT_STAGED_BOOT_HANDOFF=HOST_ORDERED_NOT_DOS_GUEST\n";
        std::cout<<"RLOFT_STAGED_BOOT_STAGE=CODE2_ORIGINAL_INIT_INFO_11_RES_PALETTE\n";
        if(run_stage(&original_code2_resource_main,
                     {"rloft_original_code2_init_resources","--root",
                      root.string(),"--cd-root",cd.string(),
                      "--palette-level","5"})!=0)
            throw std::runtime_error("Original CODE.2 source INIT/RES/palette checkpoint failed");
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
