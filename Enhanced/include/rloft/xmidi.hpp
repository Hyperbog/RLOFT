#pragma once

#include <cstdint>
#include <vector>

namespace rloft {

enum class XmiEventKind : std::uint8_t {
    ShortMessage,
    NoteOn,
    SystemExclusive,
    EndOfTrack,
};

struct XmiEvent {
    std::uint32_t delay_ticks{};
    XmiEventKind kind{XmiEventKind::ShortMessage};
    std::vector<std::uint8_t> bytes;
    std::uint32_t note_duration{};
};

struct XmiSequence {
    std::vector<XmiEvent> events;
};

[[nodiscard]] XmiSequence parse_xmidi_sequence(
    const std::vector<std::uint8_t>& xmi,
    int sequence_index);

}  // namespace rloft
