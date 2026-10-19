// Run actual compiled, reconstructed CODE.2 graphics code against a bounded VGA
// port/aperture host. Optional owner-provided RES images run through the
// original compiled resource lookup and image layout/draw routines.
#ifndef RLOFT_CANONICAL_CODE2_INCLUDED
#include "../../Code2/Code2.cpp"
#endif
#include "original_vga_host.hpp"
#include "../../Code2/graphics/original_res_image_archive.hpp"
#include "../../Code2/graphics/original_37700_control.hpp"
#include "../../Code2/graphics/original_res_type6.hpp"
#include <array>
#include <algorithm>
#include <cstdint>
#include <iterator>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
using u8=std::uint8_t; using u16=std::uint16_t; using u32=std::uint32_t;
#define CHECK(expr) do { if (!(expr)) throw std::runtime_error("CODE2 graphics: " #expr); } while(false)
struct Session {
    rloft::graphics::OriginalVgaHost video{};
    std::vector<u8> memory=std::vector<u8>(8u*1024u*1024u,0);
    code2::Machine machine{};
    code2::Graphics2Boundary gfx{};
    code2::Application61Boundary hardware{};
    code2::Application68Boundary resource_loader{};
    code2::Application69Boundary resources{};
    code2::Application57Boundary renderer{};
    code2::Application83Boundary original_compositor{};
    unsigned sweep_flushes=0;
    std::uint32_t resident_resource_id=0, resident_slot=0, resident_image=0;
    unsigned resolver_calls=0, draw_calls=0;
    bool unsupported=false;
    bool trace_original_res=false;
    // Read-only RES stream, the CODE.2 guest file handle remains opaque.
    std::vector<u8> archive_bytes{};
    std::vector<u32> entry_offsets{};
    u32 stream_cursor=0, active_entry_end=0, active_entry_start=0;
    u32 heap_cursor=0x400000u;
    u32 stream_handle=0x51u;
    std::string expected_archive_basename{};
    unsigned startup_open_calls=0;
    unsigned file_seeks=0,file_reads=0,source_2fd9c_calls=0;
    unsigned bounded_lzss_callbacks=0;
    unsigned source_type6_load_calls=0;
    bool stream_error=false;
    code2::Application70Boundary image_loader{};
    code2::Runtime125Boundary type6_source{};
    static void fail_stream() noexcept {state().stream_error=true;}
    static u32 alloc_guest(u32 bytes) noexcept {
        auto& s=state();
        const auto aligned=(std::uint64_t(s.heap_cursor)+7u)&~std::uint64_t(7);
        if(bytes==0 || aligned+bytes>s.memory.size()) {s.stream_error=true;return 0u;}
        s.heap_cursor=static_cast<u32>(aligned+bytes);
        return static_cast<u32>(aligned);
    }
    static void free_guest(u32) noexcept {} // bounded arena lives for session
    static code2::s32 open_archive(const u8* filename) noexcept {
        auto& s=state();
        std::size_t count=0;
        while(count<0x54u && filename[count]!=0u) ++count;
        if(count==0u || count>=0x54u ||
            std::string(reinterpret_cast<const char*>(filename),count)!=
            s.expected_archive_basename) {
            fail_stream();return -1;
        }
        // The owner-provided archive has already been opened read-only by
        // the host. This maps source DOS-open to its bounded virtual handle.
        ++s.startup_open_calls;
        s.stream_cursor=0u;
        return static_cast<code2::s32>(s.stream_handle);
    }
    static void seek_item(u32 index,u32 handle) noexcept {
        auto& s=state();
        if(handle!=s.stream_handle || index+1u>=s.entry_offsets.size()) {fail_stream();return;}
        s.active_entry_start=s.entry_offsets[index];
        s.active_entry_end=s.entry_offsets[index+1u];
        s.stream_cursor=s.active_entry_start;
        ++s.file_seeks;
    }
    static u32 read_file(u32 dst,u32 size,u32 handle) noexcept {
        auto& s=state();
        if(handle!=s.stream_handle || std::uint64_t(dst)+size>s.memory.size() ||
           s.stream_cursor>s.archive_bytes.size()) {fail_stream();return 0u;}
        const u32 count=std::min<u32>(size,
           static_cast<u32>(s.archive_bytes.size()-s.stream_cursor));
        if(count) std::copy_n(s.archive_bytes.data()+s.stream_cursor,count,
                              s.memory.data()+dst);
        s.stream_cursor+=count;
        ++s.file_reads;
        return count;
    }
    static void read_resource_header(u32 dst,u32 size,u32 handle) noexcept {
        (void)read_file(dst,size,handle);
    }
    static u32 position_file(u32 handle,u32 origin,u32 delta) noexcept {
        auto& s=state();
        if(handle!=s.stream_handle || origin!=1u || delta!=0u) {fail_stream();return 0u;}
        return s.stream_cursor;
    }
    static u32 logical_file_position() noexcept {
        auto& s=state();
        const u32 buffered=code2::load_le32(s.memory.data()+0xF4D8u);
        if(buffered>s.stream_cursor) {fail_stream();return 0u;}
        return s.stream_cursor-buffered;
    }
    static u8 original_buffered_byte() noexcept {
        auto& s=state();
        if(s.stream_error || logical_file_position()>=s.active_entry_end) {
            fail_stream();return 0u;
        }
        return code2::function_2f9e4(s.machine,s.image_loader);
    }
    // Invoke canonical CODE.2 37700; the recovered implementation reads the
    // live guest 2B6C2 stop flag and bounds malformed/unfinished streams.
    static bool input_37700(u8* output) noexcept {
        auto& s=state();
        if(s.stream_error || logical_file_position()>=s.active_entry_end)
            return false;
        *output=code2::function_2f9e4(s.machine,s.image_loader);
        return !s.stream_error;
    }
    static void output_37700(u8 value) noexcept {
        auto& s=state();
        code2::function_2fa30(s.machine,s.image_loader,value);
    }
    static u32 recovered_37700() noexcept {
        auto& s=state();
        ++s.bounded_lzss_callbacks;
        const u32 desc=code2::load_le32(s.memory.data()+0x2B6A4u);
        const u32 count=desc?code2::load_le16(s.memory.data()+desc):1u;
        const u32 per=code2::load_le32(s.memory.data()+0x2B6B4u);
        code2::Runtime126Boundary boundary{};
        boundary.bounded_read=&input_37700;
        boundary.put_output=&output_37700;
        const u32 completed=code2::function_37700(
            s.machine,boundary,static_cast<u32>(std::min<std::uint64_t>(
                std::uint64_t(count)*per+18u,4u*1024u*1024u)));
        if(completed!=1u || logical_file_position()!=s.active_entry_end)
            fail_stream();
        return completed;
    }
    static void reorder_source_image(u32 desc) noexcept {
        auto& s=state();
        const u32 frames=code2::load_le16(s.memory.data()+desc+2u);
        std::vector<u16> table(4u*frames);
        code2::Runtime127Boundary boundary{};
        boundary.permutation_words=table.data();
        code2::function_37810(boundary,s.memory.data()+desc);
    }
    static u32 source_image_loader(u32 record,u32 handle) noexcept {
        auto& s=state();
        ++s.source_2fd9c_calls;
        if(handle!=s.stream_handle) {fail_stream();return 0u;}
        const u32 image=code2::function_2fd9c(s.machine,s.image_loader,record,handle);
        if(s.stream_error || image==0u || logical_file_position()!=s.active_entry_end)
            fail_stream();
        return image;
    }
    // RES type 2 is the retail 301A8 font loader, not an ordinary
    // 2FD9C image. It allocates a font descriptor (glyph-width table
    // and image pointer) and decodes its embedded image using 2FD9C.
    static u32 source_font_loader(u32 record,u32 handle) noexcept {
        auto& s=state();
        if(handle!=s.stream_handle || s.stream_error ||
           std::uint64_t(record)+9u>s.memory.size()) {
            fail_stream();return 0u;
        }
        const u32 payload=code2::load_le32(s.memory.data()+record+5u);
        const u32 span=code2::load_le16(s.memory.data()+record+3u);
        if(span<8u || payload<8u ||
           std::uint64_t(payload)+span>s.memory.size()) {
            fail_stream();return 0u;
        }
        const u32 result=code2::function_301a8(
            s.machine,s.image_loader,record,handle);
        if(!result || s.stream_error ||
           logical_file_position()!=s.active_entry_end) {
            fail_stream();return 0u;
        }
        return result;
    }
    // Compile and execute the original 37590 type-6 loader; the header
    // pointer passed through 2ECB4 is distinct from the DOS stream handle.
    static u16 type6_source_read_word(u32 header) noexcept {
        auto& s=state();
        if(header!=0x2B670u || s.stream_cursor+2u>s.active_entry_end){
            fail_stream();return 0u;
        }
        constexpr u32 guest_len=0x2B680u;
        if(read_file(guest_len,2u,s.stream_handle)!=2u){fail_stream();return 0u;}
        return code2::load_le16(s.memory.data()+guest_len);
    }
    static void type6_source_set_stream(u32 stream) noexcept {
        auto& s=state();
        code2::function_2f960(s.machine,stream);
    }
    static void type6_source_align() noexcept {
        auto& s=state();
        code2::function_2f96c(s.machine,s.image_loader);
    }
    static void type6_source_decompress(u32 out,u32 desc,u32 data,u32 count,u32 mode) noexcept {
        auto& s=state();
        (void)code2::function_2fb04(s.machine,s.image_loader,out,desc,data,count,mode);
    }
    static u32 type6_loader(u32 header,u32 handle) noexcept {
        auto& s=state();
        ++s.source_type6_load_calls;
        const u8 kind=*code2::dg(s.machine,header+2u);
        if(handle!=s.stream_handle || header!=0x2B670u ||
           (kind!=6u && kind!=7u)){fail_stream();return 0u;}
        const u32 allocated=code2::function_37590(s.type6_source,header,handle);
        if(!allocated || s.stream_error || logical_file_position()!=s.active_entry_end)
            fail_stream();
        return allocated;
    }
    void open_readonly_resource(const std::string& filename,u32 item,u32 slot) {
        archive_bytes=rloft::original_res::read_only(filename);
        if(archive_bytes.size()<8u)throw std::runtime_error("RES file too short");
        const u32 count=rloft::original_res::le32(archive_bytes.data());
        if(count==0u || count>200000u || std::uint64_t(4u)+4ull*count>archive_bytes.size() ||
           item>=count || slot>12u)
            throw std::runtime_error("Invalid original RES directory");
        entry_offsets.clear();
        for(u32 i=0;i<count;++i)
            entry_offsets.push_back(rloft::original_res::le32(archive_bytes.data()+4u+4u*i));
        entry_offsets.push_back(static_cast<u32>(archive_bytes.size()));
        if(entry_offsets[0]!=4u+4u*count)
            throw std::runtime_error("Invalid original RES first entry offset");
        for(unsigned i=1;i<entry_offsets.size();++i)
            if(entry_offsets[i]<entry_offsets[i-1] ||
               entry_offsets[i]>archive_bytes.size())
                throw std::runtime_error("Invalid RES offset ordering");
        // Use source CODE.2 2E7F0 archive initialization instead of manually
        // inserting the 16-byte slot and nine-byte unloaded entry.
        // The host simulates all slots before the requested one with the
        // same owner-supplied archive. Complete multi-file game archive
        // discovery is not yet wired; this verifies the original initializer.
        const auto name_at=filename.find_last_of("/\\");
        expected_archive_basename=filename.substr(name_at==std::string::npos?0:name_at+1);
        if(expected_archive_basename.empty()||expected_archive_basename.size()>=0x54u)
            throw std::runtime_error("Invalid original RES basename");
        constexpr u32 guest_basename=0x25000u;
        std::copy(expected_archive_basename.begin(),expected_archive_basename.end(),
                  memory.begin()+guest_basename);
        memory[guest_basename+expected_archive_basename.size()]=0u;
        file_reads=0u;startup_open_calls=0u;stream_cursor=0u;stream_error=false;
        code2::function_2e7f0(machine,resource_loader,guest_basename,slot+1u);
        const u32 base=code2::load_le32(memory.data()+0xF0B8u);
        const u32 record=base+slot*16u;
        const u32 item_table=code2::load_le32(memory.data()+record+12u);
        if(stream_error || startup_open_calls!=slot+1u ||
           code2::load_le32(memory.data()+record+8u)!=count ||
           code2::load_le32(memory.data()+record)!=stream_handle ||
           item_table==0u || memory[item_table+9u*item+2u]!=0u)
            throw std::runtime_error("Source 2E7F0 failed archive startup");
        resident_resource_id=item;resident_slot=slot;
        stream_cursor=0u;active_entry_end=0u;
    }
    void render_unloaded(const rloft::original_res::Image& image,
                         const std::string& path,u32 slot,u32 frame) {
        CHECK(frame<image.frames);
        open_readonly_resource(path,image.archive_entry,slot);
        // The source path is opaque to the host renderer: look up the actual
        // original unloaded resource on its first draw request.
        resolver_calls=0;draw_calls=0;file_seeks=0;file_reads=0;
        const u32 x=(320u-image.width_units*8u)/2u;
        const u32 y=(400u-image.rows)/2u;
        (void)code2::function_2975c(renderer,frame,y,x,image.archive_entry,slot);
        CHECK(resolver_calls==1u && draw_calls==1u);
        CHECK(source_2fd9c_calls==1u && file_seeks==1u && file_reads>0);
        CHECK(startup_open_calls==slot+1u);
        CHECK(!stream_error && !unsupported && !video.unsupported_read());
        const u32 slotrec=code2::load_le32(memory.data()+0xF0B8u)+slot*16u;
        const u32 itemrec=code2::load_le32(memory.data()+slotrec+12u)+9u*image.archive_entry;
        CHECK(memory[itemrec+2u]==1u);
        CHECK(code2::load_le32(memory.data()+itemrec+5u)!=0u);
        // Loaded item must become a genuine source cache hit on the next
        // request without another file seek or source image allocation.
        const u32 image_cached=code2::function_2f29c(machine,resources,image.archive_entry,slot);
        CHECK(image_cached==code2::load_le32(memory.data()+itemrec+5u));
        CHECK(file_seeks==1u && source_2fd9c_calls==1u);
        std::cout<<"CODE2_UNLOADED_2ECB4_2FD9C_SOURCE_CACHE=PASS; read_only_seeks="
                 <<file_seeks<<" reads="<<file_reads<<" source_startup_opens="<<startup_open_calls
                 <<" source_37700="<<bounded_lzss_callbacks<<"\n";
    }

    static u32 composed_338c8(u32 shading,u32 scale,u32 source,u32 destination) noexcept {
        auto& session=state();
        code2::Application94Boundary sample(0u);
        code2::function_338c8(session.machine,sample,shading,scale,source,destination);
        // The canonical reconstruction currently targets guest A0000 shadow
        // rather than the external VGA aperture, so mirror this selected
        // column's 256 scanlines using the map mask from source 3263C.
        if(destination<0xA0000u || std::uint64_t(destination)+255u*80u>=0xB0000u){
            session.unsupported=true;return 0u;
        }
        for(u32 row=0;row<256u;++row) {
            const u32 p=destination+row*80u;
            if(!session.video.write_aperture(static_cast<u16>(p),
                                            session.memory[p]))
                session.unsupported=true;
        }
        return 1u;
    }
    static u32 source_sweep_lookup(u32 item,u32 slot) noexcept {
        auto& s=state();
        if(slot!=4u){s.unsupported=true;return 0u;}
        if(item==0x93u)return 0u; // empty first-pass synthetic sweep material
        if(item!=s.resident_resource_id){s.unsupported=true;return 0u;}
        return code2::function_2f29c(s.machine,s.resources,item,slot);
    }
    static void source_sweep_flush() noexcept {
        auto& s=state();
        ++s.sweep_flushes;
        code2::function_3263c(s.machine,s.original_compositor);
    }
    void render_res4_compositor(const rloft::original_res::Texture6& tex,
                               u32 height,u32 shade) {
        if(height<16u || height>256u || (height&7u)!=0u || shade>8u)
            throw std::invalid_argument("Original compositor preview height must be 16..256 step 8, shade 0..8");
        const u32 image=code2::function_2f29c(machine,resources,
                                               tex.archive_entry,resident_slot);
        if(!image || source_type6_load_calls!=1u ||
           code2::load_le16(memory.data()+image)!=tex.decoded_bytes)
            throw std::runtime_error("CODE2 compositor: source 37590 RES4 cache not populated");
        const u32 screen_x=(320u-tex.columns)/2u;
        const u32 shade_pair=shade|(shade<<8u);
        code2::store_le32(memory.data()+0x258C4u,0x1F000u);
        code2::store_le16(memory.data()+0x1F000u,0u);
        code2::store_le16(memory.data()+0xFCFAu,
                           static_cast<u16>(shade_pair));
        code2::store_le32(memory.data()+0xC730u,0u);  // sampler branch
        code2::store_le32(memory.data()+0xC6E0u,screen_x>>2u);
        code2::store_le32(memory.data()+0xC6F4u,1u);
        memory[0xEF4Du]=0u;
        original_compositor.out16=&out16;
        original_compositor.call_338c8=&composed_338c8;
        // The records are synthetic, while the following two-pass traversal,
        // texture pointer selection, plane masks and sample rendering are
        // reconstructed original CODE.2 335DC -> 3263C -> 338C8.
        for(u32 column=0;column<tex.columns;++column) {
            const u32 record=0x15A18u+column*0x1Cu;
            code2::store_le32(memory.data()+record,height*2u);
            code2::store_le32(memory.data()+record+4u,0x93u);
            code2::store_le32(memory.data()+record+8u,tex.archive_entry);
            code2::store_le32(memory.data()+record+12u,1u);
            code2::store_le32(memory.data()+record+16u,column*0x40u);
            code2::store_le32(memory.data()+record+24u,0u);
            const u32 mask_index=1u+((column&3u)<<3u);
            memory[0xC668u+mask_index]=static_cast<u8>(1u<<((screen_x+column)&3u));
            memory[0xC688u+mask_index]=0u;
            memory[0xC6A8u+mask_index]=0u;
        }
        code2::store_le32(memory.data()+0xC6D8u,tex.columns);
        code2::Application91Boundary sweep{};
        sweep.call_2f29c=&source_sweep_lookup;
        sweep.call_3263c=&source_sweep_flush;
        code2::function_335dc(machine,sweep);
        CHECK(sweep_flushes==tex.columns*2u);
        CHECK(!stream_error && !unsupported && !video.unsupported_read());
        std::cout<<"CODE2_RES4_SOURCE_335DC_3263C_338C8_COMPOSITOR_PASS"
                 <<" columns="<<tex.columns<<"\\n";
        std::cout<<"AUTHENTIC_WORLD_CAMERA_AND_OCCLUSION=NOT_YET_EXECUTED\\n";
    }
    void render_res4_columns(const rloft::original_res::Texture6& tex,
                             u32 height,u32 shade) {
        if(height<8u || height>256u || shade>63u)
            throw std::invalid_argument("RES4 column height must be 8..256 and shade 0..63");
        // Source textures are column-major, 128 texels per column. The
        // original 33647..33664 selects +2*field_10 = 128*column bytes.
        const u32 pointer=code2::function_2f29c(
            machine,resources,tex.archive_entry,resident_slot);
        if(pointer==0u || source_type6_load_calls!=1u ||
           code2::load_le16(memory.data()+pointer)!=tex.decoded_bytes)
            throw std::runtime_error("Original CODE.2 2ECB4 type-6 lookup failed");
        const u32 pixels=pointer+2u;
        // Original 3395C writes map masks for successive VGA planes; original
        // 338C8 writes into the guest A0000 shadow. Replicate those writes
        // to the host VGA port aperture until CODE.2 guest memory is shared.
        code2::Runtime95Boundary plane{};
        plane.out16=&out16;
        const u32 screen_x=(320u-tex.columns)/2u;
        const u32 shade_pair=shade|(shade<<8u);
        for(u32 col=0;col<tex.columns;++col) {
            const u32 x=screen_x+col;
            code2::function_3395c(plane,1u<<(x&3u));
            const u32 dest=0xA0000u+(x>>2u);
            const u32 src=pixels+col*128u;
            code2::Application94Boundary scale_state(0u); // original stack residue unknown
            code2::function_338c8(machine,scale_state,shade_pair,height,src,dest);
            for(u32 row=0;row<256u;++row) {
                const u32 guest=dest+row*80u;
                if(!video.write_aperture(static_cast<u16>(guest),memory[guest]))
                    throw std::runtime_error("RES4 video transfer failed");
            }
        }
        CHECK(!stream_error && !unsupported && !video.unsupported_read());
        std::cout<<"COMPILED_CODE2_RES4_2ECB4_37590_37700_3395C_338C8_PASS"
                 <<" columns="<<tex.columns<<" height="<<height<<"\n";
        std::cout<<"FULL_WORLD_FIRST_PERSON_SCENE_ACCEPTED=NO; projection/screen geometry not connected\n";
    }
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
    // Source 2975C uses 2F29C(set/key), which enters 2ECB4 on an
    // unloaded original nine-byte resource cache item and calls the
    // compiled 2FD9C image loader through read-only file callbacks.
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
        resource_loader.call_312a0_stack=&open_archive;
        resource_loader.call_31334=&seek_item;
        resource_loader.call_312ec=&read_resource_header;
        resource_loader.call_3104c=&alloc_guest;
        resource_loader.call_310b0=&free_guest;
        resource_loader.call_2fd9c=&source_image_loader;
        resource_loader.call_301a8=&source_font_loader;
        resource_loader.call_37590=&type6_loader;
        type6_source.read_word=&type6_source_read_word;
        type6_source.alloc=&alloc_guest;
        type6_source.read8=&read8;
        type6_source.write8=&write8;
        type6_source.call_2f960=&type6_source_set_stream;
        type6_source.call_2f96c=&type6_source_align;
        type6_source.call_2fb04=&type6_source_decompress;
        image_loader.app69=&resources;
        image_loader.call_312ec=&read_file;
        image_loader.call_3131c=&position_file;
        image_loader.call_3104c=&alloc_guest;
        image_loader.call_310b0=&free_guest;
        image_loader.call_37700=&recovered_37700;
        image_loader.call_37810=&reorder_source_image;
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
    void owned_colors_palette(const std::string& path,unsigned bank,unsigned strength,
                              bool original_dac_observed_slice=false) {
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
        if(original_dac_observed_slice) {
            // Independent original DOSBox-X 2026.10.01 VGA/DAC saves
            // (two instants) both show palette indices 246..253 taken
            // exactly from retail COLORS bank 3 while all other entries
            // match bank 0. Preserve the *original game asset values*,
            // not screenshot-derived RGB literals. The retail caller
            // sequencing that installs this slice is not yet traced:
            // this opt-in path is an observed-hardware hypothesis.
            CHECK(bank==0u && strength==0u);
            constexpr u32 src_bank=3u, first=246u, entries=8u;
            const auto src=content.begin()+1u+
                static_cast<std::ptrdiff_t>(src_bank*768u+first*3u);
            std::copy_n(src,entries*3u,
                        memory.begin()+destination+first*3u);
        }
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
        if(argc>=2 && std::string(argv[1])=="--res4-compositor") {
            if(argc!=7 && argc!=10)
                throw std::invalid_argument("usage: --res4-compositor <owner-RES4-path> <entry-id> <height> <shade> <out-stem> [COLORS bank brightness]");
            const u32 entry=static_cast<u32>(std::stoul(argv[3]));
            const u32 height=static_cast<u32>(std::stoul(argv[4]));
            const u32 shade=static_cast<u32>(std::stoul(argv[5]));
            const auto tex=rloft::original_res::load_type6(argv[2],entry);
            auto session=std::make_unique<Session>();
            session->mode(3u);
            if(argc==10)
                session->owned_colors_palette(argv[7],
                    static_cast<unsigned>(std::stoul(argv[8])),
                    static_cast<unsigned>(std::stoul(argv[9])));
            else session->indexed_palette();
            session->open_readonly_resource(argv[2],entry,4u);
            session->render_res4_compositor(tex,height,shade);
            write_frame(argv[6],session->video);
            return 0;
        }
        if(argc>=2 && std::string(argv[1])=="--res4") {
            if(argc!=7 && argc!=10)
                throw std::invalid_argument(
                    "usage: --res4 <owner-RES4-path> <entry-id> <height> <shade-0..63> <output-stem> [COLORS bank brightness]");
            const u32 entry=static_cast<u32>(std::stoul(argv[3]));
            const u32 height=static_cast<u32>(std::stoul(argv[4]));
            const u32 shade=static_cast<u32>(std::stoul(argv[5]));
            const auto image=rloft::original_res::load_type6(argv[2],entry);
            auto session=std::make_unique<Session>();
            session->mode(3u);
            if(argc==10) {
                session->owned_colors_palette(argv[7],
                    static_cast<unsigned>(std::stoul(argv[8])),
                    static_cast<unsigned>(std::stoul(argv[9])));
            } else session->indexed_palette();
            session->open_readonly_resource(argv[2],entry,4u);
            session->render_res4_columns(image,height,shade);
            write_frame(argv[6],session->video);
            return 0;
        }
        if (argc>=2 && std::string(argv[1])=="--res") {
            if(argc!=7 && argc!=10)
                throw std::invalid_argument("usage: --res <original-RES-path> <entry-id> <slot-id> <frame-id> <out-stem> [COLORS bank brightness]");
            const u32 entry=static_cast<u32>(std::stoul(argv[3]));
            const u32 slot=static_cast<u32>(std::stoul(argv[4]));
            const u32 frame=static_cast<u32>(std::stoul(argv[5]));
            std::cerr << "RES_STAGE=read_archive" << std::endl;
            const auto image=rloft::original_res::load_type1(argv[2],entry);
            std::cerr << "RES_STAGE=decode_complete" << std::endl;
            auto session=std::make_unique<Session>();
            session->trace_original_res=true;
            session->mode(3u);
            if(argc==10)
                session->owned_colors_palette(argv[7],
                    static_cast<unsigned>(std::stoul(argv[8])),
                    static_cast<unsigned>(std::stoul(argv[9])));
            else
                session->indexed_palette(); // visualization only, NOT original palette
            session->render_unloaded(image,argv[2],slot,frame);
            std::cerr << "RES_STAGE=capture_begin" << std::endl;
            write_frame(argv[6],session->video);
            std::cerr << "RES_STAGE=capture_complete" << std::endl;
            std::cout<<"ORIGINAL_RES_TYPE1_2ECB4_2FD9C_2F29C_2975C_2A380_PASS"
                     <<" archive_entry="<<entry<<" frame="<<frame<<"\\n";
            std::cout<<"REAL_RES_INPUT="<<argv[2]
                     <<"; SCENE_AND_ORIGINAL_DOS_PARITY_ACCEPTED=NO\\n";
            return 0;
        }
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
        auto s=std::make_unique<Session>();
        s->mode(3u);
        CHECK(s->video.repeat_scanlines()==1u);
        CHECK(s->video.line_compare()>=400u);
        s->indexed_palette();
        s->verify_original_brightness_transform();
        if (argc==5) s->owned_colors_palette(argv[2],owned_bank,brightness);
        s->seed_image();
        // This is the actual compiled CODE.2 2A380 original planar image
        // source code, operating on a synthetic image record in DGROUP.
        code2::function_2a380(s->machine,s->gfx,0u,10u,24u,0x18000u);
        const auto first=s->video.scanout_indices();
        CHECK(first[10u*320u+24u]==30u);
        CHECK(first[10u*320u+25u]==35u);
        CHECK(first[10u*320u+28u]==31u);
        CHECK(first[11u*320u+24u]==31u);
        // Original 29AA4 pixel path: indexed VGA plane write, read-back 29AF0.
        code2::function_29aa4(s->machine,s->gfx,11u,32u,70u);
        CHECK(code2::function_29af0(s->gfx,11u,32u)==70u);
        code2::function_29aa4(s->machine,s->gfx,0u,2u,70u);
        // Real CODE.2 2AB6C changes drawing base; 2ABF4 changes visible page.
        code2::function_2ab6c(s->machine,1u);
        CHECK(code2::load_le32(s->memory.data()+0xF001u)==0xA8000u);
        code2::function_2a380(s->machine,s->gfx,1u,10u,24u,0x18000u);
        CHECK(s->video.indexed_scanout(24u,10u)==30u);
        code2::function_2abf4(s->hardware,0x8000u);
        CHECK(s->video.indexed_scanout(24u,10u)==50u);
        // Original 2AC10 programs CRTC 18/07/09 line compare for split.
        code2::function_2ac10(s->machine,s->hardware,12u);
        CHECK(s->video.line_compare()==24u);
        CHECK(s->video.indexed_scanout(2u,25u)==70u); // lower split shows page 0
        CHECK(!s->unsupported && !s->video.unsupported_read());
        if (argc==2 || argc==5) write_frame(argv[1],s->video);
        // Mode 2 is a 200-line logical framebuffer doubled to 400 scanlines.
        s->mode(2u);
        CHECK(s->video.repeat_scanlines()==2u);
        code2::function_29aa4(s->machine,s->gfx,5u,24u,70u);
        CHECK(s->video.indexed_scanout(24u,10u)==70u);
        CHECK(s->video.indexed_scanout(24u,11u)==70u);
        CHECK(!s->unsupported && !s->video.unsupported_read());
        std::cout<<"COMPILED_CODE2_29930_29CA0_2A380_29AA4_2AB6C_2ABF4_2AC10_INDEXED_FRAME_PASS\n";
        std::cout<<"SOURCE_GAME_FRAME_ACCEPTED=NO; input textures are synthetic\n";
        return 0;
    } catch(const std::exception& e) {
        std::cerr<<"COMPILED_CODE2_GRAPHICS_FAIL: "<<e.what()<<'\n';
        return 1;
    }
}
