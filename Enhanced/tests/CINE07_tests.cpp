#include "rloft/CINE07.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <vector>

namespace {
using rloft::Cine06File;
using rloft::Cine06Frame;
using rloft::Cine07Edits;
using rloft::Cine07File;
using rloft::Cine07RangeEdit;

void require(bool result, const char* msg) {
    if (!result) throw std::runtime_error(msg);
}
template<class F> void rejects(F&& f) {
    bool failed = false;
    try { f(); } catch (const std::exception&) { failed = true; }
    require(failed, "CINE07 invalid request was accepted");
}
void add16(std::vector<std::uint8_t>& b, std::uint16_t x) {
    b.push_back(static_cast<std::uint8_t>(x));
    b.push_back(static_cast<std::uint8_t>(x >> 8U));
}
void add32(std::vector<std::uint8_t>& b, std::uint32_t x) {
    for (unsigned s = 0U; s < 32U; s += 8U)
        b.push_back(static_cast<std::uint8_t>(x >> s));
}
void packet(std::vector<std::uint8_t>& b, std::uint16_t id,
            const std::vector<std::uint8_t>& data) {
    add16(b, id);
    add32(b, static_cast<std::uint32_t>(data.size()));
    b.insert(b.end(), data.begin(), data.end());
}
std::vector<std::uint8_t> solid_rle(std::uint8_t color) {
    std::vector<std::uint8_t> b;
    b.reserve(400U * 6U);
    for (std::size_t y = 0U; y < 400U; ++y)
        b.insert(b.end(), {0x81U,color,0x81U,color,0xC1U,color});
    return b;
}
std::vector<std::uint8_t> synthetic() {
    std::vector<std::uint8_t> b;
    add16(b,6U); add16(b,1U);
    add16(b,3U); // 0: blackout, palette, full-screen image.
    packet(b,1U,{});
    packet(b,0U,std::vector<std::uint8_t>(768U,21U));
    packet(b,2U,solid_rle(7U));
    add16(b,1U); packet(b,5U,{20U,0U,0U,0U}); // 1: music.
    add16(b,2U); // 2: image and subtitle.
    packet(b,8U,std::vector<std::uint8_t>(0x14000U,42U));
    packet(b,7U,{21U,0U,5U,0U});
    add16(b,1U); packet(b,3U,{0U,0U}); // 3: empty delta.
    add16(b,2U); // 4: palette + image update.
    packet(b,0U,std::vector<std::uint8_t>(768U,33U));
    packet(b,8U,std::vector<std::uint8_t>(0x14000U,77U));
    add16(b,1U); packet(b,9U,{25U,0U}); // 5: direct sound.
    return b;
}
Cine07RangeEdit make_range(const Cine07File& f, std::size_t first,
                           std::size_t last,
                           std::vector<Cine06Frame> replacement) {
    return {first,last,f.frame_bytes(first,last),std::move(replacement)};
}
void verify_seek(const Cine07File& f) {
    const auto& timeline = f.timeline();
    require(timeline.size() == f.stats().frames, "CINE07 index frame count");
    std::uint64_t elapsed = 0U;
    std::size_t previous_end = 4U;
    for (const auto& frame : timeline) {
        require(frame.start_tick == elapsed && frame.byte_begin == previous_end &&
                frame.byte_end > frame.byte_begin &&
                frame.byte_end <= f.bytes().size(),
                "CINE07 byte/tick index discontinuity");
        const auto raw = f.frame_bytes(frame.frame,frame.frame+1U);
        require(raw.size() == frame.byte_end-frame.byte_begin,
                "CINE07 frame span size disagrees with index");
        if (frame.duration_ticks) {
            const auto first = f.seek_tick(elapsed);
            const auto last = f.seek_tick(elapsed+frame.duration_ticks-1U);
            require(first.frame.frame == frame.frame &&
                    first.tick_within_frame == 0U &&
                    last.frame.frame == frame.frame &&
                    last.tick_within_frame == frame.duration_ticks-1U,
                    "CINE07 tick lookup did not resolve interval bounds");
        }
        previous_end = frame.byte_end;
        elapsed += frame.duration_ticks;
    }
    require(elapsed == f.total_ticks() && previous_end == f.bytes().size(),
            "CINE07 index does not cover original bytes or timing");
    rejects([&] {(void)f.seek_tick(f.total_ticks());});
    rejects([&] {(void)f.frame_bytes(f.stats().frames+1U,f.stats().frames+1U);});
    rejects([&] {(void)f.extract_range(1U,1U);});
    rejects([&] {(void)f.frame_index(f.stats().frames);});
}
void exercise(std::vector<std::uint8_t> original, bool retail) {
    Cine07File file(original);
    require(file.bytes() == original && file.encode() == original,
            "CINE07 constructor changed original source");
    verify_seek(file);
    if (retail) {
        require(original.size() == 34569939U && file.stats().frames == 551U &&
                file.stats().chunks == 555U && file.stats().default_wait == 1U &&
                file.stats().by_id ==
                  std::array<std::size_t,10>{{36U,1U,1U,70U,0U,1U,0U,16U,421U,9U}} &&
                file.total_ticks() == 3006U && file.cues().size() == 26U,
                "CINE07 original corpus audit differs");
        const auto& a = file.frame_index(80U);
        const auto& b = file.frame_index(482U);
        const auto& c = file.frame_index(550U);
        require(a.byte_begin == 3827555U && a.start_tick == 385U &&
                a.duration_ticks == 6U && a.last_palette_frame == 80U &&
                a.last_image_frame == 80U &&
                b.start_tick == 2652U && b.duration_ticks == 6U &&
                b.last_palette_frame == 482U && b.last_image_frame == 482U &&
                c.start_tick == 3005U && c.duration_ticks == 1U &&
                c.last_palette_frame == 550U && c.last_image_frame == 489U &&
                !c.active_subtitle_frame &&
                file.seek_tick(3005U).frame.frame == 550U,
                "CINE07 original indexed timing or visual provenance differs");
        require(file.frame_index(2U).active_subtitle_frame == 2U &&
                !file.frame_index(49U).active_subtitle_frame &&
                file.frame_index(54U).active_subtitle_frame == 54U,
                "CINE07 subtitle activation index differs");
    } else {
        require(file.stats().frames == 6U && file.stats().chunks == 10U &&
                file.total_ticks() == 21U && file.cues().size() == 3U &&
                file.seek_tick(2U).frame.frame == 2U &&
                file.seek_tick(7U).frame.frame == 2U &&
                file.seek_tick(8U).frame.frame == 3U &&
                file.seek_tick(20U).frame.frame == 5U &&
                file.frame_index(3U).last_image_frame == 2U &&
                file.frame_index(4U).last_image_frame == 4U &&
                file.frame_index(5U).active_subtitle_frame == 2U,
                "CINE07 synthetic timeline/provenance differs");
        Cine07File other(original,2U);
        require(other.total_ticks() == 9U &&
                other.seek_tick(8U).frame.frame == 5U,
                "CINE07 alternate image duration cannot be overridden");
    }
    const std::size_t first = retail ? 10U : 1U;
    const std::size_t insert_at = retail ? 130U : 3U;
    const std::size_t remove_at = retail ? 150U : 5U;
    const auto source_frames = file.stats().frames;
    const auto f0 = file.frame(first);
    const auto f1 = file.frame(first+1U);
    const auto removed = file.frame(remove_at);
    const auto insert0 = file.frame(0U);
    const auto insert1 = file.frame(1U);
    const auto extra = file.frame(first+2U);

    // The extraction is a standalone *structural* ANM, not a playback-state seed.
    const auto excerpt = file.extract_range(first,first+2U);
    Cine06File extracted(excerpt);
    require(extracted.stats().frames == 2U &&
            std::equal(excerpt.begin()+4,excerpt.end(),
                       file.frame_bytes(first,first+2U).begin()),
            "CINE07 range extraction changed original packet bytes");

    Cine07Edits edits{};
    edits.expected_frame_count = source_frames;
    edits.expected_wait = 1U;
    edits.replacement_wait = 2U;
    edits.ranges = {
        make_range(file,first,first+2U,{extra, f0, f1}),
        make_range(file,insert_at,insert_at,{insert0,insert1}),
        make_range(file,remove_at,remove_at+1U,{})
    };
    file.apply(edits);
    require(file.stats().frames == source_frames+2U &&
            file.stats().default_wait == 2U &&
            file.frame_bytes(first,first+3U).size() > 0U,
            "CINE07 multi-range edit did not commit atomically");
    verify_seek(file);
    const auto checkpoint = file.encode();
    rejects([&] { file.apply(edits); });
    Cine07Edits bad{};
    bad.expected_frame_count = file.stats().frames;
    bad.ranges = {make_range(file,first,first+1U,{f0}),
                  make_range(file,first,first+1U,{f1})};
    rejects([&] {file.apply(bad);}); // Duplicate range.
    bad.ranges = {make_range(file,first,first+2U,{f0}),
                  make_range(file,first+1U,first+3U,{f1})};
    rejects([&] {file.apply(bad);}); // Overlapping ranges.
    bad.ranges = {make_range(file,first,first+1U,{f0})};
    bad.ranges[0U].expected_frame_bytes[0U] ^= 0xFFU;
    rejects([&] {file.apply(bad);}); // Stale source bytes.
    bad.ranges = {make_range(file,first,first+1U,{Cine06Frame{{{4U,{}}}}})};
    rejects([&] {file.apply(bad);}); // No certified type-4 replacement.
    bad.ranges = {make_range(file,first,first+1U,{Cine06Frame{{{2U,{0x81U}}}}})};
    rejects([&] {file.apply(bad);}); // Invalid RLE packet.
    bad.ranges = {make_range(file,0U,0U,{})};
    bad.ranges[0U].replacements.resize(65536U);
    rejects([&] {file.apply(bad);}); // uint16 frame-count overflow.
    bad.ranges = {make_range(file,first,first+1U,{f0})};
    bad.expected_frame_count = source_frames;
    rejects([&] {file.apply(bad);}); // Stale frame-count precondition.
    require(file.encode() == checkpoint,
            "CINE07 invalid transaction mutated the source");

    // Original indices after: replacement +1, insertion +2, deletion -1.
    // Adjacent edits and empty insertion ranges are both legitimate.
    Cine07Edits undo{};
    undo.expected_frame_count = file.stats().frames;
    undo.expected_wait = 2U;
    undo.replacement_wait = 1U;
    undo.ranges = {
        make_range(file,first,first+3U,{f0,f1}),
        make_range(file,insert_at+1U,insert_at+3U,{}),
        make_range(file,remove_at+3U,remove_at+3U,{removed})
    };
    file.apply(undo);
    require(file.encode() == original && file.stats().frames == source_frames,
            "CINE07 inverse transaction did not restore original bytes");
    file.apply({});
    require(file.bytes() == original, "CINE07 no-op changed source bytes");
}
void near_capacity_regression() {
    // A replace+delete transaction may be valid at 65535 frames even if
    // its first, source-ordered operation is an insertion.
    std::vector<std::uint8_t> raw;
    add16(raw,65535U); add16(raw,1U);
    for (std::size_t i = 0U; i < 65535U; ++i) add16(raw,0U);
    Cine07File file(raw);
    Cine07Edits edits{};
    edits.expected_frame_count = 65535U;
    edits.ranges = {make_range(file,0U,0U,{Cine06Frame{}}),
                    make_range(file,65534U,65535U,{})};
    file.apply(edits);
    require(file.encode() == raw && file.stats().frames == 65535U,
            "CINE07 net-valid edit at uint16 maximum was rejected");
}
} // namespace

int main(int argc, char** argv) {
    near_capacity_regression();
    exercise(synthetic(),false);
    if (argc > 2) throw std::invalid_argument("usage: CINE07_tests [CINE07.ANM]");
    if (argc == 2) {
        std::ifstream stream(argv[1],std::ios::binary);
        if (!stream) throw std::runtime_error("cannot open original CINE07.ANM");
        std::vector<std::uint8_t> bytes{
            std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()};
        exercise(std::move(bytes),true);
    }
}
