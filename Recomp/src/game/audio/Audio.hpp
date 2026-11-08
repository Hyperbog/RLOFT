#pragma once
// Restrictive decoder for verified Creative Voice File type-1 unsigned
// 8-bit PCM with optional type-2 continuations. Unsupported codecs,
// silence/loop/extended blocks fail closed. This DOES NOT emulate AIL.
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <algorithm>

namespace rloft { namespace host_voc {
struct Sample {
    std::uint32_t rate_hz=0;
    std::vector<std::uint8_t> pcm8{};
};
Sample decode_pcm8_mono(const std::vector<std::uint8_t>& voc);
}} // namespaces

// Legacy CODE.2 resource-packet envelope, consolidated with VOC parsing.
// Bounded original CODE.2 27F24 / 2F29C resource-packet transport.
// Packet is the original LE32 byte-length followed by Creative VOC bytes;
// this is NOT proof of active AIL/32 status, callbacks or sound-card output.
#include <cstdint>
#include <stdexcept>
#include <vector>
namespace rloft { namespace source_audio {
host_voc::Sample decode_original_length_prefixed_voc(
    const std::vector<std::uint8_t>& packet);
}} // namespaces
