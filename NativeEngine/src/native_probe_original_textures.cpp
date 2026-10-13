#include "rloft_native/native_probe_original_textures.hpp"
#include "rloft_native/native_code2_ray_quadrant.hpp"
#include "rloft_native/native_palette_context.hpp"
#include "rloft_native/native_type6_column_sampler.hpp"
#include "rloft_native/native_type6_texture.hpp"
#include "rloft_native/native_vga_rgb6_scanout.hpp"

#include <algorithm>
#include <array>
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
    // Original CODE.2 1F317..1F3C8 reads the *authored* upper/lower
    // surface texture selectors from the original level header +0x05
    // and +0x07, loading RES4 at the surface-class base 0x258F8.
    // The original RES4 corpus has 600 wall (64x128) followed by 67
    // surface (32x128) EH6 entries. A signed -1 is preserved as a
    // no-texture sentinel, showing the original CODE.2 gradient.
    // The source CHOICE is now original; these host plane UVs,
    // per-column depth and draw order still await source 34A74 parity.
    const double focal=double(view.columns.size())/
        std::tan(view.camera.field_of_view_radians*0.5);
    const auto& level=world.original().level(view.camera.level);
    std::array<const NativeType1Raster*,2> surface{};
    for(unsigned surface_slot=0;surface_slot<2U;++surface_slot) {
        const unsigned at=5U+2U*surface_slot;
        const auto original_id=static_cast<std::int16_t>(
            std::uint16_t(level.original_header[at]) |
            (std::uint16_t(level.original_header[at+1U])<<8U));
        if(original_id<0)continue; // Native original no-texture state
        const unsigned id=wide_count_+unsigned(original_id);
        if(id>=source_.entry_count())
            throw std::runtime_error("original world surface ID outside RES4");
        auto tex=textures_.find(id);
        if(tex==textures_.end()) {
            auto decoded=decode_native_type6_texture(source_.entry(id));
            if(decoded.frames!=1U || decoded.width!=32U ||
               decoded.height!=128U || decoded.pixels.size()!=4096U)
                throw std::runtime_error(
                    "original level surface is not narrow 32x128 EH6");
            tex=textures_.emplace(id,std::move(decoded)).first;
            ++out.source_texture_decompressions;
        }
        surface[surface_slot]=&tex->second;
        out.original_environment_surface_source_verified=true;
    }
    const unsigned horizon=128U;
    for(unsigned x=0;x<view.columns.size();++x) {
        const double sample=2.0*(double(x)+0.5)/
                            double(view.columns.size())-1.0;
        const double angle=view.camera.heading_radians+
            std::atan(std::tan(view.camera.field_of_view_radians*0.5)*sample);
        const double cosine=std::cos(angle-view.camera.heading_radians);
        if(cosine<=0.0)continue;
        const double dir_x=std::cos(angle),dir_y=std::sin(angle);
        for(unsigned sy=0;sy<256U;++sy) {
            const int from_horizon=int(sy)-int(horizon)-pitch_pixels;
            if(from_horizon==0)continue;
            const bool lower=from_horizon>0;
            const auto* tex=surface[lower?1U:0U];
            if(!tex)continue; // actual original backdrop remains visible
            const double depth=0.5*focal/std::abs(double(from_horizon));
            const double distance=depth/cosine;
            const double wx=view.camera.column_position+distance*dir_x;
            const double wy=view.camera.row_position+distance*dir_y;
            if(!std::isfinite(wx)||!std::isfinite(wy) ||
               wx<0.0||wy<0.0||wx>=double(level.width)||
               wy>=double(level.height))continue;
            const double u=wx-std::floor(wx),v=wy-std::floor(wy);
            const auto tx=std::min(31U,static_cast<unsigned>(u*32.0));
            const auto ty=std::min(127U,static_cast<unsigned>(v*128.0));
            const auto index=tex->pixels[std::size_t(ty)*32U+tx];
            video.masked_write(std::size_t(sy+4U)*80U+(x+32U)/4U,
                static_cast<std::uint8_t>(1U<<((x+32U)&3U)),index);
            if(lower)++out.preview_floor_texels;
            else ++out.preview_ceiling_texels;
        }
    }
    std::vector<NativeType6ProjectedColumn> original_columns;
    original_columns.reserve(view.columns.size()*2U);
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

        // Original level header has two 64-word pairs for each
        // geometric wall axis: BASE + UPPER (not west/east and north/
        // south alternatives). The recovered 32F8C/33124 emitter
        // proves the 3/2 and 1/0 pairs but not their live call order.
        // A horizontal world-grid crossing (ray.side==1) looks onto
        // the y-normal face and selects base bank 2; x-normal
        // crossings select base bank 0. The +1 banks describe the
        // optional upper tier (cell bit 0x0200), NOT the reverse face.
        // This mapping was independently present in the older
        // owner-provided v0.7 mesh viewer and matches the original
        // level's four author-assigned texture tables.
        const unsigned selected=ray.side==0U?0U:2U;
        const bool authored_upper=(world.cell(view.camera.level,
            ray.hit_column,ray.hit_row).word&0x0200U)!=0U;

        (void)ray_quadrant; // original sign/reflect still used below
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
        // Match the v0.7 camera's 2-world-unit wall and eye at 1:
        // for the 256-pixel-tall viewport this means a 1-unit
        // half-height and a 128-pixel center horizon. The previous
        // focal/d formula ignored this geometry and snapped to the
        // full 256-line viewport at ordinary walking distances.
        // Deliberately keep authentic 338C8 resampling but do not
        // mistake this HOST perspective for DOS 35730 registers.
        const double scaled=0.5*focal /
            std::max(0.26,ray.corrected_distance);
        const auto projected_height=std::max(1U,std::min(256U,
            static_cast<unsigned>(scaled)));
        original_columns.push_back({
            &texture->second,source_column,x+32U,projected_height,
            0U,0U,working_top,pitch_pixels});
        // Authored cell flag 0x0200 adds a separate upper wall
        // band. The source lookup's next bank is an upper MATERIAL,
        // never a reverse-facing texture. The upper source covers
        // the first header[9] texel rows, as independently confirmed
        // by the original v0.7 level/mesh reader.
        if(authored_upper) {
            const unsigned upper_rows=std::min(128U,unsigned(
                level.original_header[9U]));
            if(upper_rows) {
                const unsigned upper_id=
                    material.candidate_resource_ids[selected+1U];
                if(upper_id>=wide_count_)
                    throw std::runtime_error(
                        "original authored upper RES4 ID outside wall bank");
                auto upper=textures_.find(upper_id);
                if(upper==textures_.end()) {
                    auto decoded=decode_native_type6_texture(
                        source_.entry(upper_id));
                    if(decoded.frames!=1U || decoded.width!=64U ||
                       decoded.height!=128U ||
                       decoded.pixels.size()!=8192U)
                        throw std::runtime_error(
                            "original authored upper EH6 texture invalid");
                    upper=textures_.emplace(
                        upper_id,std::move(decoded)).first;
                    ++out.source_texture_decompressions;
                }
                const unsigned upper_h=std::max(1U,std::min(256U,
                    (projected_height*upper_rows+64U)/128U));
                const int base_top=int((256U-projected_height)/2U);
                const int upper_center_top=int((256U-upper_h)/2U);
                const int upper_offset=base_top-int(upper_h)-
                                       upper_center_top;
                // When optional mouse pitch moves the entire upper
                // tier outside the 256-line view, do not enqueue an
                // invalid VGA strip. Clipping a genuinely offscreen
                // wall must never disable mouse-look.
                const int upper_screen_top=base_top-int(upper_h)+
                                           pitch_pixels;
                const int upper_screen_bottom=base_top+pitch_pixels;
                if(upper_screen_bottom>0 && upper_screen_top<256) {
                    original_columns.push_back({
                        &upper->second,source_column,x+32U,upper_h,
                        0U,0U,working_top,pitch_pixels,
                        upper_offset,upper_rows});
                    ++out.preview_upper_wall_columns;
                }
            }
        }
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
