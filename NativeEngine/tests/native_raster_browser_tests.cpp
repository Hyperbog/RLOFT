#include "rloft_native/native_raster_browser.hpp"

#include <cassert>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;
using namespace rloft_native;
namespace {

void u32(std::vector<std::uint8_t>& b, std::size_t at, std::uint32_t value) {
    for (unsigned i=0; i<4; ++i) b.at(at+i)=std::uint8_t(value>>(8*i));
}
void write(const fs::path& path, const std::vector<std::uint8_t>& content) {
    std::ofstream out(path,std::ios::binary);
    if (!out) throw std::runtime_error("fixture cannot create file");
    out.write(reinterpret_cast<const char*>(content.data()),
              static_cast<std::streamsize>(content.size()));
    if (!out) throw std::runtime_error("fixture cannot finish file");
}
std::vector<std::uint8_t> raster(std::uint8_t index) {
    std::vector<std::uint8_t> raw{'E','H',1,8,0,1,0,1,0,1,255,0,0};
    for(unsigned i=0;i<8;++i) raw.push_back(index);
    return raw;
}
template<class Fn> void rejects(Fn fn) {
    bool thrown=false;
    try{fn();}catch(const std::exception&){thrown=true;}
    assert(thrown);
}
void verify() {
    std::vector<std::vector<std::uint8_t>> entries{
        {0x10,0x20}, raster(9), {'E','H',4,0}, raster(23),
        {'E','H',1,0} // candidate EH1 with corrupt header must fail closed
    };
    constexpr std::size_t count=5;
    std::vector<std::uint8_t> archive(4+4*count,0);
    u32(archive,0,count);
    for(std::size_t i=0;i<count;++i) {
        u32(archive,4+i*4,static_cast<std::uint32_t>(archive.size()));
        archive.insert(archive.end(),entries[i].begin(),entries[i].end());
    }
    const auto root=fs::temp_directory_path() /
        ("rloft-browse-"+std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count()));
    fs::create_directory(root);
    try {
        write(root/"INIT",{0,0});
        write(root/"T",{1});
        write(root/"RES0",archive);
        const auto source=NativeMedia::open(root);
        const auto one=load_native_raster_record(source,"res0",1);
        assert(one.archive=="RES0" && one.record_index==1);
        assert(one.raster.width==8 && one.raster.height==1);
        for(auto pixel:one.raster.pixels) assert(pixel==9);
        const auto two=seek_native_raster_record(source,"RES0",1,true);
        assert(two.record_index==3 && two.raster.pixels[0]==23);
        const auto before=seek_native_raster_record(source,"RES0",3,false);
        assert(before.record_index==1 && before.raster.pixels[0]==9);
        const auto first=seek_native_raster_record(source,"RES0",0,true);
        assert(first.record_index==1);
        rejects([&] {static_cast<void>(load_native_raster_record(source,"RES0",0));});
        rejects([&] {static_cast<void>(load_native_raster_record(source,"RES0",4));});
        rejects([&] {static_cast<void>(seek_native_raster_record(source,"RES0",3,true));});
        rejects([&] {static_cast<void>(seek_native_raster_record(source,"RES0",1,false));});
        rejects([&] {static_cast<void>(load_native_raster_record(source,"RES0",5));});
        rejects([&] {static_cast<void>(load_native_raster_record(source,"../RES0",1));});
        rejects([&] {static_cast<void>(load_native_raster_record(source,"COLORS",1));});
        assert(fs::file_size(root/"RES0")==archive.size());
    } catch (...) {
        fs::remove_all(root);throw;
    }
    fs::remove_all(root);
}
}
int main(){verify();}
