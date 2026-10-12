#include "rloft_native/native_assets.hpp"
#include "rloft_native/native_media.hpp"
#include "rloft_native/native_sha256.hpp"
#include "rloft_native/native_type7_scene_shape.hpp"

#include <exception>
#include <filesystem>
#include <iostream>

namespace {
int run(const std::filesystem::path& original_install) {
    try {
        const auto media=rloft_native::NativeMedia::open(original_install);
        const auto init=media.read("INIT");
        const auto original_res3=media.read("RES3");
        const rloft_native::NativeResArchive res3(original_res3);
        const auto corpus=rloft_native::audit_native_res3_scene_corpus(
            init,res3);
        // Read-only metadata; this does not export 4,234 decoded shapes
        // or the original compressed bytes. The hashes pin the inputs.
        auto json=rloft_native::native_res3_scene_corpus_json(corpus);
        if(json.size()<2 || json[json.size()-2]!='}')
            throw std::runtime_error("native RES3 scene audit JSON malformed");
        json.pop_back(); // newline
        json.pop_back(); // closing brace
        json+=",\"original_init_sha256\":\""+
              rloft_native::native_sha256_hex(init)+"\"";
        json+=",\"original_res3_sha256\":\""+
              rloft_native::native_sha256_hex(original_res3)+"\"}\n";
        std::cout<<json;
        return 0;
    }catch(const std::exception& error) {
        std::cerr<<"Native RES3 scene corpus FAILED: "
                 <<error.what()<<"\n";
        return 3;
    }
}
}
#ifdef _WIN32
int wmain(int argc,wchar_t** argv) {
    if(argc!=2) {
        std::cerr<<"Usage: rloft_native_res3_scene_audit <original install folder or game.gog>\n";
        return 64;
    }
    return run(std::filesystem::path(argv[1]));
}
#else
int main(int argc,char** argv) {
    if(argc!=2) {
        std::cerr<<"Usage: rloft_native_res3_scene_audit <original install folder>\n";
        return 64;
    }
    return run(std::filesystem::path(argv[1]));
}
#endif
