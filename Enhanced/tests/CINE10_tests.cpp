#include "rloft/CINE10.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {
using rloft::Cine10File;
using rloft::Cine01CueKind;
using rloft::Cine07Edits;

void require(bool test, const char* reason) {
    if (!test) throw std::runtime_error(reason);
}
template <class F> void rejects(F&& action) {
    bool caught = false;
    try { action(); } catch (const std::exception&) { caught = true; }
    require(caught, "CINE10 accepted invalid input or transaction");
}
void word(std::vector<std::uint8_t>& b, std::uint16_t value) {
    b.push_back(static_cast<std::uint8_t>(value));
    b.push_back(static_cast<std::uint8_t>(value >> 8U));
}
void dword(std::vector<std::uint8_t>& b, std::uint32_t value) {
    for (unsigned shift = 0U; shift < 32U; shift += 8U)
        b.push_back(static_cast<std::uint8_t>(value >> shift));
}
void packet(std::vector<std::uint8_t>& b, std::uint16_t id,
            const std::vector<std::uint8_t>& payload) {
    word(b, id);
    dword(b, static_cast<std::uint32_t>(payload.size()));
    b.insert(b.end(), payload.begin(), payload.end());
}
std::vector<std::uint8_t> synthetic() {
    std::vector<std::uint8_t> b;
    word(b, 8U); word(b, 0U); // Zero default wait, CINE10-like.
    word(b, 1U); packet(b, 1U, {});                     // Frame 0: blackout.
    word(b, 1U); packet(b, 5U, {21U, 0U, 0U, 0U});    // Frame 1: music.
    word(b, 1U); packet(b, 0U, std::vector<std::uint8_t>(768U, 12U));
    word(b, 1U); packet(b, 8U, std::vector<std::uint8_t>(81920U, 5U));
    word(b, 1U); packet(b, 7U, {39U, 0U, 13U, 0U});   // Frame 4: subtitle.
    word(b, 1U); packet(b, 6U, {15U, 0U});           // Frame 5: suppressed.
    word(b, 1U); packet(b, 3U, {0U, 0U});           // Frame 6: image wait.
    word(b, 1U); packet(b, 9U, {44U, 0U});           // Frame 7: terminal cue.
    return b;
}
std::size_t csv_lines(const std::string& text) {
    return static_cast<std::size_t>(std::count(text.begin(), text.end(), '\n'));
}
void verify_common(const Cine10File& f) {
    const auto& batches = f.batches();
    std::size_t frames = 0U, zero = 0U;
    std::uint64_t covered_tick = 0U;
    std::size_t last_event_frame = 0U, last_event_chunk = 0U;
    bool first_event = true;
    for (std::size_t i = 0U; i < batches.size(); ++i) {
        const auto& batch = batches[i];
        require(batch.first_frame == frames && batch.last_frame > frames &&
                batch.last_frame <= f.stats().frames,
                "CINE10 batch frame ranges are not contiguous");
        require(batch.start_tick == covered_tick,
                "CINE10 batch time ranges have a gap");
        for (auto frame = batch.first_frame; frame < batch.last_frame; ++frame)
            require(&f.batch_for_frame(frame) == &batch,
                    "CINE10 frame-to-batch mapping differs");
        if (batch.held_frame) {
            require(*batch.held_frame == batch.last_frame - 1U &&
                    batch.hold_ticks > 0U,
                    "CINE10 held frame is not last in its batch");
            covered_tick += batch.hold_ticks;
            for (auto t = batch.start_tick; t < covered_tick; ++t)
                require(&f.batch_for_tick(t) == &batch,
                        "CINE10 tick selection missed a valid held frame");
            zero += batch.last_frame - batch.first_frame - 1U;
        } else {
            require(i + 1U == batches.size() && batch.hold_ticks == 0U,
                    "CINE10 zero-only batch is not terminal");
            zero += batch.last_frame - batch.first_frame;
        }
        frames = batch.last_frame;
    }
    require(frames == f.stats().frames && covered_tick == f.total_ticks() &&
            zero == f.zero_wait_frames(),
            "CINE10 batch coverage differs from source");
    for (const auto& e : f.events()) {
        require(e.cue.frame < frames && e.start_tick ==
                f.batch_for_frame(e.cue.frame).start_tick,
                "CINE10 command lost original timestamp");
        require(e.batch_index < batches.size() &&
                batches[e.batch_index].first_frame <= e.cue.frame &&
                e.cue.frame < batches[e.batch_index].last_frame,
                "CINE10 event has wrong batch");
        if (!first_event)
            require(e.cue.frame > last_event_frame ||
                    (e.cue.frame == last_event_frame && e.cue.chunk > last_event_chunk),
                    "CINE10 events are out of original packet order");
        first_event = false;
        last_event_frame = e.cue.frame;
        last_event_chunk = e.cue.chunk;
    }
    require(csv_lines(f.event_csv()) == f.events().size() + 1U,
            "CINE10 event export lost a command");
    rejects([&] { (void)f.batch_for_frame(frames); });
    rejects([&] { (void)f.batch_for_tick(f.total_ticks()); });
    rejects([&] { (void)f.events_at_tick(f.total_ticks() + 1U); });
}
void exercise(const std::vector<std::uint8_t>& original, bool retail) {
    Cine10File f(original);
    require(f.encode() == original && f.bytes() == original,
            "CINE10 initial source bytes are not lossless");
    verify_common(f);
    if (retail) {
        require(original.size() == 56273227U && f.stats().frames == 826U &&
                f.stats().chunks == 835U && f.stats().default_wait == 0U &&
                f.stats().by_id ==
                std::array<std::size_t, 10>{{36U,1U,1U,86U,0U,2U,5U,12U,686U,6U}},
                "CINE10 original structure differs from verified file");
        require(f.total_ticks() == 4632U && f.batches().size() == 772U &&
                f.zero_wait_frames() == 54U && f.events().size() == 25U,
                "CINE10 source-backed timing or cue counts differ");
        const auto& first = f.batch_for_tick(0U);
        require(first.first_frame == 0U && first.last_frame == 8U &&
                first.held_frame == 7U && first.hold_ticks == 6U &&
                f.events_at_tick(0U).size() == 1U &&
                f.events_at_tick(0U)[0U].cue.kind == Cine01CueKind::MusicSequence,
                "CINE10 zero-wait opening sequence differs");
        const auto tick120 = f.events_at_tick(120U);
        require(tick120.size() == 2U && tick120[0].cue.frame == 27U &&
                tick120[0].cue.kind == Cine01CueKind::Subtitle &&
                tick120[1].cue.frame == 28U &&
                tick120[1].cue.kind == Cine01CueKind::DirectSound &&
                f.batch_for_tick(120U).first_frame == 27U &&
                f.batch_for_tick(120U).last_frame == 30U,
                "CINE10 same-tick subtitle and audio order differs");
        const auto tick390 = f.events_at_tick(390U);
        require(tick390.size() == 2U && tick390[0].subtitle_clear &&
                tick390[1].cue.queue_suppressed &&
                tick390[1].cue.first == 15U,
                "CINE10 no-op queued SFX or subtitle clear differs");
        require(f.events_at_tick(4386U).size() == 1U &&
                f.events_at_tick(4386U)[0].cue.first == 22U &&
                f.batch_for_tick(4631U).held_frame == 825U,
                "CINE10 ending timeline differs");
        require(static_cast<std::size_t>(std::count_if(
                f.events().begin(),f.events().end(),
                [](const auto& e){return e.cue.queue_suppressed;})) == 4U,
                "CINE10 suppressed sound requests differ");
        for (const auto& e : f.events())
            require(e.zero_wait_frame,
                    "CINE10 original command unexpectedly has an image wait");
    } else {
        require(f.stats().frames == 8U && f.stats().default_wait == 0U &&
                f.stats().chunks == 8U && f.total_ticks() == 12U &&
                f.zero_wait_frames() == 6U && f.batches().size() == 3U &&
                f.events().size() == 4U,
                "CINE10 synthetic zero-wait timeline differs");
        require(f.batch_for_tick(0U).first_frame == 0U &&
                f.batch_for_tick(5U).last_frame == 4U &&
                f.batch_for_tick(6U).first_frame == 4U &&
                f.batch_for_tick(11U).last_frame == 7U &&
                f.batch_for_frame(7U).start_tick == 12U &&
                !f.batch_for_frame(7U).held_frame &&
                f.events_at_tick(6U).size() == 2U &&
                f.events_at_tick(6U)[1].cue.queue_suppressed &&
                f.events_at_tick(12U).size() == 1U &&
                f.events_at_tick(12U)[0].cue.kind == Cine01CueKind::DirectSound,
                "CINE10 terminal zero-wait dispatch differs");
        Cine10File two_tick(original, 2U);
        require(two_tick.total_ticks() == 4U &&
                two_tick.events_at_tick(4U).size() == 1U,
                "CINE10 configurable image wait differs");
    }
    // Change header wait atomically and verify all timestamp ties re-index.
    Cine07Edits edits{};
    edits.expected_wait = 0U;
    edits.replacement_wait = 1U;
    f.apply(edits);
    require(f.stats().default_wait == 1U &&
            f.total_ticks() == (retail ? 4686U : 18U) &&
            f.zero_wait_frames() == 0U && f.batches().size() == f.stats().frames &&
            f.events().size() == (retail ? 25U : 4U),
            "CINE10 changed timing did not regenerate schedule");
    const auto changed = f.encode();
    rejects([&] { f.apply(edits); });
    require(f.encode() == changed, "CINE10 rejected edit modified source");
    Cine07Edits revert{};
    revert.expected_wait = 1U;
    revert.replacement_wait = 0U;
    f.apply(revert);
    require(f.encode() == original, "CINE10 undo failed byte-exact restore");
    verify_common(f);
    f.apply({});
    require(f.encode() == original, "CINE10 no-op changed source");
    auto broken = original;
    broken.pop_back();
    rejects([&] {(void)Cine10File(std::move(broken));});
}
std::vector<std::uint8_t> read(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) throw std::runtime_error("Cannot read ANM original: " + path);
    return {std::istreambuf_iterator<char>(file),std::istreambuf_iterator<char>()};
}
} // namespace

int main(int argc, char** argv) {
    exercise(synthetic(), false);
    if (argc > 2) throw std::invalid_argument("usage: CINE10_tests [CINE10.ANM]");
    if (argc == 2) exercise(read(argv[1]), true);
}
