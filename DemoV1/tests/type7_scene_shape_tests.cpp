#include "rloft/resource_assets.hpp"

#include <array>
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

void put32(
    std::vector<std::uint8_t>& bytes,
    std::size_t offset,
    std::uint32_t value) {
    for (int i = 0; i < 4; ++i) {
        bytes[offset + static_cast<std::size_t>(i)] =
            static_cast<std::uint8_t>(
                value >> (i * 8));
    }
}

std::vector<std::uint8_t> literal_lzss(
    const std::vector<std::uint8_t>& body) {
    std::vector<std::uint8_t> out;
    for (std::size_t at = 0;
         at < body.size();) {
        const auto count =
            std::min<std::size_t>(
                8U,
                body.size() - at);
        out.push_back(
            static_cast<std::uint8_t>(
                (1U << count) - 1U));
        out.insert(
            out.end(),
            body.begin() +
                static_cast<std::ptrdiff_t>(at),
            body.begin() +
                static_cast<std::ptrdiff_t>(
                    at + count));
        at += count;
    }
    return out;
}

}  // namespace

int main() {
    // Decoded body:
    //   width=64, x=31..32, y=20..100
    //   two column pointers, both aliasing one descriptor list at +14
    //   descriptor {stop=2, adjustment=-3, entry=4}, then zero terminator.
    std::vector<std::uint8_t> body(22U, 0);
    put16(body, 0U, 64U);
    put16(body, 2U, 31U);
    put16(body, 4U, 32U);
    put16(body, 6U, 20U);
    put16(body, 8U, 100U);
    put16(body, 10U, 14U);
    put16(body, 12U, 14U);
    put16(body, 14U, 2U);
    put16(body, 16U, 0xFFFDU);
    put16(body, 18U, 4U);
    put16(body, 20U, 0U);

    const auto compressed =
        literal_lzss(body);

    // One-entry RES archive. Outer header is count + one absolute offset.
    std::vector<std::uint8_t> archive(
        8U + 7U + compressed.size(), 0);
    put32(archive, 0U, 1U);
    put32(archive, 4U, 8U);

    auto p = 8U;
    archive[p++] = 'E';
    archive[p++] = 'H';
    archive[p++] = 7U;
    put16(archive, p, 0U);
    p += 2U;
    put16(
        archive,
        p,
        static_cast<std::uint16_t>(
            body.size()));
    p += 2U;
    std::copy(
        compressed.begin(),
        compressed.end(),
        archive.begin() +
            static_cast<std::ptrdiff_t>(p));

    const rloft::ResArchive res(
        std::move(archive));
    const auto shape =
        res.decode_type7(0);

    assert(shape.geometry.source_width == 64U);
    assert(shape.geometry.x_min == 31);
    assert(shape.geometry.x_max == 32);
    assert(shape.geometry.y_min == 20);
    assert(shape.geometry.y_max == 100);

    assert(shape.columns.size() == 2U);
    assert(shape.columns[0].source_x == 31);
    assert(shape.columns[1].source_x == 32);
    assert(shape.columns[0].list_offset == 14U);
    assert(shape.columns[1].list_offset == 14U);

    for (const auto& column : shape.columns) {
        assert(column.descriptors.size() == 1U);
        const auto& descriptor =
            column.descriptors.front();
        assert(
            descriptor.raster_stop_selector ==
            2);
        assert(
            descriptor.signed_adjustment ==
            -3);
        assert(
            descriptor.raster_entry_selector ==
            4);
    }

    // Geometry-only API is exactly the full decoder's header.
    const auto geometry =
        res.decode_type7_geometry(0);
    assert(
        geometry.source_width ==
        shape.geometry.source_width);
    assert(geometry.x_min == shape.geometry.x_min);
    assert(geometry.x_max == shape.geometry.x_max);
    assert(geometry.y_min == shape.geometry.y_min);
    assert(geometry.y_max == shape.geometry.y_max);

    return 0;
}
