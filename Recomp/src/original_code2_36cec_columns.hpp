#pragma once
// Original CODE.2 36CEC input/RES3 plan, not a completed VGA rasterizer.
// Source: retail CODE.2 SHA256 853e0a4d...265bd314,
// object-1 logical 36CEC..36EE8 and 36EED..372C3.
// Caller: 2C4A3..2C4E6 (six cdecl arguments, left-to-right below).
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace rloft_recomp {
struct Original36cecCall {
    std::uint32_t screen_x{};
    std::uint32_t res3_id{};
    std::uint32_t projected_size{};
    std::uint32_t lookup_bias{};
    std::uint32_t vertical_mode{};
    std::uint32_t ray_depth{};
};

struct Original36cecDescriptor {
    std::uint16_t source_x{};
    std::int16_t signed_adjustment{};
    std::uint16_t stop_selector{};
    std::uint16_t entry_selector{};
};

struct Original36cecPlan {
    bool drawable{};
    std::uint32_t original_scale{};
    std::uint32_t clamped_scale{};
    std::uint32_t scale_pointer_index{};
    std::uint32_t lookup_index{};
    std::uint16_t packed_lookup{};
    std::vector<std::uint16_t> source_x_order;
    std::vector<Original36cecDescriptor> descriptors;
};

inline std::uint16_t read_le16_36cec(
    const std::uint8_t* body, std::size_t length, std::size_t at) {
    if (!body || at > length || length - at < 2)
        throw std::out_of_range("36CEC truncated RES3 word");
    return static_cast<std::uint16_t>(
        static_cast<unsigned>(body[at]) |
        (static_cast<unsigned>(body[at + 1]) << 8));
}

// An original display-list entry is *not* an RGBA bitmap. The selectors
// address the scale-specific SINTAB renderer and are interpreted only by
// the subsequent 369D8/36B14 pixel stage.
inline Original36cecPlan prepare_original_36cec(
    const Original36cecCall& call,
    const std::uint8_t* body, std::size_t length,
    std::uint32_t scale_block_count,
    const std::uint16_t* runtime_lookup,
    std::size_t runtime_lookup_count,
    std::uint32_t runtime_lookup_max) {
    Original36cecPlan result{};
    if (call.res3_id >= 4234)
        throw std::out_of_range("36CEC RES3 ID outside original archive");
    result.original_scale = call.projected_size >> 3;
    // 36D07: a zero projected scale returns without loading RES3 data.
    if (result.original_scale == 0) return result;
    if (!scale_block_count || scale_block_count > 127)
        throw std::invalid_argument("36CEC requires original SINTAB scales");
    result.clamped_scale = std::min(result.original_scale, scale_block_count);
    // 36D34 and 36D49 differ in scratch 10450, but both produce
    // clamped_scale - (clamped_scale >> 3) at 36D56.
    result.scale_pointer_index =
        result.clamped_scale - (result.clamped_scale >> 3);
    if (!result.scale_pointer_index ||
        result.scale_pointer_index > scale_block_count)
        throw std::out_of_range("36CEC invalid SINTAB code pointer index");
    // Original 36CFA AND 0xFFF8 truncates the incoming size to 16 bits.
    const std::uint32_t scaled =
        (call.projected_size & 0xFFF8u) >> 1;
    const std::uint64_t index =
        static_cast<std::uint64_t>(scaled) + call.lookup_bias;
    if (index > UINT32_MAX)
        throw std::out_of_range("36CEC lookup index overflow");
    result.lookup_index = static_cast<std::uint32_t>(index);
    if (result.lookup_index <= runtime_lookup_max) {
        if (!runtime_lookup ||
            result.lookup_index >= runtime_lookup_count)
            throw std::out_of_range("36CEC missing original lookup word");
        result.packed_lookup =
            std::min<std::uint16_t>(
                runtime_lookup[result.lookup_index], 0x0808u);
    }
    // Original 36E94 retrieves archive 3 through 2F29C, then +2 to the
    // decompressed geometric header. Validate the *decoded body*.
    const auto width = read_le16_36cec(body, length, 0);
    const auto x_min = static_cast<std::int16_t>(
        read_le16_36cec(body, length, 2));
    const auto x_max = static_cast<std::int16_t>(
        read_le16_36cec(body, length, 4));
    const auto y_min = static_cast<std::int16_t>(
        read_le16_36cec(body, length, 6));
    const auto y_max = static_cast<std::int16_t>(
        read_le16_36cec(body, length, 8));
    if ((width != 64 && width != 96 && width != 128) ||
        x_min < 0 || x_max < x_min || x_max >= width ||
        y_min < 0 || y_max < y_min || y_max > 128)
        throw std::invalid_argument("36CEC invalid original EH7 bounds");
    const auto columns =
        static_cast<std::size_t>(x_max - x_min + 1);
    const auto table_end = 10u + 2u * columns;
    if (table_end > length)
        throw std::out_of_range("36CEC truncated column offset table");
    const auto half = static_cast<unsigned>(width / 2u);
    // 36EED..370AE: center-left, reverse table; then 370B6..372C3
    // center-right in forward order. Preserve shared/unaligned list pointers.
    auto append = [&](unsigned x) {
        const auto index = x - static_cast<unsigned>(x_min);
        const auto ptr = read_le16_36cec(body, length, 10u + 2u * index);
        if (ptr < table_end || ptr >= length)
            throw std::out_of_range("36CEC list pointer outside body");
        result.source_x_order.push_back(static_cast<std::uint16_t>(x));
        std::size_t at = ptr;
        for (;;) {
            const auto stop = read_le16_36cec(body, length, at);
            if (!stop) break;
            if (at > length || length - at < 6)
                throw std::out_of_range("36CEC incomplete raster descriptor");
            const auto adjustment = static_cast<std::int16_t>(
                read_le16_36cec(body, length, at + 2));
            const auto entry = read_le16_36cec(body, length, at + 4);
            if (stop > 0x100 || entry > 0x100 ||
                (stop & 1) || (entry & 1))
                throw std::invalid_argument("36CEC unsupported selector");
            result.descriptors.push_back({
                static_cast<std::uint16_t>(x), adjustment, stop, entry
            });
            at += 6;
        }
    };
    if (x_min < static_cast<int>(half)) {
        auto x = std::min<unsigned>(half - 1u,
                                   static_cast<unsigned>(x_max));
        for (;;) {
            if (x < static_cast<unsigned>(x_min)) break;
            append(x);
            if (x == 0) break;
            --x;
        }
    }
    for (unsigned x = std::max<unsigned>(
             half, static_cast<unsigned>(x_min));
         x <= static_cast<unsigned>(x_max); ++x)
        append(x);
    if (result.source_x_order.size() != columns)
        throw std::logic_error("36CEC lost source columns");
    result.drawable = true; // Parsed source, NOT rasterized VGA output.
    return result;
}
} // namespace rloft_recomp
