#include "rloft_native/original_menu_preview.hpp"
#include "rloft_native/cpu_reference_presenter.hpp"
#include <cassert>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;
namespace {
void write(const fs::path& path, const std::vector<std::uint8_t>& bytes) {
    std::ofstream out(path, std::ios::binary);
    if (!out) throw std::runtime_error("cannot create synthetic original resource");
    if (!bytes.empty())
        out.write(reinterpret_cast<const char*>(bytes.data()),
                  static_cast<std::streamsize>(bytes.size()));
    if (!out) throw std::runtime_error("synthetic file output failed");
}
void u32(std::vector<std::uint8_t>& buf, std::size_t at, std::uint32_t value) {
    for (unsigned i=0;i<4;++i) buf[at+i]=static_cast<std::uint8_t>(value>>(8*i));
}
std::vector<std::uint8_t> eh_raster(std::uint8_t color) {
    // EH type-1, 8x2 pixels; transparent key FF; uncompressed.
    std::vector<std::uint8_t> bytes{
        'E','H',1,8,0,1,0,2,0,1,255,0,0};
    bytes.insert(bytes.end(),16U,color);
    return bytes;
}
std::vector<std::uint8_t> make_res0() {
    constexpr int count=102;
    const std::size_t header=4+4*count;
    std::vector<std::uint8_t> bytes(header,0);
    u32(bytes,0,count);
    for(int i=0;i<count;++i) {
        u32(bytes,4+4*i,static_cast<std::uint32_t>(bytes.size()));
        if(i>=97&&i<=101) {
            const auto body=eh_raster(static_cast<std::uint8_t>(i-95));
            bytes.insert(bytes.end(),body.begin(),body.end());
        }
    }
    return bytes;
}
void test_source_menu() {
    const auto dir=fs::temp_directory_path() /
        ("rloft-win-menu-fixture-"+std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count()));
    fs::create_directory(dir);
    try {
        write(dir/"RES0",make_res0());
        // The inherited boot-assets class reads these archives eagerly.
        for(const auto* name:{"RES1","RES5","RES6","RES7","RES8","RES10"})
            write(dir/name,{0,0,0,0});
        write(dir/"T",{0});
        write(dir/"INIT",{0,0});
        std::vector<std::uint8_t> palette(1+15*768,0);
        palette[0]=15;
        for(int i=2;i<=6;++i) palette[1+12*768+3*i]=63;
        write(dir/"COLORS",palette);
        // Original CODE.1 class checks certified length, not this fixture's
        // contents. All bytes are synthetic and never treated as game code.
        write(dir/"CODE.1",std::vector<std::uint8_t>(0x43D1BU,0));
        const auto original=rloft::GameSource::open(dir);
        const auto frame=rloft_native::make_source_menu_preview(original);
        assert(frame.pixels.size()==320U*400U);
        assert(frame.pixels[0]==2);
        assert(frame.pixels[7*320+85]==3);
        assert(frame.pixels[79*320]==4);
        assert(frame.pixels[200*320]==5);
        assert(frame.pixels[341*320+143]==6);
        assert(frame.dac6[3*2]==63);
        rloft_native::CpuReferencePresenter cpu;
        cpu.present(frame);
        assert(cpu.rgba().size()==320U*400U*4U);
        assert(cpu.rgba()[0]==255);
        const auto copy=rloft_native::make_source_menu_preview(original);
        assert(rloft_native::frame_identity(copy).pixels_fnv1a64 ==
               rloft_native::frame_identity(frame).pixels_fnv1a64);
        // Fail closed when a required original startup archive disappears.
        fs::remove(dir/"RES1");
        bool rejected=false;
        try {
            static_cast<void>(rloft_native::make_source_menu_preview(original));
        } catch(const std::exception&) {rejected=true;}
        assert(rejected);
    } catch (...) {fs::remove_all(dir);throw;}
    fs::remove_all(dir);
}
}
int main() {test_source_menu();}
