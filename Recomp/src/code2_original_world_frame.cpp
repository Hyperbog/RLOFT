// Integrated native world-source diagnostic: original 181B4 -> 03838 ->
// 334C4 -> 32954 -> 335DC -> 3263C -> 32620/SINTAB -> RES4:0 -> VGA.
// Both existing harnesses are included into one translation unit because
// canonical CODE.2 is reconstructed as a single instruction-accountable .cpp.
// This deliberately does not replace either standalone regression executable.
#define Session WorldRaySource
#define main rloft_level_loader_fixture_main
#include "code2_original_level_loader.cpp"
#undef main
#undef Session
// Reuse original 07938/06CAD and 181B4 with the same guest machine.
using OriginalLevelGuest=WorldRaySource;
#define RLOFT_EMBEDDED_SAVE_LEVEL_GUEST
#define main rloft_original_save_fixture_main
#include "code2_original_save_game.cpp"
#undef main
#undef RLOFT_EMBEDDED_SAVE_LEVEL_GUEST
#define RLOFT_CANONICAL_CODE2_INCLUDED
#define Session WorldGraphicsSource
#define main rloft_graphics_fixture_main
#include "code2_compiled_graphics_frame.cpp"
#undef main
#undef Session
#undef RLOFT_CANONICAL_CODE2_INCLUDED
#include "original_sintab_raster.hpp"
#include <array>
#include <filesystem>
#include <map>
#include <set>
#include <iterator>
#include <fstream>
#include <numeric>
#include <functional>

namespace {
struct WorldFrame {
    WorldRaySource& ray;
    WorldGraphicsSource& gfx;
    struct TextureSpan { u32 first=0,last=0,image=0; };
    std::map<u32,TextureSpan> materials{};
    unsigned resource_cache_hits=0u,loaded_materials=0u;
    u32 code_start=0,code_end=0;
    u32 callback_calls=0,pixel_writes=0,patches=0,shader_flushes=0,legacy_fallbacks=0;
    u32 original_sky_calls=0,original_ground_calls=0,original_background_bytes=0;
    u32 textured_sky_dispatch=0,textured_ground_dispatch=0,unrendered_ui_requests=0;
    u32 outdoor_setup_calls=0,outdoor_patch_words=0,outdoor_patch_bytes=0;
    u32 outdoor_raster_bytes=0,outdoor_4plane_rows=0,outdoor_2plane_rows=0;
    u32 outdoor_nonzero_stores=0,outdoor_page0_stores=0,outdoor_page1_stores=0;
    u32 hud_res0_lookups=0,hud_blits=0,hud_distinct_res0=0;
    u32 scene_candidates=0,scene_quad_eligible=0,scene_draw_requests=0;
    std::map<u32,u32> scene_quad_res4_ids{};
    std::set<u32> hud_resource_ids{};
    std::vector<u8> original_init_header{},original_npcs{},original_objects{};
    u32 init_stream_cursor=0,init_opens=0,init_reads=0,init_closes=0;
    u32 npc_cursor=0,object_cursor=0,metadata_reads=0;
    std::map<u32,u32> background_resources{};
    bool failure=false;
    code2::Application82Boundary scaler{};
    static WorldFrame* current;
    static WorldFrame& self() noexcept {return *current;}
    WorldFrame(WorldRaySource& source,WorldGraphicsSource& render)
        :ray(source),gfx(render) {
        CHECK(current==nullptr);
        current=this;
        code_start=0x100000u;
        code_end=ray.heap_top;
        CHECK(code_end>code_start && code_end<0x400000u);
        scaler.invoke_register_callback=&invoke_generated_scale;
    }
    ~WorldFrame(){current=nullptr;}
    void copy_guest(u32 address,u32 count) {
        CHECK(std::uint64_t(address)+count<=ray.guest.size());
        CHECK(std::uint64_t(address)+count<=gfx.memory.size());
        std::copy_n(ray.guest.begin()+address,count,gfx.memory.begin()+address);
    }
    void install_world() {
        // Do not overwrite the active 2E7F0/2F29C RES4 cache descriptors.
        copy_guest(0x100000u,ray.heap_top-0x100000u); // 03838 SINTAB arena
        copy_guest(0x15618u,128u*4u);                // scale code sizes
        copy_guest(0x193E0u,0x100u);                // original 34998 row coefficients
        copy_guest(0x19660u,0x100u);                // original 34998 row coefficients
        copy_guest(0x18FE0u,128u*4u);                // scale code pointers
        copy_guest(0xC65Cu,0xC73Cu+0x400u-0xC65Cu); // source trig and camera tables
        copy_guest(0x255D0u,0x258BCu-0x255D0u);    // source camera+level header
        copy_guest(0x258F8u,4u);                  // original 1F1C8 resource base
        copy_guest(0xCF4Cu,4u);                    // original world 16.16 clock
        copy_guest(0xF91Cu,8u);                   // saved light/shade scalars
        copy_guest(0xBF48u,0xA0u);               // original 1EA04 HUD placements
        copy_guest(0xC034u,4u);                  // source HUD mode
        copy_guest(0xF9CCu,4u);                   // original 31F94 time phase
        copy_guest(0xFE14u,0x110u);               // 31EDC shade/FE94 lookup
        copy_guest(0xC026u,3u);                  // 1F1C8 outdoor state flags
        copy_guest(0xC050u,1u);                  // original display page
        copy_guest(0x258C4u,4u);                    // authentic 509-word lookup
        copy_guest(0x15A18u,256u*0x1Cu);           // original 32954 ray records
        // The original 181B4 standalone loader constructed per-column
        // scene index arrays and the mutable per-level scene records.
        // Source 1E8C8/34DE8 must see *these* original guest bytes.
        copy_guest(0xCB3Cu,0x400u);                // source object column indices
        copy_guest(0x14D80u,0x7C00u);             // first-party, transient, secondary level records
        CHECK(code2::load_le32(gfx.memory.data()+0xC6D8u)==256u);
        CHECK(code2::load_le32(gfx.memory.data()+0xC730u)==508u);
        // These map-mask defaults correspond to the four interleaved VGA
        // planes. Original DATA initialization remains unconnected here.
        for(u32 plane=0u;plane<4u;++plane) {
            const u32 slot=1u+(plane<<3u);
            gfx.memory[0xC668u+slot]=static_cast<u8>(1u<<plane);
            gfx.memory[0xC688u+slot]=0u;
            gfx.memory[0xC6A8u+slot]=0u;
        }
        // Source 1F1C8 -> 291B0 selects the active draw page. Do not
        // force wall writes to page 0 while the outdoor layer uses page 1.
        for(u32 slot=0u;slot<256u;++slot) {
            const u32 addr=0x15A18u+slot*0x1Cu;
            const u32 front=code2::load_le32(gfx.memory.data()+addr+4u);
            const u32 back=code2::load_le32(gfx.memory.data()+addr+8u);
            const u32 count=static_cast<u32>(gfx.entry_offsets.size()-1u);
            CHECK((front==0x93u || front<count) &&
                  (back==0x93u || back<count));
            CHECK(code2::load_le32(gfx.memory.data()+addr)<=65535u);
        }
        gfx.original_compositor.app82=&scaler;
        gfx.original_compositor.out16=&WorldGraphicsSource::out16;
        gfx.original_compositor.code_load8=&code8;
        gfx.original_compositor.code_load16=&code16;
        gfx.original_compositor.code_store8=&patch8;
        gfx.original_compositor.call_337c8=&weighted_entry;
        gfx.original_compositor.call_338c8=&legacy_raster_fallback;
    }
    // Original 2F29C already supports multiple resources. The former
    // single-resident restriction was solely in the graphics test adapter.
    // Keep its original 2ECB4/37590/37700 loader and nine-byte cache intact.
    void prepare_materials() {
        const auto item_count=static_cast<u32>(gfx.entry_offsets.size()-1u);
        CHECK(item_count>0u && item_count<200000u);
        std::set<u32> needed;
        for(u32 slot=0u;slot<256u;++slot) {
            const u32 addr=0x15A18u+slot*0x1Cu;
            const u32 flag=code2::load_le32(gfx.memory.data()+addr+12u);
            const u32 front=code2::load_le32(gfx.memory.data()+addr+4u);
            const u32 back=code2::load_le32(gfx.memory.data()+addr+8u);
            if(flag==1u && front!=0x93u)needed.insert(front);
            if(back!=0x93u)needed.insert(back);
        }
        CHECK(!needed.empty() && needed.size()<128u);
        for(const u32 id:needed) {
            CHECK(id<item_count);
            const u32 offset=gfx.entry_offsets[id];
            const u32 end=gfx.entry_offsets[id+1u];
            CHECK(end>offset && std::uint64_t(offset)+5u<=end);
            // Type-six owner archive entries only; the canonical 2EC B4
            // loader must decide their eventual decoded guest layout.
            CHECK(gfx.archive_bytes[offset]=='E' &&
                  gfx.archive_bytes[offset+1u]=='H' &&
                  gfx.archive_bytes[offset+2u]==6u);
            const unsigned loads=gfx.source_type6_load_calls;
            const u32 before=gfx.heap_cursor;
            const u32 image=code2::function_2f29c(
                gfx.machine,gfx.resources,id,4u);
            CHECK(!gfx.stream_error && image>=before && image<gfx.heap_cursor);
            CHECK(gfx.source_type6_load_calls==loads+1u);
            const u32 size=code2::load_le16(gfx.memory.data()+image);
            CHECK(size==4096u || size==8192u);
            CHECK(std::uint64_t(image)+2u+size<=gfx.heap_cursor);
            CHECK(std::uint64_t(image)+2u+size<=gfx.memory.size());
            const u32 slot_rec=code2::load_le32(gfx.memory.data()+0xF0B8u)+4u*16u;
            const u32 table=code2::load_le32(gfx.memory.data()+slot_rec+12u);
            const u32 record=table+9u*id;
            CHECK(std::uint64_t(record)+9u<=gfx.memory.size());
            CHECK(gfx.memory[record+2u]==1u);
            CHECK(code2::load_le32(gfx.memory.data()+record+5u)==image);
            const unsigned read_before=gfx.file_reads;
            const unsigned seek_before=gfx.file_seeks;
            CHECK(code2::function_2f29c(gfx.machine,gfx.resources,id,4u)==image);
            CHECK(gfx.file_reads==read_before && gfx.file_seeks==seek_before);
            materials.emplace(id,TextureSpan{image+2u,image+2u+size,image});
            ++loaded_materials;
        }
        CHECK(loaded_materials==needed.size());
        std::cout<<"CODE2_RES4_ORIGINAL_MULTI_ITEM_CACHE_PASS materials="
                 <<loaded_materials<<" reads="<<gfx.file_reads
                 <<" source_loads="<<gfx.source_type6_load_calls<<"\\n";
    }
    static u32 material_lookup(u32 id,u32 slot) noexcept {
        auto& s=self();
        if(id==0x93u)return 0u; // Original no-material sentinel.
        const auto it=s.materials.find(id);
        if(slot!=4u || it==s.materials.end()) {
            s.failure=true;
            return 0u;
        }
        const unsigned reads=s.gfx.file_reads;
        const u32 ptr=code2::function_2f29c(
            s.gfx.machine,s.gfx.resources,id,slot);
        if(ptr!=it->second.image || reads!=s.gfx.file_reads ||
           s.gfx.stream_error)s.failure=true;
        ++s.resource_cache_hits;
        return ptr;
    }
    static u8 code8(u32 address) noexcept {
        auto& s=self();
        if(address<s.code_start || address>=s.code_end) {
            s.failure=true;return 0;
        }
        return s.gfx.memory[address];
    }
    static u16 code16(u32 address) noexcept {
        return static_cast<u16>(code8(address)|
               (static_cast<u16>(code8(address+1u))<<8u));
    }
    static void patch8(u32 address,u8 value) noexcept {
        auto& s=self();
        if(address<s.code_start || address>=s.code_end) {
            s.failure=true;return;
        }
        // Only the original first-pass SINTAB RET patch is currently modeled.
        if(value!=0xC3u && value!=0x8Au && value!=0x88u &&
           value!=0x29u && value!=0xEBu) {
            // Restoring a source byte may have any value at a patched offset.
            // Its source validation instead occurs inside the emulator.
        }
        s.gfx.memory[address]=value;
        ++s.patches;
    }
    static u32 legacy_raster_fallback(u32 shade,u32 scale,u32 src,u32 dst) noexcept {
        ++self().legacy_fallbacks;
        return WorldGraphicsSource::composed_338c8(shade,scale,src,dst);
    }
    static u32 weighted_entry(u32 table,u32 offset) noexcept {
        return code2::function_337c8(self().gfx.machine,table,offset);
    }
    static u32 invoke_generated_scale(u32 target,u32 source,
                                       u32 destination,u32 shade) noexcept {
        auto& s=self();
        ++s.callback_calls;
        // Strictly bind every generated SINTAB read to the material
        // currently selected by original 335DC (guest C700), not merely
        // to any still-live guest image allocation.
        const u32 selected=code2::load_le32(s.gfx.memory.data()+0xC700u);
        const auto found=s.materials.find(selected);
        if(found==s.materials.end() ||
           source<found->second.first || source>=found->second.last) {
            s.failure=true;
            return 0u;
        }
        const TextureSpan* span=&found->second;
        const auto result=rloft_recomp::execute_sintab_scale(
            target,source,destination,shade,s.code_start,s.code_end,
            span->first,span->last,
            [&](u32 address)->u8 {
                if(address>=s.gfx.memory.size()) {
                    s.failure=true;return 0u;
                }
                return s.gfx.memory[address];
            },
            [&](u32 address,u8 pixel) {
                s.gfx.memory[address]=pixel;
                if(!s.gfx.video.write_aperture(static_cast<u16>(address),pixel))
                    s.failure=true;
            });
        if(!result.ok)s.failure=true;
        s.pixel_writes+=result.pixels;
        return result.ok?1u:0u;
    }
    static void fill_backdrop(u32 addr,u8 color,u32 count) noexcept {
        auto& s=self();
        if(count!=0x40u || addr<0xA0000u ||
           std::uint64_t(addr)+count>0xB0000u) {
            s.failure=true;return;
        }
        for(u32 k=0u;k<count;++k) {
            s.gfx.memory[addr+k]=color;
            if(!s.gfx.video.write_aperture(static_cast<u16>(addr+k),color))
                s.failure=true;
        }
        s.original_background_bytes+=count;
    }
    static code2::Runtime102Boundary background_callbacks() noexcept {
        code2::Runtime102Boundary b{};
        b.out16=&WorldGraphicsSource::out16;
        b.rep_stosb_es=&fill_backdrop;
        return b;
    }
    static void source_sky(u32 shade) noexcept {
        auto& s=self();
        auto b=background_callbacks();
        code2::function_33e28(s.gfx.machine,b,shade);
        ++s.original_sky_calls;
    }
    static void source_ground(u32 shade) noexcept {
        auto& s=self();
        auto b=background_callbacks();
        code2::function_33de0(s.gfx.machine,b,shade);
        ++s.original_ground_calls;
    }
    static u32 source_background_texture(u32 id,u32 slot) noexcept {
        auto& s=self();
        if(slot==0u) {
            // 1F1C8's terminal 1EA04 HUD draw requests RES0. The
            // background-only gate has no archive-0 table or UI renderer.
            ++s.unrendered_ui_requests;
            return 0u;
        }
        if(slot!=4u || id>=s.gfx.entry_offsets.size()-1u) {
            s.failure=true;return 0u;
        }
        const u32 ptr=code2::function_2f29c(s.gfx.machine,s.gfx.resources,id,slot);
        if(ptr==0u || std::uint64_t(ptr)+2u>s.gfx.memory.size() ||
           s.gfx.stream_error) {
            s.failure=true;return 0u;
        }
        const u32 size=code2::load_le16(s.gfx.memory.data()+ptr);
        if((size!=4096u && size!=8192u) ||
           std::uint64_t(ptr)+2u+size>s.gfx.heap_cursor) {
            s.failure=true;return 0u;
        }
        s.background_resources[id]=ptr;
        return ptr;
    }
    static u32 source_page_29fec(u32 page) noexcept {
        return code2::function_29fec(self().gfx.machine,page);
    }
    static void source_select_draw_page(u32 page) noexcept {
        auto& s=self();
        code2::Application56Boundary b{};
        b.call_29fec=&source_page_29fec;
        code2::function_291b0(s.gfx.machine,b,page);
        if(page>1u || s.gfx.memory[0xEF4Du]!=page ||
           code2::load_le32(s.gfx.memory.data()+0xF001u)!=
           0xA0000u+page*0x8000u)s.failure=true;
    }
    static void texture_patch_word(u32 addr,u16) noexcept {
        auto& s=self();
        if(addr!=0x34A11u && addr!=0x34A27u &&
           addr!=0x34A3Du && addr!=0x34A54u) {
            s.failure=true;return;
        }
        ++s.outdoor_patch_words;
        // Original 34998 patched code immediates are represented by the
        // compiled source's returned guest FF58 entries, not writable x86.
    }
    static void texture_patch_byte(u32 addr,u8) noexcept {
        auto& s=self();
        switch(addr) {
            case 0x34AF9u: case 0x34B69u: case 0x34AF6u:
            case 0x34B66u: case 0x34B07u: case 0x34B77u: break;
            default:s.failure=true;return;
        }
        ++s.outdoor_patch_bytes;
    }
    static void source_texture_view_setup() noexcept {
        auto& s=self();
        code2::Runtime107Boundary b{};
        b.patch_code16=&texture_patch_word;
        code2::function_34998(s.gfx.machine,b);
        ++s.outdoor_setup_calls;
    }
    static void texture_es_write(u32 addr,u8 pixel) noexcept {
        auto& s=self();
        if(addr<0xA0000u || addr>=0xB0000u ||
           addr>=s.gfx.memory.size()) {
            s.failure=true;return;
        }
        if(pixel)++s.outdoor_nonzero_stores;
        if(addr<0xA8000u)++s.outdoor_page0_stores;
        else ++s.outdoor_page1_stores;
        s.gfx.memory[addr]=pixel;
        if(!s.gfx.video.write_aperture(static_cast<u16>(addr),pixel))
            s.failure=true;
        ++s.outdoor_raster_bytes;
    }
    static void textured_backdrop_dispatch() noexcept {
        auto& s=self();
        const u32 dir=code2::load_le32(s.gfx.memory.data()+0xFF4Cu);
        const u32 active=code2::load_le32(s.gfx.memory.data()+0xC040u);
        const bool owned=std::any_of(s.background_resources.begin(),
            s.background_resources.end(),
            [&](const auto& item){return item.second==active;});
        if(!active || !owned) {
            s.failure=true;return;
        }
        if(dir==1u)++s.textured_sky_dispatch;
        else if(dir==0xFFFFFFFFu)++s.textured_ground_dispatch;
        else {s.failure=true;return;}
        const u32 rows=code2::load_le32(s.gfx.memory.data()+0xFF50u);
        const u32 texture=active+2u;
        const auto image=std::find_if(s.background_resources.begin(),
            s.background_resources.end(),[&](const auto& item){
                return item.second==active;
            });
        if(image==s.background_resources.end() || rows==0u || rows>128u ||
           std::uint64_t(texture)+4096u>s.gfx.heap_cursor) {
            s.failure=true;return;
        }
        code2::Runtime108Boundary driver{};
        driver.patch_code8=&texture_patch_byte;
        code2::Runtime106Boundary raster{};
        raster.out16=&WorldGraphicsSource::out16;
        raster.store_es8=&texture_es_write;
        const u32 before=s.outdoor_raster_bytes;
        code2::function_34a74(s.gfx.machine,driver,raster);
        const u32 first=std::min(rows,0x50u);
        const u32 second=rows>0x50u?rows-0x50u:0u;
        const u32 expected=first*4u*64u+second*2u*64u;
        if(s.outdoor_raster_bytes-before!=expected)s.failure=true;
        s.outdoor_4plane_rows+=first;
        s.outdoor_2plane_rows+=second;
    }
    static void unexpected_panorama(u32) noexcept {self().failure=true;}
    void render_saved_background() {
        CHECK(gfx.memory[0x258A8u]==0xFFu);
        // The saved camera selects textured RES4 panorama code 34A74
        // for both halves, not flat fills 33E28/33DE0.
        std::cout<<"CODE2_ORIGINAL_OUTDOOR_DISPATCH_FLAGS"
                 <<" c026="<<unsigned(gfx.memory[0xC026u])
                 <<" c027="<<unsigned(gfx.memory[0xC027u])
                 <<" c028="<<unsigned(gfx.memory[0xC028u])
                 <<" ground_offset="<<code2::load_le16(gfx.memory.data()+0x2561Du)
                 <<" sky_offset="<<code2::load_le16(gfx.memory.data()+0x2561Fu)
                 <<" res4_environment_base="<<code2::load_le32(gfx.memory.data()+0x258F8u)
                 <<" FF50="<<code2::load_le32(gfx.memory.data()+0xFF50u)
                 <<"\\n";
        std::cout.flush();
        CHECK(gfx.memory[0xC026u]==1u && gfx.memory[0xC027u]==1u);
        code2::Application43Boundary lookup{};
        lookup.call_2f29c=&source_background_texture;
        code2::Application44Boundary dispatch{};
        dispatch.app43=&lookup;
        dispatch.call_291b0=&source_select_draw_page;
        dispatch.call_314b0=&unexpected_panorama;
        dispatch.call_34998=&source_texture_view_setup;
        dispatch.call_33e28=&source_sky;
        dispatch.call_33de0=&source_ground;
        dispatch.call_34a74=&textured_backdrop_dispatch;
        // In a continuous boot 34998 initializes FF58 when the angle changes.
        // This isolated saved-game source diagnostic must seed that original
        // coefficient table explicitly before 1F1C8's optional refresh.
        source_texture_view_setup();
        const u32 coeff_a=code2::load_le16(gfx.memory.data()+0x193E0u);
        const u32 coeff_b=code2::load_le16(gfx.memory.data()+0x19660u);
        u32 populated_rows=0u;
        for(u32 i=0u;i<128u;++i) {
            const auto p=0xFF58u+i*6u;
            if(code2::load_le16(gfx.memory.data()+p)!=0u ||
               code2::load_le16(gfx.memory.data()+p+2u)!=0u ||
               gfx.memory[p+4u]!=0u || gfx.memory[p+5u]!=0u)
                ++populated_rows;
        }
        std::cout<<"CODE2_ORIGINAL_OUTDOOR_COEFFICIENTS"
                 <<" src_193E0="<<coeff_a
                 <<" src_19660="<<coeff_b
                 <<" active_34998_rows="<<populated_rows<<"\\n";
        std::cout.flush();
        // The original 334C4/32954 rays have already run on the source
        // save-loaded machine; do not duplicate geometry on gfx machine.
        code2::function_1f1c8(gfx.machine,dispatch);
        std::cout<<"CODE2_SOURCE_OUTDOOR_DISPATCH_COUNTS sky="<<textured_sky_dispatch
                 <<" ground="<<textured_ground_dispatch
                 <<" res4="<<background_resources.size()
                 <<" ui_res0_requests="<<unrendered_ui_requests
                 <<" unsupported="<<gfx.unsupported
                 <<" stream_error="<<gfx.stream_error
                 <<" callback_failure="<<failure<<"\\n";
        std::cout.flush();
        CHECK(!failure && !gfx.unsupported);
        CHECK(textured_sky_dispatch==1u && textured_ground_dispatch==1u);
        CHECK(original_sky_calls==0u && original_ground_calls==0u &&
              original_background_bytes==0u);
        const auto background_pixels=gfx.video.scanout_indices();
        const u32 background_nonzero=static_cast<u32>(
            std::count_if(background_pixels.begin(),background_pixels.end(),
                [](u8 p){return p!=0u;}));
        std::cout<<"CODE2_ORIGINAL_OUTDOOR_POST_RASTER"
                 <<" nonzero_scanout="<<background_nonzero
                 <<" pixels_written="<<outdoor_raster_bytes
                 <<" nonzero_writes="<<outdoor_nonzero_stores
                 <<" page0_writes="<<outdoor_page0_stores
                 <<" page1_writes="<<outdoor_page1_stores
                 <<" guest_C050="<<unsigned(gfx.memory[0xC050u])
                 <<" guest_FDBC="<<code2::load_le32(gfx.memory.data()+0xFDBCu)
                 <<"\\n";
        std::cout.flush();
        CHECK(background_resources.size()==2u);
        if(code2::load_le32(gfx.memory.data()+0x258F8u)==600u) {
            CHECK(background_resources.count(634u)==1u);
            CHECK(background_resources.count(635u)==1u);
            std::cout<<"CODE2_SAVED_LEVEL5_ORIGINAL_INIT_RES4_BASED_SKY_GROUND"
                     <<" sky_res4=634 ground_res4=635\\n";
        }
        CHECK(outdoor_setup_calls>=1u && outdoor_patch_words>=4u);
        CHECK(outdoor_patch_bytes>=12u);
        CHECK(outdoor_4plane_rows>0u && outdoor_2plane_rows>0u);
        CHECK(outdoor_raster_bytes>=16384u);
        std::cout<<"CODE2_SAVED_LEVEL5_ORIGINAL_1F1C8_34A74_TEXTURED_OUTDOOR_DISPATCH_PASS"
                 <<" sky=1 ground=1 source_RES4_items="
                 <<background_resources.size()
                 <<" unrendered_ui_requests="<<unrendered_ui_requests
                 <<" texel_rasterizer_executed=YES\\n";
        std::cout<<"CODE2_SAVED_LEVEL5_ORIGINAL_34998_34A74_342E4_34954_OUTDOOR_VGA_PASS"
                 <<" source_outdoor_raster_bytes="<<outdoor_raster_bytes
                 <<" rows_4planes="<<outdoor_4plane_rows
                 <<" rows_2plane_pairs="<<outdoor_2plane_rows
                 <<" setup_calls="<<outdoor_setup_calls<<"\\n";
    }
    static u32 source_scene_record(code2::s32 x,code2::s32 y) noexcept {
        auto& s=self();
        if(x<0 || y<0 || x>=64 || y>=64){s.failure=true;return 0u;}
        const u32 addr=code2::function_1e8c8(s.gfx.machine,
                                             static_cast<u32>(x),
                                             static_cast<u32>(y));
        if(addr && (std::uint64_t(addr)+10u>s.gfx.memory.size()))
            s.failure=true;
        if(addr)++s.scene_candidates;
        return addr;
    }
    void verify_original_scene_index() {
        const u32 p=code2::load_le16(gfx.memory.data()+0x258A2u);
        const u32 t=code2::load_le16(gfx.memory.data()+0x258A4u);
        const u32 n=code2::load_le16(gfx.memory.data()+0x258A6u);
        CHECK(p==90u && t==68u && n==163u);
        for(code2::s32 x=0;x<64;++x)
            for(code2::s32 y=0;y<64;++y){
                const u32 rec=source_scene_record(x,y);
                if(!rec)continue;
                const u8 flags=gfx.memory[rec+1u];
                if((flags&0x10u)==0u)continue;
                if(gfx.memory[rec]==3u && (flags&0x08u)==0u)continue;
                ++scene_quad_eligible;
                const u32 id=code2::load_le32(gfx.memory.data()+0x258F8u)+gfx.memory[rec+9u];
                ++scene_quad_res4_ids[id];
            }
        std::cout<<"CODE2_SAVE5_ORIGINAL_181B4_1E8C8_SCENE_INDEX_PASS"
                 <<" primary="<<p<<" transient="<<t<<" secondary="<<n
                 <<" lookup_hits="<<scene_candidates
                 <<" source_quad_candidates="<<scene_quad_eligible
                 <<" distinct_quad_res4="<<scene_quad_res4_ids.size()<<"\\n";
        CHECK(!failure && scene_candidates>0u);
    }
    static code2::s32 source_open_definition(u32 pointer) noexcept {
        auto& s=self();
        if(pointer==0x6F8u && !s.original_npcs.empty()) {
            s.npc_cursor=0;return 0x4Au;
        }
        if(pointer==0x704u && !s.original_objects.empty()) {
            s.object_cursor=0;return 0x4Bu;
        }
        s.failure=true;return -1;
    }
    static void source_close_definition(u32 handle) noexcept {
        if(handle!=0x4Au && handle!=0x4Bu)self().failure=true;
    }
    static void source_read_definition(u32 target,u32 length,u32 handle) noexcept {
        auto& s=self();
        const std::vector<u8>* input=nullptr;
        u32* offset=nullptr;
        if(handle==0x49u){input=&s.original_init_header;offset=&s.init_stream_cursor;}
        else if(handle==0x4Au){input=&s.original_npcs;offset=&s.npc_cursor;}
        else if(handle==0x4Bu){input=&s.original_objects;offset=&s.object_cursor;}
        if(!input || !offset ||
           std::uint64_t(*offset)+length>input->size() ||
           std::uint64_t(target)+length>s.gfx.memory.size()) {
            s.failure=true;return;
        }
        std::copy_n(input->data()+*offset,length,s.gfx.memory.data()+target);
        *offset+=length;
        ++s.metadata_reads;
    }
    static u32 hud_open_init(u32 pointer) noexcept {
        auto& s=self();
        if(pointer!=0x718u || s.original_init_header.size()!=1879u ||
           s.gfx.memory[0x718u]!='I') {
            s.failure=true;return 0xFFFFFFFFu;
        }
        ++s.init_opens;s.init_stream_cursor=0u;return 0x49u;
    }
    static u32 hud_read_init(u8* dest,u32 size,u32 handle) noexcept {
        auto& s=self();
        if(handle!=0x49u || !dest ||
           std::uint64_t(s.init_stream_cursor)+size>s.original_init_header.size()) {
            s.failure=true;return 0u;
        }
        std::copy_n(s.original_init_header.data()+s.init_stream_cursor,size,dest);
        s.init_stream_cursor+=size;++s.init_reads;return size;
    }
    static void hud_close_init(u32 handle) noexcept {
        auto& s=self();
        if(handle!=0x49u)s.failure=true;
        ++s.init_closes;
    }
    void initialize_original_hud_assets(const std::string& filename,
                                         const std::string& npc_file="",
                                         const std::string& object_file="") {
        CHECK(std::filesystem::path(filename).filename()=="INIT");
        const bool with_scene=!npc_file.empty() && !object_file.empty();
        if(with_scene) {
            CHECK(std::filesystem::path(npc_file).filename()=="NPCS.DAT");
            CHECK(std::filesystem::path(object_file).filename()=="OBJECTS.DAT");
            original_npcs=rloft::original_res::read_only(npc_file);
            original_objects=rloft::original_res::read_only(object_file);
            CHECK(original_npcs.size()==5731u &&
                  original_objects.size()==21844u);
            CHECK(code2::load_le32(original_npcs.data())==83u &&
                  code2::load_le32(original_objects.data())==336u);
        }
        original_init_header=rloft::original_res::read_only(filename);
        CHECK(original_init_header.size()==1879u);
        constexpr char label[]="INIT";
        std::copy_n(label,sizeof(label),gfx.memory.begin()+0x718u);
        code2::Application30Boundary init{};
        code2::Application25Boundary definitions{};
        code2::Application26Boundary files{};
        code2::Application28Boundary context{};
        code2::Application29Boundary source{};
        if(with_scene) {
            definitions.call_312ec=&source_read_definition;
            files.call_312a0=&source_open_definition;
            files.call_312dc=&source_close_definition;
            files.call_3104c=&WorldGraphicsSource::alloc_guest;
            context.app25=&definitions;
            context.app26=&files;
            source.app28=&context;
            source.call_312ec_local=&hud_read_init;
            init.app29=&source;
        }
        init.call_312a0=&hud_open_init;
        init.call_312ec_local=&hud_read_init;
        init.call_312dc=&hud_close_init;
        code2::function_16d04(gfx.machine,init);
        CHECK(!failure && init_stream_cursor==(with_scene?1879u:36u));
        CHECK(init_opens==1u && init_reads>=18u && init_closes==1u);
        if(with_scene) {
            CHECK(npc_cursor==original_npcs.size() &&
                  object_cursor==original_objects.size());
            CHECK(code2::load_le32(gfx.memory.data()+0x258F4u)==83u);
            CHECK(code2::load_le32(gfx.memory.data()+0x258E8u)==336u);
            CHECK(code2::load_le32(gfx.memory.data()+0x258E0u)==175u);
            CHECK(code2::load_le32(gfx.memory.data()+0x258F0u)==12u);
            for(u32 field: {0x22230u,0x255D8u,0x255DCu,0x22234u,0x2B764u})
                CHECK(code2::load_le32(gfx.memory.data()+field)>=0x400000u);
            std::cout<<"CODE2_SAVE5_ORIGINAL_16810_168C8_1686C_16934_16990_SCENE_DEFINITIONS_PASS"
                     <<" npc=83 objects=336 class1=175 class3=12 pano_profiles=2"
                     <<" source_reads="<<metadata_reads<<"\\n";
        }
        CHECK(code2::load_le32(gfx.memory.data()+0x258DCu)==15u);
        CHECK(code2::load_le32(gfx.memory.data()+0x258F8u)==600u);
        std::cout<<"CODE2_SAVE5_ORIGINAL_16D04_INIT_HUD_BASE_PASS"
                 <<" res0_base=15 source_reads=18\\n";
    }
    static u32 source_hud_resource(u32 id,u32 slot) noexcept {
        auto& s=self();
        const u32 count=static_cast<u32>(s.gfx.entry_offsets.size()-1u);
        if(slot!=0u || id>=count) {
            std::cout<<"CODE2_HUD_LOOKUP_REJECT id="<<id
                     <<" slot="<<slot<<" count="<<count<<"\\n";
            s.failure=true;return 0u;
        }
        const u32 ptr=code2::function_2f29c(s.gfx.machine,s.gfx.resources,id,slot);
        if(ptr==0u || ptr>=s.gfx.heap_cursor || s.gfx.stream_error) {
            const u32 offset=s.gfx.entry_offsets[id];
            std::cout<<"CODE2_SOURCE_HUD_RES0_LOADING_ERROR id="<<id
                     <<" ptr="<<ptr<<" heap="<<s.gfx.heap_cursor
                     <<" bad_stream="<<s.gfx.stream_error
                     <<" header="<<unsigned(s.gfx.archive_bytes[offset+2u])
                     <<" entry_bytes="<<(s.gfx.entry_offsets[id+1u]-offset)
                     <<" calls="<<s.gfx.source_2fd9c_calls<<"\\n";
            s.failure=true;return 0u;
        }
        ++s.hud_res0_lookups;s.hud_resource_ids.insert(id);
        return ptr;
    }
    static void source_hud_image(u32 frame,u32 y,u32 x,u32 image) noexcept {
        auto& s=self();
        if(!image)return;
        code2::Application56Boundary b{};
        b.call_2a380=&WorldGraphicsSource::draw_image;
        code2::function_2939c(b,frame,y,x,image);
        ++s.hud_blits;
    }
    void render_saved_hud(const std::string& filename) {
        CHECK(code2::load_le32(gfx.memory.data()+0x258DCu)==15u);
        CHECK(std::filesystem::path(filename).filename()=="RES0");
        CHECK(gfx.memory[0xEF4Du]==1u);
        CHECK(code2::load_le32(gfx.memory.data()+0xF001u)==0xA8000u);
        code2::function_2abf4(gfx.hardware,0x8000u);
        const auto before=gfx.video.scanout_indices();
        CHECK(before.size()==128000u);
        gfx.open_readonly_resource(filename,0u,0u);
        const u32 old_calls=gfx.draw_calls;
        code2::Application43Boundary hud{};
        hud.call_2f29c=&source_hud_resource;
        hud.call_2939c=&source_hud_image;
        code2::function_1ea04(gfx.machine,hud);
        std::cout<<"CODE2_ORIGINAL_RES0_HUD_SOURCE_DIAGNOSTIC"
                 <<" lookups="<<hud_res0_lookups
                 <<" blits="<<hud_blits
                 <<" failure="<<failure
                 <<" unsupported="<<gfx.unsupported
                 <<" stream_error="<<gfx.stream_error
                 <<" source_loads="<<gfx.source_2fd9c_calls
                 <<" draws="<<gfx.draw_calls
                 <<" resource_base="<<code2::load_le32(gfx.memory.data()+0x258DCu)
                 <<" hud_mode="<<code2::load_le32(gfx.memory.data()+0xC034u)
                 <<" res0_entries="<<(gfx.entry_offsets.size()-1u)<<"\\n";
        std::cout.flush();
        CHECK(!failure && !gfx.unsupported && !gfx.stream_error);
        CHECK(hud_res0_lookups==12u && hud_blits>=12u);
        CHECK(gfx.draw_calls-old_calls==hud_blits);
        hud_distinct_res0=static_cast<u32>(hud_resource_ids.size());
        const auto after=gfx.video.scanout_indices();
        const u32 changed=static_cast<u32>(
            std::inner_product(before.begin(),before.end(),after.begin(),0u,
                std::plus<u32>{},[](u8 old_pixel,u8 new_pixel) {
                    return static_cast<u32>(old_pixel!=new_pixel);
                }));
        CHECK(changed>0u);
        std::cout<<"CODE2_SAVE5_HUD_INDEXED_OUTPUT_DIAGNOSTIC"
                 <<" changed_visible_pixels="<<changed
                 <<" positions_first=("
                 <<code2::load_le32(gfx.memory.data()+0xBF5Cu)<<","
                 <<code2::load_le32(gfx.memory.data()+0xBF8Cu)<<")"
                 <<" hud_page="<<unsigned(gfx.memory[0xEF4Du])<<"\\n";
        std::cout<<"CODE2_SAVE5_ORIGINAL_1EA04_2F29C_2939C_2A380_RES0_HUD_PASS"
                 <<" lookups="<<hud_res0_lookups
                 <<" distinct_resources="<<hud_distinct_res0
                 <<" drawn_images="<<hud_blits<<"\\n";
    }
    void render(bool saved_outdoor=false,const std::string& hud_file="",
                const std::string& init_file="",
                const std::string& npc_file="",
                const std::string& object_file="") {
        install_world();
        if(saved_outdoor && !init_file.empty())
            initialize_original_hud_assets(init_file,npc_file,object_file);
        prepare_materials();
        if(saved_outdoor && !npc_file.empty())verify_original_scene_index();
        if(saved_outdoor)render_saved_background();
        const u32 before=gfx.source_type6_load_calls;
        code2::Application91Boundary sweep{};
        sweep.call_2f29c=&material_lookup;
        sweep.call_3263c=&WorldGraphicsSource::source_sweep_flush;
        code2::function_335dc(gfx.machine,sweep);
        shader_flushes=gfx.sweep_flushes;
        CHECK(!failure && !gfx.unsupported && !gfx.stream_error);
        CHECK(!gfx.video.unsupported_read());
        CHECK(gfx.source_type6_load_calls==before);
        CHECK(resource_cache_hits>0u);
        CHECK(materials.size()==loaded_materials);
        u32 flagged=0u,nonempty=0u;
        for(u32 slot=0u;slot<256u;++slot) {
            const u32 addr=0x15A18u+slot*0x1Cu;
            if(code2::load_le32(gfx.memory.data()+addr+0x0Cu)==1u)++flagged;
            if(code2::load_le32(gfx.memory.data()+addr+0x04u)!=0x93u)++nonempty;
        }
        std::cout<<"CODE2_WORLD_DISPATCH_DIAGNOSTIC flags="<<flagged
                 <<" first_nonempty="<<nonempty
                 <<" scale_calls="<<callback_calls
                 <<" write_count="<<pixel_writes
                 <<" patches="<<patches
                 <<" flushes="<<shader_flushes
                 <<" materials="<<loaded_materials
                 <<" cache_hits="<<resource_cache_hits
                 <<" fallback_338c8="<<legacy_fallbacks<<"\\n";
        std::cout.flush();
        if(!saved_outdoor)CHECK(callback_calls>0u && pixel_writes>0u && patches>0u);
        else CHECK((pixel_writes>0u || legacy_fallbacks>0u) &&
                   (callback_calls>0u || legacy_fallbacks>0u));
        CHECK(shader_flushes>0u);
        if(saved_outdoor && !hud_file.empty())
            render_saved_hud(hud_file);
        if(saved_outdoor) {
            const u32 page=static_cast<u32>(gfx.memory[0xC050u]);
            CHECK(page==1u && gfx.memory[0xEF4Du]==1u);
            CHECK(code2::load_le32(gfx.memory.data()+0xF001u)==0xA8000u);
            // Original CODE.2 2ABF4 programs CRTC start to expose the
            // completed draw page, after source outdoor + wall passes.
            code2::function_2abf4(gfx.hardware,page*0x8000u);
            CHECK(gfx.video.crtc_start()==0x8000u);
            std::cout<<"CODE2_SAVE5_ORIGINAL_291B0_2ABF4_ACTIVE_PAGE_PASS"
                     <<" draw_page="<<page<<" crtc_start=32768\\n";
        }
        const auto pixels=gfx.video.scanout_indices();
        const auto nonzero=std::count_if(pixels.begin(),pixels.end(),
                                        [](u8 value){return value!=0u;});
        CHECK(nonzero>0);
        if(saved_outdoor) {
            // The owner save has an entire original textured background:
            // require visible indexed pixels beyond the prior wall-only
            // snapshot rather than accepting writes to a hidden page.
            CHECK(nonzero>20407u);
            std::cout<<"CODE2_SAVE5_VISIBLE_OUTDOOR_AND_WALL_PIXELS="
                     <<nonzero<<"\\n";
        }
        std::cout<<(saved_outdoor?
                 "CODE2_SAVED_LEVEL5_ORIGINAL_WALL_FRAME_PASS level=5":
                 "CODE2_AUTHENTIC_32954_335DC_3263C_SINTAB_RES4_VGA_PASS level=11")
                 <<" rows=400 rays=256 callbacks="<<callback_calls
                 <<" raster_writes="<<pixel_writes
                 <<" color_pixels="<<nonzero
                 <<" flushes="<<shader_flushes<<"\\n";
        if(!saved_outdoor)
            std::cout<<"CODE2_338C8_FALLBACK_USED_IN_REAL_WORLD_FIRST_PASS=NO\\n";
        else
            std::cout<<"CODE2_LEVEL5_SOURCE_OUTDOOR_34A74_SKY_GROUND_RENDERED=YES"
                     <<" RES3_SCENERY_AND_HUD_RENDERED=NO\\n";
        std::cout<<"CODE2_FULL_DOS_WORLD_PARITY_ACCEPTED=NO\\n";
    }
};
WorldFrame* WorldFrame::current=nullptr;
} // namespace

int main(int argc,char** argv) {
    try {
        const bool saved_outdoor=(argc==9 || argc==11 || argc==14) &&
                                 std::string(argv[1])=="--save-world";
        const bool with_hud=saved_outdoor && argc>=11;
        const bool with_scene=saved_outdoor && argc==14;
        const bool original_world=argc==7 && std::string(argv[1])=="--world";
        if(!saved_outdoor && !original_world)
            throw std::invalid_argument(
                "usage: --world LEVELS.DAT SINTAB RES4 COLORS output-stem"
                " | --save-world SAVE.DAT LEV005.DAT SINTAB RES4 COLORS CODE.2 [INIT RES0 [NPCS.DAT OBJECTS.DAT RES3]] output-stem");
        const std::filesystem::path levels(argv[2]),sintab(argv[3+(saved_outdoor?1:0)]);
        const std::filesystem::path res4(argv[4+(saved_outdoor?1:0)]),
                                    colors(argv[5+(saved_outdoor?1:0)]);
        if(saved_outdoor)CHECK(levels.filename()=="SAVE.DAT");
        else CHECK(levels.filename()=="LEVELS.DAT");
        CHECK(sintab.filename()=="SINTAB");
        CHECK(res4.filename()=="RES4" && colors.filename()=="COLORS");
        CHECK(levels.parent_path()==sintab.parent_path());
        CHECK(levels.parent_path()==res4.parent_path());
        CHECK(levels.parent_path()==colors.parent_path());
        std::vector<u8> bytes;
        rloft::original_levels::LevelGrid grid{};
        if(saved_outdoor) {
            const std::filesystem::path lev(argv[3]);
            CHECK(lev.filename()=="LEV005.DAT" &&
                  lev.parent_path()==levels.parent_path());
            bytes=rloft::original_levels::read_archive(lev.string());
            grid=rloft::original_levels::decode_saved_grid(bytes,5u);
        } else {
            bytes=rloft::original_levels::read_archive(levels.string());
            grid=rloft::original_levels::decode_grid(bytes,11u);
        }
        auto ray=std::make_unique<WorldRaySource>(levels.parent_path(),true);
        if(saved_outdoor) {
            const auto save=rloft::original_levels::read_archive(levels.string());
            const std::filesystem::path codefile(argv[7]);
            CHECK(codefile.filename()=="CODE.2");
            const auto image=rloft::original_levels::read_archive(codefile.string());
            constexpr std::size_t data_image=0x75F24u;
            CHECK(image.size()==558689u &&
                  image.size()>data_image+0x7330u+620u);
            // Actual CODE.2 data object-2 initialization, including the
            // 62 original 10-byte container descriptors for 07938.
            std::fill(ray->guest.begin(),ray->guest.end(),0u);
            std::copy(image.begin()+data_image,image.end(),
                      ray->guest.begin());
            CHECK(ray->guest[0x7330u]==4u &&
                  code2::load_le32(ray->guest.data()+0x7335u)==0x6FC4u);
            constexpr char name[]="SINTAB";
            std::copy_n(name,sizeof(name),ray->guest.begin()+0x24u);
            ray->load_original_sintab();
            {
                SavedGame original_save(*ray);
                original_save.execute(save,grid);
            }
            // The exact guest buffers from original 07938 -> 181B4 now
            // feed compiled original source view/rays, without reloading
            // or reserializing the saved header/grid.
            code2::Application90Boundary view{};
            view.call_35680=&code2::function_35680;
            view.call_32954=&WorldRaySource::original_ray_sweep;
            code2::function_334c4(ray->machine,view);
            CHECK(!ray->failed && ray->rays==1u &&
                  code2::load_le32(ray->guest.data()+0xFD0Cu)==grid.cells.size());
            CHECK(code2::load_le32(ray->guest.data()+0xC658u)==252u);
            CHECK(code2::load_le32(ray->guest.data()+0xF920u)==10u);
            CHECK(code2::load_le32(ray->guest.data()+0xF91Cu)==3u);
            CHECK(ray->guest[0x258A8u]==0xFFu);
            std::cout<<"CODE2_SAVED_LEVEL5_07938_TO_181B4_TO_32954_PASS"
                     <<" level=5 rays=256 profile=255\\n";
        } else {
            // Original 13C88 case-7 transition camera, tile (2,65), 270.
            code2::store_le32(ray->guest.data()+0x255D0u,0x00028000u);
            code2::store_le32(ray->guest.data()+0x255D4u,0x00418000u);
            ray->run(grid,true);
        }
        if(saved_outdoor) {
            const u32 primary=code2::load_le16(ray->guest.data()+0x258A2u);
            const u32 placed=code2::load_le16(ray->guest.data()+0x258A4u);
            const u32 secondary=code2::load_le16(ray->guest.data()+0x258A6u);
            u32 nonzero_primary_columns=0,nonzero_secondary_columns=0;
            for(u32 col=0;col<64u;++col) {
                if(code2::load_le32(ray->guest.data()+0xCB3Cu+col*4u))
                    ++nonzero_primary_columns;
                if(code2::load_le32(ray->guest.data()+0xCD3Cu+col*4u))
                    ++nonzero_secondary_columns;
            }
            std::cout<<"CODE2_SAVE5_SCENE_INDEX_PROBE primary="<<primary
                     <<" placed="<<placed<<" secondary="<<secondary
                     <<" primary_columns="<<nonzero_primary_columns
                     <<" secondary_columns="<<nonzero_secondary_columns
                     <<" probe_at_6_6="<<code2::function_1e8c8(
                         ray->machine,6u,6u)
                     <<" first_primary_flag="<<unsigned(ray->guest[0x14D81u])
                     <<"\\n";
            std::cout<<"CODE2_SAVE5_SOURCE_SCENE_INPUTS"
                     <<" primary="<<code2::load_le16(ray->guest.data()+0x258A2u)
                     <<" transient="<<code2::load_le16(ray->guest.data()+0x258A4u)
                     <<" secondary="<<code2::load_le16(ray->guest.data()+0x258A6u)
                     <<" placed="<<code2::load_le32(ray->guest.data()+0xC038u)
                     <<" npc_descriptors="<<code2::load_le32(ray->guest.data()+0x22230u)
                     <<" secondary_descriptors="<<code2::load_le32(ray->guest.data()+0x255DCu)
                     <<" placed_descriptors="<<code2::load_le32(ray->guest.data()+0x22234u)
                     <<" first_scene_type="<<code2::load_le16(ray->guest.data()+0x14D8Eu)
                     <<" first_scene_flags="<<unsigned(ray->guest[0x14D8Bu])
                     <<"\\n";
            std::cout.flush();
        }
        std::map<u32,unsigned> front_materials;
        for(u32 i=0u;i<256u;++i) {
            const u32 addr=0x15A18u+i*0x1Cu;
            if(code2::load_le32(ray->guest.data()+addr+0x0Cu)!=1u)continue;
            const u32 mat=code2::load_le32(ray->guest.data()+addr+4u);
            if(mat!=0x93u)++front_materials[mat];
        }
        std::cout<<"CODE2_SOURCE_SCENE_FRONT_MATERIAL_COUNT="
                 <<front_materials.size()<<" eligible=";
        for(const auto& [id,n]:front_materials)
            std::cout<<id<<":"<<n<<",";
        std::cout<<"\\n";
        if(!saved_outdoor) {
            CHECK(front_materials.size()==3u);
            CHECK(front_materials.count(48u)==1u &&
                  front_materials.count(53u)==1u &&
                  front_materials.count(59u)==1u);
        }
        auto gfx=std::make_unique<WorldGraphicsSource>();
        gfx->mode(3u);
        gfx->owned_colors_palette(colors.string(),0u,0u);
        gfx->open_readonly_resource(res4.string(),0u,4u);
        {
            WorldFrame world(*ray,*gfx);
            world.render(saved_outdoor,with_hud?std::string(argv[9]):"",
                         with_hud?std::string(argv[8]):"",
                         with_scene?std::string(argv[10]):"",
                         with_scene?std::string(argv[11]):"");
        }
        const std::string output=argv[saved_outdoor?(with_scene?13:(with_hud?10:8)):6];
        write_frame(output,gfx->video);
        // Indexed pixels alone do not determine output colors. Export the
        // guest DAC RGB6 values exactly (no 6-to-8-bit rounding) so an
        // independently captured DOS palette can also be compared.
        std::ofstream pal(output+".pal",
                          std::ios::binary|std::ios::trunc);
        CHECK(bool(pal));
        for(u32 i=0u;i<256u;++i) {
            const auto rgb=gfx->video.palette(static_cast<u8>(i));
            pal.put(static_cast<char>(rgb.r));
            pal.put(static_cast<char>(rgb.g));
            pal.put(static_cast<char>(rgb.b));
        }
        CHECK(bool(pal));
        std::cout<<"CODE2_SOURCE_FRAME_INDEX_AND_RGB6_PALETTE_WRITTEN=YES\\n";
        std::cout<<"CODE2_INDEPENDENT_DOS_FRAME_PARITY=NOT_VERIFIED\\n";
        return 0;
    } catch(const std::exception& e) {
        std::cerr<<"CODE2_AUTHENTIC_WORLD_RES4_FRAME_FAIL: "<<e.what()<<"\\n";
        return 1;
    }
}
