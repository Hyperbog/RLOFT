#include "rloft/CINE02.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {
using rloft::Cine02File;
using rloft::Cine02Edits;
using rloft::Cine02PalettePatch;

void require(bool ok, const char* message) {
    if (!ok) throw std::runtime_error(message);
}
template<class Action> void rejects(Action&& action) {
    bool thrown = false;
    try { action(); } catch (const std::exception&) { thrown = true; }
    require(thrown, "CINE02 failed to reject an invalid transaction");
}
void push16(std::vector<std::uint8_t>& b, std::uint16_t v) {
    b.push_back(static_cast<std::uint8_t>(v));
    b.push_back(static_cast<std::uint8_t>(v >> 8U));
}
void push32(std::vector<std::uint8_t>& b, std::uint32_t v) {
    for (unsigned i = 0U; i < 4U; ++i)
        b.push_back(static_cast<std::uint8_t>(v >> (8U * i)));
}
void append(std::vector<std::uint8_t>& b, std::uint16_t id,
            const std::vector<std::uint8_t>& p) {
    push16(b,id); push32(b,static_cast<std::uint32_t>(p.size()));
    b.insert(b.end(),p.begin(),p.end());
}
std::vector<std::uint8_t> rle_solid(std::uint8_t color) {
    std::vector<std::uint8_t> b;
    b.reserve(400U * 6U);
    for (unsigned y = 0U; y < 400U; ++y)
        b.insert(b.end(),{0x81U,color,0x81U,color,0xC1U,color});
    return b;
}
std::vector<std::uint8_t> synthetic() {
    std::vector<std::uint8_t> b;
    push16(b,5U); push16(b,1U);
    push16(b,3U); // frame 0: screen, palette, full-frame RLE.
    append(b,1U,{});
    append(b,0U,std::vector<std::uint8_t>(768U,13U));
    append(b,2U,rle_solid(7U));
    push16(b,1U); append(b,5U,{19U,0U,0U,0U}); // frame 1: music.
    push16(b,3U); // frame 2: new palette, image, direct SFX.
    append(b,0U,std::vector<std::uint8_t>(768U,22U));
    append(b,8U,rloft::Cine00File::encode_viewport(
        std::vector<std::uint8_t>(320U*256U,42U)));
    append(b,9U,{35U,0U});
    push16(b,2U); // frame 3: nonempty delta and subtitle.
    // x=3, y=4..6, three distinct pixels. Valid CODE.2 grammar.
    append(b,3U,{1U,0U, 3U,0U,1U,0U, 4U,0U,3U,0U,88U,89U,90U});
    append(b,7U,{35U,0U,5U,0U});
    push16(b,2U); // frame 4: empty delta and palette update.
    append(b,3U,{0U,0U});
    append(b,0U,std::vector<std::uint8_t>(768U,5U));
    return b;
}
std::pair<std::size_t,std::size_t> first_packet(const Cine02File& f,
                                                std::uint16_t kind) {
    for (std::size_t frame = 0U; frame < f.stats().frames; ++frame)
        for (std::size_t c = 0U; c < f.frame_info(frame).chunks; ++c)
            if (f.info(frame,c).id == kind) return {frame,c};
    throw std::runtime_error("CINE02 required packet not found");
}
void exercise(const std::vector<std::uint8_t>& original, bool retail) {
    Cine02File file(original);
    const auto original_stats = file.stats();
    require(file.encode() == original && file.bytes() == original &&
            original_stats.default_wait == 1U,
            "CINE02 original file failed byte-exact round trip");
    if (retail) {
        require(original.size() == 7498347U && original_stats.frames == 128U &&
                original_stats.chunks == 131U && original_stats.by_id ==
                std::array<std::size_t,10>{{24U,1U,1U,9U,0U,1U,0U,2U,91U,2U}},
                "CINE02 uploaded source inventory mismatch");
        require(file.frame_info(0U).chunks == 3U &&
                file.info(0U,0U).id == 1U && file.info(0U,1U).id == 0U &&
                file.info(0U,2U).id == 2U &&
                file.info(0U,2U).payload_bytes == 24115U &&
                file.cues().size() == 5U,
                "CINE02 original opening or cue count mismatch");
        const auto cues = file.cues();
        require(cues[0U].frame == 1U && cues[0U].first == 19U &&
                cues[1U].frame == 7U && cues[1U].first == 34U &&
                cues[1U].second == 6U &&
                cues[2U].frame == 8U && cues[2U].first == 34U &&
                cues[3U].frame == 76U && cues[3U].first == 35U &&
                cues[3U].second == 5U && cues[4U].frame == 82U &&
                cues[4U].first == 35U,
                "CINE02 original audiovisual cue ordering differs");
        std::size_t alternate = 0U;
        for (std::size_t frame = 0; frame < original_stats.frames; ++frame) {
            const auto timing = file.frame_info(frame);
            alternate += timing.uses_image_wait ? 1U : 0U;
            require(timing.wait_ticks == (timing.uses_image_wait ? 6U : 1U),
                    "CINE02 original frame wait differs");
        }
        require(alternate == 100U, "CINE02 alternate-image wait count differs");
        const auto s0 = file.snapshot(0U),s9 = file.snapshot(9U);
        const auto s75 = file.snapshot(75U),s116 = file.snapshot(116U);
        const auto s127 = file.snapshot(127U);
        require(s127.last_palette_frame == 127U &&
                s127.last_image_frame == 116U &&
                s116.last_image_frame == 116U,
                "CINE02 empty sparse delta incorrectly changed image provenance");
        require(s0.pixels.size() == 128000U && s0.pixels[306U*320U+319U] == 7U &&
                s0.pixels[200U*320U+200U] == 11U &&
                s9.palette[3U] == 1U && s9.palette[8U] == 4U &&
                s75.pixels[51U*320U] == 35U &&
                s75.pixels[51U*320U+1U] == 227U &&
                s75.pixels[200U*320U+200U] == 32U &&
                s116.pixels[51U*320U] == 4U &&
                s116.pixels[51U*320U+1U] == 6U &&
                s127.palette[3U] == 0U &&
                s127.pixels[51U*320U] == 4U &&
                s127.pixels[200U*320U+200U] == 32U,
                "CINE02 source-backed frame snapshots differ");
        const auto delta = first_packet(file,3U);
        require(file.payload(delta.first,delta.second) ==
                std::vector<std::uint8_t>({0U,0U}),
                "CINE02 source sparse delta is not empty");
    } else {
        require(original_stats.frames == 5U && original_stats.chunks == 11U &&
                file.cues().size() == 3U,
                "CINE02 synthetic fixture inventory mismatch");
        const auto s0 = file.snapshot(0U);
        const auto s1 = file.snapshot(1U);
        const auto s2 = file.snapshot(2U);
        const auto s3 = file.snapshot(3U);
        const auto s4 = file.snapshot(4U);
        require(s0.palette[0U] == 13U && s0.pixels[4U*320U+3U] == 7U &&
                s1.palette[0U] == 13U && s1.wait_ticks == 1U &&
                s2.palette[0U] == 22U && s2.pixels[51U*320U] == 42U &&
                s2.pixels[0U] == 7U && s2.wait_ticks == 6U &&
                s3.pixels[4U*320U+3U] == 88U &&
                s3.pixels[5U*320U+3U] == 89U &&
                s3.pixels[6U*320U+3U] == 90U &&
                s3.pixels[51U*320U+3U] == 42U &&
                s3.last_image_frame == 3U && s4.last_image_frame == 3U &&
                s4.palette[0U] == 5U && s4.pixels[4U*320U+3U] == 88U,
                "CINE02 palette, RLE, planar or sparse-delta compositor differs");
    }
    const auto palette = first_packet(file,0U);
    const auto planar = first_packet(file,8U);
    const auto original_palette = file.payload(palette.first,palette.second);
    const auto original_planar = file.payload(planar.first,planar.second);
    auto edited_planar = original_planar;
    edited_planar[0U] ^= 0x03U;
    const auto music = file.cues().front();
    const auto source_snapshot = file.snapshot(planar.first);
    const auto changed_color = static_cast<std::uint8_t>(original_palette[3U] ^ 3U);
    const std::pair<std::size_t,std::size_t> second_palette{2U,0U};
    const auto second_raw = file.payload(second_palette.first,second_palette.second);
    const auto changed_second = static_cast<std::uint8_t>(second_raw[7U] ^ 1U);
    Cine02Edits edits{};
    edits.palette = {{palette.first,palette.second,3U,original_palette[3U],changed_color},
                     {second_palette.first,second_palette.second,7U,second_raw[7U],changed_second}};
    edits.base.packets.expected_wait = 1U;
    edits.base.packets.replacement_wait = 2U;
    edits.base.packets.chunks = {{planar.first,planar.second,original_planar,edited_planar}};
    edits.base.cues = {{music.frame,music.chunk,music.kind,music.first,music.second,
                        static_cast<std::uint16_t>(music.first+1U),music.second}};
    file.apply(edits);
    require(file.stats().default_wait == 2U &&
            file.payload(planar.first,planar.second) == edited_planar &&
            file.payload(palette.first,palette.second)[3U] == changed_color &&
            file.payload(second_palette.first,second_palette.second)[7U] == changed_second &&
            file.cues().front().first == music.first+1U &&
            file.snapshot(planar.first).pixels[51U*320U] ==
                static_cast<std::uint8_t>(source_snapshot.pixels[51U*320U] ^ 3U) &&
            file.stats().by_id == original_stats.by_id &&
            file.encode().size() == original.size(),
            "CINE02 atomic palette, image, timing and music edit failed");
    const auto checkpoint = file.encode();
    rejects([&] {file.apply(edits);}); // Stale DAC/header/image.
    Cine02Edits invalid{};
    invalid.palette = {{palette.first,palette.second,3U,changed_color,64U}};
    rejects([&] {file.apply(invalid);});
    invalid.palette = {{palette.first,palette.second,768U,0U,1U}};
    rejects([&] {file.apply(invalid);});
    invalid.palette = {{palette.first,palette.second,3U,changed_color,1U},
                       {palette.first,palette.second,3U,changed_color,2U}};
    rejects([&] {file.apply(invalid);});
    invalid.palette = {{planar.first,planar.second,3U,0U,1U}};
    rejects([&] {file.apply(invalid);});
    invalid.palette = {{palette.first,palette.second,3U,original_palette[3U],1U}};
    rejects([&] {file.apply(invalid);});
    invalid.palette = {{palette.first,palette.second,3U,changed_color,1U}};
    invalid.base.packets.chunks = {{palette.first,palette.second,
        file.payload(palette.first,palette.second),original_palette}};
    rejects([&] {file.apply(invalid);}); // Typed/raw collision.
    invalid = {};
    invalid.palette = {{palette.first,palette.second,3U,changed_color,1U}};
    invalid.base.cues = {{music.frame,music.chunk,music.kind,music.first,music.second,
                          static_cast<std::uint16_t>(music.first+2U),music.second}};
    rejects([&] {file.apply(invalid);}); // One valid patch, stale cue.
    require(file.encode() == checkpoint, "CINE02 failed transaction modified live bytes");

    Cine02Edits undo{};
    undo.palette = {{palette.first,palette.second,3U,changed_color,original_palette[3U]},
                    {second_palette.first,second_palette.second,7U,changed_second,second_raw[7U]}};
    undo.base.packets.expected_wait = 2U;
    undo.base.packets.replacement_wait = 1U;
    undo.base.packets.chunks = {{planar.first,planar.second,edited_planar,original_planar}};
    const auto new_music = file.cues().front();
    undo.base.cues = {{new_music.frame,new_music.chunk,new_music.kind,new_music.first,
                       new_music.second,music.first,music.second}};
    file.apply(undo);
    require(file.encode() == original,"CINE02 inverse did not restore exact source");
    file.apply({});
    require(file.encode() == original,"CINE02 empty transaction modified bytes");
    rejects([&] {(void)file.snapshot(original_stats.frames);});
    auto broken = original; broken.pop_back();
    rejects([&] {(void)Cine02File(std::move(broken));});
}
} // namespace

int main(int argc, char** argv) {
    exercise(synthetic(), false);
    if (argc > 2) throw std::invalid_argument("usage: CINE02_tests [CINE02.ANM]");
    if (argc == 2) {
        std::ifstream source(argv[1], std::ios::binary);
        if (!source) throw std::runtime_error("cannot read original CINE02.ANM");
        const std::vector<std::uint8_t> bytes{
            std::istreambuf_iterator<char>(source),std::istreambuf_iterator<char>()};
        exercise(bytes, true);
    }
}
