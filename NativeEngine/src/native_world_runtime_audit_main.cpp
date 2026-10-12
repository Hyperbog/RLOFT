#include "rloft_native/native_media.hpp"
#include "rloft_native/native_world_payload.hpp"
#include "rloft_native/native_world_runtime.hpp"

#include <exception>
#include <filesystem>
#include <iostream>

namespace {
int run(const std::filesystem::path& original_install) {
    try {
        const auto media=rloft_native::NativeMedia::open(original_install);
        auto world=rloft_native::NativeWorldRuntime(
            rloft_native::load_native_world_payload(media));
        // Audit all 32 source states. A scene-state report is not original
        // gameplay execution, nor is this a user-facing renderer.
        for(unsigned level=0;level<32;++level) {
            world.select_level(level);
            const auto& source=world.original().level(level);
            for(unsigned row=0;row<source.height;++row)
                for(unsigned col=0;col<source.width;++col)
                    static_cast<void>(world.cell(level,col,row));
        }
        std::cout<<world.evidence_json();
        return 0;
    } catch(const std::exception& e) {
        std::cerr<<"Native world runtime evidence FAILED: "<<e.what()<<"\n";
        return 3;
    }
}
}
#ifdef _WIN32
int wmain(int argc,wchar_t** argv) {
    if(argc!=2) {
        std::cerr<<"Usage: rloft_native_world_runtime_audit <original install directory or game.gog>\n";
        return 64;
    }
    return run(std::filesystem::path(argv[1]));
}
#else
int main(int argc,char** argv) {
    if(argc!=2) {
        std::cerr<<"Usage: rloft_native_world_runtime_audit <original install directory>\n";
        return 64;
    }
    return run(std::filesystem::path(argv[1]));
}
#endif
