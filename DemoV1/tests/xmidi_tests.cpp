#include "rloft/xmidi.hpp"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <string_view>
#include <vector>

namespace {

void be32(std::vector<std::uint8_t>& out, std::uint32_t value) {
    out.push_back(static_cast<std::uint8_t>(value >> 24U));
    out.push_back(static_cast<std::uint8_t>(value >> 16U));
    out.push_back(static_cast<std::uint8_t>(value >> 8U));
    out.push_back(static_cast<std::uint8_t>(value));
}

std::vector<std::uint8_t> chunk(
    std::string_view tag,
    const std::vector<std::uint8_t>& payload) {
    std::vector<std::uint8_t> out;
    out.insert(out.end(), tag.begin(), tag.end());
    be32(out, static_cast<std::uint32_t>(payload.size()));
    out.insert(out.end(), payload.begin(), payload.end());
    if (payload.size() & 1U) out.push_back(0);
    return out;
}

std::vector<std::uint8_t> form_xmid(
    const std::vector<std::uint8_t>& evnt) {
    auto evnt_chunk = chunk("EVNT", evnt);
    std::vector<std::uint8_t> payload{'X','M','I','D'};
    payload.insert(payload.end(), evnt_chunk.begin(), evnt_chunk.end());
    return chunk("FORM", payload);
}

}  // namespace

int main() {
    const auto seq0 = form_xmid({
        1, 0xC0, 5,
        2, 0xFF, 0x2F, 0
    });

    const auto seq1 = form_xmid({
        0xB0, 116, 2,
        1, 0x90, 60, 100, 2,
        1, 0xB0, 117, 127,
        1, 0xFF, 0x2F, 0
    });

    std::vector<std::uint8_t> catalog_payload{'X','M','I','D'};
    catalog_payload.insert(catalog_payload.end(), seq0.begin(), seq0.end());
    catalog_payload.insert(catalog_payload.end(), seq1.begin(), seq1.end());
    const auto xmi = chunk("CAT ", catalog_payload);

    const auto first = rloft::parse_xmidi_sequence(xmi, 0);
    assert(first.events.size() == 2);
    assert(first.events[0].delay_ticks == 1);
    assert(first.events[0].bytes[0] == 0xC0);
    assert(first.events[1].kind == rloft::XmiEventKind::EndOfTrack);

    const auto second = rloft::parse_xmidi_sequence(xmi, 1);
    assert(second.events.size() == 4);
    assert(second.events[0].bytes[1] == 116);
    assert(second.events[1].kind == rloft::XmiEventKind::NoteOn);
    assert(second.events[1].note_duration == 2);
    assert(second.events[2].bytes[1] == 117);
    assert(second.events[3].kind == rloft::XmiEventKind::EndOfTrack);

    std::cout << "xmidi_tests: PASS\n";
}
