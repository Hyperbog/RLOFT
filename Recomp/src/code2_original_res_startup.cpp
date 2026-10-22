// Source-backed startup checkpoint: CODE.2 16D04 INIT header and 2E768/2E7F0
// INFO.DAT -> eleven original RES0..RES10 archive slot tables.
// This executable is an isolated CODE.2 subsystem gate, NOT CODE.1->game boot.
// Canonical startup and resources share one compiled CODE.2 source TU.
// The original non-inline CODE.2 function definitions must NOT be linked
// in two object files (duplicate symbols and disconnected source state).
// Disable only the standalone startup CLI; this resource file retains
// its own CLI when built independently.
#define RLOFT_STARTUP_GATE_NO_MAIN
#include "code2_startup_gate.cpp"
#include "original_code2_palette_loader.hpp"
#include "original_code2_scene_palette.hpp"
#include "original_code2_guest_session.hpp"
#include "../../Code2/graphics/original_levels_grid.hpp"
#include "../../Code2/graphics/original_res_image_archive.hpp"
#include "canonical_code2_runtime_owner.hpp"
#include <memory>
#include "original_media_identity.hpp"
#include "original_vga_host.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <csetjmp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace rloft_code2_resource_internal { namespace {
using u8=std::uint8_t; using u16=std::uint16_t; using u32=std::uint32_t;
#define REQUIRE(c) do {if(!(c))throw std::runtime_error("INIT/RES source: " #c);}while(false)
std::vector<u8> disk_read(const std::filesystem::path& p) {
    std::ifstream f(p,std::ios::binary|std::ios::ate);
    if(!f)throw std::runtime_error("Cannot read original file "+p.filename().string());
    const auto size=f.tellg();
    if(size<=0||size>64*1024*1024)throw std::runtime_error("Unsupported source length");
    f.seekg(0);
    std::vector<u8> data(static_cast<std::size_t>(size));
    f.read(reinterpret_cast<char*>(data.data()),data.size());
    if(!f)throw std::runtime_error("Partial file: "+p.filename().string());
    return data;
}
struct Fixture {
    std::array<std::vector<u8>,13> data{};
    std::string cd_prefix{};
    static constexpr std::array<u16,18> init_words={
        15,168,336,0,2169,3316,4158,600,49,49,69,166,192,266,32,103,48,414
    };
    explicit Fixture(const std::string& root,const std::string& cd_root) {
        if(!root.empty()) {
            const auto dir=std::filesystem::path(root);
            data[0]=disk_read(dir/"INIT");
            data[1]=disk_read(dir/"INFO.DAT");
            for(unsigned i=0;i<11;++i) {
                const auto& media=(i>=9 && !cd_root.empty()) ?
                    std::filesystem::path(cd_root) : dir;
                data[i+2]=disk_read(media/("RES"+std::to_string(i)));
            }
        } else {
            data[0].resize(1879);
            for(unsigned i=0;i<18;++i)code2::store_le16(data[0].data()+2u*i,init_words[i]);
            data[1]={'0','0','0','0','0','0','0','0','0','1','1','1','1',2,0,'D',':'};
            for(unsigned i=0;i<11;++i) {
                auto& b=data[i+2];b.resize(20);
                code2::store_le32(b.data(),2);code2::store_le32(b.data()+4,12);
                code2::store_le32(b.data()+8,12);
                b[12]='E';b[13]='H';b[14]=1;
                code2::store_le16(b.data()+15,3);
                b[17]=0;b[18]=0;b[19]=0;
            }
        }
        REQUIRE(data[0].size()==1879u);
        REQUIRE(data[1].size()>=15u && data[1].size()<=80u);
        REQUIRE(std::string(data[1].begin(),data[1].begin()+13)=="0000000001111");
        const auto n=code2::load_le16(data[1].data()+13);
        REQUIRE(data[1].size()==15u+n);
        REQUIRE(n>=2u && n<=60u);
        cd_prefix.assign(data[1].begin()+15,data[1].end());
        REQUIRE(cd_prefix.size()==n && cd_prefix.find('\0')==std::string::npos);
        REQUIRE(cd_prefix.find("..")==std::string::npos);
        REQUIRE(cd_prefix[1]==':'); // original DOS drive-qualified media path
        for(unsigned i=0;i<18;++i)
            REQUIRE(code2::load_le16(data[0].data()+i*2u)==init_words[i]);
        for(unsigned i=0;i<11;++i) {
            const auto& b=data[i+2];
            REQUIRE(b.size()>=8u);
            const u32 count=code2::load_le32(b.data());
            REQUIRE(count>0 && count<=15000 && 4u+std::uint64_t(count)*4u<=b.size());
            REQUIRE(code2::load_le32(b.data()+4u)==4u+4u*count);
            u32 prev=4u+4u*count;
            for(u32 j=0;j<count;++j) {
                const u32 off=code2::load_le32(b.data()+4u+4u*j);
                REQUIRE(off>=prev && off<=b.size());prev=off;
            }
        }
    }
};
struct Session {
    Fixture fixture;
    // In standalone tests this Session owns its guest. In the integrated
    // runtime, BOTH CODE.2 source resource routines and canonical
    // 353A0/20608/heap callbacks operate on the same referenced memory
    // vector and exact same CPU Machine; neither copies the guest state.
    std::vector<u8> local_memory;
    std::vector<u8>& memory;
    code2::Machine local_machine{};
    code2::Machine& m;
    rloft::canonical_code2_runtime::Opaque* canonical_owner=nullptr;
    code2::Application30Boundary init_callbacks{};
    code2::Application67Boundary info_callbacks{};
    code2::Application68Boundary res_callbacks{};
    code2::Application70Boundary original_image_callbacks{};
    u32 original_image_record=0xFFFFFFFFu;
    u32 original_image_handle=0u;
    u32 original_image_end=0u;
    u32 original_image_start=0u;
    unsigned original_type1_decodes=0;
    bool original_image_invalid=false;
    struct Handle {unsigned file; std::size_t cursor;};
    std::map<u32,Handle> files{};
    u32 heap_cursor=0x400000u;
    unsigned init_opens=0,info_opens=0,res_opens=0;
    u32 music_base=0;
    bool error=false;
    bool trace_materialize=false;
    unsigned traced_allocs=0,traced_reads=0,traced_opens=0,traced_seeks=0;
    // Bounded execution of canonical 20608 on THIS resource guest. No
    // substitute getenv/atoi result or reset of the shared memory.
    std::jmp_buf original_game_escape{};
    unsigned original_game_getenv_callbacks=0;
    u32 original_game_getenv_query=0;
    bool original_main_reached_environment=false;
    static u32 original_game_environment(u32 query) noexcept {
        auto& t=state();
        ++t.original_game_getenv_callbacks;
        t.original_game_getenv_query=query;
        if(query!=0x114Cu && query!=0x1154u &&
           query!=0x115Cu && query!=0x1164u)
            std::longjmp(t.original_game_escape,5);
        // Use the ACTUAL canonical CODE.2 352EE lookup and its original
        // 39526 case-insensitive string comparator. The original DGROUP
        // contains no reconstructed Watcom environment table yet, so
        // this is an authentic no-DOS-environment branch, not fabricated
        // environment variable text supplied by the host.
        code2::Runtime111Boundary lookup{};
        lookup.call_39526=&code2::function_39526;
        return code2::function_352ee(t.m,lookup,query);
    }
    static code2::s32 stop_at_game_atoi(u32) noexcept {
        auto& t=state();
        std::longjmp(t.original_game_escape,2);
    }
    static void stop_at_game_audio(u32,u32) noexcept {
        auto& t=state();
        std::longjmp(t.original_game_escape,3);
    }
    static u32 stop_at_game_disk(u32, u16&, u16&, u16&) noexcept {
        auto& t=state();
        std::longjmp(t.original_game_escape,4);
    }
    void execute_real_20608_to_first_dependency() {
        REQUIRE(!original_main_reached_environment);
        REQUIRE(m.data==memory.data());
        code2::Application46Boundary calls{};
        calls.call_352ee=&original_game_environment;
        calls.call_35349=&stop_at_game_atoi;
        calls.call_2f2ec=&stop_at_game_audio;
        calls.call_35286=&stop_at_game_disk;
        const u8* const base=memory.data();
        const int boundary=setjmp(original_game_escape);
        if(boundary==0) {
            // ORIGINAL compiled CODE2 20608, not a host copy of its
            // results. The first unavailable environment call stops
            // before any fabricated value reaches original gameplay.
            code2::function_20608(m,calls);
            throw std::runtime_error("Original 20608 returned before a bounded host dependency");
        }
        REQUIRE(m.data==base && memory.data()==base);
        REQUIRE(boundary==3 && original_game_getenv_callbacks>=1u &&
                original_game_getenv_query>=0x1000u &&
                original_game_getenv_query<0x2000u);
        original_main_reached_environment=true;
        std::cout<<"CODE2_PERSISTENT_GUEST_ORIGINAL_20608_ENTRY_PASS"
                 <<" first_unavailable_dependency=ORIGINAL_2F2EC_AUDIO_CALLBACK"
                 <<" source_game_entry=20608"
                 <<" original_guest_memory=RETAINED"
                 <<" compiled_getenv=352EE"
                 <<" synthetic_environment_answer=NO"
                 <<" game_loop_1FC14=NOT_REACHED\n";
    }
    rloft::graphics::OriginalVgaHost video{};
    std::filesystem::path owner_palette_file{};
    std::ifstream palette_stream{};
    u32 video_port_writes=0;
    bool palette_bad_port=false;
    static u8 palette_get8(u32 addr) {
        auto& t=state();
        if(addr>=t.memory.size()){t.palette_bad_port=true;return 0;}
        return t.memory[addr];
    }
    static u8 palette_in8(u16 port) {return state().video.read_port8(port);}
    static void palette_out8(u16 port,u8 value) {
        auto& t=state();
        if(!t.video.write_port8(port,value))t.palette_bad_port=true;
        else ++t.video_port_writes;
    }
    static void palette_sync(u32) {
        // Source retrace callback: use the same modeled 3DA register used
        // by the native compiled graphics adapter, not a fake DOS tick.
        (void)state().video.read_port8(0x3DAu);
    }
    static bool colors_open(void* opaque,const char* name) {
        auto& t=*static_cast<Session*>(opaque);
        if(std::string(name)!="COLORS")return false;
        t.palette_stream.open(t.owner_palette_file,std::ios::binary|std::ios::ate);
        if(!t.palette_stream || t.palette_stream.tellg()!=std::streamoff(11521))
            return false;
        t.palette_stream.seekg(0,std::ios::beg);
        char count=0;
        t.palette_stream.read(&count,1);
        return bool(t.palette_stream) && static_cast<u8>(count)==15u;
    }
    static bool colors_seek(void* opaque,u32 offset) {
        auto& t=*static_cast<Session*>(opaque);
        t.palette_stream.seekg(static_cast<std::streamoff>(offset),std::ios::beg);
        return bool(t.palette_stream);
    }
    static bool colors_read(void* opaque,u8* dst,u32 size) {
        auto& t=*static_cast<Session*>(opaque);
        t.palette_stream.read(reinterpret_cast<char*>(dst),
                              static_cast<std::streamsize>(size));
        return bool(t.palette_stream) &&
            std::all_of(dst,dst+size,[](u8 v){return v<=63u;});
    }
    static void colors_close(void* opaque) {
        static_cast<Session*>(opaque)->palette_stream.close();
    }
    static Session* current;
    static Session& state(){REQUIRE(current);return *current;}
    static void guest_string(u32 addr,const char* s) {
        auto& t=state();
        const std::size_t n=std::char_traits<char>::length(s);
        REQUIRE(addr+n<t.memory.size());
        std::copy_n(s,n+1,t.memory.data()+addr);
    }
    static u32 alloc_guest(u32 bytes) {
        auto& t=state();
        if(t.trace_materialize && ++t.traced_allocs<=8u)
            std::cerr<<"CODE2_2ECB4_RESOURCE_CALLBACK=ALLOC bytes="<<bytes<<"\n"<<std::flush;
        // Original 2EB60 sign-extends a 16-bit encoded resource length.
        // If 31334 was not called, the RES offset table is mistaken for
        // a header and produces an impossible almost-4GB allocation.
        // Reject that before handing it to original Watcom 37B42 heap.
        if(bytes==0u || bytes>t.memory.size()) {
            t.error=true;
            std::cerr<<"CODE2_2ECB4_INVALID_SOURCE_ALLOCATION bytes="<<bytes<<"\n"<<std::flush;
            return 0u;
        }
        if(t.canonical_owner) {
            const auto ptr=rloft::canonical_code2_runtime::allocate_source_heap(
                t.canonical_owner,bytes);
            if(std::uint64_t(ptr)+bytes>t.memory.size()){
                t.error=true;return 0;
            }
            std::fill_n(t.memory.begin()+ptr,bytes,0);
            return ptr;
        }
        const auto aligned=(std::uint64_t(t.heap_cursor)+7u)&~std::uint64_t(7);
        if(!bytes || aligned+bytes>t.memory.size()){t.error=true;return 0;}
        t.heap_cursor=static_cast<u32>(aligned+bytes);
        std::fill_n(t.memory.begin()+static_cast<std::ptrdiff_t>(aligned),bytes,0);
        return static_cast<u32>(aligned);
    }
    static void free_guest(u32 payload) {
        auto& t=state();
        if(t.canonical_owner && payload)
            rloft::canonical_code2_runtime::release_source_heap(
                t.canonical_owner,payload);
        // The standalone synthetic source fixture does not represent a
        // Watcom heap, and keeps its historic bounded no-op release.
    }
    static u32 open_init(u32 pointer) {
        auto& t=state();
        if(pointer!=0x718u || std::string(reinterpret_cast<const char*>(t.memory.data()+pointer))!="INIT"){
            t.error=true;return 0xffffffffu;
        }
        ++t.init_opens;t.files[0x41u]={0u,0u};return 0x41u;
    }
    static u32 open_info(u32 pointer) {
        auto& t=state();
        if(pointer!=0x1384u ||
           std::string(reinterpret_cast<const char*>(t.memory.data()+pointer))!="INFO.DAT"){
            t.error=true;return 0xffffffffu;
        }
        ++t.info_opens;t.files[0x42u]={1u,0u};return 0x42u;
    }
    static code2::s32 open_resource(const u8* name) {
        auto& t=state();
        if(t.trace_materialize && ++t.traced_opens<=8u)
            std::cerr<<"CODE2_2ECB4_RESOURCE_CALLBACK=OPEN\n"<<std::flush;
        std::size_t n=0;
        while(n<84 && name[n]!=0)++n;
        if(n==0 || n==84){std::cerr<<"OPEN_RES_INVALID_LENGTH="<<n<<'\n';t.error=true;return -1;}
        std::string path(reinterpret_cast<const char*>(name),n);
        const bool cd_path=path.rfind(t.fixture.cd_prefix,0)==0;
        std::string base=cd_path?path.substr(t.fixture.cd_prefix.size()):path;
        if(cd_path && !base.empty() && (base.front()=='/' || base.front()=='\\'))
            base.erase(base.begin());
        if(base.size()<4||base.size()>5||base.substr(0,3)!="RES"){
            t.error=true;return -1;
        }
        unsigned id=0;
        for(std::size_t i=3;i<base.size();++i) {
            if(base[i]<'0'||base[i]>'9'){t.error=true;return -1;}
            id=id*10u+unsigned(base[i]-'0');
        }
        if(id>=11){t.error=true;return -1;}
        // Source INFO.DAT prefix flags: RES9/RES10 are CD path prefixed.
        if(id>=9 && !cd_path){t.error=true;return -1;}
        if(id<9 && cd_path){t.error=true;return -1;}
        const u32 handle=0x100u+id;
        t.files[handle]={id+2u,0u};
        ++t.res_opens;
        return static_cast<code2::s32>(handle);
    }
    static u32 position(u32 offset,u32 origin,u32 handle) {
        auto& t=state();
        if(t.trace_materialize && ++t.traced_seeks<=8u)
            std::cerr<<"CODE2_2ECB4_RESOURCE_CALLBACK=SEEK handle="<<handle
                     <<" offset="<<offset<<" origin="<<origin<<"\n"<<std::flush;
        const auto it=t.files.find(handle);
        if(it==t.files.end()){t.error=true;return 0u;}
        const auto& bytes=t.fixture.data[it->second.file];
        if(origin==1u && offset==0u)
            return static_cast<u32>(it->second.cursor);
        if(origin!=0u || offset>bytes.size()){t.error=true;return 0u;}
        it->second.cursor=offset;return offset;
    }
    static void resource_record_seek(u32 item,u32 handle) {
        auto& t=state();
        const auto it=t.files.find(handle);
        if(it==t.files.end() || it->second.file<2u ||
           it->second.file>=t.fixture.data.size()) {
            t.error=true;
            return;
        }
        const auto& file=t.fixture.data[it->second.file];
        const u32 count=code2::load_le32(file.data());
        const std::uint64_t table_end=4u+std::uint64_t(count)*4u;
        if(item>=count || table_end>file.size() ||
           4u+std::uint64_t(item)*4u+4u>table_end) {
            t.error=true;
            return;
        }
        // Original RES directory: count:u32, then count absolute
        // u32 record offsets. 2EB60 expects 31334(index,stream)
        // to set the stream cursor BEFORE reading its 5-byte header.
        const u32 entry=code2::load_le32(file.data()+4u+item*4u);
        if(entry<table_end || std::uint64_t(entry)+5u>file.size()) {
            t.error=true;
            return;
        }
        it->second.cursor=entry;
        if(t.canonical_owner && it->second.file==2u &&
           item==code2::load_le32(t.memory.data()+0x258DCu)+0x2Cu) {
            const u32 next=(item+1u<count)?
                code2::load_le32(file.data()+4u+(item+1u)*4u):
                static_cast<u32>(file.size());
            if(next<=entry+13u || next>file.size() ||
               file[entry]!='E' || file[entry+1u]!='H' ||
               file[entry+2u]!=1u ||
               code2::load_le16(file.data()+entry+3u)!=8u ||
               (file[entry+11u]!=0u && file[entry+11u]!=3u) ||
               code2::load_le16(file.data()+entry+5u)==0u ||
               code2::load_le16(file.data()+entry+7u)==0u ||
               file[entry+9u]==0u) {
                t.original_image_invalid=true;t.error=true;return;
            }
            t.original_image_record=item;
            t.original_image_handle=handle;
            t.original_image_start=entry;
            t.original_image_end=next;
            t.original_image_invalid=false;
            std::cerr<<"CODE2_ORIGINAL_20440_TYPED_IMAGE_METADATA"
                     <<" entry="<<item
                     <<" mode="<<unsigned(file[entry+11u])
                     <<" frames="<<code2::load_le16(file.data()+entry+5u)
                     <<" rows="<<code2::load_le16(file.data()+entry+7u)
                     <<" width_units="<<unsigned(file[entry+9u])
                     <<" end="<<next<<"\n"<<std::flush;
        }
        if(t.trace_materialize) {
            std::cerr<<"CODE2_2ECB4_RESOURCE_CALLBACK=31334_INDEXED_SEEK"
                     <<" item="<<item<<" handle="<<handle
                     <<" absolute_offset="<<entry
                     <<" header_type="<<unsigned(file[entry+2u])
                     <<" encoded_length="<<static_cast<std::int16_t>(
                         code2::load_le16(file.data()+entry+3u))
                     <<"\n"<<std::flush;
        }
    }
    static u32 copy_handle(u8* destination,u32 size,u32 handle) {
        auto& t=state();
        if(t.trace_materialize && ++t.traced_reads<=8u)
            std::cerr<<"CODE2_2ECB4_RESOURCE_CALLBACK=READ handle="<<handle
                     <<" size="<<size<<"\n"<<std::flush;
        const auto it=t.files.find(handle);
        if(it==t.files.end()){t.error=true;return 0;}
        auto& h=it->second;
        const auto& bytes=t.fixture.data[h.file];
        const std::size_t n=std::min<std::size_t>(size,bytes.size()-h.cursor);
        std::copy_n(bytes.data()+h.cursor,n,destination);
        h.cursor+=n;
        if(n!=size)t.error=true;
        return static_cast<u32>(n);
    }
    // The original 2FD9C type-1 image parser uses the same source file
    // stream, buffer globals and authentic Watcom allocator. This runs on
    // the same canonical guest that called 20440 -> 2ECB4; no image
    // bytes are filled from the independent media decoder.
    // Application70's call_3131c signature is (handle,origin,delta),
    // unlike the Application30 file callback's (delta,origin,handle).
    static u32 original_image_position(u32 handle,u32 origin,u32 delta) {
        return position(delta,origin,handle);
    }
    static u32 original_logical_position() {
        auto& t=state();
        const auto it=t.files.find(t.original_image_handle);
        if(it==t.files.end()) {t.original_image_invalid=true;return 0u;}
        const u32 buffered=code2::load_le32(t.memory.data()+0xF4D8u);
        if(buffered>it->second.cursor){t.original_image_invalid=true;return 0u;}
        return static_cast<u32>(it->second.cursor-buffered);
    }
    static u8 original_image_buffered_byte() {
        auto& t=state();
        if(t.original_image_invalid ||
           original_logical_position()>=t.original_image_end){
            t.original_image_invalid=true;return 0u;
        }
        return code2::function_2f9e4(t.m,t.original_image_callbacks);
    }
    static bool original_image_37700_read(u8* output) {
        auto& t=state();
        if(t.original_image_invalid || !output ||
           original_logical_position()>=t.original_image_end)return false;
        *output=code2::function_2f9e4(t.m,t.original_image_callbacks);
        return !t.original_image_invalid && !t.error;
    }
    static void original_image_37700_output(u8 value) {
        auto& t=state();
        code2::function_2fa30(t.m,t.original_image_callbacks,value);
    }
    static u32 original_image_37700() {
        auto& t=state();
        const u32 descriptor=code2::load_le32(t.memory.data()+0x2B6A4u);
        if(!descriptor || std::uint64_t(descriptor)+2u>t.memory.size()){
            t.original_image_invalid=true;return 0u;
        }
        const u32 frames=code2::load_le16(t.memory.data()+descriptor);
        const u32 per=code2::load_le32(t.memory.data()+0x2B6B4u);
        if(frames==0u || per==0u || std::uint64_t(frames)*per>2u*1024u*1024u){
            t.original_image_invalid=true;return 0u;
        }
        code2::Runtime126Boundary callbacks{};
        callbacks.bounded_read=&original_image_37700_read;
        callbacks.put_output=&original_image_37700_output;
        const u32 ok=code2::function_37700(t.m,callbacks,frames*per+18u);
        if(ok!=1u || original_logical_position()!=t.original_image_end)
            t.original_image_invalid=true;
        return ok;
    }
    static void original_image_37810(u32 record) {
        auto& t=state();
        if(std::uint64_t(record)+4u>t.memory.size()){
            t.original_image_invalid=true;return;
        }
        const u32 frames=code2::load_le16(t.memory.data()+record+2u);
        if(!frames || frames>1024u){t.original_image_invalid=true;return;}
        std::vector<u16> permutation(4u*frames);
        code2::Runtime127Boundary callbacks{};
        callbacks.permutation_words=permutation.data();
        code2::function_37810(callbacks,t.memory.data()+record);
    }
    static u32 original_type1_source_decoder(u32 header,u32 handle) {
        auto& t=state();
        if(header!=0x2B670u || handle!=t.original_image_handle ||
           t.original_image_record==0xFFFFFFFFu ||
           t.memory[header+2u]!=1u ||
           code2::load_le16(t.memory.data()+header+3u)!=8u) {
            t.original_image_invalid=true;return 0u;
        }
        const auto image=code2::function_2fd9c(t.m,t.original_image_callbacks,
                                                header,handle);
        if(t.error || t.original_image_invalid || !image ||
           original_logical_position()!=t.original_image_end) {
            t.original_image_invalid=true;return 0u;
        }
        ++t.original_type1_decodes;
        return image;
    }
    static u32 init_local_read(u8* dst,u32 size,u32 handle) {return copy_handle(dst,size,handle);}
    static u32 read_guest(u32 dst,u32 size,u32 handle) {
        auto& t=state();
        if(std::uint64_t(dst)+size>t.memory.size()){t.error=true;return 0;}
        return copy_handle(t.memory.data()+dst,size,handle);
    }
    static void read_guest_void(u32 dst,u32 size,u32 handle) {(void)read_guest(dst,size,handle);}
    static void info_read_word(u16* dst,u32 handle) {
        u8 bytes[2]{};if(copy_handle(bytes,2,handle)!=2){state().error=true;return;}
        *dst=code2::load_le16(bytes);
    }
    static void close_file(u32 handle) {
        auto& t=state();
        if(!t.files.erase(handle))t.error=true;
    }
    static void fill(u32 dst,u32 value,u32 count) {
        auto& t=state();if(std::uint64_t(dst)+count>t.memory.size()){t.error=true;return;}
        std::fill_n(t.memory.begin()+dst,count,static_cast<u8>(value));
    }
    static void int_to_ascii(u32 value,u32 dst,u32 base) {
        auto& t=state();
        if(base!=10u){t.error=true;return;}
        const auto number=std::to_string(value);
        if(dst+number.size()+1>t.memory.size()){t.error=true;return;}
        std::copy(number.begin(),number.end(),t.memory.begin()+dst);
        t.memory[dst+number.size()]=0;
    }
    static void record_music(u32 v){state().music_base=v;}
    Session(std::string root,std::string cd_root,
            std::vector<u8>* borrowed_memory=nullptr,
            code2::Machine* borrowed_machine=nullptr,
            rloft::canonical_code2_runtime::Opaque* heap_owner=nullptr)
        :fixture(root,cd_root),
         local_memory(borrowed_memory?std::vector<u8>{}:
                                 std::vector<u8>(16u*1024u*1024u)),
         memory(borrowed_memory?*borrowed_memory:local_memory),
         m(borrowed_machine?*borrowed_machine:local_machine),
         canonical_owner(heap_owner) {
        REQUIRE((borrowed_memory==nullptr)==(borrowed_machine==nullptr));
        REQUIRE((borrowed_memory!=nullptr)==(heap_owner!=nullptr));
        REQUIRE(current==nullptr);current=this;m.data=memory.data();
        // The canonical guest is still executing its original startup.
        // Do NOT overwrite DGROUP pathname scratch until the actual
        // original 2F468 resource-selection callback is suspended.
        if(!canonical_owner) {
            guest_string(0x718u,"INIT");
            guest_string(0x1384u,"INFO.DAT");
            guest_string(0x26000u,"RES");
        }
        init_callbacks.call_312a0=&open_init;
        init_callbacks.call_3131c=&position;
        init_callbacks.call_312ec_local=&init_local_read;
        init_callbacks.call_312dc=&close_file;
        init_callbacks.call_288f0=&record_music;
        info_callbacks.call_312a0=&open_info;
        info_callbacks.call_312ec=&read_guest_void;
        info_callbacks.call_312ec_word=&info_read_word;
        info_callbacks.call_30d30=&fill;
        info_callbacks.call_312dc=&close_file;
        info_callbacks.call_373bf=&int_to_ascii;
        res_callbacks.app67=&info_callbacks;
        res_callbacks.call_3104c=&alloc_guest;
        res_callbacks.call_310b0=&free_guest;
        res_callbacks.call_312a0_stack=&open_resource;
        res_callbacks.call_312ec=&read_guest_void;
        res_callbacks.call_31334=&resource_record_seek;
        res_callbacks.call_2fd9c=&original_type1_source_decoder;
        original_image_callbacks.call_312ec=&read_guest;
        original_image_callbacks.call_3131c=&original_image_position;
        original_image_callbacks.call_3104c=&alloc_guest;
        original_image_callbacks.call_310b0=&free_guest;
        original_image_callbacks.call_37700=&original_image_37700;
        original_image_callbacks.call_37810=&original_image_37810;
    }
    ~Session(){current=nullptr;}
    void seed_authenticated_code2(const std::filesystem::path& code_file) {
        const auto original=disk_read(code_file);
        REQUIRE(rloft_recomp::is_original_code2(original));
        constexpr u32 object2_file=0x75F24u;
        REQUIRE(original.size()>object2_file+0x81ACu+32u*3u);
        // Stage the authentic initialized retail data object into the
        // same guest memory used by original INIT/INFO/RES entry functions.
        std::copy(original.begin()+object2_file,original.end(),memory.begin());
        guest_string(0x718u,"INIT");
        guest_string(0x1384u,"INFO.DAT");
        guest_string(0x26000u,"RES");
        REQUIRE(m.data==memory.data());
    }
    void source_palette_from_scene(const std::filesystem::path& root,unsigned level) {
        REQUIRE(init_opens==1u && info_opens==1u && res_opens==11u);
        const unsigned bank=rloft::original_scene_palette::select_bank(memory,level);
        owner_palette_file=root/"COLORS";
        // The original 1F8E0 setter is mapped, but the continuous original
        // CODE.1->CODE.2 main is not. Explicitly stage the selected source
        // scene value here; this is a host-orchestrated transition.
        code2::store_le32(memory.data()+0xC034u,bank);
        memory[0xCF58u]=1u;
        constexpr u32 profile=0x300000u;
        code2::store_le32(memory.data()+0xF050u,profile);
        memory[profile+0x6Cu]=0u; // original startup default brightness
        rloft::original_code2_palette::ReadOnlyFile file{};
        file.opaque=this;
        file.open=&colors_open;
        file.seek_absolute=&colors_seek;
        file.read_exact=&colors_read;
        file.close=&colors_close;
        const auto loaded=rloft::original_code2_palette::function_325b0(
            m,file,0xC054u,0u,bank,256u,static_cast<u32>(memory.size()));
        REQUIRE(loaded.offset==1u+bank*768u &&
                loaded.brightness_source_executed && loaded.count==256u);
        code2::Graphics2Boundary io{};
        io.read8=&palette_get8;
        io.in8=&palette_in8;
        io.out8=&palette_out8;
        io.sync_2abc4=&palette_sync;
        code2::function_29ca0(io,0xC054u,0u,768u);
        REQUIRE(!palette_bad_port && !video.unsupported_read());
        REQUIRE(video_port_writes==769u);
        for(u32 i=0;i<256u;++i) {
            const auto rgb=video.palette(static_cast<u8>(i));
            REQUIRE(rgb.r==memory[0xC054u+3u*i] &&
                    rgb.g==memory[0xC054u+3u*i+1u] &&
                    rgb.b==memory[0xC054u+3u*i+2u]);
        }
        std::cout<<"CODE2_REAL_STARTUP_RES_TO_SCENE_PALETTE_PASS"
                 <<" level="<<level<<" bank="<<bank
                 <<" source_palette_loader=325B0_RECONSTRUCTED"
                 <<" compiled_brightness=32150"
                 <<" compiled_DAC_writer=29CA0"
                 <<" DAC_port_writes="<<video_port_writes
                 <<" guest_palette=C054"
                 <<" source_COLOR_file=READ_ONLY"
                 <<" DOS_CPU_CALL_ORDER=NOT_VERIFIED\n";
    }
    void verify_synthetic_first_indexed_record() {
        REQUIRE(canonical_owner==nullptr && init_opens==1u &&
                res_opens==11u && !error);
        // The synthetic RES0 has two directory entries, both pointing
        // at offset 12: original EH header, type1, 3 payload bytes.
        // This executes canonical 2ECB4 -> 2EB60 -> 31334 and observes
        // the final cursor after the 5+3 byte materialization read.
        const u32 rec=code2::load_le32(memory.data()+0xF0B8u);
        REQUIRE(code2::load_le32(memory.data()+rec)==0x100u);
        REQUIRE(files.at(0x100u).cursor==4u);
        trace_materialize=true;
        code2::function_2ecb4(m,res_callbacks,0u,0u);
        trace_materialize=false;
        const u32 items=code2::load_le32(memory.data()+rec+12u);
        REQUIRE(!error && files.at(0x100u).cursor==20u &&
                code2::load_le16(memory.data()+0x2B670u)==0x4845u &&
                memory[0x2B672u]==1u &&
                code2::load_le16(memory.data()+0x2B673u)==3u &&
                memory[items+2u]==1u &&
                traced_allocs==1u && traced_reads==2u &&
                traced_seeks==0u);
        std::cout<<"ORIGINAL_CODE2_2ECB4_2EB60_31334_INDEXED_RES_SEEK_PASS"
                 <<" source_calls=2ECB4_2EB60"
                 <<" original_offset=12"
                 <<" header=EH"
                 <<" payload_bytes=3"
                 <<" decoder=NOT_CONNECTED\n";
    }
    void boot() {
        REQUIRE(init_opens==0u && info_opens==0u && res_opens==0u);
        // The standalone fixture already carries these strings; for
        // borrowed real CODE2 startup they are staged only HERE, at
        // the observed original resource-selection host boundary.
        guest_string(0x718u,"INIT");
        guest_string(0x1384u,"INFO.DAT");
        guest_string(0x26000u,"RES");
        code2::function_16d04(m,init_callbacks);
        REQUIRE(init_opens==1u && music_base==49u);
        REQUIRE(code2::load_le32(memory.data()+0x258DCu)==15u);
        REQUIRE(code2::load_le32(memory.data()+0x258E4u)==336u);
        REQUIRE(code2::load_le32(memory.data()+0x258F8u)==600u);
        REQUIRE(code2::load_le16(memory.data()+0x7FCCu)==414u);
        code2::function_2e7f0(m,res_callbacks,0x26000u,11u);
        REQUIRE(!error && info_opens==1u && res_opens==11u);
        REQUIRE(code2::load_le32(memory.data()+0xF0B4u)==11u);
        const u32 base=code2::load_le32(memory.data()+0xF0B8u);
        REQUIRE(base>0u);
        for(unsigned i=0;i<11u;++i) {
            const u32 rec=base+i*16u;
            REQUIRE(code2::load_le32(memory.data()+rec)==0x100u+i);
            const u32 count=code2::load_le32(memory.data()+rec+8u);
            REQUIRE(count==code2::load_le32(fixture.data[i+2].data()));
            const u32 items=code2::load_le32(memory.data()+rec+12u);
            REQUIRE(items>0u);
            for(unsigned j=0;j<count;++j) {
                const u32 p=items+9u*j;
                REQUIRE((memory[p]&0x1Fu)==15u);
                REQUIRE(memory[p+2u]==0u);
                REQUIRE(code2::load_le32(memory.data()+p+5u)==0u);
            }
            std::cout<<"CODE2_RES_STARTUP_SLOT="<<i<<" entries="<<count
                     <<" unloaded=PASS\n";
        }
        std::cout<<"CODE2_16D04_INIT_SOURCE_HEADER_PASS\n"
                 <<"CODE2_2E768_INFO_SOURCE_PATH_FLAGS_PASS\n"
                 <<"CODE2_2E7F0_ALL_11_RES_ARCHIVES_PASS\n"
                 <<"CODE1_TO_CODE2_FULL_GAME_BOOT_ACCEPTED=NO\n";
    }
};
Session* Session::current=nullptr;
}} // namespace rloft_code2_resource_internal::{anonymous}
// Reference the source-resource Session explicitly: the canonical
// startup owner also has an independent internal Session type.

namespace rloft { namespace original_code2_session {
struct Opaque {
    // The canonical 353A0 runtime (source heap, selectors, machine and RAM)
    // is the lifetime owner. The resource Session BORROWS that exact
    // machine/RAM instead of constructing a second guest.
    std::unique_ptr<rloft::canonical_code2_runtime::Opaque,
                    void(*)(rloft::canonical_code2_runtime::Opaque*)> canonical;
    std::unique_ptr<rloft_code2_resource_internal::Session> guest;
    std::filesystem::path original_root;
    unsigned pending_scene_level=0;
    bool resources_loaded=false;
    std::uint64_t dispatches=0;
    bool initialized=false;
    explicit Opaque(const std::filesystem::path& root,
                    const std::filesystem::path& cd,unsigned level)
        :original_root(root),pending_scene_level(level),
         canonical(rloft::canonical_code2_runtime::start_stackful(root/"CODE.2"),
                   &rloft::canonical_code2_runtime::destroy),
         guest(std::make_unique<rloft_code2_resource_internal::Session>(
             root.string(),cd.string(),
             &rloft::canonical_code2_runtime::guest_memory(canonical.get()),
             static_cast<code2::Machine*>(
                rloft::canonical_code2_runtime::canonical_machine(canonical.get())),
             canonical.get())) {
        if(level>=32u)throw std::out_of_range("CODE.2 scene level out of range");
        const auto source=rloft::canonical_code2_runtime::inspect(canonical.get());
        REQUIRE(source.source_353a0_executed &&
                source.source_20608_executed &&
                source.source_heap_provider_retained &&
                source.original_getenv_calls>=1u &&
                source.source_keyboard_vectors_installed &&
                source.source_sound_config_read &&
                source.source_cpu_stack_resumable &&
                source.paused_at_original_mouse_reset &&
                source.pending_DOS_service_sequence==1u &&
                source.heap_arena_header!=0u &&
                source.memory_address==guest->memory.data() &&
                guest->m.data==source.memory_address);
        // The original main has now executed the original 375FE
        // keyboard vector path and read SOUND.CFG before stopping at
        // the actual external DOS INT33 mouse reset dependency.
        // DO NOT initialize RES/INFO, scene palette, VGA or their
        // guest globals at the mouse-reset boundary. On original
        // startup this state is not yet needed. Preloading here
        // changed execution after the Y mouse-range callback and
        // prevented the expected DOS AH36 request from completing.
        // Keep the canonical source state untouched until it reaches
        // its actual 2F468 resource dependency.
        REQUIRE(guest->init_opens==0u && guest->res_opens==0u &&
                code2::load_le32(guest->memory.data()+0xF0B4u)==0u);
        initialized=true;
    }
    void load_resources_at_original_selection() {
        REQUIRE(initialized && !resources_loaded && pending_scene_level<32u);
        namespace c=rloft::canonical_code2_runtime;
        REQUIRE(c::pending_host_service(canonical.get())==5u);
        const auto before=c::inspect(canonical.get());
        REQUIRE(before.memory_address==guest->memory.data() &&
                before.source_cpu_stack_resumable);
        std::cerr<<"CODE2_SOURCE_CHAIN_STAGE=ORIGINAL_2F468_LAZY_RES_INIT_BEGIN\n"<<std::flush;
        const auto interrupted_cpu=guest->m.cpu;
        try {
            // Exact same canonical DOS/4GW heap and guest memory, but
            // now at the real suspended 2F468 dependency, after
            // both mouse ranges and original disk-space request.
            guest->boot();
            guest->source_palette_from_scene(original_root,pending_scene_level);
        }catch(...) {
            guest->m.cpu=interrupted_cpu;
            throw;
        }
        guest->m.cpu=interrupted_cpu;
        REQUIRE(guest->init_opens==1u && guest->res_opens==11u &&
                code2::load_le32(guest->memory.data()+0xF0B4u)==11u &&
                guest->video_port_writes==769u);
        c::attach_original_resource_callbacks(
            canonical.get(),static_cast<void*>(&guest->res_callbacks));
        // Keep the SAME original Application30 INIT file reader available
        // when original CODE2 20440 later calls 16D04 a second time.
        c::attach_original_init_callbacks(
            canonical.get(),static_cast<void*>(&guest->init_callbacks));
        c::attach_original_vga(canonical.get(),&guest->video);
        resources_loaded=true;
        std::cerr<<"CODE2_SOURCE_CHAIN_STAGE=ORIGINAL_2F468_LAZY_RES_INIT_DONE\n"<<std::flush;
    }
};
static std::uint64_t guest_source_fingerprint(const Opaque& s) {
    std::uint64_t hash=14695981039346656037ULL;
    for(const auto b:s.guest->memory) {
        hash^=b;
        hash*=1099511628211ULL;
    }
    return hash;
}
Opaque* create(const std::filesystem::path& root,
               const std::filesystem::path& cd,unsigned level) {
    return new Opaque(root,cd,level);
}
Evidence inspect(const Opaque* s) {
    if(!s || !s->guest)throw std::invalid_argument("Null original CODE2 guest");
    const auto canonical=rloft::canonical_code2_runtime::inspect(s->canonical.get());
    if(s->guest->m.data!=s->guest->memory.data() ||
       canonical.memory_address!=s->guest->memory.data())
        throw std::runtime_error("Canonical CODE.2 startup/resource RAM diverged");
    Evidence e{};
    e.memory_address=s->guest->memory.data();
    e.source_state_fingerprint=guest_source_fingerprint(*s);
    e.resource_cache_base=code2::load_le32(s->guest->memory.data()+0xF0B8u);
    e.resource_archives=code2::load_le32(s->guest->memory.data()+0xF0B4u);
    e.selected_palette=code2::load_le32(s->guest->memory.data()+0xC034u);
    e.vga_port_writes=s->guest->video_port_writes;
    e.dispatch_calls=s->dispatches;
    e.initialized=s->initialized;
    e.original_20608_executed_to_host_dependency=canonical.source_20608_executed;
    e.original_352ee_getenv_calls=canonical.original_getenv_calls;
    e.original_game_loop_connected=false;
    // The canonical runtime has a real suspended source call stack;
    // this resource guest borrows its RAM while no instruction runs.
    return e;
}
std::uint32_t test_advance_to_original_video_mode(Opaque* s) {
    if(!s || !s->initialized)
        throw std::invalid_argument("Original CODE.2 RES guest unavailable");
    namespace c=rloft::canonical_code2_runtime;
    const auto first=inspect(s);
    REQUIRE(first.resource_archives==0u && first.vga_port_writes==0u &&
            !s->resources_loaded);
    auto* const owner=s->canonical.get();
    // Every call resumes the actual waiting source thread, rather than
    // replaying 353A0, 20608 or 20440. Device replies are TEST-ONLY.
    std::cerr<<"CODE2_SOURCE_CHAIN_STAGE=START_MOUSE_RESET\n"<<std::flush;
    REQUIRE(c::pending_host_service(owner)==1u);
    REQUIRE(c::answer_mouse_reset(owner,true,3u)==3u);
    std::cerr<<"CODE2_SOURCE_CHAIN_STAGE=MOUSE_RESET_RETURNED\n"<<std::flush;
    REQUIRE(c::answer_real_mode_mouse(owner,1u,3u,0u,0u)==2u);
    std::cerr<<"CODE2_SOURCE_CHAIN_STAGE=INT31_0300_RETURNED\n"<<std::flush;
    REQUIRE(c::answer_mouse_range(owner,7u)==2u);
    std::cerr<<"CODE2_SOURCE_CHAIN_STAGE=MOUSE_RANGE_X_DONE\n"<<std::flush;
    REQUIRE(c::answer_mouse_range(owner,8u)==4u);
    std::cerr<<"CODE2_SOURCE_CHAIN_STAGE=MOUSE_RANGE_Y_DONE\n"<<std::flush;
    REQUIRE(c::answer_host_disk_free(owner)==5u);
    std::cerr<<"CODE2_SOURCE_CHAIN_STAGE=AH36_DISK_DONE\n"<<std::flush;
    // The original guest has NOW reached resource selection; only
    // at this boundary may the same-guest archive tables and palette
    // be initialized by our still host-ordered reconstruction.
    s->load_resources_at_original_selection();
    // The 2F468 source callback is now connected to the actual
    // original eleven-archive Application68 table in THIS guest.
    std::cerr<<"CODE2_SOURCE_CHAIN_STAGE=RESOURCE_2F468_BEGIN\n"<<std::flush;
    s->guest->trace_materialize=true;
    REQUIRE(c::answer_source_resource_selection(owner)==6u);
    s->guest->trace_materialize=false;
    std::cerr<<"CODE2_SOURCE_CHAIN_STAGE=RESOURCE_2F468_DONE_AT_VIDEO_290F4\n"<<std::flush;
    const auto after=inspect(s);
    REQUIRE(after.memory_address==first.memory_address &&
            after.resource_archives==11u &&
            after.resource_cache_base!=0u &&
            after.selected_palette==3u &&
            after.vga_port_writes==769u &&
            s->resources_loaded &&
            !after.original_game_loop_connected);
    std::cout<<"ORIGINAL_CODE2_SAME_GUEST_BOOTSTRAP_TO_VGA_PASS"
             <<" original_source=353A0_20608_20440_2F468"
             <<" retained_archive_slots=11"
             <<" original_resource_selection=EXECUTED"
             <<" real_mode_mouse=0300_RETURNED"
             <<" DOS_memory_0101=RELEASED"
             <<" mouse_XY_ranges=EXECUTED"
             <<" disk_AH36=HOST_BACKED"
             <<" next_original_dependency=290F4_VIDEO_MODE"
             <<" original_1FC14_loop=NOT_REACHED\n";
    return c::pending_host_service(owner);
}
std::uint32_t test_advance_through_original_video_mode(Opaque* s) {
    if(!s || !s->initialized)
        throw std::invalid_argument("CODE2 original video guest missing");
    namespace c=rloft::canonical_code2_runtime;
    const auto* const original_machine=s->guest->m.data;
    const auto* const source_before=s->guest->memory.data();
    REQUIRE(test_advance_to_original_video_mode(s)==6u);
    std::cerr<<"CODE2_SOURCE_CHAIN_STAGE=VIDEO_290F4_HANDOFF_BEGIN\n"<<std::flush;
    const auto source_mode=c::answer_source_video_mode(s->canonical.get());
    std::cerr<<"CODE2_SOURCE_CHAIN_STAGE=VIDEO_290F4_HANDOFF_RETURNED\n"<<std::flush;
    const auto evidence=c::inspect(s->canonical.get());
    REQUIRE(source_mode==10u &&
            original_machine==source_before &&
            evidence.memory_address==source_before &&
            evidence.original_source_video_mode_calls==1u &&
            evidence.original_source_palette_streams==1u &&
            evidence.original_source_page_pushes==1u &&
            evidence.original_source_page_pops==1u &&
            evidence.original_source_page_selects==4u &&
            evidence.original_source_planar_fills==2u &&
            evidence.source_290f4_320x400_video_initialized &&
            evidence.source_cpu_stack_resumable &&
            c::pending_host_service(s->canonical.get())==10u &&
            s->guest->video.bios_mode()==0x13u &&
            s->guest->video.repeat_scanlines()==1u);
    std::cout<<"ORIGINAL_CODE2_SAME_GUEST_FULL_VGA_MODE_PASS"
             <<" source=20440_290F4_29930_2AB98_2ABF4_2AC10"
             <<" retained_archive_slots=11"
             <<" CRTC_320x400=ORIGINAL_SOURCE"
             <<" DAC_clear=29234_29CA0"
             <<" original_game_stack=PRESERVED"
             <<" original_20294_double_clear=2E590_291B0_29B40_2E5D4"
             <<" original_5E38_game_options=ABSENT_SOURCE_RETURN"
             <<" original_28F58_volume=COMPILED"
             <<" original_31218_CHARS_check=SOURCE"
             <<" original_312A0_readonly_open=SOURCE_37E77"
             <<" next_original_dependency=20440_16D04"
             <<" original_1FC14_loop=NOT_REACHED\n";
    return source_mode;
}
std::uint32_t test_advance_past_original_second_init(Opaque* s) {
    if(!s || !s->initialized || !s->resources_loaded ||
       s->guest->init_opens!=1u || s->guest->res_opens!=11u)
        throw std::invalid_argument("Original 16D04 second INIT prerequisite invalid");
    namespace c=rloft::canonical_code2_runtime;
    auto* const owner=s->canonical.get();
    REQUIRE(c::pending_host_service(owner)==10u);
    auto* const base=s->guest->memory.data();
    std::cerr<<"CODE2_SOURCE_CHAIN_STAGE=SECOND_16D04_SOURCE_RESUME_BEGIN\n"<<std::flush;
    const auto next=c::advance_original_second_init(owner);
    // A full original 16D04 must reopen the unchanged original INIT
    // resource through the retained original Application30 callbacks.
    REQUIRE(s->guest->memory.data()==base && s->guest->m.data==base &&
            s->guest->init_opens==2u && !s->guest->error);
    REQUIRE(next==11u);
    // Independent original archive parser cross-checks the actual source
    // 181B4 guest result, rather than another host-decoded world copy.
    const auto archive=rloft::original_levels::read_archive(
        (s->original_root/"LEVELS.DAT").string());
    const auto grid=rloft::original_levels::decode_grid(archive,22u);
    const auto& memory=s->guest->memory;
    REQUIRE(code2::load_le32(memory.data()+0xC6C8u)==22u &&
            code2::load_le16(memory.data()+0x25619u)==grid.width &&
            code2::load_le16(memory.data()+0x2561Bu)==grid.height &&
            memory.data()==base);
    for(std::size_t i=0;i<grid.header.size();++i) {
        if(i==0x29Au || i==0x29Bu)continue; // source 181B4 scale fix
        REQUIRE(memory[0x25618u+i]==grid.header[i]);
    }
    const std::uint16_t scale=rloft::original_levels::le16(
        grid.header.data()+0x29Au);
    REQUIRE(code2::load_le16(memory.data()+0x258B2u)==
            (scale?scale:1u));
    for(std::size_t i=0;i<grid.cells.size();++i)
        REQUIRE(code2::load_le16(memory.data()+0x198E0u+2u*i)==
                grid.cells[i]);
    std::cout<<"ORIGINAL_CODE2_20440_LEVEL22_SOURCE_GRID_PASS"
             <<" source=181B4"
             <<" input=AUTHENTIC_LEVELS_DAT"
             <<" width="<<grid.width
             <<" height="<<grid.height
             <<" cells="<<grid.cells.size()
             <<" guest_memory=PRESERVED"
             <<" rendering=NOT_CONNECTED\n"<<std::flush;
    // Inspection of the exact original 20440 RES0 record at the point
    // before its currently-unwired 2ECB4 callback. No guest bytes changed.
    const u32 target=code2::load_le32(memory.data()+0x258DCu)+0x2Cu;
    const u32 catalog=code2::load_le32(memory.data()+0xF0B8u);
    REQUIRE(catalog>0u && std::uint64_t(catalog)+16u<=memory.size());
    const u32 records=code2::load_le32(memory.data()+catalog+0x0Cu);
    const u32 record_count=code2::load_le32(memory.data()+catalog+8u);
    REQUIRE(target<record_count && record_count>0u &&
            std::uint64_t(records)+std::uint64_t(record_count)*9u<=memory.size());
    const u32 slot=records+target*9u;
    const u32 source_handle=code2::load_le32(memory.data()+catalog);
    const auto& archive0=s->guest->fixture.data[2u];
    REQUIRE(source_handle!=0u && archive0.size()>8u);
    const u32 archive_count=code2::load_le32(archive0.data());
    REQUIRE(target<archive_count &&
            4u+std::uint64_t(target)*4u+4u<=archive0.size());
    const u32 at=code2::load_le32(archive0.data()+4u+target*4u);
    REQUIRE(std::uint64_t(at)+5u<=archive0.size() &&
            archive0[at]==0x45u && archive0[at+1u]==0x48u);
    const u32 raw_type=archive0[at+2u];
    const std::int16_t encoded_length=static_cast<std::int16_t>(
        code2::load_le16(archive0.data()+at+3u));
    std::cout<<"ORIGINAL_CODE2_20440_RESOURCE_DISCOVERY"
             <<" selected_index="<<target
             <<" catalog_count="<<record_count
             <<" source_archive=RES0"
             <<" encoded_type="<<raw_type
             <<" encoded_length="<<encoded_length
             <<" prior_loaded="<<static_cast<unsigned>(memory[slot+2u])
             <<" metadata_offset="<<at
             <<" decoded=NOT_YET_CONNECTED\n"<<std::flush;
    // Independent source-vs-original-image proof: the standalone media
    // reader decodes RES0:59 but never writes to the canonical guest.
    const auto expected=rloft::original_res::decode_type1(archive0,target);
    REQUIRE(raw_type==1u && target==59u && encoded_length==8 &&
            s->guest->original_type1_decodes==1u &&
            !s->guest->original_image_invalid && !s->guest->error);
    const u32 loaded=code2::load_le32(memory.data()+slot+5u);
    REQUIRE(memory[slot+2u]==1u &&
            (memory[slot]&0x10u)!=0u &&
            loaded>=8u && std::uint64_t(loaded)+8u<memory.size());
    const u32 decoded_frame_bytes=expected.frame_size();
    const u32 frame_stride=decoded_frame_bytes+
        (expected.tag==0xFFu?0u:1u);
    REQUIRE(std::uint64_t(loaded)+8u+
                std::uint64_t(frame_stride)*expected.frames<=memory.size());
    // The original 37810 permutation changes frame layout; accept only
    // the nonpermuted startup record until independent permutation parity
    // is added. Never confuse an unverified image with a decoded one.
    REQUIRE(expected.reorder==0u &&
            code2::load_le16(memory.data()+loaded)==frame_stride &&
            code2::load_le16(memory.data()+loaded+2u)==expected.frames &&
            code2::load_le16(memory.data()+loaded+4u)==expected.rows &&
            memory[loaded+6u]==expected.width_units);
    const u32 plane_bytes=decoded_frame_bytes/4u;
    REQUIRE(decoded_frame_bytes!=0u && decoded_frame_bytes%4u==0u &&
            expected.decoded.size()==
                std::uint64_t(decoded_frame_bytes)*expected.frames);
    for(u32 frame=0;frame<expected.frames;++frame) {
        const u32 out=loaded+8u+frame*frame_stride;
        const u32 data=out+(expected.tag==0xFFu?0u:1u);
        if(expected.tag!=0xFFu)REQUIRE(memory[out]==expected.tag);
        for(u32 i=0;i<plane_bytes;++i)
            for(u32 plane=0;plane<4u;++plane)
                REQUIRE(memory[data+plane*plane_bytes+i]==
                    expected.decoded[frame*decoded_frame_bytes+4u*i+plane]);
    }
    std::cout<<"ORIGINAL_CODE2_20440_RES0_TYPE1_DECODE_PARITY_PASS"
             <<" entry="<<target
             <<" source=2ECB4_2FD9C"
             <<" byte_count="<<expected.decoded.size()
             <<" frames="<<expected.frames
             <<" method="<<unsigned(expected.method)
             <<" all_source_planar_bytes=MATCH"
             <<" retention_2F184=YES"
             <<" guest_memory=PRESERVED\n"<<std::flush;
    const auto entry=c::inspect(owner);
    REQUIRE(entry.source_cpu_stack_resumable &&
            entry.memory_address==base &&
            entry.original_source_page_pops==2u &&
            entry.original_source_planar_fills==4u &&
            entry.original_source_palette_streams==2u &&
            entry.original_source_325b0_loads==1u &&
            entry.original_boot_render_page_selects==1u &&
            entry.original_boot_scanout_page_selects==1u);
    const auto output_page=memory[0xC050u];
    const auto scanout_page=static_cast<std::uint8_t>(output_page^1u);
    REQUIRE(memory[0xEF4Du]==output_page &&
            memory[0xEF4Eu]==scanout_page &&
            memory[0xF0D0u]==0u);
    const std::uint16_t expected_crtc=static_cast<std::uint16_t>(
        ((static_cast<std::uint32_t>(scanout_page)*
          (code2::load_le32(memory.data()+0xF005u)>>4u))&0xFFu)<<8u);
    REQUIRE(s->guest->video.crtc_start()==expected_crtc);
    for(std::uint32_t i=0u;i<256u;++i) {
        const auto rgb=s->guest->video.palette(static_cast<std::uint8_t>(i));
        REQUIRE(rgb.r==memory[0xC054u+i*3u] &&
                rgb.g==memory[0xC054u+i*3u+1u] &&
                rgb.b==memory[0xC054u+i*3u+2u]);
    }
    std::cout<<"ORIGINAL_CODE2_20440_COLORS_PRESENTATION_SOURCE_PASS"
             <<" compiled_palette=325B0"
             <<" palette_bytes=768"
             <<" compiled_output=291B0"
             <<" compiled_scanout=291C8"
             <<" CRTC=SOURCE_FORMULA"
             <<" original_vga_palette=256_RGB6_MATCH"
             <<" guest_memory=PRESERVED"
             <<" rendering=NOT_CONNECTED\n"<<std::flush;
    // Two explicit no-key host replies: the original 1FC14 source loop
    // executes between these 30B40 polls on its original retained stack.
    // This proves source iteration, NOT any real-world frame production.
    for(std::uint64_t i=1u;i<=2u;++i) {
        REQUIRE(c::answer_original_keyboard_event(owner,0u)==11u);
        const auto frame=c::inspect(owner);
        REQUIRE(frame.memory_address==base &&
                frame.pending_DOS_service_sequence==
                    entry.pending_DOS_service_sequence+i &&
                frame.original_source_page_pops==2u);
    }
    std::cout<<"ORIGINAL_CODE2_1FC14_TWO_SOURCE_LOOP_ITERATIONS_PASS"
             <<" original_keyboard_poll=30B40"
             <<" no_key_host_replies=2"
             <<" next_service=11"
             <<" guest_memory=PRESERVED"
             <<" game_rendering=NOT_CONNECTED\n"<<std::flush;
    std::cout<<"ORIGINAL_CODE2_SECOND_16D04_SOURCE_PASS"
             <<" entry=20440_16D04"
             <<" INIT_reopens=2"
             <<" original_app30_file_callbacks=YES"
             <<" source_guest_memory=PRESERVED"
             <<" next_host_service="<<next
             <<" gameplay_loop=NOT_VERIFIED\n"<<std::flush;
    return next;
}
void dispatch_pending(Opaque* s) {
    if(!s || !s->initialized)
        throw std::invalid_argument("CODE2 startup guest is not initialized");
    if(s->guest->m.data!=s->guest->memory.data() ||
       rloft::canonical_code2_runtime::inspect(s->canonical.get()).memory_address
           !=s->guest->memory.data())
        throw std::runtime_error("CODE2 source runtime lost its guest memory");
    ++s->dispatches;
    // 353A0/20608 has executed to the first unimplemented DOS 375FE
    // vector-install callback. There is no resumable original C++ stack
    // after the bounded longjmp stop, so NO fake original 1FC14 tick.
}
void destroy(Opaque* s) noexcept {delete s;}
}} // namespace rloft::original_code2_session

int original_code2_resource_main(int argc,char** argv) {
    try {
        if(argc==2 && std::string(argv[1])=="--test-indexed-31334") {
            rloft_code2_resource_internal::Session fixture("","");
            fixture.boot();
            fixture.verify_synthetic_first_indexed_record();
            return 0;
        }
        const bool palette=(argc==7 && std::string(argv[1])=="--root" &&
            std::string(argv[3])=="--cd-root" &&
            std::string(argv[5])=="--palette-level");
        if(argc!=1 && !(argc==3 && std::string(argv[1])=="--root") &&
           !(argc==5 && std::string(argv[1])=="--root" &&
             std::string(argv[3])=="--cd-root") && !palette)
            throw std::invalid_argument("usage: rloft_original_code2_init_resources [--root INSTALL_DIR [--cd-root CD_RLOFT_DIR [--palette-level LEVEL]]]");
        const unsigned level=palette?
            static_cast<unsigned>(std::stoul(argv[6])):0u;
        if(palette && (level>=32u || argv[6][0]=='-'))
            throw std::invalid_argument("Palette scene level must be 0..31");
        rloft_code2_resource_internal::Session s(argc==1?"":argv[2],argc>=5?argv[4]:"");
        if(palette)s.seed_authenticated_code2(std::filesystem::path(argv[2])/"CODE.2");
        s.boot();
        if(palette)s.source_palette_from_scene(argv[2],level);
        return 0;
    }catch(const std::exception& e){
        std::cerr<<"SOURCE_RESOURCE_STARTUP_FAIL: "<<e.what()<<'\n';return 1;
    }
}
#ifndef RLOFT_BUNDLE_NO_MAIN
int main(int argc,char** argv) {return original_code2_resource_main(argc,argv);}
#endif
