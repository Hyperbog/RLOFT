#include "rloft_native/native_probe_original_textures.hpp"
#include "rloft_native/native_palette_context.hpp"
#include "rloft_native/native_type6_texture.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft_native {
namespace {
constexpr double kMinDist=0.05;
void pixel(std::vector<std::uint8_t>& dest,std::size_t index,
           const NativePalette6& pal,unsigned color) {
    if(color>255U)throw std::runtime_error("invalid original DAC index");
    const auto offset=index*4U;
    const auto rgb=std::size_t(color)*3U;
    const auto expand=[](unsigned channel) {
        return static_cast<std::uint8_t>((channel*255U+31U)/63U);
    };
    dest[offset+0U]=expand(pal.rgb[rgb+2U]);
    dest[offset+1U]=expand(pal.rgb[rgb+1U]);
    dest[offset+2U]=expand(pal.rgb[rgb]);
    dest[offset+3U]=255U;
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
    if(width==0 || width>640U || height==0 || height>480U ||
       view.columns.size()!=width || view.camera.level>=32U)
        throw std::invalid_argument("invalid native original-texture host frame");
    if(world.active_level()!=view.camera.level)
        throw std::invalid_argument("native probe camera/world level mismatch");

    const auto bank=native_world_palette_bank(view.camera.level);
    auto found=palettes_.find(bank);
    if(found==palettes_.end())
        found=palettes_.emplace(bank,
            decode_native_palette(original_colors_,bank)).first;
    const auto& pal=found->second;

    NativeProbeTexturedFrame out;
    out.width=width;out.height=height;
    out.bgra.resize(std::size_t(width)*height*4U);
    // Preview-only ceiling/floor colors are real COLORS entries, but
    // their selection is NOT an original floor/ceiling render path.
    for(unsigned y=0;y<height;++y)
        for(unsigned x=0;x<width;++x)
            pixel(out.bgra,std::size_t(y)*width+x,pal,
                  y<height/2U?1U:2U);
    const auto fov=view.camera.field_of_view_radians;
    const double plane=std::tan(fov/2.0);
    for(unsigned x=0;x<width;++x) {
        const auto& ray=view.columns[x];
        if(!ray.hit)continue;
        if(ray.hit_column>=world.maps().levels[view.camera.level].width ||
           ray.hit_row>=world.maps().levels[view.camera.level].height)
            throw std::runtime_error("original textured ray outside source level");
        const auto material=native_scene_resource_candidates(
            world,view.camera.level,ray.hit_column,ray.hit_row);
        const double sample=(2.0*(double(x)+0.5)/double(width)-1.0);
        const double angle=view.camera.heading_radians+
                           std::atan(plane*sample);
        const auto dx=std::cos(angle),dy=std::sin(angle);
        // Host ray-side guess; original CODE.2 material branch not yet
        // reconstructed. Keep these bank selectors INSIDE preview only.
        const unsigned selected=ray.side==0U?
            (dx>0.0?3U:2U):(dy>0.0?1U:0U);
        const unsigned id=material.candidate_resource_ids[selected];
        if(id>=wide_count_)
            throw std::runtime_error("source-selected preview RES4 ID outside wide textures");
        auto image=textures_.find(id);
        if(image==textures_.end()) {
            const auto decoded=decode_native_type6_texture(source_.entry(id));
            if(decoded.frames!=1 || decoded.width!=64U ||
               decoded.height!=128U || decoded.pixels.size()!=8192U)
                throw std::runtime_error("original preview source texture is not 64x128 EH6");
            image=textures_.emplace(id,std::move(decoded)).first;
            ++out.source_texture_decompressions;
        }
        const auto& tex=image->second;
        const auto hit_u=ray.side==0U?
            view.camera.row_position+dy*ray.ray_distance:
            view.camera.column_position+dx*ray.ray_distance;
        const auto fractional=hit_u-std::floor(hit_u);
        auto tex_x=std::min(63U,
            static_cast<unsigned>(std::floor(fractional*64.0)));
        if((ray.side==0U && dx>0.0) ||
           (ray.side!=0U && dy<0.0))tex_x=63U-tex_x;
        const auto column_height=std::min(
            unsigned(height*2U),
            std::max(1U,static_cast<unsigned>(
                double(height)/std::max(kMinDist,ray.corrected_distance))));
        const auto top=int(height)/2-int(column_height)/2;
        const auto lower=std::max(0,top);
        const auto upper=std::min(int(height),top+int(column_height));
        for(int y=lower;y<upper;++y) {
            const auto tex_y=std::min(127U,
                static_cast<unsigned>((std::uint64_t(y-top)*128U)/column_height));
            const auto index=tex.pixels[std::size_t(tex_y)*64U+tex_x];
            pixel(out.bgra,std::size_t(y)*width+x,pal,index);
            ++out.original_texel_samples;
        }
        ++out.wall_columns;
    }
    return out;
}
} // namespace rloft_native
