#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {
class ResArchive;

// RES6 consists of Creative Voice File (VOC) sound effects in EH type-4
// containers. This is a resource/edit interface, not another audio engine.
struct Res6VocInfo final {
    std::uint16_t version{};
    std::size_t bytes{};
    std::size_t blocks{};
    std::size_t sound_blocks{};       // VOC block 1 or 9
    std::size_t continuation_blocks{}; // VOC block 2
    std::size_t padding_bytes{};      // Up to three original zero bytes
};

struct Res6ArchiveStats final {
    std::size_t entries{};
    std::size_t standard_sounds{};
    std::size_t new_format_sounds{};
    std::size_t continuation_blocks{};
    std::size_t padded_sounds{};
    std::uint64_t voc_bytes{};
};

[[nodiscard]] Res6VocInfo inspect_res6_voc(
    const std::vector<std::uint8_t>& voc);
[[nodiscard]] Res6VocInfo inspect_res6_entry(
    const ResArchive& archive, int index);
[[nodiscard]] Res6ArchiveStats inspect_res6_archive(
    const ResArchive& archive);
[[nodiscard]] std::vector<std::uint8_t> decode_res6_voc(
    const ResArchive& archive, int index);

// Replace one sound with another valid VOC, retaining the original
// type-4 envelope and the byte-exact other 265 entries. Unknown blocks
// are retained verbatim, never reinterpreted or transcoded.
void replace_res6_voc(ResArchive& archive, int index,
                      const std::vector<std::uint8_t>& voc);

} // namespace rloft
