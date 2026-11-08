#include "rloft/SINTAB.hpp"

#include <algorithm>
#include <cstddef>
#include <limits>
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

    // Reject malicious block counts before reserving an unbounded vector.
    // Every physical scale block needs at least a 2-byte size and 0x205
    // body bytes; even the original 512-byte trailer only tightens this.
    if (out.scale_block_count > (b.size() - p) / (2U + 0x205U)) {
        throw std::runtime_error(
            "SINTAB scale-block count exceeds remaining file");
    }
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

// Rebuild only the documented SINTAB fields; x86 rasterizer bytes are
// serialized as opaque data and never executed by this serializer.
std::vector<std::uint8_t> RendererTables::encode() const {
    if (scale_block_count != scale_blocks.size() ||
        static_cast<std::uint64_t>(runtime_lookup_max) + 1U !=
            runtime_lookup.size() ||
        zero_trailer.size() != 512U ||
        !std::all_of(zero_trailer.begin(), zero_trailer.end(),
                     [](std::uint8_t byte) { return byte == 0U; })) {
        throw std::invalid_argument("SINTAB inconsistent count, lookup or trailer");
    }

    std::uint64_t sum = 0U;
    for (const auto& block : scale_blocks) {
        if (block.rasterizer_x86.empty() ||
            block.rasterizer_x86.back() != 0xC3U ||
            block.rasterizer_x86.size() > 0xFFFFU - 0x204U ||
            block.serialized_size != 0x204U + block.rasterizer_x86.size() ||
            !std::is_sorted(block.code_patch_offset.begin(),
                            block.code_patch_offset.end())) {
            throw std::invalid_argument("SINTAB invalid scale block layout");
        }
        for (const auto offset : block.code_patch_offset) {
            if (offset < 0x204U || offset >= block.serialized_size) {
                throw std::invalid_argument("SINTAB code patch offset outside block");
            }
        }
        sum += block.serialized_size;
    }
    if (sum != scale_block_payload_bytes) {
        throw std::invalid_argument("SINTAB scale payload total mismatch");
    }
    const std::uint64_t output_size = 0x1B44ULL +
        2ULL * runtime_lookup.size() + 4ULL +
        2ULL * scale_blocks.size() + sum + zero_trailer.size();
    if (output_size > std::numeric_limits<std::uint32_t>::max()) {
        throw std::overflow_error("SINTAB exceeds supported 32-bit file size");
    }

    std::vector<std::uint8_t> out;
    out.reserve(static_cast<std::size_t>(output_size));
    const auto write16 = [&out](std::uint16_t value) {
        out.push_back(static_cast<std::uint8_t>(value));
        out.push_back(static_cast<std::uint8_t>(value >> 8U));
    };
    const auto write32 = [&out](std::uint32_t value) {
        for (unsigned shift = 0; shift < 32U; shift += 8U) {
            out.push_back(static_cast<std::uint8_t>(value >> shift));
        }
    };
    for (auto value : sine_q16_signmag) write32(value);
    for (auto value : tangent_q16) write32(value);
    for (auto value : projection_a) write16(value);
    for (auto value : projection_b) write16(value);
    for (auto value : constants) write32(value);
    for (auto value : view_angle_tenths) write32(static_cast<std::uint32_t>(value));
    write32(scale_block_count);
    write32(runtime_lookup_max);
    write32(renderer_aux_value);
    for (auto value : runtime_lookup) write16(value);
    write32(scale_block_payload_bytes);
    for (const auto& block : scale_blocks) {
        write16(block.serialized_size);
        for (auto value : block.code_patch_offset) write16(value);
        for (auto value : block.scale_step) write16(value);
        out.insert(out.end(), block.rasterizer_x86.begin(),
                   block.rasterizer_x86.end());
    }
    out.insert(out.end(), zero_trailer.begin(), zero_trailer.end());
    if (out.size() != output_size) {
        throw std::logic_error("SINTAB serialized size accounting mismatch");
    }
    return out;
}

RendererTables RendererTables::load(
    const GameSource& source) {
    return parse(source.read("SINTAB"));
}

std::size_t RendererTables::scale_block_index_for_extent(
    std::uint32_t extent) const {
    const auto runtime_slot =
        static_cast<std::size_t>((extent & 0xFFFCU) >> 2U);
    const auto block_index =
        runtime_slot == 0U ? 0U : runtime_slot - 1U;
    if (block_index >= scale_blocks.size()) {
        throw std::out_of_range(
            "surface extent exceeds SINTAB scale blocks");
    }
    return block_index;
}

const RendererScaleBlock& RendererTables::scale_block_for_extent(
    std::uint32_t extent) const {
    return scale_blocks[
        scale_block_index_for_extent(extent)];
}

}  // namespace rloft
