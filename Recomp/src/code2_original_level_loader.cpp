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
#include <map>
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
    u32 standalone_level=0xFFFFFFFFu;
    bool failed=false;
    unsigned opens=0,closes=0,reads=0,rays=0,materialized=0,code_patches=0;
    static Session* active;
    static Session& now() noexcept {return *active;}
    explicit Session(const std::filesystem::path& root,bool authentic_camera=false):files(root) {
        VERIFY(active==nullptr);
        active=this;
        machine.data=guest.data();
        constexpr char name[]="LEVELS.DAT";
        std::copy_n(name,sizeof(name),guest.data()+0x170A0u);
        // -1 is the original "no prior level" sentinel. The loader will
        // attempt LEVnnn.DAT and fall back to the archive's index.
        code2::store_le32(guest.data()+0xC6C8u,0xFFFFFFFFu);
        code2::store_le32(guest.data()+0x258B4u,0u);
        if(authentic_camera) {
            // The original CODE.2 function_20608 defaults (without argv
            // overrides) to level 0x16, tile (4,4), facing 0x10E (270).
            // Replay only these precisely decoded entry assignments.
            code2::store_le32(guest.data()+0x255D0u,0x00048000u);
            code2::store_le32(guest.data()+0x255D4u,0x00048000u);
            code2::store_le32(guest.data()+0xC658u,0x10Eu);
            constexpr char sintab[]="SINTAB";
            std::copy_n(sintab,sizeof(sintab),guest.data()+0x24u);
        }
    }
    ~Session(){active=nullptr;}
    static u32 missing_standalone(u32 name) noexcept {
        auto& s=now();
        if(s.standalone_level==0xFFFFFFFFu)return 0xFFFFFFFFu;
        if(name!=0x819Cu || s.standalone_level>=32u) {
            s.failed=true;return 0xFFFFFFFFu;
        }
        const u32 level=s.standalone_level;
        std::string filename="LEV";
        filename.push_back(static_cast<char>('0'+(level/100u)));
        filename.push_back(static_cast<char>('0'+((level/10u)%10u)));
        filename.push_back(static_cast<char>('0'+(level%10u)));
        filename+=".DAT";
        const std::string original_guest_name(
            reinterpret_cast<const char*>(s.guest.data()+name));
        if(original_guest_name!="WORK\\"+filename) {
            s.failed=true;return 0xFFFFFFFFu;
        }
        const auto opened=s.files.open(filename,0u);
        if(opened.carry){s.failed=true;return 0xFFFFFFFFu;}
        ++s.opens;
        return opened.ax;
    }
    static u32 open_archive(const u8* name) noexcept {
        auto& s=now();
        if(s.standalone_level!=0xFFFFFFFFu ||
           !name){s.failed=true;return 0xFFFFFFFFu;}
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
    static void read_void(u32 dst,u32 size,u32 handle) noexcept {
        (void)read_guest(dst,size,handle);
    }
    static u32 open_sintab(u32 name) noexcept {
        auto& s=now();
        if(name!=0x24u || std::string(reinterpret_cast<char*>(
                s.guest.data()+name))!="SINTAB") {
            s.failed=true;return 0xFFFFFFFFu;
        }
        const auto r=s.files.open("SINTAB",0u);
        if(r.carry){s.failed=true;return 0xFFFFFFFFu;}
        ++s.opens;return r.ax;
    }
    static u32 word_read(u32 size,u32 handle) noexcept {
        if(size!=2u){now().failed=true;return 0u;}
        u8 data[2]{};
        if(read_local(data,size,handle)!=size)return 0u;
        return code2::load_le16(data);
    }
    void load_original_sintab() {
        code2::Application06Boundary sin{};
        sin.call_312a0=&open_sintab;
        sin.call_3104c=&allocate;
        sin.call_312ec=&read_void;
        sin.call_312ec_local=&word_read;
        sin.call_312dc=&close;
        code2::function_03838(machine,sin);
        VERIFY(!failed && files.open_handles()==0u);
        VERIFY(code2::load_le32(guest.data()+0xC6D8u)==256u);
        VERIFY(code2::load_le32(guest.data()+0xC738u)==127u);
        VERIFY(code2::load_le32(guest.data()+0xC730u)==508u);
        VERIFY(code2::load_le32(guest.data()+0x6090u)==324989u);
        const u32 sine=code2::load_le32(guest.data()+0xC65Cu);
        const u32 tangent=code2::load_le32(guest.data()+0xC660u);
        VERIFY(code2::load_le32(guest.data()+sine+90u*4u)==0xFFFFu);
        VERIFY(code2::load_le32(guest.data()+tangent+450u*4u)==0x10000u);
        VERIFY(code2::load_le32(guest.data()+0xC73Cu)==359u);
        VERIFY(code2::load_le32(guest.data()+0xC73Cu+128u*4u)==0u);
        std::cout<<"CODE2_03838_AUTHENTIC_SINTAB_SOURCE_LOADER_PASS\\n";
    }
    static void patch_code(u32 addr,u8 op) noexcept {
        auto& s=now();
        if((addr!=0x32C1Au && addr!=0x32D04u) ||
           (op!=0x8Du && op!=0x8Eu))s.failed=true;
        ++s.code_patches;
    }
    static u32 depth() noexcept {return code2::function_35730(now().machine);}
    static u32 optional_left(u32 index) noexcept {
        code2::Application43Boundary b{};
        return code2::function_1e9a0(now().machine,b,index);
    }
    static u32 optional_right(u32 index) noexcept {
        code2::Application43Boundary b{};
        return code2::function_1e930(now().machine,b,index);
    }
    static void blank_record() noexcept {
        code2::function_32f4c(now().machine);
    }
    static code2::Application86Boundary walls() noexcept {
        code2::Application86Boundary b{};
        b.call_35730=&depth;
        b.call_1e9a0=&optional_left;
        return b;
    }
    static void left_wall() noexcept {
        auto b=walls();
        code2::function_32f8c(now().machine,b);
        ++now().materialized;
    }
    static void right_wall() noexcept {
        auto b=walls();
        code2::function_33124(now().machine,b);
        ++now().materialized;
    }
    static void left_companion() noexcept {
        auto b=walls();
        code2::function_332b4(now().machine,b);
        ++now().materialized;
    }
    static void right_companion() noexcept {
        auto b=walls();
        code2::function_333bc(now().machine,b);
        ++now().materialized;
    }
    static void original_ray_sweep() noexcept {
        auto& s=now();
        ++s.rays;
        code2::Application84Boundary b{};
        b.code_store8=&patch_code;
        b.call_356d8=&code2::function_356d8;
        b.call_32f4c=&blank_record;
        b.call_32f8c=&left_wall;
        b.call_33124=&right_wall;
        b.call_332b4=&left_companion;
        b.call_333bc=&right_companion;
        b.call_1e930=&optional_right;
        code2::function_32954(s.machine,b);
    }

    void run(const rloft::original_levels::LevelGrid& grid,bool raycast=false,
             const std::vector<u8>* standalone=nullptr) {
        if(standalone) {
            VERIFY(!raycast && grid.level<32u);
            standalone_level=grid.level;
            // Original DATA contains WORK\\LEV000.DAT at 819C. Source 181B4
            // mutates only the three filename digits at 81A4..81A6.
            // Verify the original generated guest path before mapping it to
            // our read-only, allowlisted host SAVE10 directory.
            constexpr char direct_name[]="WORK\\LEV000.DAT";
            std::copy_n(direct_name,sizeof(direct_name),
                        guest.begin()+0x819Cu);
        }
        if(raycast) {
            // Both cameras are literal original-source branch assignments:
            // 20608 defaults to level 22 (4,4), while 13C88 case 7 enters
            // level 11 at (2,65). No generated/spawn-guess coordinates.
            VERIFY((grid.level==22u && grid.width==30u &&
                    grid.height==15u && (grid.cells[4u*grid.width+4u]&0x3Fu)==0u) ||
                   (grid.level==11u && grid.width==128u &&
                    grid.height==128u &&
                    (grid.cells[65u*grid.width+2u]&0x3Fu)==0u));
            load_original_sintab();
        }
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
        VERIFY(opens==(raycast?2u:1u) && closes==opens && reads>2u);
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
        if(standalone) {
            // Verify independent serialized script, explored automap and
            // annotation bytes against the actual original direct-file IO.
            const auto script_end=rloft::original_levels::le32(
                standalone->data()+1u+6u*4u);
            VERIFY(script_end>=0x514u && script_end+4u<=standalone->size());
            const u32 script=code2::load_le32(guest.data()+0xCF44u);
            VERIFY(script!=0u && std::uint64_t(script)+0x514u<=guest.size());
            VERIFY(std::equal(standalone->begin()+script_end-0x514u,
                              standalone->begin()+script_end,
                              guest.begin()+script));
            const std::size_t automap=std::size_t(script_end)+4u;
            const std::size_t cells=grid.cells.size();
            VERIFY(automap+cells+4u<=standalone->size());
            VERIFY(std::equal(standalone->begin()+automap,
                              standalone->begin()+automap+cells,
                              guest.begin()+0x1760u));
            const u32 text_bytes=rloft::original_levels::le32(
                standalone->data()+automap+cells);
            VERIFY(text_bytes<0x7FFu);
            VERIFY(automap+cells+4u+text_bytes==standalone->size());
            VERIFY(std::equal(standalone->begin()+automap+cells+4u,
                              standalone->end(),
                              guest.begin()+0x5760u));
            std::cout<<"CODE2_181B4_DIRECT_SAVE_MUTABLE_STATE_PASS level="
                     <<grid.level<<" cells="<<cells
                     <<" text_bytes="<<text_bytes<<"\\n";
        }
        // Both paths consume the same source-loaded guest grid. The new
        // path uses real 03838 SINTAB and the 20608 default camera.
        code2::Application90Boundary view{};
        if(raycast) {
            view.call_35680=&code2::function_35680;
            view.call_32954=&original_ray_sweep;
        } else {
            code2::store_le32(guest.data()+0xC658u,0u);
            code2::store_le32(guest.data()+0xC65Cu,0x40000u);
            view.call_35680=&diagnostic_fixed_mul;
            view.call_32954=&stop_before_rays;
        }
        code2::function_334c4(machine,view);
        VERIFY(!failed && rays==1u);
        if(raycast) {
            VERIFY(code_patches==512u);
            VERIFY(code2::load_le32(guest.data()+0xC6FCu)==256u);
            VERIFY(materialized>0u && materialized<=256u);
            unsigned side_a=0,side_b=0,empty_a=0,empty_b=0;
            u32 min_depth=0xFFFFFFFFu,max_depth=0u;
            std::map<u32,unsigned> scene_counts;
            std::map<u32,unsigned> source_materials;
            for(u32 slot=0u;slot<256u;++slot) {
                const u32 addr=0x15A18u+slot*0x1Cu;
                const u32 extent=code2::load_le32(guest.data()+addr);
                const u32 a=code2::load_le32(guest.data()+addr+4u);
                const u32 b=code2::load_le32(guest.data()+addr+8u);
                ++scene_counts[b];
                ++source_materials[a];
                if(a==0x93u)++empty_a; else ++side_a;
                if(b==0x93u)++empty_b; else ++side_b;
                min_depth=std::min(min_depth,extent);
                max_depth=std::max(max_depth,extent);
            }
            std::cout<<"CODE2_32954_REAL_RECORDS first_nonempty="<<side_a
                     <<" second_nonempty="<<side_b
                     <<" first_empty="<<empty_a
                     <<" second_empty="<<empty_b
                     <<" depth_min="<<min_depth<<" depth_max="<<max_depth
                     <<" unique_materials="<<scene_counts.size()<<" ids=";
            for(const auto& [key,count]:scene_counts)
                std::cout<<key<<":"<<count<<",";
            std::cout<<"\\nCODE2_32954_FIRST_PASS_MATERIALS=";
            for(const auto& [key,count]:source_materials)
                std::cout<<key<<":"<<count<<",";
            std::cout<<"\\n";
            std::cout<<"CODE2_32954_AUTHENTIC_SOURCE_RAYCAST_PASS level="<<grid.level
                     <<" rays=256 materialized="<<materialized<<"\n";
            std::cout<<"CODE2_FULL_FIRST_PERSON_RENDER_PARITY_ACCEPTED=NO\\n";
        }
        VERIFY(code2::load_le32(guest.data()+0xFD0Cu)==grid.cells.size());
        unsigned source_visited=0;
        for(std::size_t i=0;i<grid.cells.size();++i) {
            const u16 original=grid.cells[i];
            const u16 baseline=static_cast<u16>(original&~0x0040u);
            const u16 after=code2::load_le16(guest.data()+0x198E0u+i*2u);
            if(raycast) {
                // 334C4 clears transient 0040. Original 32954 then
                // sets 0440 for traversed cells, and must not touch any
                // other authored wall/material/state bits.
                VERIFY(after==baseline ||
                       after==static_cast<u16>(baseline|0x0440u));
                if(after!=baseline)++source_visited;
            } else VERIFY(after==baseline);
        }
        if(raycast) {
            VERIFY(source_visited>0u);
            std::cout<<"CODE2_32954_ORIGINAL_WORLD_VISIBILITY_PASS cells="
                     <<source_visited<<"\n";
            std::cout<<"CODE2_181B4_TO_334C4_TO_32954_PASS\\n";
        }
        std::cout<<"CODE2_181B4_TO_334C4_ORIGINAL_GUEST_GRID_PASS level="
                 <<grid.level<<"\n";
        std::cout<<(standalone?"CODE2_181B4_REAL_DOS_STANDALONE_FILE_LOAD_PASS level=":
                               "CODE2_181B4_REAL_DOS_ARCHIVE_FILE_LOAD_PASS level=")
                 <<grid.level<<" rows="<<grid.height<<" columns="<<grid.width
                 <<" cells="<<grid.cells.size()<<" reads="<<reads<<"\n";
    }
};
Session* Session::active=nullptr;
}

int main(int argc,char** argv) {
    try {
        const bool saved=argc==3 && std::string(argv[1])=="--saved-levels";
        if(saved) {
            const std::filesystem::path directory(argv[2]);
            VERIFY(std::filesystem::is_directory(directory));
            unsigned validated=0u;
            std::array<bool,32u> seen{};
            for(const auto& entry:std::filesystem::directory_iterator(directory)) {
                const auto name=entry.path().filename().string();
                if(name.size()!=10u || name.substr(0,3)!="LEV" ||
                   name.substr(6)!=".DAT")continue;
                VERIFY(name[3]>='0' && name[3]<='9' &&
                       name[4]>='0' && name[4]<='9' &&
                       name[5]>='0' && name[5]<='9');
                const u32 level=static_cast<u32>(name[3]-'0')*100u+
                                static_cast<u32>(name[4]-'0')*10u+
                                static_cast<u32>(name[5]-'0');
                VERIFY(level<32u && !seen[level] &&
                       entry.is_regular_file() && !entry.is_symlink());
                seen[level]=true;
                const auto bytes=rloft::original_levels::read_archive(
                    entry.path().string());
                const auto grid=rloft::original_levels::decode_saved_grid(bytes,level);
                auto run=std::make_unique<Session>(directory);
                run->run(grid,false,&bytes);
                ++validated;
            }
            VERIFY(validated>0u);
            std::cout<<"CODE2_181B4_SAVED_LEVELS_ORIGINAL_SOURCE_GATE_PASS count="
                     <<validated<<"\\n"
                     <<"CODE2_COMPLETE_SAVE_BOOT_AND_DOS_PIXEL_PARITY_ACCEPTED=NO\\n";
            return 0;
        }
        const bool raycast=argc==4 && std::string(argv[1])=="--raycast";
        if(argc!=2 && !raycast)throw std::invalid_argument(
            "usage: rloft_original_code2_level_loader /owned/LEVELS.DAT"
            " | --raycast /owned/LEVELS.DAT /owned/SINTAB");
        const std::filesystem::path path(argv[raycast?2:1]);
        if(raycast) {
            const std::filesystem::path sintab(argv[3]);
            VERIFY(sintab.filename()=="SINTAB");
            VERIFY(sintab.parent_path()==path.parent_path());
        }
        VERIFY(path.filename()=="LEVELS.DAT");
        const auto bytes=rloft::original_levels::read_archive(path.string());
        for(u32 level=raycast?22u:0u;level<(raycast?23u:32u);++level) {
            const auto grid=rloft::original_levels::decode_grid(bytes,level);
            auto run=std::make_unique<Session>(path.parent_path(),raycast);
            run->run(grid,raycast);
        }
        if(!raycast)std::cout<<"CODE2_181B4_ORIGINAL_32_LEVEL_FILE_GATE_PASS\n"
                 <<"CODE2_COMPLETE_WORLD_STATE_AND_GAME_BOOT_ACCEPTED=NO\n";
        return 0;
    }catch(const std::exception& e) {
        std::cerr<<"CODE2_181B4_ORIGINAL_LEVEL_FILE_GATE_FAIL: "<<e.what()<<"\n";
        return 1;
    }
}
