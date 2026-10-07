#include "rloft/xmidi.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace rloft {
namespace {

std::uint32_t be32(const std::vector<std::uint8_t>& data, std::size_t pos) {
    if (pos + 4 > data.size()) {
        throw std::runtime_error("XMIDI truncated IFF length");
    }
    return
        (static_cast<std::uint32_t>(data[pos]) << 24U) |
        (static_cast<std::uint32_t>(data[pos + 1]) << 16U) |
        (static_cast<std::uint32_t>(data[pos + 2]) << 8U) |
        static_cast<std::uint32_t>(data[pos + 3]);
}

bool tag_at(
    const std::vector<std::uint8_t>& data,
    std::size_t pos,
    std::string_view tag) {
    if (pos + tag.size() > data.size()) return false;
    for (std::size_t i = 0; i < tag.size(); ++i) {
        if (data[pos + i] != static_cast<std::uint8_t>(tag[i])) return false;
    }
    return true;
}

std::pair<std::uint32_t, std::size_t> read_vlq(
    const std::vector<std::uint8_t>& data,
    std::size_t pos) {
    std::uint32_t value = 0;
    for (int i = 0; i < 4; ++i) {
        if (pos >= data.size()) {
            throw std::runtime_error("XMIDI truncated variable-length value");
        }
        const auto byte = data[pos++];
        value = (value << 7U) | (byte & 0x7FU);
        if ((byte & 0x80U) == 0) return {value, pos};
    }
    throw std::runtime_error("XMIDI variable-length value exceeds four bytes");
}

void collect_sequences(
    const std::vector<std::uint8_t>& data,
    std::size_t start,
    std::size_t end,
    std::vector<std::pair<std::size_t, std::size_t>>& evnt_chunks) {
    std::size_t pos = start;
    while (pos + 8 <= end) {
        const auto length = static_cast<std::size_t>(be32(data, pos + 4));
        const auto payload = pos + 8;
        const auto payload_end = payload + length;
        if (payload_end > end) {
            throw std::runtime_error("XMIDI IFF chunk exceeds parent");
        }

        const bool is_form = tag_at(data, pos, "FORM");
        const bool is_cat = tag_at(data, pos, "CAT ");
        if (is_form || is_cat) {
            if (length < 4) {
                throw std::runtime_error("XMIDI short IFF container");
            }

            if (is_form && tag_at(data, payload, "XMID")) {
                std::size_t child = payload + 4;
                bool found_evnt = false;
                while (child + 8 <= payload_end) {
                    const auto child_length =
                        static_cast<std::size_t>(be32(data, child + 4));
                    const auto child_payload = child + 8;
                    const auto child_end = child_payload + child_length;
                    if (child_end > payload_end) {
                        throw std::runtime_error(
                            "XMIDI child chunk exceeds FORM XMID");
                    }
                    if (tag_at(data, child, "EVNT")) {
                        if (found_evnt) {
                            throw std::runtime_error(
                                "FORM XMID contains multiple EVNT chunks");
                        }
                        evnt_chunks.emplace_back(child_payload, child_end);
                        found_evnt = true;
                    }
                    child = child_end + (child_length & 1U);
                }
                if (!found_evnt) {
                    throw std::runtime_error("FORM XMID is missing EVNT");
                }
            } else {
                collect_sequences(
                    data,
                    payload + 4,
                    payload_end,
                    evnt_chunks);
            }
        }

        pos = payload_end + (length & 1U);
    }

    if (pos != end) {
        throw std::runtime_error("XMIDI IFF container has trailing bytes");
    }
}

XmiSequence parse_evnt(
    const std::vector<std::uint8_t>& data,
    std::size_t start,
    std::size_t end) {
    XmiSequence sequence;
    std::size_t pos = start;

    while (pos < end) {
        std::uint32_t delay = 0;
        while (pos < end && data[pos] < 0x80U) {
            delay += data[pos++];
        }
        if (pos >= end) {
            if (delay != 0) {
                throw std::runtime_error(
                    "XMIDI EVNT ends after interval count");
            }
            break;
        }

        const auto status = data[pos++];
        const auto family = status & 0xF0U;

        if (family == 0x90U) {
            if (pos + 2 > end) {
                throw std::runtime_error("XMIDI truncated note-on");
            }
            const auto note = data[pos++];
            const auto velocity = data[pos++];
            auto [duration, next] = read_vlq(data, pos);
            if (next > end) {
                throw std::runtime_error("XMIDI note duration exceeds EVNT");
            }
            pos = next;
            sequence.events.push_back(
                {delay,
                 XmiEventKind::NoteOn,
                 {status, note, velocity},
                 duration});
            continue;
        }

        if (family == 0x80U || family == 0xA0U ||
            family == 0xB0U || family == 0xE0U) {
            if (pos + 2 > end) {
                throw std::runtime_error(
                    "XMIDI truncated three-byte channel event");
            }
            sequence.events.push_back(
                {delay,
                 XmiEventKind::ShortMessage,
                 {status, data[pos], data[pos + 1]},
                 0});
            pos += 2;
            continue;
        }

        if (family == 0xC0U || family == 0xD0U) {
            if (pos >= end) {
                throw std::runtime_error(
                    "XMIDI truncated two-byte channel event");
            }
            sequence.events.push_back(
                {delay,
                 XmiEventKind::ShortMessage,
                 {status, data[pos++]},
                 0});
            continue;
        }

        if (status == 0xF0U || status == 0xF7U) {
            auto [length, next] = read_vlq(data, pos);
            pos = next;
            if (pos + length > end) {
                throw std::runtime_error("XMIDI truncated SysEx payload");
            }
            std::vector<std::uint8_t> bytes;
            bytes.reserve(static_cast<std::size_t>(length) + 1U);
            bytes.push_back(status);
            bytes.insert(
                bytes.end(),
                data.begin() + static_cast<std::ptrdiff_t>(pos),
                data.begin() + static_cast<std::ptrdiff_t>(pos + length));
            pos += length;
            sequence.events.push_back(
                {delay, XmiEventKind::SystemExclusive, std::move(bytes), 0});
            continue;
        }

        if (status == 0xFFU) {
            if (pos >= end) {
                throw std::runtime_error("XMIDI truncated meta-event");
            }
            const auto meta_type = data[pos++];
            auto [length, next] = read_vlq(data, pos);
            pos = next;
            if (pos + length > end) {
                throw std::runtime_error(
                    "XMIDI truncated meta-event payload");
            }

            if (meta_type == 0x2FU) {
                if (length != 0) {
                    throw std::runtime_error(
                        "XMIDI end-of-track has nonzero payload");
                }
                sequence.events.push_back(
                    {delay, XmiEventKind::EndOfTrack, {}, 0});
            } else {
                // Meta events are not emitted to the MIDI device. XMIDI's
                // wall-clock event timing remains fixed at 120 Hz.
            }
            pos += length;
            continue;
        }

        throw std::runtime_error("XMIDI contains unsupported status byte");
    }

    if (sequence.events.empty()) {
        throw std::runtime_error("XMIDI EVNT contains no events");
    }
    return sequence;
}

}  // namespace

XmiSequence parse_xmidi_sequence(
    const std::vector<std::uint8_t>& xmi,
    int sequence_index) {
    if (sequence_index < 0) {
        throw std::out_of_range("XMIDI sequence index is negative");
    }

    std::vector<std::pair<std::size_t, std::size_t>> chunks;
    collect_sequences(xmi, 0, xmi.size(), chunks);
    if (sequence_index >= static_cast<int>(chunks.size())) {
        throw std::out_of_range("XMIDI sequence index exceeds XDIR/CAT contents");
    }

    const auto [start, end] =
        chunks[static_cast<std::size_t>(sequence_index)];
    return parse_evnt(xmi, start, end);
}

}  // namespace rloft
