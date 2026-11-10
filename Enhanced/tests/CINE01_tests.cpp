#include "rloft/CINE01.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
using rloft::Cine00File;
using rloft::Cine01Cue;
using rloft::Cine01CueKind;
using rloft::Cine01CuePatch;
using rloft::Cine01Edits;
using rloft::Cine01File;

void need(bool ok, const char* message) {
    if (!ok) throw std::runtime_error(message);
}
template <class F> void rejects(F&& operation) {
    bool failed = false;
    try { operation(); } catch (const std::exception&) { failed = true; }
    need(failed, "CINE01 invalid or stale change was accepted");
}
void put16(std::vector<std::uint8_t>& out, std::uint16_t v) {
    out.push_back(static_cast<std::uint8_t>(v));
    out.push_back(static_cast<std::uint8_t>(v >> 8U));
}
void put32(std::vector<std::uint8_t>& out, std::uint32_t v) {
    for (unsigned i = 0; i < 4; ++i)
        out.push_back(static_cast<std::uint8_t>(v >> (i * 8U)));
}
std::vector<std::uint8_t> words(std::uint16_t first, std::uint16_t second,
                                bool two = true) {
    std::vector<std::uint8_t> bytes;
    put16(bytes, first);
    if (two) put16(bytes, second);
    return bytes;
}
void add_chunk(std::vector<std::uint8_t>& out, std::uint16_t id,
               const std::vector<std::uint8_t>& data) {
    put16(out, id);
    put32(out, static_cast<std::uint32_t>(data.size()));
    out.insert(out.end(), data.begin(), data.end());
}
std::vector<std::uint8_t> solid_rle(std::uint8_t index) {
    std::vector<std::uint8_t> out;
    out.reserve(400U * 6U);
    for (unsigned y = 0; y < 400U; ++y)
        out.insert(out.end(), {0x81U,index,0x81U,index,0xC1U,index});
    return out;
}
std::vector<std::uint8_t> synthetic() {
    std::vector<std::uint8_t> out;
    put16(out, 4U); put16(out, 1U);
    put16(out, 5U); // Frame 0: blackout, palette, RLE, music, subtitle.
    add_chunk(out, 1U, {});
    add_chunk(out, 0U, std::vector<std::uint8_t>(768U, 0x13U));
    add_chunk(out, 2U, solid_rle(0x34U));
    add_chunk(out, 5U, words(18U, 0U));
    add_chunk(out, 7U, words(1U, 5U));

    put16(out, 3U); // Frame 1: alternate wait and two SFX types.
    std::vector<std::uint8_t> indexed(320U * 256U);
    for (std::size_t i = 0U; i < indexed.size(); ++i)
        indexed[i] = static_cast<std::uint8_t>((i * 7U) & 0xFFU);
    add_chunk(out, 8U, Cine00File::encode_viewport(indexed));
    add_chunk(out, 6U, words(3U, 0U, false)); // Suppressed by CODE.2.
    add_chunk(out, 9U, words(10U, 0U, false));

    put16(out, 3U); // Frame 2: empty sparse delta, queued sound, clear text.
    add_chunk(out, 3U, {0U,0U});
    add_chunk(out, 6U, words(123U, 0U, false));
    add_chunk(out, 7U, words(0U, 0U));

    put16(out, 1U); // Frame 3: direct audio only.
    add_chunk(out, 9U, words(4U, 0U, false));
    return out;
}
Cine01Cue find_cue(const Cine01File& file, Cine01CueKind kind,
                    std::size_t ordinal = 0U) {
    for (const auto& cue : file.cues()) {
        if (cue.kind == kind && ordinal-- == 0U) return cue;
    }
    throw std::runtime_error("missing CINE01 fixture cue");
}
std::pair<std::size_t,std::size_t> find_chunk(const Cine01File& file,
                                              std::uint16_t id) {
    for (std::size_t f = 0U; f < file.stats().frames; ++f)
        for (std::size_t c = 0U; c < file.chunk_count(f); ++c)
            if (file.info(f,c).id == id) return {f,c};
    throw std::runtime_error("missing CINE01 fixture chunk");
}
Cine01CuePatch patch(const Cine01Cue& prior, std::uint16_t next_first,
                     std::uint16_t next_second = 0U) {
    return {prior.frame, prior.chunk, prior.kind, prior.first, prior.second,
            next_first, next_second};
}

void exercise(const std::vector<std::uint8_t>& original, bool retail) {
    Cine01File file(original);
    const auto stats = file.stats();
    need(file.encode() == original && stats.default_wait == 1U,
         "CINE01 initial round-trip is not lossless");
    if (retail) {
        need(original.size() == 16009364U && stats.frames == 453U &&
             stats.chunks == 462U && stats.by_id ==
             std::array<std::size_t,10>{{31,1,1,215,0,1,2,8,195,8}},
             "CINE01 uploaded file structure differs");
        need(file.chunk_count(0U) == 3U && file.info(0U,0U).id == 1U &&
             file.info(0U,1U).id == 0U && file.info(0U,2U).id == 2U &&
             file.info(0U,2U).payload_bytes == 6988U,
             "CINE01 opening packets differ");
        need(file.frame_info(6U).uses_image_wait &&
             file.frame_info(6U).wait_ticks == 6U &&
             file.frame_info(2U).wait_ticks == 1U,
             "CINE01 source-backed wait selection differs");
        need(file.cues().size() == 19U &&
             file.cues_at(1U).size() == 1U &&
             file.cue(1U,0U).kind == Cine01CueKind::MusicSequence &&
             file.cue(1U,0U).first == 18U &&
             file.cue(77U,0U).kind == Cine01CueKind::Subtitle &&
             file.cue(77U,0U).first == 1U &&
             file.cue(77U,0U).second == 5U &&
             file.cue(169U,0U).kind == Cine01CueKind::QueuedSound &&
             file.cue(169U,0U).first == 123U &&
             file.cue(209U,0U).first == 18U &&
             file.cue(400U,0U).kind == Cine01CueKind::Subtitle &&
             file.cue(400U,0U).first == 7U &&
             file.cue(401U,0U).kind == Cine01CueKind::DirectSound &&
             file.cue(401U,0U).first == 10U,
             "CINE01 original command timeline differs");
        for (const auto& c : file.cues())
            need(!c.queue_suppressed,
                 "CINE01 original queued sound unexpectedly suppressed");
    } else {
        need(stats.frames == 4U && stats.chunks == 12U &&
             stats.by_id == std::array<std::size_t,10>{{1,1,1,1,0,1,2,2,1,2}},
             "synthetic CINE01 structure differs");
        need(file.frame_info(0U).wait_ticks == 1U &&
             file.frame_info(1U).wait_ticks == 6U &&
             file.frame_info(2U).wait_ticks == 6U &&
             file.frame_info(3U).wait_ticks == 1U &&
             file.cue(1U,1U).queue_suppressed &&
             !file.cue(2U,1U).queue_suppressed,
             "CINE01 synthetic cue interpretation differs");
    }

    const auto music = find_cue(file, Cine01CueKind::MusicSequence);
    const auto subtitle = find_cue(file, Cine01CueKind::Subtitle);
    const auto queued = find_cue(file, Cine01CueKind::QueuedSound);
    const auto direct = find_cue(file, Cine01CueKind::DirectSound);
    const auto palette = find_chunk(file,0U);
    const auto rle = find_chunk(file,2U);
    const auto planar = find_chunk(file,8U);
    const auto delta = find_chunk(file,3U);

    const auto source_palette = file.payload(palette.first,palette.second);
    const auto source_rle = file.payload(rle.first,rle.second);
    const auto source_planar = file.payload(planar.first,planar.second);
    const auto source_delta = file.payload(delta.first,delta.second);
    const auto decoded = file.full_frame(rle.first,rle.second);
    need(decoded.size() == 320U * 400U,
         "CINE01 full-frame native dimensions changed");
    if (!retail) need(decoded.front() == 0x34U && decoded.back() == 0x34U,
                      "CINE01 synthetic RLE pixels differ");
    const auto pixels = file.viewport(planar.first,planar.second);
    need(pixels.size() == 320U * 256U &&
         Cine00File::encode_viewport(pixels) == source_planar,
         "CINE01 native VGA planar conversion changed bytes");

    auto changed_palette = source_palette;
    changed_palette[101U] ^= 0x1EU;
    auto changed_pixels = pixels;
    changed_pixels[0U] ^= 0x3DU;
    changed_pixels[319U] ^= 0x0FU;
    changed_pixels[320U * 255U + 1U] ^= 0x55U;
    const auto changed_planar = Cine00File::encode_viewport(changed_pixels);
    const auto changed_rle = solid_rle(0x57U);
    const std::uint16_t next_queue = retail ? 3U : 123U;
    Cine01Edits edits{};
    edits.packets.expected_wait = 1U;
    edits.packets.replacement_wait = 2U;
    edits.packets.chunks = {
        {palette.first,palette.second,source_palette,changed_palette},
        {rle.first,rle.second,source_rle,changed_rle},
        {planar.first,planar.second,source_planar,changed_planar}
    };
    edits.cues = {patch(music,19U,2U),
                  patch(subtitle,2U,5U),
                  patch(queued,next_queue),
                  patch(direct,static_cast<std::uint16_t>(direct.first+1U))};
    file.apply(edits);
    need(file.encode().size() == original.size() + changed_rle.size() - source_rle.size() &&
         file.stats().frames == stats.frames && file.stats().chunks == stats.chunks &&
         file.stats().by_id == stats.by_id && file.stats().default_wait == 2U &&
         file.payload(palette.first,palette.second) == changed_palette &&
         file.payload(rle.first,rle.second) == changed_rle &&
         file.viewport(planar.first,planar.second) == changed_pixels &&
         file.payload(delta.first,delta.second) == source_delta &&
         file.cue(music.frame,music.chunk).first == 19U &&
         file.cue(music.frame,music.chunk).second == 2U &&
         file.cue(subtitle.frame,subtitle.chunk).first == 2U &&
         file.cue(queued.frame,queued.chunk).first == next_queue &&
         file.cue(queued.frame,queued.chunk).queue_suppressed == retail &&
         file.cue(direct.frame,direct.chunk).first == direct.first+1U &&
         file.frame_info(planar.first).wait_ticks == 6U,
         "CINE01 combined audiovisual/image edit failed");
    const auto checkpoint = file.encode();
    rejects([&] {file.apply(edits);}); // Expected header/commands stale.

    Cine01Edits invalid{};
    invalid.cues = {patch(music,20U,0U)};
    rejects([&] {file.apply(invalid);}); // Old music word stale.
    invalid = {};
    invalid.cues = {patch(file.cue(queued.frame,queued.chunk),17U,1U)};
    rejects([&] {file.apply(invalid);}); // Queued sound has only one word.
    invalid = {};
    const auto revised_music = file.cue(music.frame,music.chunk);
    invalid.cues = {patch(revised_music,21U,2U),patch(revised_music,22U,2U)};
    rejects([&] {file.apply(invalid);}); // Duplicate typed cue.
    invalid = {};
    invalid.cues = {patch(revised_music,21U,2U)};
    invalid.packets.chunks = {{music.frame,music.chunk,
        file.payload(music.frame,music.chunk),words(22U,2U)}};
    rejects([&] {file.apply(invalid);}); // Typed/raw alias collision.
    invalid = {};
    invalid.cues = {patch(revised_music,21U,2U),patch(subtitle,4U,5U)};
    rejects([&] {file.apply(invalid);}); // Valid cue then stale cue; rollback.
    invalid = {};
    invalid.cues = {{music.frame,music.chunk,Cine01CueKind::DirectSound,
                     19U,2U,20U,0U}};
    rejects([&] {file.apply(invalid);}); // Incorrect command kind.
    invalid = {};
    invalid.cues = {{stats.frames,0U,Cine01CueKind::MusicSequence,
                     19U,2U,20U,2U}};
    rejects([&] {file.apply(invalid);}); // Missing frame.
    invalid = {};
    invalid.packets.chunks = {{rle.first,rle.second,changed_rle,{0x81U}}};
    rejects([&] {file.apply(invalid);}); // Truncated image.
    invalid = {};
    invalid.packets.expected_wait = 1U;
    invalid.packets.replacement_wait = 3U;
    rejects([&] {file.apply(invalid);});
    need(file.encode() == checkpoint,
         "CINE01 rejected transaction modified live source");

    Cine01Edits undo{};
    undo.packets.expected_wait = 2U;
    undo.packets.replacement_wait = 1U;
    undo.packets.chunks = {
        {palette.first,palette.second,changed_palette,source_palette},
        {rle.first,rle.second,changed_rle,source_rle},
        {planar.first,planar.second,changed_planar,source_planar}
    };
    undo.cues = {patch(file.cue(music.frame,music.chunk), music.first,music.second),
                 patch(file.cue(subtitle.frame,subtitle.chunk),subtitle.first,subtitle.second),
                 patch(file.cue(queued.frame,queued.chunk),queued.first),
                 patch(file.cue(direct.frame,direct.chunk),direct.first)};
    file.apply(undo);
    need(file.encode() == original,
         "CINE01 inverse transaction failed byte-exact restoration");
    file.apply({});
    need(file.encode() == original,"CINE01 empty edit changed source");
    rejects([&] {(void)file.cues_at(stats.frames);});
    rejects([&] {(void)file.cue(palette.first,palette.second);});
    rejects([&] {(void)file.full_frame(palette.first,palette.second);});
    auto damaged = original;
    damaged.pop_back();
    rejects([&] {(void)Cine01File(damaged);});
    damaged = original;
    damaged.push_back(0U);
    rejects([&] {(void)Cine01File(damaged);});
    damaged = original;
    damaged.at(6U) = 10U; // Unsupported CODE.2 chunk handler.
    rejects([&] {(void)Cine01File(damaged);});
}
} // namespace

int main(int argc, char** argv) {
    exercise(synthetic(),false);
    std::cout << "CINE01 synthetic regression PASS\n";
    if (argc > 2)
        throw std::invalid_argument("usage: CINE01_tests [CINE01.ANM]");
    if (argc == 2) {
        std::ifstream input(argv[1],std::ios::binary);
        if (!input) throw std::runtime_error("cannot open original CINE01.ANM");
        const std::vector<std::uint8_t> bytes{
            std::istreambuf_iterator<char>(input),
            std::istreambuf_iterator<char>()};
        exercise(bytes,true);
        std::cout << "CINE01 original-file regression PASS\n";
    }
}
