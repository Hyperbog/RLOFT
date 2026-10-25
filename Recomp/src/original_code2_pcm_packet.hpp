#pragma once
// Bounded original CODE.2 27F24 / 2F29C resource-packet transport.
// Packet is the original LE32 byte-length followed by Creative VOC bytes;
// this is NOT proof of active AIL/32 status, callbacks or sound-card output.
#include "original_voc_pcm8.hpp"
#include <cstdint>
#include <stdexcept>
#include <vector>
namespace rloft { namespace source_audio {
inline host_voc::Sample decode_original_length_prefixed_voc(
    const std::vector<std::uint8_t>& packet) {
    if(packet.size()<4u)
        throw std::runtime_error("Original 27F24 packet has no LE32 length");
    const std::uint32_t count =
        static_cast<std::uint32_t>(packet[0]) |
        (static_cast<std::uint32_t>(packet[1])<<8u) |
        (static_cast<std::uint32_t>(packet[2])<<16u) |
        (static_cast<std::uint32_t>(packet[3])<<24u);
    // Original source 27F24 copies EXACTLY count bytes from source+4.
    // Our adapter refuses overhangs, truncated packets and a mismatched
    // real guest prefix: it will not guess or pad original VOC data.
    if(count>16u*1024u*1024u || count==0u ||
       packet.size()-4u!=static_cast<std::size_t>(count))
        throw std::runtime_error("Original 27F24 sound packet length mismatch");
    std::vector<std::uint8_t> voc(packet.begin()+4,packet.end());
    return host_voc::decode_pcm8_mono(voc);
}
}} // namespaces
