#include "rloft/renderer_tables.hpp"

#include <algorithm>
#include <cstddef>
#include <stdexcept>

namespace rloft {
namespace {

std::uint16_t u16(const std::uint8_t* p) {
    return static_cast<std::uint16_t>(p[0]) |
           (static_cast<std::uint16_t>(p[1]) << 8U);
}

std::uint32_t u32(const std::uint8_t* p) {
    return static_cast<std::uint32_t>(p[0]) |
           (static_cast<std::uint32_t>(p[1]) << 8U) |
           (static_cast<std::uint32_t>(p[2]) << 16U) |
           (static_cast<std::uint32_t>(p[3]) << 24U);
}

std::size_t parse_fixed(
    const std::vector<std::uint8_t>& b,
    RendererTables& out) {
    constexpr std::size_t kLookupAt = 0x1B44U;
    if (b.size() < kLookupAt) {
        throw std::runtime_error(
            "SINTAB fixed prefix is truncated");
    }

    std::size_t p = 0;
    for (auto& v : out.sine_q16_signmag) {
        v = u32(b.data() + p);
        p += 4U;
    }
    for (auto& v : out.tangent_q16) {
        v = u32(b.data() + p);
        p += 4U;
    }
    for (auto& v : out.projection_a) {
        v = u16(b.data() + p);
        p += 2U;
    }
    for (auto& v : out.projection_b) {
        v = u16(b.data() + p);
        p += 2U;
    }
    for (auto& v : out.constants) {
        v = u32(b.data() + p);
        p += 4U;
    }
    for (auto& v : out.view_angle_tenths) {
        v = static_cast<std::int32_t>(
            u32(b.data() + p));
        p += 4U;
    }

    out.scale_block_count = u32(b.data() + p);
    p += 4U;
    out.runtime_lookup_max = u32(b.data() + p);
    p += 4U;
    out.renderer_aux_value = u32(b.data() + p);
    p += 4U;

    if (p != kLookupAt) {
        throw std::runtime_error(
            "SINTAB fixed-prefix layout mismatch");
    }

    const auto lookup_count =
        static_cast<std::size_t>(
            out.runtime_lookup_max) + 1U;
    if (lookup_count > (b.size() - p) / 2U) {
        throw std::runtime_error(
            "SINTAB runtime lookup is truncated");
    }

    out.runtime_lookup.resize(lookup_count);
    for (auto& v : out.runtime_lookup) {
        v = u16(b.data() + p);
        p += 2U;
    }
    return p;
}

}  // namespace

RendererTables RendererTables::parse_prefix(
    const std::vector<std::uint8_t>& b) {
    RendererTables out{};
    (void)parse_fixed(b, out);
    return out;
}

RendererTables RendererTables::parse(
    const std::vector<std::uint8_t>& b) {
    RendererTables out{};
    std::size_t p = parse_fixed(b, out);

    if (b.size() - p < 4U) {
        throw std::runtime_error(
            "SINTAB missing scale-block byte count");
    }
    out.scale_block_payload_bytes =
        u32(b.data() + p);
    p += 4U;

    out.scale_blocks.reserve(
        static_cast<std::size_t>(
            out.scale_block_count));

    std::uint64_t payload_sum = 0;
    for (std::uint32_t block_index = 0;
         block_index < out.scale_block_count;
         ++block_index) {
        if (b.size() - p < 2U) {
            throw std::runtime_error(
                "SINTAB scale block length is truncated");
        }

        const auto block_size = u16(b.data() + p);
        p += 2U;
        if (block_size < 0x205U ||
            static_cast<std::size_t>(block_size) >
                b.size() - p) {
            throw std::runtime_error(
                "SINTAB scale block has invalid size");
        }

        const auto block_at = p;
        RendererScaleBlock block{};
        block.serialized_size = block_size;

        for (auto& value : block.code_patch_offset) {
            value = u16(b.data() + p);
            p += 2U;
        }
        for (auto& value : block.scale_step) {
            value = u16(b.data() + p);
            p += 2U;
        }

        if (!std::is_sorted(
                block.code_patch_offset.begin(),
                block.code_patch_offset.end())) {
            throw std::runtime_error(
                "SINTAB patch offsets are not monotone");
        }
        for (const auto value :
             block.code_patch_offset) {
            if (value < 0x204U ||
                value >= block_size) {
                throw std::runtime_error(
                    "SINTAB patch offset leaves block");
            }
        }

        const auto code_bytes =
            static_cast<std::size_t>(block_size) -
            0x204U;
        block.rasterizer_x86.assign(
            b.begin() +
                static_cast<std::ptrdiff_t>(
                    block_at + 0x204U),
            b.begin() +
                static_cast<std::ptrdiff_t>(
                    block_at + block_size));

        if (block.rasterizer_x86.size() !=
                code_bytes ||
            block.rasterizer_x86.empty() ||
            block.rasterizer_x86.back() !=
                0xC3U) {
            throw std::runtime_error(
                "SINTAB generated rasterizer is invalid");
        }

        p = block_at +
            static_cast<std::size_t>(block_size);
        payload_sum += block_size;
        out.scale_blocks.push_back(
            std::move(block));
    }

    if (payload_sum !=
        out.scale_block_payload_bytes) {
        throw std::runtime_error(
            "SINTAB scale-block byte sum mismatch");
    }

    constexpr std::size_t kTrailerSize = 512U;
    if (b.size() - p != kTrailerSize) {
        throw std::runtime_error(
            "SINTAB zero trailer has unexpected size");
    }
    out.zero_trailer.assign(
        b.begin() + static_cast<std::ptrdiff_t>(p),
        b.end());
    if (!std::all_of(
            out.zero_trailer.begin(),
            out.zero_trailer.end(),
            [](std::uint8_t value) {
                return value == 0;
            })) {
        throw std::runtime_error(
            "SINTAB trailer is not all zero");
    }

    return out;
}

RendererTables RendererTables::load(
    const GameSource& source) {
    return parse(source.read("SINTAB"));
}

}  // namespace rloft
