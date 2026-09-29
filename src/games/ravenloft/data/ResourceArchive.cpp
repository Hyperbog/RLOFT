#include "games/ravenloft/data/ResourceArchive.h"
#include "games/ravenloft/data/Binary.h"
#include "engine/core/FileSystem.h"
#include <stdexcept>

namespace ravenloft::data {
ResourceArchive::ResourceArchive(const std::filesystem::path& path) : bytes_(ssi::core::readFile(path)) {
    if (bytes_.size() < 8) throw std::runtime_error("Resource archive too small: " + path.string());
    const auto n = u32(bytes_,0);
    if (n == 0 || 4ull + 4ull*n > bytes_.size()) throw std::runtime_error("Invalid resource count: " + path.string());
    offsets_.reserve(n);
    for (std::uint32_t i=0;i<n;++i) offsets_.push_back(u32(bytes_,4+4*i));
    const std::size_t tableEnd = 4 + 4*n;
    for (std::size_t i=0;i<offsets_.size();++i) {
        const auto a=offsets_[i]; const auto e=(i+1<offsets_.size()?offsets_[i+1]:bytes_.size());
        if (a < tableEnd || a > e || e > bytes_.size()) throw std::runtime_error("Invalid resource offsets: " + path.string());
    }
}
std::span<const std::uint8_t> ResourceArchive::record(std::size_t index) const {
    if (index>=offsets_.size()) throw std::out_of_range("Resource index");
    const auto a=offsets_[index]; const auto e=(index+1<offsets_.size()?offsets_[index+1]:bytes_.size());
    return std::span<const std::uint8_t>(bytes_.data()+a, e-a);
}
}
