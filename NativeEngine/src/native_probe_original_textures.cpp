#include "rloft_native/native_probe_original_textures.hpp"
#include "rloft_native/native_code2_ray_quadrant.hpp"
#include "rloft_native/native_palette_context.hpp"
#include "rloft_native/native_type6_column_sampler.hpp"
#include "rloft_native/native_type6_texture.hpp"
#include "rloft_native/native_vga_rgb6_scanout.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace rloft_native {
namespace {
constexpr double kPi=3.14159265358979323846;
constexpr double kMinDist=0.05;
unsigned host_angle_to_original_quadrant_tenths(double dx,double dy) {
    // The host DDA angle is NOT the original CODE.2 c714+c73c
    // ray-angle source. This adapter converts only its quadrant
    // for the already source-verified 32954 signed stepping logic.
    auto angle=std::atan2(-dy,dx);
    if(angle<0.0)angle+=2.0*kPi;
    auto tenths=static_cast<unsigned>(std::floor(
        angle*(1800.0/kPi)));
    if(tenths>=3600U)tenths=0U;
    return tenths;
}
}
NativeProbeOriginalTextures::NativeProbeOriginalTextures(
    const NativeMedia& media,unsigned original_wide_count)
    :source_(media.read("RES4")),
     original_colors_(media.read("COLORS")),
     wide_count_(original_wide_count) {
    if(wide_count_==0 || wide_count_>=source_.entry_count())
        throw std::runtime_error("native probe original RES4 wide source count invalid");
}

NativeProbeTexturedFrame NativeProbeOriginalTextures::render(
    const NativeWorldRuntime& world,const NativeProbeView& view,
    unsigned width,unsigned height) {
    if(width==0 || width>320U ||
       (height!=200U && height!=400U) ||
       view.columns.size()!=width || view.camera.level>=32U)
        throw std::invalid_argument("invalid original planar preview frame or host rays");
    const auto& map=world.maps().levels.at(view.camera.level);

    const auto bank=native_world_palette_bank(view.camera.level);
    auto palette=palettes_.find(bank);
    if(palette==palettes_.end())
        palette=palettes_.emplace(bank,
            decode_native_palette(original_colors_,bank)).first;

    // Original CODE.2 VGA: 4 x 65536 bytes, one 80-byte scanline per
    // plane with x&3 as the sequencer mask. Source background indices
    // 1/2 are a *host presentation choice*, NOT original floor art.
    NativeIndexedVgaPlanes video;
    for(unsigned y=0;y<height;++y) {
        const auto index=static_cast<std::uint8_t>(
            y<height/2U?1U:2U);
        for(unsigned x=0;x<width;++x) {
            const auto byte=std::size_t(y)*80U+x/4U;
            video.masked_write(byte,
                static_cast<std::uint8_t>(1U<<(x&3U)),index);
        }
    }

    NativeProbeTexturedFrame out;
    out.width=width;out.height=height;
    std::vector<NativeType6ProjectedColumn> original_columns;
    original_columns.reserve(width);
    const auto plane=std::tan(view.camera.field_of_view_radians*0.5);
    const unsigned working_top=height==400U?72U:0U;
    for(unsigned x=0;x<width;++x) {
        const auto& ray=view.columns[x];
        if(!ray.hit)continue;
        if(ray.hit_column>=map.width || ray.hit_row>=map.height)
            throw std::runtime_error("original textured ray outside source level");
        const auto material=native_scene_resource_candidates(
            world,view.camera.level,ray.hit_column,ray.hit_row);
        const double sample=2.0*(double(x)+0.5)/double(width)-1.0;
        const double angle=view.camera.heading_radians+
                           std::atan(plane*sample);
        const auto dx=std::cos(angle),dy=std::sin(angle);
        const auto ray_quadrant=quadrant_native_code2_32954(
            host_angle_to_original_quadrant_tenths(dx,dy));

        // CODE.2 32F8C stores banks 3+2 for x-wall path, 33124
        // banks 1+0 for y-wall path. Which of the TWO words is drawn
        // remains an unverified host wall-face choice, so we select
        // according to its source quadrant for preview ONLY.
        const unsigned selected=ray.side==0U?
            (ray_quadrant.original_step_x_25610==0x00010000U?
                ray_quadrant.original_x_wall_first_bank:
                ray_quadrant.original_x_wall_second_bank):
            (ray_quadrant.original_step_y_25604==0x00010000U?
                ray_quadrant.original_y_wall_first_bank:
                ray_quadrant.original_y_wall_second_bank);
        const unsigned id=material.candidate_resource_ids[selected];
        if(id>=wide_count_)
            throw std::runtime_error("original RES4 host-selected ID outside wide textures");
        auto texture=textures_.find(id);
        if(texture==textures_.end()) {
            const auto decoded=decode_native_type6_texture(source_.entry(id));
            if(decoded.frames!=1U || decoded.width!=64U ||
               decoded.height!=128U || decoded.pixels.size()!=8192U)
                throw std::runtime_error("original preview selected non-wide EH6 texture");
            texture=textures_.emplace(id,std::move(decoded)).first;
            ++out.source_texture_decompressions;
        }

        // Convert the *host* wall intersection into its 64-column
        // approximate phase. Original CODE.2's two mirror flags are
        // then applied exactly (32FA5 / 3313D). This does not claim
        // we recovered the authentic 255E8/255F0 phase registers.
        const auto u=ray.side==0U?
            view.camera.row_position+dy*ray.ray_distance:
            view.camera.column_position+dx*ray.ray_distance;
        const double fraction=u-std::floor(u);
        unsigned source_column=std::min(63U,
            static_cast<unsigned>(std::floor(fraction*64.0)));
        const bool reflect=ray.side==0U?
            ray_quadrant.x_path_reflect_32f8c:
            ray_quadrant.y_path_reflect_33124;
        if(reflect)source_column=63U-source_column;

        // CODE.2 338C8's rasterizer accepts 1..256 vertical pixels.
        // The incoming height is STILL host DDA perspective, NOT
        // reconstructed original CODE.2 35730 depth.
        const auto scaled=double(height)/
            std::max(kMinDist,ray.corrected_distance);
        const auto projected_height=std::max(1U,std::min(256U,
            static_cast<unsigned>(scaled)));
        original_columns.push_back({
            &texture->second,source_column,x,projected_height,
            0U,0U,working_top});
        ++out.wall_columns;
    }

    if(!original_columns.empty()) {
        // This is the previously original-byte-verified CODE.2
        // 338C8 fixed-point 0x800000 / projected_height sampler,
        // NOT the old host 128*row/height approximation.
        const auto drawn=draw_native_type6_columns(original_columns,video);
        out.original_texel_samples=
            static_cast<unsigned>(drawn.source_texel_reads);
    }
    const auto rgb6=scanout_native_vga_rgb6(
        video,palette->second,bank,0U,width,height);
    out.bgra.resize(std::size_t(width)*height*4U);
    for(std::size_t i=0;i<std::size_t(width)*height;++i) {
        const auto rgb=3U*i,bgra=4U*i;
        const auto expand=[](std::uint8_t c) {
            return static_cast<std::uint8_t>(
                (unsigned(c)*255U+31U)/63U);
        };
        out.bgra[bgra]=expand(rgb6.rgb6[rgb+2U]);
        out.bgra[bgra+1U]=expand(rgb6.rgb6[rgb+1U]);
        out.bgra[bgra+2U]=expand(rgb6.rgb6[rgb]);
        out.bgra[bgra+3U]=255U;
    }
    out.original_code2_sampler_used=true;
    out.source_quadrant_state_used=true;
    return out;
}
} // namespace rloft_native
