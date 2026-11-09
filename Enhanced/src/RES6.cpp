#include "rloft/RES6.hpp"
#include "rloft/resource_assets.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

namespace rloft {
namespace {
constexpr std::size_t kHeaderBytes = 26U;
constexpr std::size_t kEnvelopeBytes = 9U;
constexpr std::uint8_t kVocSignature[20] = {
    'C','r','e','a','t','i','v','e',' ','V','o','i','c','e',
    ' ','F','i','l','e',0x1AU
};
std::uint16_t le16(const std::vector<std::uint8_t>& b,
                   std::size_t at) noexcept {
    return static_cast<std::uint16_t>(
        static_cast<unsigned>(b[at]) |
        (static_cast<unsigned>(b[at+1U]) << 8U));
}
std::uint32_t le24(const std::vector<std::uint8_t>& b,
                   std::size_t at) noexcept {
    return std::uint32_t(b[at]) | (std::uint32_t(b[at+1U]) << 8U) |
           (std::uint32_t(b[at+2U]) << 16U);
}
void append_le32(std::vector<std::uint8_t>& out, std::uint32_t n) {
    for (unsigned shift = 0U; shift < 32U; shift += 8U)
        out.push_back(static_cast<std::uint8_t>(n >> shift));
}
} // namespace

Res6VocInfo inspect_res6_voc(const std::vector<std::uint8_t>& b) {
    if (b.size() < kHeaderBytes)
        throw std::runtime_error("RES6 VOC truncated before file header");
    for (std::size_t i=0U; i<20U; ++i) {
        if (b[i] != kVocSignature[i])
            throw std::runtime_error("RES6 VOC signature mismatch");
    }
    const auto at = static_cast<std::size_t>(le16(b,20U));
    const auto version = le16(b,22U);
    const auto checksum = le16(b,24U);
    const auto expected = static_cast<std::uint16_t>(
        (0x1234U + (~static_cast<unsigned>(version) & 0xFFFFU)) & 0xFFFFU);
    if (at < kHeaderBytes || at >= b.size() || checksum != expected)
        throw std::runtime_error("RES6 VOC data offset or version checksum invalid");

    Res6VocInfo result{};
    result.version=version;
    result.bytes=b.size();
    std::size_t cursor=at;
    bool have_sound=false;
    bool terminated=false;
    while (cursor < b.size()) {
        const auto kind=b[cursor++];
        if (kind == 0U) { terminated=true; break; }
        if (cursor > b.size() || b.size()-cursor < 3U)
            throw std::runtime_error("RES6 VOC block length truncated");
        const auto length=static_cast<std::size_t>(le24(b,cursor));
        cursor+=3U;
        if (length > b.size()-cursor)
            throw std::runtime_error("RES6 VOC block exceeds file");
        if ((kind==1U && length<2U) || (kind==9U && length<12U))
            throw std::runtime_error("RES6 VOC sound-data block truncated");
        if (kind==2U && !have_sound)
            throw std::runtime_error("RES6 VOC continuation without initial sound");
        if (kind==1U || kind==9U) {
            ++result.sound_blocks;
            have_sound=true;
        } else if (kind==2U) {
            ++result.continuation_blocks;
        }
        ++result.blocks;
        cursor+=length;
    }
    if (!terminated || !have_sound || b.size()-cursor > 3U)
        throw std::runtime_error("RES6 VOC missing sound/terminator or excessive trailer");
    for (std::size_t i=cursor; i<b.size(); ++i) {
        if (b[i]!=0U)
            throw std::runtime_error("RES6 VOC nonzero bytes after terminator");
    }
    result.padding_bytes=b.size()-cursor;
    return result;
}

std::vector<std::uint8_t> decode_res6_voc(
    const ResArchive& archive, int index) {
    // The existing type-4 loader owns EH header parsing and byte extraction.
    // RES6's shipped entry lengths match the payload exactly, unlike two
    // documented anomaly records in the unrelated RES11/RES12 archives.
    const auto raw=archive.entry(index);
    if (raw.size()<kEnvelopeBytes || raw[0]!='E' || raw[1]!='H' ||
        raw[2]!=4U || raw[3]!=0U || raw[4]!=0U)
        throw std::runtime_error("RES6 requires EH type-4 header size zero");
    const auto declared=std::uint32_t(raw[5]) |
        (std::uint32_t(raw[6]) << 8U) |
        (std::uint32_t(raw[7]) << 16U) |
        (std::uint32_t(raw[8]) << 24U);
    if (declared != raw.size()-kEnvelopeBytes)
        throw std::runtime_error("RES6 EH4 declared/stored VOC lengths differ");
    auto voc=archive.decode_type4(index);
    (void)inspect_res6_voc(voc);
    return voc;
}

Res6VocInfo inspect_res6_entry(const ResArchive& archive, int index) {
    return inspect_res6_voc(decode_res6_voc(archive,index));
}

Res6ArchiveStats inspect_res6_archive(const ResArchive& archive) {
    Res6ArchiveStats result{};
    result.entries=static_cast<std::size_t>(archive.entry_count());
    for (int i=0; i<archive.entry_count(); ++i) {
        const auto info=inspect_res6_entry(archive,i);
        const auto voc=archive.decode_type4(i);
        // The first block is the original sound-data block. Distinguish
        // VOC revision-1 and revision-9 records without guessing audio use.
        const auto first=voc[le16(voc,20U)];
        if (first==1U) ++result.standard_sounds;
        else if (first==9U) ++result.new_format_sounds;
        else throw std::runtime_error("RES6 unexpected first VOC block");
        result.continuation_blocks+=info.continuation_blocks;
        if (info.padding_bytes!=0U) ++result.padded_sounds;
        if (info.bytes > std::numeric_limits<std::uint64_t>::max()-result.voc_bytes)
            throw std::overflow_error("RES6 total VOC bytes overflow");
        result.voc_bytes+=static_cast<std::uint64_t>(info.bytes);
    }
    return result;
}

void replace_res6_voc(ResArchive& archive, int index,
                      const std::vector<std::uint8_t>& voc) {
    (void)decode_res6_voc(archive,index); // Reject non-RES6 source slots.
    (void)inspect_res6_voc(voc);
    if (voc.size() > std::numeric_limits<std::uint32_t>::max()-kEnvelopeBytes)
        throw std::overflow_error("RES6 VOC exceeds 32-bit EH4 payload");
    std::vector<std::uint8_t> raw;
    raw.reserve(kEnvelopeBytes+voc.size());
    raw.insert(raw.end(),{'E','H',4U,0U,0U});
    append_le32(raw,static_cast<std::uint32_t>(voc.size()));
    raw.insert(raw.end(),voc.begin(),voc.end());
    // Original ResArchive transactionally rebuilds the directory.
    archive.replace_entry(index,raw);
}
} // namespace rloft
