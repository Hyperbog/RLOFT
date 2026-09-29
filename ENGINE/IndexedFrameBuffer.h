#pragma once
#include "games/ravenloft/data/PaletteDatabase.h"
#include <cstdint>
#include <filesystem>
#include <span>
#include <vector>

namespace ssi::render {
class IndexedFrameBuffer {
public:
    IndexedFrameBuffer(std::uint16_t width,std::uint16_t height);
    std::uint16_t width()const{return width_;}
    std::uint16_t height()const{return height_;}
    std::span<std::uint8_t> pixels(){return pixels_;}
    std::span<const std::uint8_t> pixels()const{return pixels_;}
    void clear(std::uint8_t color);
    void set(int x,int y,std::uint8_t color);
    void blitMasked(int dstX,int dstY,std::uint16_t srcW,std::uint16_t srcH,
                    std::span<const std::uint8_t> source,std::span<const std::uint8_t> alpha);
    std::vector<std::uint8_t> toRgb(const std::array<ravenloft::data::Rgb8,256>& palette)const;
    void writePpm(const std::filesystem::path& path,const std::array<ravenloft::data::Rgb8,256>& palette)const;
    void writeBmp(const std::filesystem::path& path,const std::array<ravenloft::data::Rgb8,256>& palette)const;
private:
    std::uint16_t width_=0,height_=0;
    std::vector<std::uint8_t> pixels_;
};
}
