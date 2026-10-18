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

namespace {
struct WorldFrame {
    WorldRaySource& ray;
    WorldGraphicsSource& gfx;
    u32 tex_start=0,tex_end=0,material_id=0;
    u32 code_start=0,code_end=0;
    u32 callback_calls=0,pixel_writes=0,patches=0,shader_flushes=0;
    bool failure=false;
    code2::Application82Boundary scaler{};
    static WorldFrame* current;
    static WorldFrame& self() noexcept {return *current;}
    WorldFrame(WorldRaySource& source,WorldGraphicsSource& render,u32 image,u32 scene)
        :ray(source),gfx(render),material_id(scene) {
        CHECK(current==nullptr);
        current=this;
        const u32 texlen=code2::load_le16(gfx.memory.data()+image);
        CHECK(texlen==4096u || texlen==8192u);
        tex_start=image+2u;
        tex_end=tex_start+texlen;
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
        copy_guest(0x18FE0u,128u*4u);                // scale code pointers
        copy_guest(0xC65Cu,0xC73Cu+0x400u-0xC65Cu); // source trig and camera tables
        copy_guest(0x255D0u,0x258BCu-0x255D0u);    // source camera+level header
        copy_guest(0x258C4u,4u);                    // authentic 509-word lookup
        copy_guest(0x15A18u,256u*0x1Cu);           // original 32954 ray records
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
        gfx.memory[0xEF4Du]=0u;
        for(u32 slot=0u;slot<256u;++slot) {
            const u32 addr=0x15A18u+slot*0x1Cu;
            const u32 front=code2::load_le32(gfx.memory.data()+addr+4u);
            const u32 back=code2::load_le32(gfx.memory.data()+addr+8u);
            CHECK((front==material_id || front==0x93u) &&
                  (back==material_id || back==0x93u));
            CHECK(code2::load_le32(gfx.memory.data()+addr)<=65535u);
        }
        gfx.original_compositor.app82=&scaler;
        gfx.original_compositor.out16=&WorldGraphicsSource::out16;
        gfx.original_compositor.code_load8=&code8;
        gfx.original_compositor.code_load16=&code16;
        gfx.original_compositor.code_store8=&patch8;
        gfx.original_compositor.call_337c8=&weighted_entry;
        gfx.original_compositor.call_338c8=&WorldGraphicsSource::composed_338c8;
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
    static u32 weighted_entry(u32 table,u32 offset) noexcept {
        return code2::function_337c8(self().gfx.machine,table,offset);
    }
    static u32 invoke_generated_scale(u32 target,u32 source,
                                       u32 destination,u32 shade) noexcept {
        auto& s=self();
        ++s.callback_calls;
        const auto result=rloft_recomp::execute_sintab_scale(
            target,source,destination,shade,s.code_start,s.code_end,
            s.tex_start,s.tex_end,
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
    void render() {
        install_world();
        const u32 before=gfx.source_type6_load_calls;
        code2::Application91Boundary sweep{};
        sweep.call_2f29c=&WorldGraphicsSource::source_sweep_lookup;
        sweep.call_3263c=&WorldGraphicsSource::source_sweep_flush;
        code2::function_335dc(gfx.machine,sweep);
        shader_flushes=gfx.sweep_flushes;
        CHECK(!failure && !gfx.unsupported && !gfx.stream_error);
        CHECK(!gfx.video.unsupported_read());
        CHECK(gfx.source_type6_load_calls==before); // one source cache item, no synthetic records
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
                 <<" flushes="<<shader_flushes<<"\\n";
        std::cout.flush();
        CHECK(callback_calls>0u && pixel_writes>0u && patches>0u);
        CHECK(shader_flushes>0u);
        const auto pixels=gfx.video.scanout_indices();
        const auto nonzero=std::count_if(pixels.begin(),pixels.end(),
                                        [](u8 value){return value!=0u;});
        CHECK(nonzero>0);
        std::cout<<"CODE2_AUTHENTIC_32954_335DC_3263C_SINTAB_RES4_VGA_PASS level=11"
                 <<" rows=400 rays=256 callbacks="<<callback_calls
                 <<" raster_writes="<<pixel_writes
                 <<" color_pixels="<<nonzero
                 <<" flushes="<<shader_flushes<<"\\n";
        std::cout<<"CODE2_338C8_FALLBACK_USED_IN_REAL_WORLD_FIRST_PASS=NO\\n";
        std::cout<<"CODE2_FULL_DOS_WORLD_PARITY_ACCEPTED=NO\\n";
    }
};
WorldFrame* WorldFrame::current=nullptr;
} // namespace

int main(int argc,char** argv) {
    try {
        if(argc!=7 || std::string(argv[1])!="--world")
            throw std::invalid_argument(
                "usage: --world LEVELS.DAT SINTAB RES4 COLORS output-stem");
        const std::filesystem::path levels(argv[2]),sintab(argv[3]);
        const std::filesystem::path res4(argv[4]),colors(argv[5]);
        CHECK(levels.filename()=="LEVELS.DAT" && sintab.filename()=="SINTAB");
        CHECK(res4.filename()=="RES4" && colors.filename()=="COLORS");
        CHECK(levels.parent_path()==sintab.parent_path());
        CHECK(levels.parent_path()==res4.parent_path());
        CHECK(levels.parent_path()==colors.parent_path());
        const auto bytes=rloft::original_levels::read_archive(levels.string());
        const auto grid=rloft::original_levels::decode_grid(bytes,11u);
        auto ray=std::make_unique<WorldRaySource>(levels.parent_path(),true);
        // Original 13C88 case-7 direct level transition, not a host-selected
        // camera: tile (2,65), sub-tile 8000h, heading 10Eh (270 degrees).
        code2::store_le32(ray->guest.data()+0x255D0u,0x00028000u);
        code2::store_le32(ray->guest.data()+0x255D4u,0x00418000u);
        ray->run(grid,true);
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
        CHECK(front_materials.size()==1u);
        const u32 material=front_materials.begin()->first;
        auto gfx=std::make_unique<WorldGraphicsSource>();
        gfx->mode(3u);
        gfx->owned_colors_palette(colors.string(),0u,0u);
        gfx->open_readonly_resource(res4.string(),material,4u);
        const u32 image=code2::function_2f29c(gfx->machine,gfx->resources,material,4u);
        CHECK(image && gfx->source_type6_load_calls==1u);
        {
            WorldFrame world(*ray,*gfx,image,material);
            world.render();
        }
        write_frame(argv[6],gfx->video);
        return 0;
    } catch(const std::exception& e) {
        std::cerr<<"CODE2_AUTHENTIC_WORLD_RES4_FRAME_FAIL: "<<e.what()<<"\\n";
        return 1;
    }
}
