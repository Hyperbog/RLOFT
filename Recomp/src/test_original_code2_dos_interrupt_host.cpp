// Media-free source-boundary contract for DOS 139AC and 30BB4 host services.
#include "game/filesystem/DosServices.hpp"
#include "game/timing/InterruptLifecycle.hpp"
#include "game/core/GuestCallbacks.hpp"
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
void require(bool ok,const char* reason) {
    if(!ok)throw std::runtime_error(reason);
}
}
int main() {
    try {
        using rloft::host_dos_services::CinematicPath;
        using rloft::host_dos_services::validate_cinematic;
        using rloft::host_dos_services::canonical_basename;
        require(validate_cinematic("D:\\CINE00.ANM","D:")==CinematicPath::Valid,
                "Original 139AC CINE00 path rejected");
        require(validate_cinematic("C:\\CINE99.ANM","C:")==CinematicPath::Valid,
                "Original 139AC CINE99 path rejected");
        require(validate_cinematic("D:/CINE00.ANM","D:")==CinematicPath::PrefixMismatch,
                "DOS separator mismatch accepted");
        require(validate_cinematic("E:\\CINE00.ANM","D:")==CinematicPath::PrefixMismatch,
                "Unauthorized CD prefix accepted");
        require(validate_cinematic("D:\\CINEA0.ANM","D:")==CinematicPath::FilenameMismatch,
                "Invalid original CINE ID accepted");
        require(validate_cinematic("D:\\CINE00.EXE","D:")==CinematicPath::FilenameMismatch,
                "Unexpected CINE file extension accepted");
        require(validate_cinematic("D:\\CINE00.ANM\\..","D:")!=CinematicPath::Valid,
                "Traversable cinematic path accepted");
        require(canonical_basename("COLORS.",0xCFCu)=="COLORS",
                "Original COLORS. guest alias lost");
        require(canonical_basename("COLORS.",0xCFAu)=="COLORS." &&
                canonical_basename("SOUND.CFG",0xCFCu)=="SOUND.CFG",
                "DOS path remapped outside original source call");

        using namespace rloft::host_interrupts;
        rloft::guest_callbacks::VectorTable ivt{};
        ivt[3u]={0x12345678u,0x11u};
        seed_source_fixture(ivt);
        require(ivt[9u].offset==0x9009u && ivt[8u].offset==0x9008u &&
                ivt[0u].offset==0x9000u && ivt[9u].selector==0x20u &&
                ivt[8u].selector==0x20u && ivt[0u].selector==0x20u &&
                ivt[3u].offset==0x12345678u,
                "Original IRQ sentinel vector fixture changed");
        require(!source_irq9_installed(ivt,0x17u) &&
                !source_vectors_installed(ivt,0x17u),
                "Uninstalled interrupt handlers accepted");
        rloft::guest_callbacks::set_vector(ivt,9u,0x308F8u,0x17u);
        require(source_irq9_installed(ivt,0x17u) &&
                !source_vectors_installed(ivt,0x17u),
                "IRQ9 source ownership incorrectly certifies IRQ8");
        rloft::guest_callbacks::set_vector(ivt,8u,0x30708u,0x17u);
        rloft::guest_callbacks::set_vector(ivt,0u,0x30B54u,0x17u);
        require(source_vectors_installed(ivt,0x17u) &&
                !source_vectors_installed(ivt,0x24u),
                "Original 30BB4 source vector contract mismatch");
        ivt[0u].offset=0u;
        require(!source_vectors_installed(ivt,0x17u),
                "Missing original divide-error handler accepted");
        std::cout<<"RLOFT_DOS_INTERRUPT_LIFECYCLE_HOST_ABI_PASS\n";
        return 0;
    }catch(const std::exception& e) {
        std::cerr<<"RLOFT_DOS_INTERRUPT_LIFECYCLE_HOST_ABI_FAIL "
                 <<e.what()<<"\n";return 1;
    }
}
