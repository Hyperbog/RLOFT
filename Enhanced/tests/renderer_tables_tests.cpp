#include "rloft/renderer_tables.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

void p16(
    std::vector<std::uint8_t>& b,
    std::size_t o,
    std::uint16_t v) {
    b[o] = static_cast<std::uint8_t>(v);
    b[o + 1U] =
        static_cast<std::uint8_t>(v >> 8U);
}

void p32(
    std::vector<std::uint8_t>& b,
    std::size_t o,
    std::uint32_t v) {
    for (int i = 0; i < 4; ++i) {
        b[o + static_cast<std::size_t>(i)] =
            static_cast<std::uint8_t>(
                v >> (i * 8));
    }
}

std::vector<std::uint8_t> fixed_prefix(
    std::uint32_t block_count) {
    std::vector<std::uint8_t> b(
        0x1B44U + 6U, 0);
    std::size_t p = 0;

    for (int i = 0; i < 450; ++i) {
        p32(b, p, static_cast<std::uint32_t>(i));
        p += 4U;
    }
    for (int i = 0; i < 900; ++i) {
        p32(b, p,
            static_cast<std::uint32_t>(1000 + i));
        p += 4U;
    }
    for (int i = 0; i < 128; ++i) {
        p16(b, p, static_cast<std::uint16_t>(i));
        p += 2U;
    }
    for (int i = 0; i < 128; ++i) {
        p16(b, p,
            static_cast<std::uint16_t>(200 + i));
        p += 2U;
    }

    const std::uint32_t constants[8] = {
        256, 256, 127, 328,
        328, 22272, 175, 179200,
    };
    for (auto value : constants) {
        p32(b, p, value);
        p += 4U;
    }
    for (int i = 0; i < 256; ++i) {
        p32(b, p,
            static_cast<std::uint32_t>(
                359 - i * 2));
        p += 4U;
    }

    p32(b, p, block_count);
    p += 4U;
    p32(b, p, 2);
    p += 4U;
    p32(b, p, 128);
    p += 4U;

    assert(p == 0x1B44U);
    p16(b, p, 11);
    p += 2U;
    p16(b, p, 22);
    p += 2U;
    p16(b, p, 33);
    return b;
}

}  // namespace

int main() {
    auto prefix = fixed_prefix(127);
    const auto t =
        rloft::RendererTables::parse_prefix(prefix);

    assert(t.sine_q16_signmag[449] == 449);
    assert(t.tangent_q16[0] == 1000);
    assert(t.c6d8() == 256);
    assert(t.c6d0() == 127);
    assert(t.grid_projection_constant() == 22272);
    assert(t.depth_numerator() == 179200);
    assert(t.scale_block_count == 127);
    assert(t.runtime_lookup_max == 2);
    assert(t.renderer_aux_value == 128);
    assert(t.runtime_lookup.size() == 3);
    assert(t.runtime_lookup[2] == 33);

    auto full = fixed_prefix(1);
    constexpr std::uint16_t block_size = 0x205U;
    const auto payload_at = full.size();
    full.resize(
        payload_at + 4U + 2U +
        block_size + 512U, 0);

    p32(full, payload_at, block_size);
    std::size_t p = payload_at + 4U;
    p16(full, p, block_size);
    p += 2U;
    const auto block_at = p;

    for (int i = 0; i < 129; ++i) {
        p16(full, p, 0x204U);
        p += 2U;
    }
    for (int i = 0; i < 129; ++i) {
        p16(full, p,
            static_cast<std::uint16_t>(
                i == 128 ? 4 : 0));
        p += 2U;
    }
    assert(p == block_at + 0x204U);
    full[p] = 0xC3U;

    const auto parsed =
        rloft::RendererTables::parse(full);
    assert(parsed.scale_block_payload_bytes ==
           block_size);
    assert(parsed.scale_blocks.size() == 1);
    assert(parsed.scale_blocks[0].serialized_size ==
           block_size);
    assert(parsed.scale_blocks[0]
               .code_patch_offset[0] == 0x204U);
    assert(parsed.scale_blocks[0]
               .scale_step[128] == 4);
    assert(parsed.scale_blocks[0]
               .rasterizer_x86.size() == 1);
    assert(parsed.scale_blocks[0]
               .rasterizer_x86[0] == 0xC3U);
    assert(parsed.zero_trailer.size() == 512U);
}
