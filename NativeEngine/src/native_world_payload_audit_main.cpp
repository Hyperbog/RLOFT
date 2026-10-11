#include "rloft_native/native_media.hpp"
#include "rloft_native/native_world_payload.hpp"
#include "rloft_native/native_world_fingerprints.hpp"
#include <exception>
#include <filesystem>
#include <iostream>

namespace {
int run(const std::filesystem::path& original_install) {
    try {
        const auto media=rloft_native::NativeMedia::open(original_install);
        const auto levels=media.read("LEVELS.DAT");
        const auto payload=rloft_native::decode_native_world_payload(levels);
        std::cout<<rloft_native::native_world_payload_fingerprints_json(
            payload,levels);
        return 0;
    } catch(const std::exception& error) {
        std::cerr<<"RLOFT native payload fingerprint FAILED: "
                 <<error.what()<<"\n";
        return 3;
    }
}
}
#ifdef _WIN32
int wmain(int argc,wchar_t** argv) {
    if(argc!=2) {
        std::cerr<<"Usage: rloft_native_payload_audit <original install folder or game.gog>\n";
        return 64;
    }
    return run(std::filesystem::path(argv[1]));
}
#else
int main(int argc,char** argv) {
    if(argc!=2) {
        std::cerr<<"Usage: rloft_native_payload_audit <original install directory>\n";
        return 64;
    }
    return run(std::filesystem::path(argv[1]));
}
#endif
