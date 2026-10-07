#include "rloft/type7_vga_raster_dispatch.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

namespace rloft {
namespace {

std::uint32_t swap_low_bytes(std::uint32_t word) noexcept {
    // CODE.2 36B39/36B72: XCHG AL,AH, not BSWAP or a 32-bit rotate.
    return (word & 0xFFFF0000U) |
           ((word & 0x000000FFU) << 8U) |
           ((word & 0x0000FF00U) >> 8U);
}

std::uint32_t sar2(std::int32_t value) noexcept {
    // Original 36A10 is SAR EDX,2. Avoid implementation-defined signed
    // right shift in portable C++17 while preserving two's-complement bits.
    auto bits = static_cast<std::uint32_t>(value);
    bits >>= 2U;
    if (value < 0) {
        bits |= 0xC0000000U;
    }
    return bits;
}

std::uint32_t address_for(
    std::int32_t destination_column,
    std::uint8_t video_page,
    std::uint32_t renderer_phase,
    std::uint32_t renderer_row_base) noexcept {
    // CODE.2 369DF..36A17. All arithmetic is 32-bit wrapping arithmetic.
    const auto page_start =
        VgaPlanarMemory::kPhysicalBase +
        static_cast<std::uint32_t>(video_page) *
            static_cast<std::uint32_t>(
                VgaPlanarMemory::kPageBytes);
    return page_start +
           renderer_phase * 0x50U +
           renderer_row_base +
           sar2(destination_column);
}

void append_369d8(
    Type7VgaRasterDispatch& out,
    const Type7VgaPlaneMaskModel& masks,
    std::int32_t column,
    std::uint8_t width,
    std::uint8_t page,
    std::uint32_t renderer_phase,
    std::uint32_t renderer_row_base,
    std::uint32_t lookup) {
    const auto triplet =
        masks.for_run(
            static_cast<std::uint32_t>(column),
            width);
    const auto first_address =
        address_for(
            column,
            page,
            renderer_phase,
            renderer_row_base);
    // 36A91..36AFF: the first 369C0 invocation always occurs; the
    // second and third are conditional, and use byte_addr+1/+2.
    for (std::size_t pass = 0U;
         pass < 3U;
         ++pass) {
        if (pass != 0U &&
            triplet.planes[pass] == 0U) {
            continue;
        }
        out.calls.push_back(
            Type7VgaRasterCall{
                column,
                first_address +
                    static_cast<std::uint32_t>(pass),
                lookup,
                triplet.planes[pass],
                static_cast<std::uint8_t>(pass),
            });
    }
}

} // namespace

Type7VgaRasterDispatch
Type7VgaRasterDispatchModel::build(
    const Type7VgaPlaneMaskModel& masks,
    std::int32_t destination_column,
    std::uint8_t run_width,
    std::uint8_t video_page,
    std::uint32_t renderer_phase,
    std::uint32_t renderer_row_base,
    std::uint32_t ecx_lookup,
    bool individual_columns_mode) {
    if (run_width == 0U ||
        run_width > 7U) {
        // 369D8's 4-by-8 mask tables have validated nonempty slots
        // only for lengths 1..7; wider original 36B14 groups require
        // an independently recovered segmentation plan.
        throw std::out_of_range(
            "unvalidated type-7 VGA raster run length");
    }

    Type7VgaRasterDispatch out{};
    out.calls.reserve(
        static_cast<std::size_t>(
            run_width) * 3U);

    if (!individual_columns_mode) {
        // 36B1A..36B40: one 369D8 pass with original run length,
        // then persist XCHG AL,AH in global 105E7.
        append_369d8(
            out, masks,
            destination_column,
            run_width,
            video_page,
            renderer_phase,
            renderer_row_base,
            ecx_lookup);
        out.lookup_after_dispatch =
            swap_low_bytes(ecx_lookup);
        return out;
    }

    if (destination_column >
        std::numeric_limits<std::int32_t>::max() -
            static_cast<std::int32_t>(
                run_width - 1U)) {
        throw std::out_of_range(
            "type-7 destination-column increment overflows");
    }
    // 36B42..36B95: temporarily set 105DF=1; loop original 105DF
    // count, advancing 105E3 and swapping lookup bytes on odd pixels.
    // The global lookup word, destination column and count are restored.
    for (unsigned i = 0U;
         i < run_width;
         ++i) {
        const auto column =
            destination_column +
            static_cast<std::int32_t>(i);
        const auto swapped =
            (static_cast<std::uint32_t>(
                column) & 1U) != 0U;
        append_369d8(
            out, masks,
            column,
            1U,
            video_page,
            renderer_phase,
            renderer_row_base,
            swapped
                ? swap_low_bytes(ecx_lookup)
                : ecx_lookup);
    }
    out.lookup_after_dispatch = ecx_lookup;
    return out;
}

} // namespace rloft
