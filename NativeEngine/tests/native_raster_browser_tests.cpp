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
std::vector<std::uint8_t> archive_of(
    const std::vector<std::vector<std::uint8_t>>& entries) {
    std::vector<std::uint8_t> data(4+entries.size()*4,0);
    u32(data,0,static_cast<std::uint32_t>(entries.size()));
    for(std::size_t i=0;i<entries.size();++i) {
        u32(data,4+i*4,static_cast<std::uint32_t>(data.size()));
        data.insert(data.end(),entries[i].begin(),entries[i].end());
    }
    return data;
}
std::vector<std::uint8_t> texture6() {
    std::vector<std::uint8_t> out{'E','H',6,0,0,0,16};
    for(unsigned i=0;i<4096;i+=8) {
        out.push_back(0xff);
        for(unsigned j=0;j<8;++j)
            out.push_back(std::uint8_t(((i+j)/128*7+(i+j)%128*3)&255U));
    }
    return out;
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
        write(root/"RES4",archive_of({{0,3},texture6(),texture6()}));
        write(root/"RES9",archive_of({raster(32),{1,2,3}}));
        write(root/"RES3",archive_of({{'E','H',7,0,0}}));
        write(root/"RES5",archive_of({{'E','H',3,0,0}}));
        write(root/"RES6",archive_of({{'E','H',4,0,0}}));
        write(root/"RES10",archive_of({{'E','H',4,0,0}}));
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
        auto tex=load_native_raster_record(source,"RES4",1);
        assert(tex.resource_type==6 && tex.raster.width==32 &&
               tex.raster.height==128 && tex.raster.frames==1);
        assert(tex.raster.pixels[0]==0);
        assert(tex.raster.pixels[32]==3);
        assert(tex.raster.pixels[1]==7);
        auto forward_tex=seek_native_raster_record(source,"RES4",1,true);
        assert(forward_tex.resource_type==6 && forward_tex.record_index==2);
        auto back_tex=seek_native_raster_record(source,"RES4",2,false);
        assert(back_tex.resource_type==6 && back_tex.record_index==1);
        auto ninth=load_native_raster_record(source,"RES9",0);
        assert(ninth.resource_type==1 && ninth.raster.pixels[0]==32);
        for(const auto* category:{"RES3","RES5","RES6","RES10"})
            rejects([&] {static_cast<void>(
                load_native_raster_record(source,category,0));});
        rejects([&] {static_cast<void>(
            seek_native_raster_record(source,"RES5",0,true));});
                assert(fs::file_size(root/"RES0")==archive.size());
    } catch (...) {
        fs::remove_all(root);throw;
    }
    fs::remove_all(root);
}
}
int main(){verify();}
