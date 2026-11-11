#include "rloft/CINE10.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {

Cine10File::Cine10File(std::vector<std::uint8_t> source,
                       std::uint16_t alternate_image_wait)
    : source_(std::move(source), alternate_image_wait),
      image_wait_(alternate_image_wait) {
    index();
}

const CineStats& Cine10File::stats() const noexcept { return source_.stats(); }
const std::vector<std::uint8_t>& Cine10File::bytes() const noexcept {
    return source_.bytes();
}
std::vector<std::uint8_t> Cine10File::encode() const {
    return source_.encode();
}
std::uint64_t Cine10File::total_ticks() const noexcept {
    return source_.total_ticks();
}
std::size_t Cine10File::zero_wait_frames() const noexcept {
    return zero_wait_frames_;
}
const std::vector<Cine10TickBatch>& Cine10File::batches() const noexcept {
    return batches_;
}
const std::vector<Cine10TimedEvent>& Cine10File::events() const noexcept {
    return events_;
}

void Cine10File::index() {
    std::vector<Cine10TickBatch> next;
    std::vector<std::size_t> frame_map;
    const auto& timeline = source_.timeline();
    next.reserve(timeline.size());
    frame_map.reserve(timeline.size());
    std::size_t zero_count = 0U;
    for (const auto& frame : timeline) {
        if (frame.duration_ticks == 0U) ++zero_count;
        if (next.empty() || next.back().start_tick != frame.start_tick) {
            if (!next.empty() && next.back().hold_ticks == 0U)
                throw std::logic_error("CINE10 zero-only batch precedes another tick");
            next.push_back({frame.frame, frame.frame, frame.start_tick,
                            0U, std::nullopt, std::nullopt,
                            std::nullopt, std::nullopt});
        }
        auto& batch = next.back();
        if (batch.hold_ticks != 0U)
            throw std::logic_error("CINE10 positive wait followed by same-tick frame");
        if (frame.duration_ticks != 0U) {
            batch.hold_ticks = frame.duration_ticks;
            batch.held_frame = frame.frame;
        }
        batch.last_frame = frame.frame + 1U;
        batch.last_palette_frame = frame.last_palette_frame;
        batch.last_image_frame = frame.last_image_frame;
        batch.active_subtitle_frame = frame.active_subtitle_frame;
        frame_map.push_back(next.size() - 1U);
    }
    if (!next.empty() &&
        next.back().start_tick + next.back().hold_ticks != total_ticks())
        throw std::logic_error("CINE10 batch timing fails source total");

    std::vector<Cine10TimedEvent> timed;
    const auto commands = source_.cues();
    timed.reserve(commands.size());
    for (const auto& cue : commands) {
        const auto& frame = timeline.at(cue.frame);
        timed.push_back({cue, frame.start_tick, frame_map.at(cue.frame),
                         frame.duration_ticks == 0U,
                         cue.kind == Cine01CueKind::Subtitle && cue.first == 0U});
    }
    zero_wait_frames_ = zero_count;
    batches_ = std::move(next);
    frame_to_batch_ = std::move(frame_map);
    events_ = std::move(timed);
}

const Cine10TickBatch& Cine10File::batch_for_frame(
    std::size_t frame) const {
    return batches_.at(frame_to_batch_.at(frame));
}
const Cine10TickBatch& Cine10File::batch_for_tick(
    std::uint64_t tick) const {
    if (tick >= total_ticks())
        throw std::out_of_range("CINE10 tick outside playback duration");
    const auto it = std::upper_bound(
        batches_.begin(), batches_.end(), tick,
        [](std::uint64_t value, const Cine10TickBatch& batch) {
            return value < batch.start_tick;
        });
    if (it == batches_.begin())
        throw std::logic_error("CINE10 time index has no starting batch");
    const auto& match = *(it - 1);
    if (match.hold_ticks == 0U || tick - match.start_tick >= match.hold_ticks)
        throw std::logic_error("CINE10 time index has an unrepresented tick");
    return match;
}

std::vector<Cine10TimedEvent> Cine10File::events_at_tick(
    std::uint64_t tick) const {
    if (tick > total_ticks())
        throw std::out_of_range("CINE10 event tick after cinematic");
    const auto begin = std::lower_bound(
        events_.begin(), events_.end(), tick,
        [](const Cine10TimedEvent& event, std::uint64_t value) {
            return event.start_tick < value;
        });
    const auto end = std::upper_bound(
        begin, events_.end(), tick,
        [](std::uint64_t value, const Cine10TimedEvent& event) {
            return value < event.start_tick;
        });
    return {begin, end};
}

std::string Cine10File::event_csv() const {
    std::ostringstream csv;
    csv << "frame,chunk,start_tick,batch_first,batch_last_exclusive,type,"
           "first,second,zero_wait,queue_suppressed,subtitle_clear\n";
    for (const auto& event : events_) {
        const auto& batch = batches_.at(event.batch_index);
        const auto& cue = event.cue;
        csv << cue.frame << ',' << cue.chunk << ',' << event.start_tick << ','
            << batch.first_frame << ',' << batch.last_frame << ','
            << static_cast<std::uint16_t>(cue.kind) << ','
            << cue.first << ',' << cue.second << ','
            << (event.zero_wait_frame ? 1 : 0) << ','
            << (cue.queue_suppressed ? 1 : 0) << ','
            << (event.subtitle_clear ? 1 : 0) << '\n';
    }
    return csv.str();
}

void Cine10File::apply(const Cine07Edits& edits) {
    Cine10File candidate(encode(), image_wait_);
    candidate.source_.apply(edits);
    candidate.index();
    *this = std::move(candidate);
}

} // namespace rloft
