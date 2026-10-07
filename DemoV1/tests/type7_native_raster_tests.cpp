#include "rloft/resource_assets.hpp"

#include <algorithm>
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

rloft::ResArchive archive_for(
    const std::vector<std::uint8_t>& body) {
    const auto compressed =
        literal_lzss(body);
    std::vector<std::uint8_t> archive(
        8U + 7U + compressed.size(),
        0U);
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
    return rloft::ResArchive(
        std::move(archive));
}

}  // namespace

int main() {
    // Header/table end at +14. Bytes +14..+15 are the packed source stream.
    // Both source columns alias the same descriptor list at +16, so those two
    // source texels are consumed once and reused for both columns.
    std::vector<std::uint8_t> body(
        24U, 0U);
    put16(body, 0U, 64U);
    put16(body, 2U, 31U);
    put16(body, 4U, 32U);
    put16(body, 6U, 2U);
    put16(body, 8U, 4U);
    put16(body, 10U, 16U);
    put16(body, 12U, 16U);

    body[14U] = 0x25U;
    body[15U] = 0x47U;

    // Native rows entry/2 .. stop/2 = 2 .. 4.
    put16(body, 16U, 8U);
    put16(body, 18U, 0xFFF0U);
    put16(body, 20U, 4U);
    put16(body, 22U, 0U);

    const auto raster =
        archive_for(body).
            decode_type7_native_raster(0);

    assert(raster.width == 64U);
    assert(raster.height == 128U);
    assert(raster.pixels.size() ==
           64U * 128U);
    assert(raster.mask.size() ==
           raster.pixels.size());

    const auto at =
        [](std::size_t x,
           std::size_t y) {
            return y * 64U + x;
        };

    for (const auto x :
         {31U, 32U}) {
        assert(
            raster.pixels[at(x, 2U)] ==
            0x25U);
        assert(
            raster.pixels[at(x, 3U)] ==
            0x47U);
        assert(
            raster.mask[at(x, 2U)] ==
            0xFFU);
        assert(
            raster.mask[at(x, 3U)] ==
            0xFFU);
        assert(
            raster.mask[at(x, 1U)] ==
            0U);
        assert(
            raster.mask[at(x, 4U)] ==
            0U);
    }

    assert(
        raster.mask[at(30U, 2U)] ==
        0U);
    assert(
        raster.mask[at(33U, 2U)] ==
        0U);

    return 0;
}
