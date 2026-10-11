#include "rloft_native/native_world_system.hpp"

#include <exception>
#include <filesystem>
#include <iostream>
#include <string>

namespace {
int audit_world(const std::filesystem::path& original_install) {
    try {
        const auto media=rloft_native::NativeMedia::open(original_install);
        const auto world=rloft_native::load_native_world_system(media);
        std::cout<<rloft_native::native_world_system_json(world);
        return 0;
    } catch(const std::exception& error) {
        std::cerr<<"RLOFT independent world-content audit FAILED: "
                 <<error.what()<<"\n";
        return 3;
    }
}
}
#ifdef _WIN32
int wmain(int argc,wchar_t** argv) {
    if(argc!=2) {
        std::cerr<<"Usage: rloft_native_world_audit <original game folder or game.gog>\n";
        return 64;
    }
    return audit_world(std::filesystem::path(argv[1]));
}
#else
int main(int argc,char** argv) {
    if(argc!=2) {
        std::cerr<<"Usage: rloft_native_world_audit <original game source>\n";
        return 64;
    }
    return audit_world(std::filesystem::path(argv[1]));
}
#endif
