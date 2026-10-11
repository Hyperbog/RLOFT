#include "rloft_native/native_media.hpp"
#include "rloft_native/native_assets.hpp"
#include <cassert>
#include <array>
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
constexpr unsigned kSector=2048;
void set32(std::vector<std::uint8_t>& x, std::size_t at, std::uint32_t n) {
    x.at(at)=std::uint8_t(n); x.at(at+1)=std::uint8_t(n>>8);
    x.at(at+2)=std::uint8_t(n>>16); x.at(at+3)=std::uint8_t(n>>24);
}
void set32be(std::vector<std::uint8_t>& x, std::size_t at, std::uint32_t n) {
    x.at(at)=std::uint8_t(n>>24); x.at(at+1)=std::uint8_t(n>>16);
    x.at(at+2)=std::uint8_t(n>>8); x.at(at+3)=std::uint8_t(n);
}
std::vector<std::uint8_t> mkres() {
    // Two records at offsets 12 and 15; no copyrighted data.
    std::vector<std::uint8_t> r(17,0);
    set32(r,0,2); set32(r,4,12); set32(r,8,15);
    r[12]=4; r[13]=7; r[14]=9; r[15]=2; r[16]=3;
    return r;
}
std::vector<std::uint8_t> mkcolors() {
    std::vector<std::uint8_t> p(1+15*768,0);
    p[0]=15; p[1+12*768+3]=63;
    return p;
}
void write(const fs::path& path, const std::vector<std::uint8_t>& bytes) {
    fs::create_directories(path.parent_path());
    std::ofstream out(path,std::ios::binary|std::ios::trunc);
    if(!out)throw std::runtime_error("fixture cannot open");
    if(!bytes.empty())out.write(reinterpret_cast<const char*>(bytes.data()),
                                static_cast<std::streamsize>(bytes.size()));
    if(!out)throw std::runtime_error("fixture cannot write");
}
template<typename Fn> void rejects(Fn fn) {
    bool failed=false;
    try{fn();}catch(const std::exception&){failed=true;}
    assert(failed);
}
std::vector<std::uint8_t> rec(const std::string& name,
                              std::uint32_t lba,std::uint32_t len,bool directory) {
    // ISO9660 both-endian record encoding with correct even padding.
    const auto n=name.size();
    std::vector<std::uint8_t> x(33+n+(n%2==0?1:0),0);
    x[0]=std::uint8_t(x.size());
    set32(x,2,lba);set32be(x,6,lba);
    set32(x,10,len);set32be(x,14,len);
    x[25]=directory?2:0;x[28]=1;x[31]=1;
    x[32]=std::uint8_t(n);
    std::copy(name.begin(),name.end(),x.begin()+33);
    return x;
}
void emit_iso(const fs::path& path,int physical,unsigned dataOffset) {
    const auto r=mkres(),colors=mkcolors();
    std::vector<std::vector<std::uint8_t>> sectors(
        64,std::vector<std::uint8_t>(kSector,0));
    const auto root=rec(std::string(1,char(0)),20,2048,true);
    sectors[16][0]=1;
    for(int i=0;i<5;++i)sectors[16][1+i]="CD001"[i];
    sectors[16][6]=1;
    std::copy(root.begin(),root.end(),sectors[16].begin()+156);
    sectors[17][0]=255;
    for(int i=0;i<5;++i)sectors[17][1+i]="CD001"[i];
    sectors[17][6]=1;
    std::vector<std::uint8_t> dir;
    auto append=[&](const std::vector<std::uint8_t>& record) {
        dir.insert(dir.end(),record.begin(),record.end());
    };
    append(root);
    append(rec(std::string(1,char(1)),20,2048,true));
    append(rec("RES0;1",21,std::uint32_t(r.size()),false));
    append(rec("INIT;1",22,2,false));
    append(rec("T;1",23,3,false));
    append(rec("COLORS;1",24,std::uint32_t(colors.size()),false));
    assert(dir.size()<2048);
    std::copy(dir.begin(),dir.end(),sectors[20].begin());
    std::copy(r.begin(),r.end(),sectors[21].begin());
    sectors[22][0]=0;sectors[22][1]=0;
    sectors[23][0]=1;sectors[23][1]=2;sectors[23][2]=3;
    for(std::size_t i=0;i<colors.size();++i)
        sectors[24+i/kSector][i%kSector]=colors[i];
    std::ofstream out(path,std::ios::binary|std::ios::trunc);
    for(const auto& s:sectors) {
        std::vector<std::uint8_t> packed(physical,0);
        std::copy(s.begin(),s.end(),packed.begin()+dataOffset);
        out.write(reinterpret_cast<const char*>(packed.data()),
                  static_cast<std::streamsize>(packed.size()));
    }
    if(!out)throw std::runtime_error("fixture ISO write error");
}
void check(NativeMedia& media) {
    assert(media.contains("RES0"));
    assert(media.contains("res0"));
    assert(media.contains("COLORS"));
    assert(media.read("RES0")==mkres());
    assert(media.read("colors")==mkcolors());
    const NativeResArchive r(media.read("RES0"));
    assert(r.entry_count()==2);
    assert((r.entry(0)==std::vector<std::uint8_t>{4,7,9}));
    assert((r.entry(1)==std::vector<std::uint8_t>{2,3}));
    rejects([&] {static_cast<void>(r.entry(2));});
    const auto palette=decode_native_palette(media.read("COLORS"),12);
    assert(palette.rgb[3]==63);
    assert(palette.rgb[0]==0);
    const auto audit=inspect_native_assets(media);
    assert(audit.indexed_files>=4);
    assert(audit.valid>=4);
    assert(audit.invalid==0);
    assert(audit.checks.size()==19);
    rejects([&]{static_cast<void>(media.read("../RES0"));});
    rejects([&]{static_cast<void>(media.read("missing"));});
}
void exercise() {
    rejects([]{static_cast<void>(native_asset_key("../CHARS"));});
    rejects([]{static_cast<void>(native_asset_key("A//B"));});
    rejects([]{static_cast<void>(native_asset_key("C:/RES0"));});
    assert(native_asset_key("res0")=="RES0");
    assert(native_asset_key("rloft\\res0")=="RLOFT/RES0");
    const auto root=fs::temp_directory_path()/
        ("rloft-native-media-"+std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count()));
    fs::create_directories(root);
    try {
        const auto loose=root/"loose";
        fs::create_directory(loose);
        write(loose/"res0",mkres());
        write(loose/"INIT",{0,0});
        write(loose/"T",{1,2,3});
        write(loose/"COLORS",mkcolors());
        auto source=NativeMedia::open(loose);
        assert(source.kind()==NativeMediaKind::LooseDirectory);
        const auto before=fs::file_size(loose/"res0");
        check(source);
        assert(fs::file_size(loose/"res0")==before);
        for(const auto test : std::array<std::pair<int,unsigned>,3>{{
            {2048,0}, {2352,16}, {2352,24}}}) {
            const auto img=root/("sector"+std::to_string(test.first)+
                                 "-"+std::to_string(test.second)+".gog");
            emit_iso(img,test.first,test.second);
            const auto original_size=fs::file_size(img);
            auto iso=NativeMedia::open(img);
            check(iso);
            assert(fs::file_size(img)==original_size);
            if(test.first==2048)assert(iso.kind()==NativeMediaKind::GogIso2048);
            else if(test.second==16)assert(iso.kind()==NativeMediaKind::GogIso2352_16);
            else assert(iso.kind()==NativeMediaKind::GogIso2352_24);
        }
        std::vector<std::uint8_t> bad(16,0);
        set32(bad,0,3);
        rejects([&]{NativeResArchive fail(bad);});
        bad=mkres();set32(bad,8,1000);
        rejects([&]{NativeResArchive fail(bad);});
        auto palette=mkcolors();palette[1+12*768+3]=64;
        rejects([&]{static_cast<void>(decode_native_palette(palette,12));});
        palette.pop_back();
        rejects([&]{static_cast<void>(decode_native_palette(palette,12));});
        fs::remove(loose/"T");
        rejects([&]{static_cast<void>(NativeMedia::open(loose));});
    } catch (...) { fs::remove_all(root); throw; }
    fs::remove_all(root);
}
}
int main() {exercise();}
