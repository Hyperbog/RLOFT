#include "games/ravenloft/data/PaletteDatabase.h"
#include "engine/core/FileSystem.h"
#include <stdexcept>
namespace ravenloft::data {
PaletteDatabase::PaletteDatabase(const std::filesystem::path& path){auto b=ssi::core::readFile(path);if(b.empty()||b[0]!=15||b.size()!=1+15*768)throw std::runtime_error("Invalid COLORS");palettes_.resize(15);std::size_t p=1;for(auto& pal:palettes_)for(auto& c:pal){auto cv=[](std::uint8_t v){return static_cast<std::uint8_t>((v<<2)|(v>>4));};c={cv(b[p]),cv(b[p+1]),cv(b[p+2])};p+=3;}}
}
