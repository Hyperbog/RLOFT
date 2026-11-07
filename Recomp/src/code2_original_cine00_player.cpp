// CODE.2 cinematic source execution on authentic ANM/T bytes.
// This is a stand-alone bridge, not yet the retained 20440->19888 game boot.
// Video packets run original 13834/135C4 and 27BD8/27AEC/27B84.
// The first integration deliberately uses original no-audio-device state;
// captions are decoded by 1C368 and logged, but not yet font-rendered.
#include "../../Code2/Code2.cpp"
#include "original_vga_host.hpp"
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
using u8=std::uint8_t; using u16=std::uint16_t; using u32=std::uint32_t;
namespace fs=std::filesystem;
using Vga=rloft::graphics::OriginalVgaHost;
[[noreturn]] void fail(const char* reason) noexcept {
    std::fprintf(stderr,"CODE2_CINE00_SOURCE_FAIL: %s\n",reason);
    std::abort(); // All CODE.2 callback ABI slots are noexcept.
}
struct Player {
    std::vector<u8> mem=std::vector<u8>(0x400000u,0u);
    code2::Machine machine{};
    Vga vga{};
    std::ifstream cine,txt;
    u32 ticks=0,frames=0,commands=0,subtitles=0;
    u8 draw_page=0,display_page=0;
    static Player* active;
    Player(const fs::path& cinematic,const fs::path& text)
        :cine(cinematic,std::ios::binary),txt(text,std::ios::binary) {
        if(!cine || !txt)throw std::runtime_error("CINE00.ANM and T are required");
        machine.data=mem.data();
        code2::store_le32(mem.data()+0x7F7Cu,1u);
        code2::store_le32(mem.data()+0x7F84u,0x1F400u);
        code2::store_le32(mem.data()+0x7F94u,1u);
        code2::store_le32(mem.data()+0x22224u,2u);
        // Original DOS no-card branch: no synthetic successful AIL driver.
        code2::store_le32(mem.data()+0xEE78u,0u);
        code2::store_le32(mem.data()+0xEE40u,0u);
        active=this;
    }
    ~Player(){active=nullptr;}
    static Player& self() noexcept {if(!active)fail("no current guest");return *active;}
    static std::ifstream& file(u32 h) noexcept {
        if(h==1u)return self().cine;
        if(h==2u)return self().txt;
        fail("unexpected DOS file handle");
    }
    static void read_bytes(void* ptr,u32 count,u32 handle) noexcept {
        auto& f=file(handle);
        if(count>0x200000u)fail("excessive DOS read");
        if(!f.read(static_cast<char*>(ptr),static_cast<std::streamsize>(count)))
            fail("short original file read");
    }
    static void read_guest(u32 dest,u32 count,u32 handle) noexcept {
        if(dest>self().mem.size() || count>self().mem.size()-dest)
            fail("guest memory read overflow");
        read_bytes(self().mem.data()+dest,count,handle);
    }
    static u32 read_scalar(u32 handle,u32 width) noexcept {
        if(width!=2u && width!=4u)fail("unexpected original scalar width");
        u8 b[4]{};
        read_bytes(b,width,handle);
        return width==2u?code2::load_le16(b):code2::load_le32(b);
    }
    static void seek(u32 offset,u32 origin,u32 handle) noexcept {
        std::ios::seekdir from;
        switch(origin){case 0:from=std::ios::beg;break;
                       case 1:from=std::ios::cur;break;
                       case 2:from=std::ios::end;break;
                       default:fail("invalid original DOS seek");}
        auto& f=file(handle);f.clear();f.seekg(offset,from);
        if(!f)fail("original DOS seek failed");
    }
    static u32 tell_seek(u32 off,u32 origin,u32 h) noexcept {
        seek(off,origin,h);
        auto n=file(h).tellg();
        if(n<0 || static_cast<std::uint64_t>(n)>0xFFFFFFFFULL)
            fail("DOS seek overflow");
        return static_cast<u32>(n);
    }
    static u8 in8(u16 port) noexcept {return self().vga.read_port8(port);}
    static void out8(u16 port,u8 val) noexcept {
        if(!self().vga.write_port8(port,val))fail("unimplemented original VGA output port");
    }
    static void out16(u16 port,u16 val) noexcept {
        if(!self().vga.write_indexed_port(port,val))fail("unimplemented indexed VGA output");
    }
    static u8 phys_read(u32 addr) noexcept {
        if(addr<0xA0000u || addr>=0xB0000u)fail("original VGA aperture read outside range");
        return self().vga.read_aperture(static_cast<u16>(addr-0xA0000u));
    }
    static void phys_write(u32 addr,u8 val) noexcept {
        if(addr<0xA0000u || addr>=0xB0000u ||
           !self().vga.write_aperture(static_cast<u16>(addr-0xA0000u),val))
            fail("original VGA aperture write outside range");
    }
    static code2::Application54Boundary video() noexcept {
        code2::Application54Boundary b{};
        b.in8=&in8;b.out8=&out8;b.out16=&out16;
        b.phys_read8=&phys_read;b.phys_write8=&phys_write;
        return b;
    }
    static void row_rle(u32 p) noexcept {
        if(p>1u)fail("invalid cinematic page");
        auto v=video(); code2::function_27bd8(self().machine,v,p);
    }
    static void sparse_delta(u32 p) noexcept {
        if(p>1u)fail("invalid cinematic page");
        auto v=video(); code2::function_27aec(self().machine,v,p);
    }
    static void raw_planes(u32 p) noexcept {
        if(p>1u)fail("invalid cinematic page");
        auto v=video(); code2::function_27b84(self().machine,v,p);
    }
    static void palette(u32 ptr) noexcept {
        if(ptr>self().mem.size()-768u)fail("palette guest bounds");
        out8(0x3C8,0u);
        for(u32 i=0;i<768u;++i)out8(0x3C9,self().mem[ptr+i]);
    }
    static void set_draw(u32 page) noexcept {
        if(page>1u)fail("invalid draw page");
        self().draw_page=static_cast<u8>(page);
        self().mem[0xEF4Du]=static_cast<u8>(page);
    }
    static void set_display(u32 page) noexcept {
        if(page>1u)fail("invalid display page");
        self().display_page=static_cast<u8>(page);
        self().mem[0xEF4Eu]=static_cast<u8>(page);
        const u16 start=static_cast<u16>(page*0x8000u);
        out16(0x3D4,static_cast<u16>(0x0Cu | ((start>>8)<<8)));
        out16(0x3D4,static_cast<u16>(0x0Du | ((start&255u)<<8)));
    }
    static void clear_rect(u32 y,u32 x,u32 height,u32 width,u32 color) noexcept {
        if(color>255u || y>400u || x>320u ||
           height>400u-y || width>320u-x)fail("clear rectangle out of range");
        const u32 base=static_cast<u32>(self().draw_page)*0x8000u;
        for(u32 row=y;row<y+height;++row)
            for(u32 col=x;col<x+width;++col) {
                out16(0x3C4,static_cast<u16>(0x02u | ((1u<<(col&3u))<<8)));
                phys_write(0xA0000u+base+row*80u+(col>>2),static_cast<u8>(color));
            }
        out16(0x3C4,0x0F02u);
    }
    static void copy_page(u32 target) noexcept {
        if(target>1u)fail("invalid page copy target");
        const u32 src=static_cast<u32>(self().draw_page)*0x8000u;
        const u32 dst=target*0x8000u;
        // DOS VGA plane/read-map host service, never host-derived scene pixels.
        for(u32 plane=0;plane<4u;++plane) {
            out16(0x3CE,static_cast<u16>(4u | (plane<<8)));
            out16(0x3C4,static_cast<u16>(2u | ((1u<<plane)<<8)));
            for(u32 i=0;i<32000u;++i) {
                const u8 v=phys_read(0xA0000u+src+i);
                phys_write(0xA0000u+dst+i,v);
            }
        }
        out16(0x3C4,0x0F02u);
    }
    static void memset_guest(u32 p,u32 value,u32 count) noexcept {
        if(p>self().mem.size() || count>self().mem.size()-p)
            fail("original guest fill bounds");
        std::fill_n(self().mem.data()+p,count,static_cast<u8>(value));
    }
    static void t_seek(u32 off,u32 origin,u32 h) noexcept {seek(off,origin,h);}
    static void read_text(u32 dest,u32 count,u32 h) noexcept {read_guest(dest,count,h);}
    static void decode_text(u32 dest,u32 index) noexcept {
        code2::Application39Boundary b{};
        b.call_3131c=&t_seek;b.call_312ec_local=&read_bytes;
        b.call_312ec_dgroup=&read_text;
        code2::function_1c368(self().machine,b,dest,index);
        ++self().subtitles;
        std::cout<<"CODE2_ORIGINAL_1C368_SUBTITLE index="<<index<<" text=";
        for(u32 i=dest;i<self().mem.size() && i<dest+512u && self().mem[i];++i)
            std::cout<<static_cast<char>(self().mem[i]);
        std::cout<<'\n';
    }
    static void no_audio_direct(code2::s32 value) noexcept {
        code2::Application26Boundary b{};
        code2::function_146b8(b,static_cast<u32>(value));
    }
    static void no_audio_queue(u32 id,u32 aux,u32 slot,u32 priority) noexcept {
        code2::function_28110(self().machine,id,aux,slot,priority);
    }
    static void audio_service() noexcept {
        code2::Application55Boundary b{};
        code2::function_28168(self().machine,b);
    }
    static void no_audio_stop(u32 arg) noexcept {
        code2::Application56Boundary b{};
        code2::function_28dcc(self().machine,b,arg);
    }
    static void no_audio_wait() noexcept {
        code2::Application56Boundary b{};
        code2::function_28ef0(self().machine,b);
    }
    static void no_audio_volume(u32 value) noexcept {
        code2::Application56Boundary b{};
        code2::function_29008(self().machine,b,value);
    }
    static void no_audio_speed(u32 value) noexcept {
        code2::Application56Boundary b{};
        code2::function_28f58(self().machine,b,value);
    }
    static void no_audio_play(code2::s32 res,code2::s32 slot,u32 arg) noexcept {
        code2::Application56Boundary b{};
        code2::function_28e58(self().machine,b,static_cast<u32>(res),
                              static_cast<u32>(slot),arg);
    }
    static void frame_time(u32,u32) noexcept {++self().frames;++self().ticks;}
    static u32 timer() noexcept {return self().ticks;}
    static u32 no_keyboard() noexcept {return 0u;}
    void run(){
        code2::Application25Boundary b{};
        b.read_312ec_value=&read_scalar;b.call_312ec=&read_guest;
        b.call_291b0=&set_draw;b.call_291c8=&set_display;
        b.call_27bd8=&row_rle;b.call_27aec=&sparse_delta;
        b.call_27b84=&raw_planes;b.call_29234=&palette;
        b.call_29b40=&clear_rect;b.call_30d30=&memset_guest;
        b.call_2a020=&copy_page;
        b.call_1c368=&decode_text;
        b.call_28dcc=&no_audio_stop;b.call_28ef0=&no_audio_wait;
        b.call_29008=&no_audio_volume;b.call_28f58=&no_audio_speed;
        b.call_28e58=&no_audio_play;
        b.call_28110=&no_audio_queue;b.call_28168=&audio_service;
        b.call_146b8=&no_audio_direct;
        b.call_2e608=[]() noexcept {};
        b.call_30700=&timer;b.call_2f868=[]() noexcept {};
        b.call_2e614=&frame_time;b.call_30b40=&no_keyboard;
        // Font rasterization is deliberately absent, rather than falsely
        // claiming title-quality subtitle pixels from a substitute font.
        const u32 result=code2::function_13834(machine,b);
        if(result!=0u || frames!=405u)
            throw std::runtime_error("original cinematic source did not finish all 405 frames");
        std::cout<<"CODE2_CINE00_ORIGINAL_13834_PASS frames="<<frames
                 <<" subtitles="<<subtitles
                 <<" audio=ORIGINAL_NO_DEVICE"
                 <<" text_pixels=NOT_CONNECTED"
                 <<" title_menu=NOT_CONNECTED\n";
    }
    void ppm(const fs::path& path) {
        std::ofstream f(path,std::ios::binary);
        if(!f)throw std::runtime_error("cannot write requested PPM");
        f<<"P6\n320 400\n255\n";
        for(u32 y=0;y<400u;++y)for(u32 x=0;x<320u;++x) {
            const auto c=vga.rgb_at(x,y);
            const std::array<char,3> rgb{static_cast<char>(c.r),
                                         static_cast<char>(c.g),
                                         static_cast<char>(c.b)};
            f.write(rgb.data(),3);
        }
        if(!f)throw std::runtime_error("partial PPM output");
    }
};
Player* Player::active=nullptr;
} // namespace

int main(int argc,char** argv){
    try {
        if(argc!=3 && argc!=4)
            throw std::invalid_argument("usage: rloft_original_code2_cine00_player CINE00.ANM T [LAST_FRAME.ppm]");
        Player player(argv[1],argv[2]);
        player.run();
        if(argc==4)player.ppm(argv[3]);
        return 0;
    }catch(const std::exception& e) {
        std::cerr<<"CODE2_CINE00_SOURCE_ERROR: "<<e.what()<<'\n';
        return 1;
    }
}
