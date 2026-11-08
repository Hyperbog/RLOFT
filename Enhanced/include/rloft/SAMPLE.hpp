#pragma once

#include <cstdint>
#include <vector>

namespace rloft {

// One Miles AIL 2.x YM3812 (OPL2) instrument. Retains original byte order.
// The 11 register fields are a modulator, channel feedback/connection,
// and carrier. No reinterpretation as General MIDI instruments is performed.
struct SampleAdTimbre {
    std::uint8_t patch{};
    std::uint8_t bank{};
    std::int8_t transpose{};
    std::uint8_t mod_avekm{};
    std::uint8_t mod_ksl_tl{};
    std::uint8_t mod_ad{};
    std::uint8_t mod_sr{};
    std::uint8_t mod_ws{};
    std::uint8_t fb_c{};
    std::uint8_t car_avekm{};
    std::uint8_t car_ksl_tl{};
    std::uint8_t car_ad{};
    std::uint8_t car_sr{};
    std::uint8_t car_ws{};
};

// Preserve serialized order: bank 0, bank 127, bank 1 in the retail data.
struct SampleAdLibrary {
    std::vector<SampleAdTimbre> timbres;

    // Bank and patch are XMIDI lookup keys, not indices. Returns first match.
    [[nodiscard]] const SampleAdTimbre* find(std::uint8_t bank,
                                             std::uint8_t patch) const noexcept;
};

// Lossless codec for SAMPLE.AD: 6-byte directory entries, FF FF terminator,
// contiguous absolute-offset 14-byte OPL2 instrument records. Reject malformed
// counts, offsets, lengths and trailing data; never embed original game media.
[[nodiscard]] SampleAdLibrary parse_sample_ad(
    const std::vector<std::uint8_t>& bytes);
[[nodiscard]] std::vector<std::uint8_t> encode_sample_ad(
    const SampleAdLibrary& library);

} // namespace rloft
