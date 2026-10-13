#include "rloft_native/native_startup_visuals.hpp"
#include "rloft_native/native_media.hpp"
#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <utility>
namespace rloft_native {
NativeStartupVisualSource::NativeStartupVisualSource(const NativeMedia& source)
    :res0_(source.read("RES0")),colors_(source.read("COLORS")){
    if(colors_.size()!=1U+15U*768U || colors_[0]!=15U)
        throw std::runtime_error("original CODE.2 startup COLORS bank count/length invalid");
}
rloft::Type1Raster NativeStartupVisualSource::res0_type1(int index) const {
    if(index<0 || std::size_t(index)>=res0_.entry_count())
        throw std::out_of_range("original startup RES0 index outside archive");
    auto source=decode_native_type1_raster(res0_.entry(std::size_t(index)));
    rloft::Type1Raster out;
    out.count=source.frames;
    out.width=source.width;
    out.height=source.height;
    out.transparent_index=source.transparent_index;
    out.pixels=std::move(source.pixels);
    return out;
}
rloft::Palette6 NativeStartupVisualSource::colors_palette(
    int index,int brightness) const {
    if(index<0 || index>=15)
        throw std::out_of_range("original startup COLORS palette index");
    brightness=std::clamp(brightness,0,63);
    rloft::Palette6 out;
    const auto from=colors_.begin()+1U+std::size_t(index)*768U;
    std::copy_n(from,768,out.rgb.begin());
    if(brightness==0)return out;
    // Same original brightness transform as the existing DemoV1 reader.
    for(std::size_t i=0;i<256;++i) {
        auto* rgb=out.rgb.data()+i*3U;
        const int peak=std::max({int(rgb[0]),int(rgb[1]),int(rgb[2])});
        if(!peak)continue;
        const int boost=((63-peak)*brightness+31)/63;
        const int target=peak+boost;
        for(int c=0;c<3;++c)
            rgb[c]=std::uint8_t((int(rgb[c])*target+peak/2)/peak);
    }
    return out;
}
} // namespace rloft_native
