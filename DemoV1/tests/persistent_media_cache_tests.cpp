#include "rloft/persistent_media_cache.hpp"
#include "rloft/game_source.hpp"
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;
namespace {
void save(const fs::path& path,const std::vector<std::uint8_t>& bytes) {
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    assert(out);
    out.write(reinterpret_cast<const char*>(bytes.data()),
              static_cast<std::streamsize>(bytes.size()));
    assert(out);
}
void put32(std::vector<std::uint8_t>& v,std::size_t at,std::uint32_t n) {
    for (unsigned i=0;i<4;++i) v.at(at+i)=static_cast<std::uint8_t>(n>>(i*8));
}
std::vector<std::uint8_t> fixture_levels() {
    std::vector<std::uint8_t> data(132 + 32*29,0);
    put32(data,0,32);
    for(unsigned i=0;i<32;++i){
        auto off=132+i*29;
        put32(data,4+i*4,off);
        put32(data,off+1,29);
    }
    return data;
}
void set_cache(const fs::path& path) {
#ifdef _WIN32
    _putenv_s("RLOFT_CACHE_DIR", path.string().c_str());
    _putenv_s("RLOFT_REIMPORT_ASSETS", "");
#else
    setenv("RLOFT_CACHE_DIR",path.string().c_str(),1);
    unsetenv("RLOFT_REIMPORT_ASSETS");
#endif
}
}
int main(){
    const auto id=std::chrono::steady_clock::now().time_since_epoch().count();
    const auto root=fs::temp_directory_path()/("rloft-cache-test-"+std::to_string(id));
    const auto original=root/"original", cache=root/"cache";
    fs::create_directories(original);
    set_cache(cache);
    // Exactly the authoritative boot-required source files; no original media bundled.
    for(const auto* name:{"RES0","RES1","RES2","RES3","RES4","RES5",
             "RES6","RES7","RES8","RES10","INIT","T","COLORS","SINTAB",
             "CODE.1","CODE.2","CINE00.ANM","CINE12.ANM","NPCS.DAT",
             "OBJECTS.DAT","INFO.DAT"}) save(original/name,{0,0,0,0});
    save(original/"LEVELS.DAT",fixture_levels());
    save(original/"SAVE.DAT",{3,4,5});
    save(original/"LEV022.DAT",{8,9});
    auto source=rloft::GameSource::open(original);
    assert(source.has_full_game_media());
    assert(!rloft::open_persistent_media_cache());
    auto loaded=rloft::import_persistent_media_cache(std::move(source));
    assert(loaded.has_full_game_media());
    assert(loaded.read("LEVELS.DAT")==fixture_levels());
    assert(fs::exists(cache/"v3"/"levels"/"level_31.bin"));
    assert(!fs::exists(cache/"v3"/"media"/"SAVE.DAT"));
    assert(!fs::exists(cache/"v3"/"media"/"LEV022.DAT"));
    assert(fs::exists(cache/"v3"/"decode_report.tsv"));
    auto reused=rloft::open_persistent_media_cache();
    assert(reused && reused->read("LEVELS.DAT")==fixture_levels());
    // Same-size corruption is caught by manifest hashes, not just size checks.
    save(cache/"v3"/"media"/"RES1",{0,0,0,1});
    assert(!rloft::open_persistent_media_cache());
    // Reimport repairs corruption without requiring an intermediate erase.
    auto repaired=rloft::import_persistent_media_cache(rloft::GameSource::open(original));
    assert(repaired.read("RES1")==std::vector<std::uint8_t>({0,0,0,0}));
    assert(rloft::open_persistent_media_cache());
    fs::remove(cache/"v3"/"media"/"RES1");
    assert(!rloft::open_persistent_media_cache());
    fs::remove_all(root);
    std::cout << "persistent media cache: import/reuse/repair-detection/save-isolation PASS\n";
}
