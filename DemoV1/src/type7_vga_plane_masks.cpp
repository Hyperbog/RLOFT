#include "rloft/type7_vga_plane_masks.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace rloft {
namespace {

// CODE.2 certified bound LE object-2 begins at file offset 0x75F24.
// Its dgroup bytes at C668..C6C7 are the original 369D8 plane masks.
constexpr std::size_t kCode2Object2FileOffset = 0x75F24U;
constexpr std::array<std::size_t, 3> kMaskOffsets{
    0xC668U, 0xC688U, 0xC6A8U,
};

unsigned bit_count(std::uint8_t value) noexcept {
    unsigned count = 0U;
    for (; value != 0U; value >>= 1U) {
        count += value & 1U;
    }
    return count;
}

} // namespace

Type7VgaPlaneMaskModel Type7VgaPlaneMaskModel::from_code2(
    const std::vector<std::uint8_t>& code2_bytes) {
    Type7VgaPlaneMaskModel model{};
    constexpr std::size_t kInitialModeAt =
        kCode2Object2FileOffset + 0xC029U;
    if (kInitialModeAt >= code2_bytes.size()) {
        throw std::runtime_error(
            "CODE.2 is truncated before initial VGA dispatcher flag");
    }
    const auto initial_mode = code2_bytes[kInitialModeAt];
    if (initial_mode > 1U) {
        throw std::runtime_error(
            "CODE.2 initial VGA dispatcher flag is not Boolean");
    }
    model.initial_individual_columns_mode_ =
        initial_mode != 0U;
    for (std::size_t plane = 0U;
         plane < kMaskOffsets.size(); ++plane) {
        const auto start =
            kCode2Object2FileOffset + kMaskOffsets[plane];
        if (start > code2_bytes.size() ||
            model.plane_tables_[plane].size() >
                code2_bytes.size() - start) {
            throw std::runtime_error(
                "CODE.2 is truncated before type-7 VGA plane masks");
        }
        std::copy_n(
            code2_bytes.begin() +
                static_cast<std::ptrdiff_t>(start),
            model.plane_tables_[plane].size(),
            model.plane_tables_[plane].begin());
    }

    // 36A2B..36A47 addresses a four-by-eight table. Only counts 1..7
    // describe actual nonempty horizontal runs. The total set bits across
    // three masks must exactly equal each run's destination-pixel count.
    for (std::size_t phase = 0U; phase < 4U; ++phase) {
        for (std::size_t count = 1U; count <= 7U; ++count) {
            const auto at = phase * 8U + count;
            unsigned written = 0U;
            for (const auto& table : model.plane_tables_) {
                const auto mask = table[at];
                if ((mask & 0xF0U) != 0U) {
                    throw std::runtime_error(
                        "CODE.2 VGA plane mask exceeds four VGA planes");
                }
                written += bit_count(mask);
            }
            if (written != count) {
                throw std::runtime_error(
                    "CODE.2 VGA masks do not cover source run width");
            }
        }
    }
    return model;
}

Type7VgaPlaneMaskModel Type7VgaPlaneMaskModel::load(
    const GameSource& source) {
    return from_code2(source.read("CODE.2"));
}

Type7VgaRunMasks Type7VgaPlaneMaskModel::for_run(
    std::uint32_t viewport_x,
    std::uint8_t column_count) const {
    if (column_count == 0U || column_count > 7U) {
        // Only sub-runs up to seven pixels fit the original 32-entry
        // three-table addressing. Wider spans need CODE.2 36F6F/37178
        // segmentation before pass dispatch.
        throw std::out_of_range(
            "type-7 VGA run width must be between one and seven");
    }
    const auto phase =
        static_cast<std::uint8_t>(viewport_x & 3U);
    const auto at =
        static_cast<std::size_t>(phase) * 8U +
        static_cast<std::size_t>(column_count);

    Type7VgaRunMasks out{};
    out.screen_phase = phase;
    out.column_count = column_count;
    for (std::size_t plane = 0U; plane < out.planes.size(); ++plane) {
        out.planes[plane] =
            plane_tables_[plane][at];
    }
    return out;
}

} // namespace rloft
