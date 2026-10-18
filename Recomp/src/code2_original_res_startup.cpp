// Source-backed startup checkpoint: CODE.2 16D04 INIT header and 2E768/2E7F0
// INFO.DAT -> eleven original RES0..RES10 archive slot tables.
// This executable is an isolated CODE.2 subsystem gate, NOT CODE.1->game boot.
#include "../../Code2/Code2.cpp"
#include <algorithm>
#include <array>
#include <cstdint>
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
int main(int argc,char** argv) {
    try {
        if(argc!=1 && !(argc==3 && std::string(argv[1])=="--root") &&
           !(argc==5 && std::string(argv[1])=="--root" && std::string(argv[3])=="--cd-root"))
            throw std::invalid_argument("usage: rloft_original_code2_init_resources [--root INSTALL_DIR [--cd-root CD_RLOFT_DIR]]");
        Session s(argc==1?"":argv[2],argc==5?argv[4]:"");
        s.boot();return 0;
    }catch(const std::exception& e){
        std::cerr<<"SOURCE_RESOURCE_STARTUP_FAIL: "<<e.what()<<'\n';return 1;
    }
}
