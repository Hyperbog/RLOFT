#pragma once
#include <array>
#include <cstdint>
#include <filesystem>
#include <vector>
namespace ravenloft::data {
struct Rgb8 { std::uint8_t r=0,g=0,b=0; };
class PaletteDatabase { public: explicit PaletteDatabase(const std::filesystem::path& path);std::size_t count()const{return palettes_.size();}const std::array<Rgb8,256>& at(std::size_t i)const{return palettes_.at(i);}private:std::vector<std::array<Rgb8,256>> palettes_;};
}
