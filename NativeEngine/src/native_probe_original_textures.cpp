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
     backdrop_(media.read("CODE.2")),
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
       width!=320U || view.columns.size()!=256U || view.camera.level>=32U)
        throw std::invalid_argument("invalid original planar preview frame or host rays");
    const auto& map=world.maps().levels.at(view.camera.level);

    const auto bank=native_world_palette_bank(view.camera.level);
    auto palette=palettes_.find(bank);
    if(palette==palettes_.end())
        palette=palettes_.emplace(bank,
            decode_native_palette(original_colors_,bank)).first;

    // Original code 33E28/33DE0 sky/floor environment fill with
    // level header bytes 298/299 and actual CODE.2 FE94 shade table.
    NativeIndexedVgaPlanes video;
    const int pitch_pixels=static_cast<int>(std::lround(
        view.camera.pitch_radians*100.0));
    backdrop_.fill(world.original().level(view.camera.level),
                   video,pitch_pixels);

    NativeProbeTexturedFrame out;
    out.width=width;out.height=height;
    // HOST-ONLY textured floor/ceiling preview. CODE.2 33E28/33DE0
    // writes the authentic background gradients, but the original
    // game overlays projected surface polygons. Until source 32620/3263C
    // automatic floor/ceiling material dispatch has been recovered,
    // source-backed RES4 pixels are mapped on the horizontal planes
    // using the real level cell's resource candidates. The surface
    // choices and projection are provisional, not DOS parity.
    const double focal=double(view.columns.size())/
        std::tan(view.camera.field_of_view_radians*0.5);
    const auto& level=world.original().level(view.camera.level);
    const unsigned horizon=128U;
    for(unsigned x=0;x<view.columns.size();++x) {
        const double sample=2.0*(double(x)+0.5)/
                            double(view.columns.size())-1.0;
        const double angle=view.camera.heading_radians+
            std::atan(std::tan(view.camera.field_of_view_radians*0.5)*sample);
        const double cosine=std::cos(angle-view.camera.heading_radians);
        const double dir_x=std::cos(angle),dir_y=std::sin(angle);
        if(cosine<=0.0)continue;
        for(unsigned sy=0;sy<256U;++sy) {
            const int from_horizon=int(sy)-int(horizon)-pitch_pixels;
            if(from_horizon==0)continue;
            const bool lower=from_horizon>0;
            // Outdoors: retain the source CODE.2 level gradient sky
            // instead of manufacturing a sky texture from wall art.
            if(!lower && level.original_header[0x290U]!=0xffU)
                continue;
            const double depth=0.5*focal/std::abs(double(from_horizon));
            const double distance=depth/cosine;
            const double wx=view.camera.column_position+distance*dir_x;
            const double wy=view.camera.row_position+distance*dir_y;
            if(!std::isfinite(wx)||!std::isfinite(wy) ||
               wx<0.0||wy<0.0||wx>=double(level.width)||
               wy>=double(level.height))continue;
            const unsigned cx=static_cast<unsigned>(wx);
            const unsigned cy=static_cast<unsigned>(wy);
            const auto candidate=native_scene_resource_candidates(
                world,view.camera.level,cx,cy);
            // The source banks are real, the assumption they supply
            // projected floor/ceiling art is a PREVIEW ONLY.
            const unsigned id=candidate.candidate_resource_ids[lower?0U:1U];
            if(id>=wide_count_)continue;
            auto tex=textures_.find(id);
            if(tex==textures_.end()) {
                auto decoded=decode_native_type6_texture(source_.entry(id));
                if(decoded.frames!=1U || decoded.width!=64U ||
                   decoded.height!=128U || decoded.pixels.size()!=8192U)
                    throw std::runtime_error("original projected surface not EH6 wide");
                tex=textures_.emplace(id,std::move(decoded)).first;
                ++out.source_texture_decompressions;
            }
            const double u=wx-std::floor(wx),v=wy-std::floor(wy);
            const auto tx=std::min(63U,static_cast<unsigned>(u*64.0));
            // A wall source 64x128 has a physical 64x64 preview
            // sample domain: every second source scanline.
            const auto ty=std::min(63U,static_cast<unsigned>(v*64.0));
            const auto index=tex->second.pixels[
                std::size_t(ty*2U)*64U+tx];
            video.masked_write(std::size_t(sy+4U)*80U+(x+32U)/4U,
                static_cast<std::uint8_t>(1U<<((x+32U)&3U)),index);
            if(lower)++out.preview_floor_texels;
            else ++out.preview_ceiling_texels;
        }
    }
    std::vector<NativeType6ProjectedColumn> original_columns;
    original_columns.reserve(view.columns.size());
    const auto plane=std::tan(view.camera.field_of_view_radians*0.5);
    const unsigned working_top=4U;
    for(unsigned x=0;x<view.columns.size();++x) {
        const auto& ray=view.columns[x];
        if(!ray.hit)continue;
        if(ray.hit_column>=map.width || ray.hit_row>=map.height)
            throw std::runtime_error("original textured ray outside source level");
        const auto material=native_scene_resource_candidates(
            world,view.camera.level,ray.hit_column,ray.hit_row);
        const double sample=2.0*(double(x)+0.5)/double(view.columns.size())-1.0;
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
        // HOST depth mapping, deliberately bounded by swept collision
        // clearance (radius 0.26). 55 px/cell avoids the previous
        // 400/distance full-height saturation when approaching walls.
        // DOS 35730 remains upstream camera reconstruction work.
        // Perspective focal length in the original 256-column
        // world view. DOS 320x400 pixels are displayed with 2:1
        // horizontal aspect; 1 world-cell projected wall height is
        // focal/distance, not the previous arbitrary 55/d estimate.
        const double scaled=focal /
            std::max(0.26,ray.corrected_distance);
        const auto projected_height=std::max(1U,std::min(256U,
            static_cast<unsigned>(scaled)));
        original_columns.push_back({
            &texture->second,source_column,x+32U,projected_height,
            0U,0U,working_top,pitch_pixels});
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
    out.original_code2_backdrop_used=true;
    out.original_code2_sampler_used=true;
    out.source_quadrant_state_used=true;
    return out;
}
} // namespace rloft_native
