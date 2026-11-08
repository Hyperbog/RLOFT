// Original Creative VOC and CODE.2 27F24 packet decoder implementation.
// Host codec only; never asserts Miles AIL, DOS IRQ or sound-device fidelity.
#include "Audio.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace rloft { namespace host_voc {
Sample decode_pcm8_mono(const std::vector<std::uint8_t>& voc) {
    constexpr std::uint8_t signature[20] = {
        'C','r','e','a','t','i','v','e',' ','V','o','i','c','e',' ','F','i','l','e',0x1A
    };
    if(voc.size()<27u || !std::equal(signature,signature+20,voc.begin()))
        throw std::runtime_error("Creative Voice signature missing");
    const auto le16=[&](std::size_t i)->std::uint16_t{
        return static_cast<std::uint16_t>(voc[i]|(unsigned(voc[i+1])<<8u));
    };
    const std::size_t begin=le16(20u);
    if(begin<26u || begin>=voc.size())
        throw std::runtime_error("Invalid VOC data-start offset");
    const unsigned version=le16(22u);
    const unsigned checksum=le16(24u);
    if((((~version)+0x1234u)&0xFFFFu)!=checksum)
        throw std::runtime_error("VOC header checksum mismatch");
    Sample out{};
    std::size_t at=begin;
    bool started=false,ended=false;
    while(at<voc.size()) {
        const unsigned type=voc[at++];
        if(type==0u) {ended=true;break;}
        if(at+3u>voc.size())
            throw std::runtime_error("Truncated VOC block length");
        const std::size_t size=std::size_t(voc[at])|
            (std::size_t(voc[at+1])<<8u)|(std::size_t(voc[at+2])<<16u);
        at+=3u;
        if(size>voc.size()-at)
            throw std::runtime_error("VOC block exceeds file");
        std::size_t payload=at,length=size;
        if(type==1u) {
            if(started || size<2u)
                throw std::runtime_error("Multiple or truncated VOC type-1 blocks");
            const unsigned divisor=256u-unsigned(voc[at]);
            const unsigned codec=voc[at+1u];
            if(divisor==0u || codec!=0u)
                throw std::runtime_error("VOC is not type-1 8-bit PCM");
            out.rate_hz=1000000u/divisor;
            if(out.rate_hz<4000u || out.rate_hz>48000u)
                throw std::runtime_error("Unverified VOC PCM sample rate");
            payload+=2u;length-=2u;started=true;
        } else if(type!=2u || !started) {
            // Do not silently drop loops, silence, markers, compressed
            // samples or VOC extended-format control records.
            throw std::runtime_error("Unsupported VOC command/codec");
        }
        if(out.pcm8.size()+length>16u*1024u*1024u)
            throw std::runtime_error("VOC PCM stream exceeds guarded limit");
        out.pcm8.insert(out.pcm8.end(),voc.begin()+static_cast<std::ptrdiff_t>(payload),
                        voc.begin()+static_cast<std::ptrdiff_t>(payload+length));
        at+=size;
    }
    if(!started || !ended || out.pcm8.empty())
        throw std::runtime_error("VOC is missing PCM data or terminator");
    // Retail VOC archives may contain alignment pads. For this narrow
    // decoder, only NUL trailing pads are unambiguous.
    if(std::any_of(voc.begin()+static_cast<std::ptrdiff_t>(at),
                   voc.end(),[](std::uint8_t c){return c!=0u;}))
        throw std::runtime_error("Unverified trailing VOC bytes");
    return out;
}
}} // namespace rloft::host_voc

namespace rloft { namespace source_audio {
host_voc::Sample decode_original_length_prefixed_voc(
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
}} // namespace rloft::source_audio
