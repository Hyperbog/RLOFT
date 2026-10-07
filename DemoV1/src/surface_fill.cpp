#include "rloft/surface_fill.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {
namespace {

constexpr std::size_t kCode2DataFileBase = 0x75F24U;
constexpr std::size_t kFillTableDgroup = 0xFE94U;

}  // namespace

SurfaceFillTable SurfaceFillTable::from_code2(
    const std::vector<std::uint8_t>& code2) {
    const auto start =
        kCode2DataFileBase + kFillTableDgroup;
    constexpr std::size_t kBytes = 129U;

    if (start > code2.size() ||
        kBytes > code2.size() - start) {
        throw std::runtime_error(
            "CODE.2 is truncated before surface fill table");
    }

    SurfaceFillTable out{};
    std::copy_n(
        code2.begin() +
            static_cast<std::ptrdiff_t>(start),
        kBytes,
        out.subtract.begin());
    return out;
}

void SurfaceFillModel::draw(
    IndexedFramebuffer& framebuffer,
    const SurfaceFillTable& table,
    int first_row,
    std::uint8_t base_color) {
    for (int row = 0; row < kRows; ++row) {
        // Encoded EBX is 0x80 on the first scanline and decrements to 1.
        const auto index =
            static_cast<std::size_t>(kRows - row);
        const auto color =
            static_cast<std::uint8_t>(
                base_color - table.subtract[index]);
        framebuffer.fill_rect(
            first_row + row,
            kColumn,
            1,
            kWidth,
            color);
    }
}

void SurfaceFillModel::draw_upper(
    IndexedFramebuffer& framebuffer,
    const SurfaceFillTable& table,
    std::uint8_t base_color) {
    draw(framebuffer, table, kUpperRow, base_color);
}

void SurfaceFillModel::draw_lower(
    IndexedFramebuffer& framebuffer,
    const SurfaceFillTable& table,
    std::uint8_t base_color) {
    draw(framebuffer, table, kLowerRow, base_color);
}

}  // namespace rloft
