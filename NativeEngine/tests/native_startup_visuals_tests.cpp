#include "rloft_native/native_startup_visuals.hpp"
#include "rloft_native/native_media.hpp"
#include "rloft/startup_presenter.hpp"
#include "rloft/character_creation_presenter.hpp"
#include "rloft/character_transfer.hpp"
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
        // The CODE.1 palette-remap tree needs a deterministic distinct
        // source palette, rather than 256 duplicate RGB triples.
        for(unsigned index=0;index<256;++index) {
            const auto at=1U+8U*768U+index*3U;
            colors[at+0U]=std::uint8_t(index&63U);
            colors[at+1U]=std::uint8_t(index>>6U);
            colors[at+2U]=0;
        }
        file(root/"RES0",record);
        file(root/"RES1",record);
        file(root/"RES7",record);
        file(root/"RES8",record);
        auto code1=std::vector<std::uint8_t>(0x43D1BU,0);
        const auto base=0x3E724U;
        code1[base+0x4B8U]=0x78U;
        code1[base+0x4B9U]=0x56U;
        code1[base+0x4BAU]=0x34U;
        code1[base+0x4BBU]=0x12U;
        code1[base+0x70U]='R';code1[base+0x71U]='L';
        // Independent synthetic fixture: source-equipment table uses the
        // original 15 x 6 x 14 word shape, with all unused IDs set to FFFF.
        for(std::size_t k=0;k<15U*0xA8U;++k)
            code1[0x3E724U+0x0988U+k]=0xFFU;
        file(root/"CODE.1",code1);
        auto objects=std::vector<std::uint8_t>(4U+65U,0);
        objects[0]=1U; // one genuine-layout OBJECTS.DAT record
        file(root/"OBJECTS.DAT",objects);
        file(root/"COLORS",colors);
        file(root/"INIT",{0U,0U});
        file(root/"T",{1U,2U,3U});
        const auto before=fs::file_size(root/"RES0");
        // The reader must not dangle after a temporary media index dies.
        rloft_native::NativeStartupVisualSource images(
            rloft_native::NativeMedia::open(root));
        auto sprite=images.res0_type1(104);
        assert(sprite.count==1&&sprite.width==8&&sprite.height==1);
        assert(sprite.pixels.front()==104U);
        assert(images.colors_palette(14).rgb.front()==14U);
        const rloft::CharacterVisualSource& neutral=images;
        assert(neutral.type1(7,104).pixels.front()==104U);
        assert(images.type1(8,2).pixels.front()==2U);
        assert(images.type1(1,3).pixels.front()==3U);
        assert(images.code1_u8(0x4B8U)==0x78U);
        assert(images.code1_u16(0x4B8U)==0x5678U);
        assert(images.code1_s32(0x4B8U)==0x12345678);
        assert(images.code1_cstring(0x70U)=="RL");
        const auto tables=rloft::CharacterTransferTables::from_original_bytes(
            code1,objects);
        assert(!tables.starting_loadout(0,0).has_shipped_items());
        std::array<rloft::CharacterTransferActor,2> fixture_actors{};
        for(int i=0;i<2;++i) {
            auto& a=fixture_actors[std::size_t(i)];
            a.draft.actor=i;
            a.draft.sex=0;a.draft.race=0;
            a.draft.classes[0]=0;
            a.draft.option=0;a.draft.face=i;
            a.draft.name=i==0?"A":"B";
            a.stats.abilities.fill(10);
            a.stats.levels[0]=1;
            a.stats.hit_points=8;
        }
        const auto chars=rloft::build_character_transfer(fixture_actors,tables);
        assert(chars.size()==0x920U);
        assert(chars[0]=='A'&&chars[0x248U]=='B');
        assert(chars[0x12U]==0&&chars[0x248U+0x12U]==1);
        bool denied=false;
        try {static_cast<void>(images.type1(2,0));}
        catch(const std::out_of_range&){denied=true;}
        assert(denied);
        denied=false;
        try {static_cast<void>(images.code1_s32(0x43D1BU));}
        catch(const std::out_of_range&){denied=true;}
        assert(denied);
        rloft::StartupPresenter presenter(images);
        presenter.first_logo();
        assert(presenter.framebuffer().pixels().at(143U*320U+40U)==94U);
        presenter.menu_animation_frame(14);
        assert(presenter.framebuffer().pixels().at(7U*320U+85U)==98U);
        presenter.menu_frame(2);
        assert(presenter.framebuffer().pixels().at(302U*320U+39U)==104U);
        // Same shared CODE.1 presenter linked by DemoV1 and NativeEngine:
        // synthetic original-format RES7 records are sufficient to verify
        // the complete 70-step indexed opening without proprietary assets.
        rloft::CharacterCreationPresenter character(images);
        character.opening_frame(0);
        assert(character.framebuffer().pixels().size()==320U*400U);
        character.opening_frame(69);
        assert(character.framebuffer().pixels().size()==320U*400U);
        bool bad_frame=false;
        try{character.opening_frame(70);}
        catch(const std::out_of_range&){bad_frame=true;}
        assert(bad_frame);
        assert(fs::file_size(root/"RES0")==before);
        bool rejected=false;
        try{static_cast<void>(images.res0_type1(106));}
        catch(const std::out_of_range&){rejected=true;}
        assert(rejected);
    }catch(...){fs::remove_all(root);throw;}
    fs::remove_all(root);
}
