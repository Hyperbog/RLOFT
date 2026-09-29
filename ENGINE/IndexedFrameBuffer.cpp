#include "engine/render/IndexedFrameBuffer.h"
#include <algorithm>
#include <fstream>
#include <stdexcept>
namespace ssi::render {
IndexedFrameBuffer::IndexedFrameBuffer(std::uint16_t w,std::uint16_t h):width_(w),height_(h),pixels_(std::size_t(w)*h){if(!w||!h)throw std::runtime_error("Invalid framebuffer size");}
void IndexedFrameBuffer::clear(std::uint8_t c){std::fill(pixels_.begin(),pixels_.end(),c);}
void IndexedFrameBuffer::set(int x,int y,std::uint8_t c){if(x<0||y<0||x>=width_||y>=height_)return;pixels_[std::size_t(y)*width_+x]=c;}
void IndexedFrameBuffer::blitMasked(int dx,int dy,std::uint16_t sw,std::uint16_t sh,std::span<const std::uint8_t> src,std::span<const std::uint8_t> a){
    if(src.size()!=std::size_t(sw)*sh||a.size()!=src.size())throw std::runtime_error("Masked blit size mismatch");
    for(int y=0;y<sh;++y)for(int x=0;x<sw;++x){auto si=std::size_t(y)*sw+x;if(a[si])set(dx+x,dy+y,src[si]);}
}
std::vector<std::uint8_t> IndexedFrameBuffer::toRgb(const std::array<ravenloft::data::Rgb8,256>& pal)const{
    std::vector<std::uint8_t> out(pixels_.size()*3);for(std::size_t i=0;i<pixels_.size();++i){const auto c=pal[pixels_[i]];out[i*3]=c.r;out[i*3+1]=c.g;out[i*3+2]=c.b;}return out;
}
void IndexedFrameBuffer::writePpm(const std::filesystem::path& p,const std::array<ravenloft::data::Rgb8,256>& pal)const{
    auto rgb=toRgb(pal);std::ofstream f(p,std::ios::binary);if(!f)throw std::runtime_error("Could not create PPM");f<<"P6\n"<<width_<<" "<<height_<<"\n255\n";f.write(reinterpret_cast<const char*>(rgb.data()),static_cast<std::streamsize>(rgb.size()));
}
void IndexedFrameBuffer::writeBmp(const std::filesystem::path& p,const std::array<ravenloft::data::Rgb8,256>& pal)const{
    const std::uint32_t rowBytes=((static_cast<std::uint32_t>(width_)*3u+3u)/4u)*4u;
    const std::uint32_t pixelBytes=rowBytes*height_;
    const std::uint32_t fileBytes=54u+pixelBytes;
    std::ofstream f(p,std::ios::binary);if(!f)throw std::runtime_error("Could not create BMP");
    auto u16=[&](std::uint16_t v){char b[2]={char(v&0xff),char((v>>8)&0xff)};f.write(b,2);};
    auto u32=[&](std::uint32_t v){char b[4]={char(v&0xff),char((v>>8)&0xff),char((v>>16)&0xff),char((v>>24)&0xff)};f.write(b,4);};
    f.put('B');f.put('M');u32(fileBytes);u16(0);u16(0);u32(54);
    u32(40);u32(width_);u32(height_);u16(1);u16(24);u32(0);u32(pixelBytes);u32(2835);u32(2835);u32(0);u32(0);
    std::vector<char> row(rowBytes,0);
    for(int y=static_cast<int>(height_)-1;y>=0;--y){
        std::fill(row.begin(),row.end(),0);
        for(std::uint32_t x=0;x<width_;++x){
            const auto c=pal[pixels_[std::size_t(y)*width_+x]];
            row[x*3+0]=static_cast<char>(c.b);row[x*3+1]=static_cast<char>(c.g);row[x*3+2]=static_cast<char>(c.r);
        }
        f.write(row.data(),static_cast<std::streamsize>(row.size()));
    }
}

}
