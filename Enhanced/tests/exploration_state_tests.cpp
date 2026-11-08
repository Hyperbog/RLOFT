#include "rloft/exploration_state.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <vector>

namespace {
void put16(std::vector<std::uint8_t>& b, std::size_t o, std::uint16_t v) {
    b[o] = static_cast<std::uint8_t>(v);
    b[o + 1] = static_cast<std::uint8_t>(v >> 8U);
}
void put32(std::vector<std::uint8_t>& b, std::size_t o, std::uint32_t v) {
    for (int i = 0; i < 4; ++i) b[o + i] = static_cast<std::uint8_t>(v >> (i * 8));
}

std::vector<std::uint8_t> make_record(bool direct) {
    constexpr std::size_t b0 = 0x1D;
    constexpr int w = 2, h = 2;
    constexpr std::size_t b1 = b0 + 0x2A4 + w * h * 2;
    const std::size_t list_size = direct ? (24 + 1 + 24) : 24;
    const std::size_t b2 = b1 + list_size;
    const std::size_t b3 = b2;
    const std::size_t b4 = direct ? b3 + 2 : b3;
    const std::size_t b5 = b4;
    const std::size_t b6 = b5 + 0x514;
    const std::size_t end = b6 + 4 + w * h + 4;
    std::vector<std::uint8_t> r(end, 0);
    put32(r, 1, b0); put32(r, 5, b1); put32(r, 9, b2);
    put32(r, 13, b3); put32(r, 17, b4); put32(r, 21, b5); put32(r, 25, b6);
    put16(r, b0 + 1, w); put16(r, b0 + 3, h);
    r[b0 + 0x009] = 0xFD;
    r[b0 + 0x290] = 0xFF;
    r[b0 + 0x298] = 0x2C;
    r[b0 + 0x299] = 0x31;
    put16(r, b0 + 0x29A, 0);
    r[b0 + 0x29C] = 7;
    put16(r, b0 + 0x2A4 + 0, 0x0400);
    put16(r, b0 + 0x2A4 + 2, 0x0081);

    std::size_t p = b1;
    auto write_group = [&](std::uint8_t kind0, std::uint8_t kind1) {
        put16(r, p, 1); p += 2;
        r[p + 0] = kind0; r[p + 4] = 1; r[p + 5] = 0; p += 10;
        put16(r, p, 1); p += 2;
        r[p + 0] = kind1; r[p + 4] = 0; r[p + 5] = 1; p += 10;
    };
    if (direct) {
        write_group(2, 3);
        r[p++] = 1;
        write_group(1, 1);
    } else {
        write_group(2, 1);
    }
    assert(p == b2);
    if (direct) put16(r, b3, 0);
    put16(r, b6 + 0, w); put16(r, b6 + 2, h);
    r[b6 + 4] = 0x2E; r[b6 + 5] = 0x01;
    put32(r, b6 + 8, 0);
    return r;
}
}  // namespace

int main() {
    std::vector<std::uint8_t> chars(rloft::PartyImage::kSize, 0xFF);
    for (std::size_t i = 0; i < 16; ++i) chars[i] = 0;
    chars[0] = 'A'; chars[1] = 'N'; chars[2] = 'N';
    chars[0x10] = 0; chars[0x11] = 0;
    auto party = rloft::PartyImage::from_bytes(chars);
    assert(party.name(0) == "ANN");
    assert(party.active(0));
    assert(!party.active(1));

    auto record = make_record(false);
    std::vector<std::uint8_t> archive(8 + record.size(), 0);
    put32(archive, 0, 1); put32(archive, 4, 8);
    std::copy(record.begin(), record.end(), archive.begin() + 8);
    auto legacy = rloft::ExplorationStateCodec::parse_archive_level(archive, 0);
    assert(legacy.width == 2 && legacy.height == 2);
    assert(legacy.music_sequence == 7);
    assert(legacy.renderer_vertical_offset == -3);
    assert(legacy.renderer_fill_base_a == 0x2C);
    assert(legacy.renderer_fill_base_b == 0x31);
    assert(legacy.header[0x29A] == 0);
    assert(legacy.header[0x29B] == 0);
    assert(legacy.render_threshold_floor == 1);
    assert(legacy.group1.size() == 1);
    assert(legacy.group2.size() == 1);
    assert(legacy.automap_cache[0] == 0x2E);

    auto direct = rloft::ExplorationStateCodec::parse_direct_level(make_record(true));
    assert(direct.group1.size() == 2);
    assert(direct.group2.size() == 2);
    assert(direct.active_animation_count == 1);
    assert(direct.storage == rloft::LevelStorage::DirectSave);
    assert(direct.renderer_fill_base_a == 0x2C);
    assert(direct.renderer_fill_base_b == 0x31);
    assert(direct.render_threshold_floor == 1);
}
