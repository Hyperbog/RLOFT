// Run actual compiled, reconstructed CODE.2 graphics code against a bounded VGA
// port/aperture host. Optional owner-provided RES images run through the
// original compiled resource lookup and image layout/draw routines.
#include "../../Code2/Code2.cpp"
#include "original_vga_host.hpp"
#include "original_res_image_archive.hpp"
#include <array>
#include <algorithm>
#include <cstdint>
#include <iterator>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
using u8=std::uint8_t; using u16=std::uint16_t; using u32=std::uint32_t;
#define CHECK(expr) do { if (!(expr)) throw std::runtime_error("CODE2 graphics: " #expr); } while(false)
struct Session {
    rloft::graphics::OriginalVgaHost video{};
    std::vector<u8> memory=std::vector<u8>(4u*1024u*1024u,0);
    code2::Machine machine{};
    code2::Graphics2Boundary gfx{};
    code2::Application61Boundary hardware{};
    code2::Application68Boundary resource_loader{};
    code2::Application69Boundary resources{};
    code2::Application57Boundary renderer{};
    std::uint32_t resident_resource_id=0, resident_slot=0, resident_image=0;
    unsigned resolver_calls=0, draw_calls=0;
    bool unsupported=false;
    static Session* live;
    static Session& state() { if (!live) throw std::logic_error("No active graphics session"); return *live; }
    static bool vram(u32 a) { return a>=0xA0000u && a<0xB0000u; }
    static u8 read8(u32 address) {
        auto& s=state();
        if (vram(address)) return s.video.read_aperture(static_cast<u16>(address));
        if (address<s.memory.size()) return s.memory[address];
        s.unsupported=true; return 0;
    }
    static void write8(u32 address,u8 value) {
        auto& s=state();
        if (vram(address)) { if(!s.video.write_aperture(static_cast<u16>(address),value)) s.unsupported=true; return; }
        if (address<s.memory.size()) { s.memory[address]=value; return; }
        s.unsupported=true;
    }
    static u16 read16(u32 a) { return static_cast<u16>(read8(a)|(u16(read8(a+1u))<<8u)); }
    static u32 read32(u32 a) { return u32(read16(a))|(u32(read16(a+2u))<<16u); }
    static void write16(u32 a,u16 v) { write8(a,static_cast<u8>(v));write8(a+1,static_cast<u8>(v>>8)); }
    static void write32(u32 a,u32 v) {write16(a,static_cast<u16>(v));write16(a+2,static_cast<u16>(v>>16));}
    static u8 in8(u16 p) {return state().video.read_port8(p);}
    static void out8(u16 p,u8 v) {if (!state().video.write_port8(p,v))state().unsupported=true;}
    static void out16(u16 p,u16 v) {if (!state().video.write_indexed_port(p,v))state().unsupported=true;}
    static u8 bios_get_mode() {return state().video.bios_mode();}
    static void bios_set_mode(u8 v) {state().video.bios_set_mode(v);}
    static void translate(u32 row,u32 x,u32* addr,u8* plane) {
        const auto a=code2::function_2ab38(state().machine,row,x);
        *addr=a.addr;*plane=static_cast<u8>(a.plane);
    }
    static void sync_clear() {code2::function_2ab98(state().machine,state().hardware);}
    static void sync_retrace(u32) {code2::function_2abc4(state().hardware);}
    static void set_page(u32 page) {code2::function_2abf4(state().hardware,page);}
    static void set_split(u32 row) {code2::function_2ac10(state().machine,state().hardware,row);}
    // The real source 2975C issues 2F29C(set/key), which checks the
    // original 9-byte item-cache record via 2ECB4. We prepopulate the
    // already-loaded cache state after obtaining the user's RES image through
    // a read-only host file adapter. The original DOS file loader is separate.
    static u32 resolve_item(u32 item,u32 slot) {
        auto& s=state();
        ++s.resolver_calls;
        if(item!=s.resident_resource_id||slot!=s.resident_slot)
            throw std::runtime_error("Requested unbound original RES entry");
        return code2::function_2f29c(s.machine,s.resources,item,slot);
    }
    static u32 draw_image(u32 frame,u32 y,u32 x,u32 image) {
        ++state().draw_calls;
        code2::function_2a380(state().machine,state().gfx,frame,y,x,image);
        return 1u;
    }
    u32 cached_image(u32 slot,u32 item,const rloft::original_res::Image& image) {
        if(slot>12u || item>8191u) throw std::runtime_error("RES slot/index exceeds safe guest cache");
        const u32 descriptor=0x180000u;
        const u32 scratch=0x60000u;
        const u32 per_frame=image.frame_size();
        const u32 stride=per_frame+(image.tag==0xFFu?0u:1u);
        const u32 total=stride*image.frames;
        if(std::uint64_t(descriptor)+8u+total>memory.size() ||
           std::uint64_t(scratch)+image.decoded.size()>descriptor)
            throw std::runtime_error("Original image exceeds bounded guest arena");
        code2::store_le16(memory.data()+descriptor,static_cast<u16>(stride));
        code2::store_le16(memory.data()+descriptor+2u,image.frames);
        code2::store_le16(memory.data()+descriptor+4u,image.rows);
        memory[descriptor+6u]=image.width_units;
        memory[descriptor+7u]=image.tag==0xFFu?0u:0x80u;
        for(u32 frame=0;frame<image.frames;++frame) {
            const u32 at=scratch+per_frame*frame;
            std::copy_n(image.decoded.begin()+per_frame*frame,per_frame,memory.begin()+at);
            code2::function_2a20c(gfx,frame,per_frame,at,descriptor,image.tag);
        }
        if(image.reorder!=0u) {
            // Original CODE.2 loader 2FD9C finishes nonzero field_7
            // image descriptors via source-derived 37810 planar permutation.
            std::vector<u16> scratch_words(4u*image.rows);
            code2::Runtime127Boundary order{};
            order.permutation_words=scratch_words.data();
            code2::function_37810(order,memory.data()+descriptor);
        }
        constexpr u32 slot_base=0x30000u, table_base=0x31000u;
        const u32 record=slot_base+slot*16u;
        const u32 item_rec=table_base+item*9u;
        CHECK(item_rec+9u<scratch);
        code2::store_le32(memory.data()+0xF0B8u,slot_base);
        code2::store_le32(memory.data()+0xF0B4u,slot+1u);
        code2::store_le32(memory.data()+record+8u,item+1u);
        code2::store_le32(memory.data()+record+12u,table_base);
        memory[item_rec]=1u;
        memory[item_rec+2u]=1u;  // cache: loaded, CODE.2 2ECB4 skips file open
        code2::store_le32(memory.data()+item_rec+5u,descriptor);
        resident_resource_id=item;
        resident_slot=slot;
        resident_image=descriptor;
        return descriptor;
    }
    void render_cached(const rloft::original_res::Image& image,u32 slot,u32 frame) {
        CHECK(frame<image.frames);
        cached_image(slot,image.archive_entry,image);
        const u32 width=image.width_units*8u;
        const u32 x=(320u-width)/2u, y=(400u-image.rows)/2u;
        resolver_calls=0;draw_calls=0;
        (void)code2::function_2975c(renderer,frame,y,x,image.archive_entry,slot);
        CHECK(resolver_calls==1u && draw_calls==1u);
        CHECK(!unsupported && !video.unsupported_read());
    }
    Session() {
        if (live) throw std::logic_error("Graphics adapter supports one active CODE.2 session");
        live=this;
        machine.data=memory.data();
        hardware.in8=&in8;hardware.out8=&out8;hardware.out16=&out16;
        hardware.vga_read8=&read8;hardware.vga_write8=&write8;
        gfx.read8=&read8;gfx.read16=&read16;gfx.read32=&read32;
        gfx.write8=&write8;gfx.write16=&write16;gfx.write32=&write32;
        gfx.in8=&in8;gfx.out8=&out8;gfx.out16=&out16;
        gfx.sync_2ab98=&sync_clear;gfx.sync_2abc4=&sync_retrace;
        gfx.call_2abf4=&set_page;gfx.call_2ac10=&set_split;
        gfx.bios_get_mode=&bios_get_mode;gfx.bios_set_mode=&bios_set_mode;
        gfx.translate_2ab38=&translate;
        resources.app68=&resource_loader;
        renderer.call_2f29c=&resolve_item;
        renderer.call_2a380=&draw_image;
    }
    ~Session() {live=nullptr;}
    void mode(u32 mode) {
        const u32 result=code2::function_29930(machine,gfx,mode,0x16000u);
        CHECK(result==0x0Du);
        CHECK(code2::load_le16(memory.data()+0x16003u)==(mode==3u?400u:200u));
        CHECK(code2::load_le16(memory.data()+0x16005u)==320u);
        CHECK(!unsupported);
        CHECK(!video.unsupported_read());
    }
    void indexed_palette() {
        // CODE.2 29CA0 streams RGB triplets via the original OUT callbacks.
        constexpr u32 src=0x17000u;
        for (unsigned i=0;i<64;++i) {
            memory[src+i*3+0]=static_cast<u8>(i);
            memory[src+i*3+1]=static_cast<u8>(63u-i);
            memory[src+i*3+2]=static_cast<u8>(i/2u);
        }
        code2::function_29ca0(gfx,src,30u,64u*3u);
        CHECK(video.palette(30).r==0 && video.palette(30).g==63);
        CHECK(video.palette(93).r==63 && video.palette(93).g==0);
    }
    void verify_original_brightness_transform() {
        // CODE.2 32150 is compiled and executed, not reimplemented.
        // Byte 6C of the active game profile selects intensity 0..63.
        constexpr u32 palette=0x1B000u,profile=0x1C000u;
        code2::store_le32(memory.data()+0xF050u,profile);
        memory[profile+0x6Cu]=31u;
        for (unsigned i=0;i<256u;++i) {
            memory[palette+i*3]=static_cast<u8>(i%64u);
            memory[palette+i*3+1u]=static_cast<u8>(63u-i%64u);
            memory[palette+i*3+2u]=static_cast<u8>((i%64u)/2u);
        }
        const u8 before=memory[palette+32u*3u];
        code2::function_32150(machine,palette);
        CHECK(memory[palette+32u*3u]>before);
        CHECK(memory[palette+0u]==0u);
        CHECK(memory[palette+1u]==63u);
        memory[profile+0x6Cu]=0u;
    }
    void owned_colors_palette(const std::string& path,unsigned bank,unsigned strength) {
        // Read-only user-owned COLORS palette bank:
        // byte 0 = 15; bank N is 768 RGB6 bytes at 1 + 768*N.
        // CODE.2 325B0 uses that exact bank offset, applies 32150, and
        // streams the bank to the indexed DAC through its graphics path.
        CHECK(bank<15u && strength<=63u);
        std::ifstream f(path,std::ios::binary);
        if (!f) throw std::runtime_error("Cannot open owner-provided COLORS file");
        const std::vector<u8> content{std::istreambuf_iterator<char>(f),
                                      std::istreambuf_iterator<char>()};
        if (content.size()!=11521u || content[0]!=15u)
            throw std::runtime_error("Unsupported COLORS bank structure");
        constexpr u32 destination=0x17000u,profile=0x1C000u;
        const auto start=content.begin()+1+static_cast<std::ptrdiff_t>(bank*768u);
        std::copy_n(start,768,memory.begin()+destination);
        code2::store_le32(memory.data()+0xF050u,profile);
        memory[profile+0x6Cu]=static_cast<u8>(strength);
        code2::function_32150(machine,destination);
        code2::function_29ca0(gfx,destination,0u,768u);
        CHECK(!unsupported && !video.unsupported_read());
    }
    void seed_image() {
        // Synthetic image in the original CODE.2 planar image wire layout.
        // Two frames, four planes, 20 rows and 2 bytes per row/plane.
        constexpr u32 start=0x18000u;
        code2::store_le16(memory.data()+start,160u); // frame stride
        code2::store_le16(memory.data()+start+4,20u);
        memory[start+6]=1u; // half width of two bytes per plane
        memory[start+7]=0u; // uncompressed, not keyed transparent
        for (u32 frame=0;frame<2;++frame)
            for (u32 plane=0;plane<4;++plane)
                for (u32 row=0;row<20;++row) {
                    const u32 dest=start+8u+frame*160u+plane*40u+row*2u;
                    const u32 base=(plane*5u+row+frame*20u)%64u;
                    memory[dest]=static_cast<u8>(30u+base);
                    memory[dest+1u]=static_cast<u8>(30u+(base+1u)%64u);
                }
    }
};
Session* Session::live=nullptr;

void put_u16(std::ofstream& out,u16 value) {
    out.put(static_cast<char>(value));out.put(static_cast<char>(value>>8));
}
void put_u32(std::ofstream& out,u32 value) {
    put_u16(out,static_cast<u16>(value));put_u16(out,static_cast<u16>(value>>16));
}
void write_frame(const std::string& stem,const rloft::graphics::OriginalVgaHost& video) {
    const std::vector<u8> pixels=video.scanout_indices();
    CHECK(pixels.size()==320u*400u);
    {
        std::ofstream f(stem+".idx",std::ios::binary|std::ios::trunc);
        CHECK(bool(f));f.write(reinterpret_cast<const char*>(pixels.data()),
                               static_cast<std::streamsize>(pixels.size()));
        CHECK(bool(f));
    }
    // Windows BMP 8-bit palettized, bottom-up. The indexed file is raw
    // 320x400 bytes; this BMP keeps a 256-entry palette for visual inspection.
    std::ofstream bmp(stem+".bmp",std::ios::binary|std::ios::trunc);
    CHECK(bool(bmp));
    const u32 size=14u+40u+256u*4u+320u*400u;
    bmp.put('B');bmp.put('M');put_u32(bmp,size);
    put_u32(bmp,0u);put_u32(bmp,14u+40u+256u*4u);
    put_u32(bmp,40u);put_u32(bmp,320u);put_u32(bmp,400u);
    put_u16(bmp,1u);put_u16(bmp,8u);put_u32(bmp,0u);
    put_u32(bmp,320u*400u);put_u32(bmp,2835u);put_u32(bmp,2835u);
    put_u32(bmp,256u);put_u32(bmp,0u);
    for (u32 index=0;index<256u;++index) {
        const auto rgb=video.palette(static_cast<u8>(index));
        bmp.put(static_cast<char>(rloft::graphics::dac6_to_byte(rgb.b)));
        bmp.put(static_cast<char>(rloft::graphics::dac6_to_byte(rgb.g)));
        bmp.put(static_cast<char>(rloft::graphics::dac6_to_byte(rgb.r)));
        bmp.put(0);
    }
    for (unsigned row=400;row>0;--row)
        bmp.write(reinterpret_cast<const char*>(pixels.data()+(row-1u)*320u),320);
    CHECK(bool(bmp));
}
} // namespace

int main(int argc,char**argv) {
    try {
        if (argc!=1 && argc!=2 && argc!=5)
            throw std::invalid_argument(
                "usage: rloft_original_code2_graphics_frame [output-stem [COLORS-file bank-index brightness]]");
        unsigned owned_bank=0,brightness=0;
        if (argc==5) {
            owned_bank=static_cast<unsigned>(std::stoul(argv[3]));
            brightness=static_cast<unsigned>(std::stoul(argv[4]));
            if (owned_bank>=15u || brightness>63u)
                throw std::invalid_argument("COLORS bank must be 0..14, brightness 0..63");
        }
        Session s;
        s.mode(3u);
        CHECK(s.video.repeat_scanlines()==1u);
        CHECK(s.video.line_compare()>=400u);
        s.indexed_palette();
        s.verify_original_brightness_transform();
        if (argc==5) s.owned_colors_palette(argv[2],owned_bank,brightness);
        s.seed_image();
        // This is the actual compiled CODE.2 2A380 original planar image
        // source code, operating on a synthetic image record in DGROUP.
        code2::function_2a380(s.machine,s.gfx,0u,10u,24u,0x18000u);
        const auto first=s.video.scanout_indices();
        CHECK(first[10u*320u+24u]==30u);
        CHECK(first[10u*320u+25u]==35u);
        CHECK(first[10u*320u+28u]==31u);
        CHECK(first[11u*320u+24u]==31u);
        // Original 29AA4 pixel path: indexed VGA plane write, read-back 29AF0.
        code2::function_29aa4(s.machine,s.gfx,11u,32u,70u);
        CHECK(code2::function_29af0(s.gfx,11u,32u)==70u);
        code2::function_29aa4(s.machine,s.gfx,0u,2u,70u);
        // Real CODE.2 2AB6C changes drawing base; 2ABF4 changes visible page.
        code2::function_2ab6c(s.machine,1u);
        CHECK(code2::load_le32(s.memory.data()+0xF001u)==0xA8000u);
        code2::function_2a380(s.machine,s.gfx,1u,10u,24u,0x18000u);
        CHECK(s.video.indexed_scanout(24u,10u)==30u);
        code2::function_2abf4(s.hardware,0x8000u);
        CHECK(s.video.indexed_scanout(24u,10u)==50u);
        // Original 2AC10 programs CRTC 18/07/09 line compare for split.
        code2::function_2ac10(s.machine,s.hardware,12u);
        CHECK(s.video.line_compare()==24u);
        CHECK(s.video.indexed_scanout(2u,25u)==70u); // lower split shows page 0
        CHECK(!s.unsupported && !s.video.unsupported_read());
        if (argc==2 || argc==5) write_frame(argv[1],s.video);
        // Mode 2 is a 200-line logical framebuffer doubled to 400 scanlines.
        s.mode(2u);
        CHECK(s.video.repeat_scanlines()==2u);
        code2::function_29aa4(s.machine,s.gfx,5u,24u,70u);
        CHECK(s.video.indexed_scanout(24u,10u)==70u);
        CHECK(s.video.indexed_scanout(24u,11u)==70u);
        CHECK(!s.unsupported && !s.video.unsupported_read());
        std::cout<<"COMPILED_CODE2_29930_29CA0_2A380_29AA4_2AB6C_2ABF4_2AC10_INDEXED_FRAME_PASS\n";
        std::cout<<"SOURCE_GAME_FRAME_ACCEPTED=NO; input textures are synthetic\n";
        return 0;
    } catch(const std::exception& e) {
        std::cerr<<"COMPILED_CODE2_GRAPHICS_FAIL: "<<e.what()<<'\n';
        return 1;
    }
}
