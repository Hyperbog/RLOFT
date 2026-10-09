#include "rloft/original_ground_res4_audit.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {
std::uint32_t fnv1a(const std::vector<std::uint8_t>& bytes) noexcept {
    std::uint32_t value=2166136261U;
    for (auto byte:bytes) {
        value^=byte;
        value*=16777619U;
    }
    return value;
}

bool is_source_ground_shape(const Type1Raster& decoded) noexcept {
    return decoded.count==1 && decoded.width==64 &&
           decoded.height==64 && decoded.pixels.size()==4096U;
}
} // namespace

OriginalGroundRes4Report OriginalGroundRes4Audit::inspect(
    const ResArchive& archive) {
    OriginalGroundRes4Report result{};
    result.total_entries=archive.entry_count();
    for(int i=0;i<result.total_entries;++i) {
        const auto raw=archive.entry(i);
        if(raw.size()<3U || raw[0]!='E' || raw[1]!='H' ||
           raw[2]!=1U) {
            continue;
        }
        ++result.type1_entries;
        if(raw.size()<13U) {
            ++result.rejected_type1_entries;
            continue;
        }
        // Only decompress the ORIGINAL header shapes that can possibly
        // become a 64x64 type-1 tile. Other RES4 type-1 images may be
        // very large portraits/backgrounds; decoding all of them
        // wastes time and exposes this diagnostics-only tool to an
        // avoidable allocation of unrelated resources.
        const auto count=static_cast<std::int16_t>(
            static_cast<std::uint16_t>(raw[5U]) |
            (static_cast<std::uint16_t>(raw[6U])<<8U));
        const auto height=static_cast<std::int16_t>(
            static_cast<std::uint16_t>(raw[7U]) |
            (static_cast<std::uint16_t>(raw[8U])<<8U));
        const auto width_units=raw[9U];
        const bool ordinary=count==1 && height==64 &&
                            width_units==8U;
        const bool original_dc90_transform=raw[12U]!=0U &&
            count==64 && height==1 && width_units==8U;
        if(!ordinary && !original_dc90_transform) {
            if(count<=0 || height<=0 || width_units==0U) {
                ++result.rejected_type1_entries;
            }
            continue;
        }
        try {
            const auto decoded=archive.decode_type1(i);
            if(!is_source_ground_shape(decoded)) {
                continue;
            }
            // Normal type-1 descriptor starts at EH header +5:
            // bytes +6 compression and +7 transform.
            result.tiles_64x64.push_back(
                OriginalGroundRes4Candidate{
                    i,raw.size(),raw[11U],raw[12U],
                    decoded.transparent_index,
                    fnv1a(decoded.pixels)
                });
        } catch(const std::exception&) {
            ++result.rejected_type1_entries;
            // Do not conceal corruption as a valid source candidate.
        }
    }
    return result;
}

int OriginalGroundRes4Audit::absolute_index(
    int source_relative_selector, int verified_runtime_base,
    int original_entry_count) {
    if (original_entry_count<=0) {
        throw std::invalid_argument(
            "RES4 original entry count must be positive");
    }
    const auto selected=
        static_cast<std::int64_t>(source_relative_selector)+
        static_cast<std::int64_t>(verified_runtime_base);
    if(selected<0 || selected>=original_entry_count) {
        throw std::out_of_range(
            "CODE.2 1F388 RES4 relative/base selector is out of range");
    }
    return static_cast<int>(selected);
}

std::vector<std::uint8_t> OriginalGroundRes4Audit::verified_tile_source(
    const ResArchive& archive, int original_absolute_index) {
    const auto decoded=archive.decode_type1(
        original_absolute_index);
    if(!is_source_ground_shape(decoded)) {
        throw std::invalid_argument(
            "selected RES4 type-1 source is not a decoded 64x64 tile");
    }
    // Host's dummy 2 bytes model the C040+2 addressing contract:
    // the pixels themselves are the actual unmodified RES4 source.
    std::vector<std::uint8_t> result{0U,0U};
    result.insert(
        result.end(),decoded.pixels.begin(),decoded.pixels.end());
    return result;
}

} // namespace rloft
