// Source-backed startup checkpoint: CODE.2 16D04 INIT header and 2E768/2E7F0
// INFO.DAT -> eleven original RES0..RES10 archive slot tables.
// This executable is an isolated CODE.2 subsystem gate, NOT CODE.1->game boot.
#include "../../Code2/Code2.cpp"
#include "original_code2_palette_loader.hpp"
#include "original_code2_scene_palette.hpp"
#include "original_code2_guest_session.hpp"
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

namespace {
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
    std::vector<u8> memory=std::vector<u8>(16u*1024u*1024u);
    code2::Machine m{};
    code2::Application30Boundary init_callbacks{};
    code2::Application67Boundary info_callbacks{};
    code2::Application68Boundary res_callbacks{};
    struct Handle {unsigned file; std::size_t cursor;};
    std::map<u32,Handle> files{};
    u32 heap_cursor=0x400000u;
    unsigned init_opens=0,info_opens=0,res_opens=0;
    u32 music_base=0;
    bool error=false;
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
                 <<" compiled_getenv=352EE"\n                 <<" synthetic_environment_answer=NO"
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
        const auto aligned=(std::uint64_t(t.heap_cursor)+7u)&~std::uint64_t(7);
        if(!bytes || aligned+bytes>t.memory.size()){t.error=true;return 0;}
        t.heap_cursor=static_cast<u32>(aligned+bytes);
        std::fill_n(t.memory.begin()+static_cast<std::ptrdiff_t>(aligned),bytes,0);
        return static_cast<u32>(aligned);
    }
    static void free_guest(u32) {}
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
        const auto it=t.files.find(handle);
        if(it==t.files.end() || origin!=0 || offset>t.fixture.data[it->second.file].size()){
            t.error=true;return 0u;
        }
        it->second.cursor=offset;return offset;
    }
    static u32 copy_handle(u8* destination,u32 size,u32 handle) {
        auto& t=state();
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
    Session(std::string root,std::string cd_root):fixture(root,cd_root) {
        REQUIRE(current==nullptr);current=this;m.data=memory.data();
        guest_string(0x718u,"INIT");
        guest_string(0x1384u,"INFO.DAT");
        guest_string(0x26000u,"RES");
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
    void boot() {
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
}

namespace rloft { namespace original_code2_session {
struct Opaque {
    Session guest;
    std::uint64_t dispatches=0;
    bool initialized=false;
    explicit Opaque(const std::filesystem::path& root,
                    const std::filesystem::path& cd,unsigned level)
        :guest(root.string(),cd.string()) {
        if(level>=32u)throw std::out_of_range("CODE.2 scene level out of range");
        guest.seed_authenticated_code2(root/"CODE.2");
        guest.execute_real_20608_to_first_dependency();
        guest.boot();
        guest.source_palette_from_scene(root,level);
        initialized=true;
    }
};
static std::uint64_t guest_source_fingerprint(const Opaque& s) {
    // FNV-1a over source guest memory is an in-process state continuity
    // fingerprint, NOT a cryptographic content authenticator or DOS trace.
    std::uint64_t hash=14695981039346656037ULL;
    for(const auto b:s.guest.memory) {
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
    if(!s)throw std::invalid_argument("Null original CODE2 guest");
    if(s->guest.m.data!=s->guest.memory.data())
        throw std::runtime_error("CODE2 guest memory base changed");
    Evidence e{};
    e.memory_address=s->guest.memory.data();
    e.source_state_fingerprint=guest_source_fingerprint(*s);
    e.resource_cache_base=code2::load_le32(s->guest.memory.data()+0xF0B8u);
    e.resource_archives=code2::load_le32(s->guest.memory.data()+0xF0B4u);
    e.selected_palette=code2::load_le32(s->guest.memory.data()+0xC034u);
    e.vga_port_writes=s->guest.video_port_writes;
    e.dispatch_calls=s->dispatches;
    e.initialized=s->initialized;
    e.original_20608_executed_to_host_dependency=
        s->guest.original_main_reached_environment;
    e.original_352ee_getenv_calls=
        s->guest.original_game_getenv_callbacks;
    e.original_game_loop_connected=false;
    return e;
}
void dispatch_pending(Opaque* s) {
    if(!s || !s->initialized)
        throw std::invalid_argument("CODE2 startup guest is not initialized");
    if(s->guest.m.data!=s->guest.memory.data())
        throw std::runtime_error("CODE2 guest lost its original state");
    ++s->dispatches;
    // The original CODE.2 20608 -> 1FC14 world loop is not yet
    // connected to the source INIT/RES guest; do NOT execute a guessed
    // frame, reset memory, send artificial keys, or claim process exit.
}
void destroy(Opaque* s) noexcept {delete s;}
}} // namespace rloft::original_code2_session

int original_code2_resource_main(int argc,char** argv) {
    try {
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
        Session s(argc==1?"":argv[2],argc>=5?argv[4]:"");
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
