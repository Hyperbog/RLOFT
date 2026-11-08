#include "rloft/scene_source_banks.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace rloft {
namespace {

std::uint16_t u16(
    const std::uint8_t* p) noexcept {
    return static_cast<std::uint16_t>(p[0]) |
           (static_cast<std::uint16_t>(p[1]) << 8U);
}

}  // namespace

std::uint16_t SceneSourceBanks::at(
    std::size_t source_class) const {
    if (source_class >= base.size()) {
        throw std::out_of_range(
            "scene source class");
    }
    return base[source_class];
}

SceneSourceBanks SceneSourceBanks::parse(
    const std::vector<std::uint8_t>& bytes) {
    // CODE.2 16D04..17049 copies the four words at INIT +06..+0C into
    // DGROUP 27100, one absolute RES3 bank base per scene source class.
    if (bytes.size() < 0x0EU) {
        throw std::runtime_error(
            "INIT is truncated before scene source banks");
    }

    SceneSourceBanks out{};
    for (std::size_t i = 0;
         i < out.base.size();
         ++i) {
        out.base[i] =
            u16(bytes.data() + 0x06U + i * 2U);
    }

    if (!(out.base[0] <= out.base[1] &&
          out.base[1] <= out.base[2] &&
          out.base[2] <= out.base[3])) {
        throw std::runtime_error(
            "INIT scene source banks are not monotone");
    }
    return out;
}

SceneSourceBanks SceneSourceBanks::load(
    const GameSource& source) {
    return parse(source.read("INIT"));
}

}  // namespace rloft
