#include "rloft/placed_scene_catalog.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

void put16(
    std::vector<std::uint8_t>& bytes,
    std::size_t offset,
    std::uint16_t value) {
    bytes[offset] =
        static_cast<std::uint8_t>(value);
    bytes[offset + 1U] =
        static_cast<std::uint8_t>(
            value >> 8U);
}

}  // namespace

int main() {
    constexpr std::size_t count = 65U;
    std::vector<std::uint8_t> init(
        0x26U + count * 7U, 0);

    put16(init, 0x24U, count);
    const auto at =
        0x26U + 62U * 7U;
    put16(init, at + 0U, 100U);
    put16(init, at + 2U, 0x1234U);
    init[at + 4U] = 16U;
    init[at + 5U] = 3U;
    init[at + 6U] = 2U;

    const auto catalog =
        rloft::PlacedSceneCatalog::parse_init(
            init);
    assert(catalog.size() == count);

    const auto& descriptor =
        catalog.at(62U);
    assert(descriptor.local_shape_base == 100U);
    assert(descriptor.spatial_extent == 0x1234U);
    assert(descriptor.shape_stride == 16U);
    assert(descriptor.variant_count == 3U);
    assert(descriptor.scene_influence_step == 2U);

    return 0;
}
