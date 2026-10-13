#include "rloft_native/native_startup_visuals.hpp"
#include "rloft_native/native_media.hpp"
#include "rloft/startup_presenter.hpp"
#include <cassert>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
namespace {
namespace fs=std::filesystem;
void put32(std::vector<std::uint8_t>& b,std::size_t at,std::uint32_t v) {
    for(unsigned k=0;k<4;++k)b.at(at+k)=std::uint8_t(v>>(8U*k));
}
void file(const fs::path& at,const std::vector<std::uint8_t>& bytes) {
    std::ofstream out(at,std::ios::binary|std::ios::trunc);
    if(!out)throw std::runtime_error("unable to create source-only fixture");
    if(!bytes.empty())
        out.write(reinterpret_cast<const char*>(bytes.data()),
                  std::streamsize(bytes.size()));
    if(!out)throw std::runtime_error("unable to write source-only fixture");
}
std::vector<std::uint8_t> res0() {
    constexpr unsigned count=106U,record_size=21U;
    const auto table=4U+count*4U;
    std::vector<std::uint8_t> out(table+count*record_size,0);
    put32(out,0,count);
    for(unsigned i=0;i<count;++i) {
        const auto at=table+i*record_size;
        put32(out,4U+i*4U,at);
        out[at+0]='E';out[at+1]='H';out[at+2]=1;
        out[at+3]=8;out[at+4]=0; // 8-byte type1 descriptor
        out[at+5]=1;out[at+6]=0; // 1 image frame
        out[at+7]=1;out[at+8]=0; // one row
        out[at+9]=1; // original 8-pixel width unit
        out[at+10]=0xFF; // no transparent pixel
        out[at+11]=0;out[at+12]=0; // raw, no transform
        for(unsigned p=0;p<8;++p)out[at+13U+p]=std::uint8_t(i);
    }
    return out;
}
}
int main() {
    const auto root=fs::temp_directory_path()/
        ("rloft-original-visuals-"+std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count()));
    fs::create_directories(root);
    try {
        const auto record=res0();
        auto colors=std::vector<std::uint8_t>(1U+15U*768U,0);
        colors[0]=15;
        for(unsigned bank=0;bank<15;++bank)
            for(unsigned i=0;i<768;++i)
                colors[1U+bank*768U+i]=std::uint8_t(bank);
        file(root/"RES0",record);
        file(root/"COLORS",colors);
        file(root/"INIT",{0U,0U});
        file(root/"T",{1U,2U,3U});
        const auto media=rloft_native::NativeMedia::open(root);
        const auto before=fs::file_size(root/"RES0");
        rloft_native::NativeStartupVisualSource images(media);
        auto sprite=images.res0_type1(104);
        assert(sprite.count==1&&sprite.width==8&&sprite.height==1);
        assert(sprite.pixels.front()==104U);
        assert(images.colors_palette(14).rgb.front()==14U);
        rloft::StartupPresenter presenter(images);
        presenter.first_logo();
        assert(presenter.framebuffer().pixels().at(143U*320U+40U)==94U);
        presenter.menu_animation_frame(14);
        assert(presenter.framebuffer().pixels().at(7U*320U+85U)==98U);
        presenter.menu_frame(2);
        assert(presenter.framebuffer().pixels().at(302U*320U+39U)==104U);
        assert(fs::file_size(root/"RES0")==before);
        bool rejected=false;
        try{static_cast<void>(images.res0_type1(106));}
        catch(const std::out_of_range&){rejected=true;}
        assert(rejected);
    }catch(...){fs::remove_all(root);throw;}
    fs::remove_all(root);
}
