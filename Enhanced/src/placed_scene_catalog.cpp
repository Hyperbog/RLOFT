#include "rloft/placed_scene_catalog.hpp"

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

PlacedSceneCatalog
PlacedSceneCatalog::parse_init(
    const std::vector<std::uint8_t>& bytes) {
    constexpr std::size_t kCountAt = 0x24U;
    constexpr std::size_t kRecordsAt = 0x26U;
    constexpr std::size_t kStride = 7U;

    if (bytes.size() < kRecordsAt) {
        throw std::runtime_error(
            "INIT is truncated before class-1 descriptors");
    }

    const auto count =
        static_cast<std::size_t>(
            u16(bytes.data() + kCountAt));
    if (count >
        (bytes.size() - kRecordsAt) /
            kStride) {
        throw std::runtime_error(
            "INIT class-1 descriptors exceed EOF");
    }

    PlacedSceneCatalog out{};
    out.descriptors_.reserve(count);

    for (std::size_t i = 0;
         i < count;
         ++i) {
        const auto* p =
            bytes.data() +
            kRecordsAt +
            i * kStride;

        PlacedSceneDescriptor descriptor{};
        descriptor.local_shape_base =
            u16(p + 0U);
        descriptor.spatial_extent =
            u16(p + 2U);
        descriptor.shape_stride =
            p[4U];
        descriptor.variant_count =
            p[5U];
        descriptor.scene_influence_step =
            p[6U];

        out.descriptors_.push_back(
            descriptor);
    }

    return out;
}

PlacedSceneCatalog PlacedSceneCatalog::load(
    const GameSource& source) {
    return parse_init(
        source.read("INIT"));
}

const PlacedSceneDescriptor&
PlacedSceneCatalog::at(
    std::size_t type_id) const {
    if (type_id >= descriptors_.size()) {
        throw std::out_of_range(
            "placed scene descriptor type");
    }
    return descriptors_[type_id];
}

}  // namespace rloft
