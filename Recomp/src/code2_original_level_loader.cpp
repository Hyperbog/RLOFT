// Execute the reconstructed original CODE.2 181B4 loader against a read-only
// DOS 3Dh/42h/3Fh/3Eh host bridge. This is NOT a complete game boot.
// Usage: rloft_original_code2_level_loader /owned/LEVELS.DAT
#include "../../Code2/Code2.cpp"
#include "../../Code2/graphics/original_levels_grid.hpp"
#include "dos_readonly_files.hpp"
#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
using u8=std::uint8_t; using u16=std::uint16_t; using u32=std::uint32_t;
#define VERIFY(x) do {if(!(x))throw std::runtime_error("181B4 source loader: " #x);}while(false)
struct Session {
    std::vector<u8> guest=std::vector<u8>(8u*1024u*1024u,0u);
    code2::Machine machine{};
    rloft_recomp::ReadOnlyDosFiles files;
    u32 heap_top=0x100000u;
    bool failed=false;
    unsigned opens=0,closes=0,reads=0,rays=0;
    static Session* active;
    static Session& now() noexcept {return *active;}
    explicit Session(const std::filesystem::path& root):files(root) {
        VERIFY(active==nullptr);
        active=this;
        machine.data=guest.data();
        constexpr char name[]="LEVELS.DAT";
        std::copy_n(name,sizeof(name),guest.data()+0x170A0u);
        // -1 is the original "no prior level" sentinel. The loader will
        // attempt LEVnnn.DAT and fall back to the archive's index.
        code2::store_le32(guest.data()+0xC6C8u,0xFFFFFFFFu);
        code2::store_le32(guest.data()+0x258B4u,0u);
    }
    ~Session(){active=nullptr;}
    static u32 missing_standalone(u32) noexcept {return 0xFFFFFFFFu;}
    static u32 open_archive(const u8* name) noexcept {
        auto& s=now();
        if(!name){s.failed=true;return 0xFFFFFFFFu;}
        std::string filename;
        for(unsigned i=0u;i<12u && name[i];++i)
            filename.push_back(static_cast<char>(name[i]));
        if(filename!="LEVELS.DAT"){s.failed=true;return 0xFFFFFFFFu;}
        const auto result=s.files.open(filename,0u);
        if(result.carry){s.failed=true;return 0xFFFFFFFFu;}
        ++s.opens;
        return result.ax;
    }
    static u32 seek(u32 offset,u32 origin,u32 handle) noexcept {
        auto& s=now();
        if(origin>2u || handle>0xFFFFu ||
           offset>static_cast<u32>(std::numeric_limits<std::int32_t>::max())){
            s.failed=true;return 0xFFFFFFFFu;
        }
        const auto result=s.files.seek(static_cast<u16>(handle),
                             static_cast<std::int32_t>(offset),
                             static_cast<u8>(origin));
        if(result.carry){s.failed=true;return 0xFFFFFFFFu;}
        return result.offset();
    }
    static u32 read_local(u8* out,u32 bytes,u32 handle) noexcept {
        auto& s=now();
        if(handle>0xFFFFu || bytes>0xFFFFu || (bytes && !out)){
            s.failed=true;return 0u;
        }
        const auto result=s.files.read(static_cast<u16>(handle),out,bytes);
        if(result.carry || result.ax!=bytes){s.failed=true;return 0u;}
        ++s.reads;
        return result.ax;
    }
    static u32 read_guest(u32 offset,u32 bytes,u32 handle) noexcept {
        auto& s=now();
        if(std::uint64_t(offset)+bytes>s.guest.size()){
            s.failed=true;return 0u;
        }
        return read_local(s.guest.data()+offset,bytes,handle);
    }
    static void close(u32 handle) noexcept {
        auto& s=now();
        if(handle>0xFFFFu || s.files.close(static_cast<u16>(handle)).carry)
            s.failed=true;
        else ++s.closes;
    }
    static void fill(u32 offset,u32 value,u32 bytes) noexcept {
        auto& s=now();
        if(std::uint64_t(offset)+bytes>s.guest.size()){s.failed=true;return;}
        std::fill_n(s.guest.data()+offset,bytes,static_cast<u8>(value));
    }
    static u32 allocate(u32 bytes) noexcept {
        auto& s=now();
        if(!bytes || bytes>0x100000u ||
           std::uint64_t(s.heap_top)+bytes>s.guest.size()){
            s.failed=true;return 0u;
        }
        const u32 p=s.heap_top;
        s.heap_top=(s.heap_top+bytes+3u)&~3u;
        return p;
    }
    static void release(u32) noexcept {}
    static u32 graphic_bank() noexcept {return 0u;}
    static u32 graphic_mode() noexcept {return 1u;}
    static u32 diagnostic_fixed_mul(u32,u32) noexcept {return 0u;}
    static void stop_before_rays() noexcept {++now().rays;}

    void run(const rloft::original_levels::LevelGrid& grid) {
        code2::Application32Boundary callbacks{};
        callbacks.call_312a0=&missing_standalone;
        callbacks.call_312a0_local=&open_archive;
        callbacks.call_3131c=&seek;
        callbacks.call_312ec=&read_guest;
        callbacks.call_312ec_local=&read_local;
        callbacks.call_312dc=&close;
        callbacks.call_30d30=&fill;
        callbacks.call_3104c=&allocate;
        callbacks.call_310b0=&release;
        callbacks.call_29050=&graphic_bank;
        callbacks.call_29048=&graphic_mode;
        code2::function_181b4(machine,callbacks,static_cast<code2::s32>(grid.level));
        VERIFY(!failed);
        VERIFY(opens==1u && closes==1u && reads>2u);
        VERIFY(files.open_handles()==0u);
        VERIFY(code2::load_le32(guest.data()+0xC6C8u)==grid.level);
        // The original 181B4 post-load tail normalizes the serialized
        // scale word at 258B2 to 1 if it was zero (header offset 0x29A).
        // Check all other bytes exactly, and check that one source mutation.
        for(std::size_t i=0;i<grid.header.size();++i) {
            if(i==0x29Au || i==0x29Bu)continue;
            VERIFY(guest[0x25618u+i]==grid.header[i]);
        }
        const u16 serialized_scale=rloft::original_levels::le16(
            grid.header.data()+0x29Au);
        VERIFY(code2::load_le16(guest.data()+0x258B2u)==
               (serialized_scale ? serialized_scale : 1u));
        VERIFY(grid.cells.size()==std::size_t(grid.width)*grid.height);
        for(std::size_t i=0;i<grid.cells.size();++i)
            VERIFY(code2::load_le16(guest.data()+0x198E0u+i*2u)==grid.cells[i]);
        // Reuse the exact guest header and cells installed by source 181B4.
        // View/trig remain synthetic; stop before source 32954 ray traversal.
        code2::store_le32(guest.data()+0xC658u,0u);
        code2::store_le32(guest.data()+0xC65Cu,0x40000u);
        code2::Application90Boundary view{};
        view.call_35680=&diagnostic_fixed_mul;
        view.call_32954=&stop_before_rays;
        code2::function_334c4(machine,view);
        VERIFY(rays==1u);
        VERIFY(code2::load_le32(guest.data()+0xFD0Cu)==grid.cells.size());
        for(std::size_t i=0;i<grid.cells.size();++i)
            VERIFY(code2::load_le16(guest.data()+0x198E0u+i*2u)==
                   (grid.cells[i]&static_cast<u16>(~0x0040u)));
        std::cout<<"CODE2_181B4_TO_334C4_ORIGINAL_GUEST_GRID_PASS level="
                 <<grid.level<<"\\n";
        std::cout<<"CODE2_181B4_REAL_DOS_ARCHIVE_FILE_LOAD_PASS level="
                 <<grid.level<<" rows="<<grid.height<<" columns="<<grid.width
                 <<" cells="<<grid.cells.size()<<" reads="<<reads<<"\n";
    }
};
Session* Session::active=nullptr;
}

int main(int argc,char** argv) {
    try {
        if(argc!=2)throw std::invalid_argument(
            "usage: rloft_original_code2_level_loader /owned/LEVELS.DAT");
        const std::filesystem::path path(argv[1]);
        VERIFY(path.filename()=="LEVELS.DAT");
        const auto bytes=rloft::original_levels::read_archive(path.string());
        for(u32 level=0;level<32u;++level) {
            const auto grid=rloft::original_levels::decode_grid(bytes,level);
            auto run=std::make_unique<Session>(path.parent_path());
            run->run(grid);
        }
        std::cout<<"CODE2_181B4_ORIGINAL_32_LEVEL_FILE_GATE_PASS\n"
                 <<"CODE2_COMPLETE_WORLD_STATE_AND_GAME_BOOT_ACCEPTED=NO\n";
        return 0;
    }catch(const std::exception& e) {
        std::cerr<<"CODE2_181B4_ORIGINAL_LEVEL_FILE_GATE_FAIL: "<<e.what()<<"\n";
        return 1;
    }
}
