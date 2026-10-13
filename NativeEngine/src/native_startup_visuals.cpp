#include "rloft_native/native_startup_visuals.hpp"
#include "rloft_native/native_media.hpp"
#include "rloft_native/native_lzss.hpp"
#include <limits>
#include <string>
#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <utility>
namespace rloft_native {
NativeStartupVisualSource::NativeStartupVisualSource(const NativeMedia& source)
    :res0_(source.read("RES0")),colors_(source.read("COLORS")),text_(source.read("T")){
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

namespace {
std::uint16_t word(const std::vector<std::uint8_t>& bytes,std::size_t at) {
    if(at>bytes.size()||bytes.size()-at<2U)
        throw std::runtime_error("original font/text truncated word");
    return std::uint16_t(unsigned(bytes[at])|(unsigned(bytes[at+1U])<<8U));
}
std::uint32_t dword(const std::vector<std::uint8_t>& bytes,std::size_t at) {
    if(at>bytes.size()||bytes.size()-at<4U)
        throw std::runtime_error("original T archive truncated DWORD");
    return std::uint32_t(bytes[at])|
           (std::uint32_t(bytes[at+1U])<<8U)|
           (std::uint32_t(bytes[at+2U])<<16U)|
           (std::uint32_t(bytes[at+3U])<<24U);
}
}
rloft::Type2Font NativeStartupVisualSource::res0_font(int index) const {
    if(index<0||std::size_t(index)>=res0_.entry_count())
        throw std::out_of_range("original RES0 font index outside archive");
    const auto raw=res0_.entry(std::size_t(index));
    if(raw.size()<5U+136U||raw[0]!='E'||raw[1]!='H'||raw[2]!=2)
        throw std::runtime_error("original RES0 subtitle font is not EH type 2");
    const auto header=int(std::int16_t(word(raw,3U)));
    if(header<136||std::size_t(header)>raw.size()-5U)
        throw std::runtime_error("original RES0 EH2 font header length invalid");
    rloft::Type2Font result;
    std::copy_n(raw.begin()+5,128,result.widths.begin());
    const int frames=int(std::int16_t(word(raw,133U)));
    const int height=int(std::int16_t(word(raw,135U)));
    const int units=int(raw[137U]);
    if(frames<=0||height<=0||units<=0)
        throw std::runtime_error("original RES0 font dimensions invalid");
    const std::uint64_t pixels=std::uint64_t(frames)*unsigned(height)*
                               (unsigned(units)*8U);
    if(pixels>32ULL*1024ULL*1024ULL)
        throw std::runtime_error("original RES0 EH2 font exceeds pixel cap");
    const std::size_t body_at=5U+std::size_t(header);
    const std::uint8_t compression=raw[139U];
    std::vector<std::uint8_t> pixels_out;
    if(compression==0) {
        if(raw.size()-body_at!=pixels)
            throw std::runtime_error("original RES0 raw EH2 font pixel count invalid");
        pixels_out.assign(raw.begin()+std::ptrdiff_t(body_at),raw.end());
    } else if(compression==3) {
        pixels_out=native_lzss4k(raw.data()+body_at,raw.size()-body_at,
                                std::size_t(pixels));
    } else {
        throw std::runtime_error("original RES0 unsupported EH2 font compression");
    }
    result.raster={frames,units*8,height,raw[138U],std::move(pixels_out)};
    return result;
}
std::string NativeStartupVisualSource::text(int index) const {
    if(index<0||text_.size()<4U)
        throw std::out_of_range("original T text header or ID invalid");
    const auto count=dword(text_,0);
    if(std::uint32_t(index)>=count||count>(text_.size()-4U)/4U)
        throw std::out_of_range("original T text record index invalid");
    const std::size_t at=std::size_t(dword(text_,4U+std::size_t(index)*4U));
    const auto length=word(text_,at);
    if(at>text_.size()||text_.size()-at<2U+length)
        throw std::runtime_error("original T text record exceeds file");
    std::string result;
    result.reserve(length);
    for(std::size_t i=0;i<length;++i)
        result.push_back(char(text_[at+2U+i]^0xDCU));
    return result;
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
