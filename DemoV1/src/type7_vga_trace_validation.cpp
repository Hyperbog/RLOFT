#include "rloft/type7_vga_trace_validation.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

namespace rloft {
namespace {

struct CallGroup final {
    std::int32_t column{};
    std::size_t first{};
    std::size_t last{};
};

std::uint32_t swap_low_bytes(std::uint32_t value) noexcept {
    return (value & 0xFFFF0000U) |
           ((value & 0x00FFU) << 8U) |
           ((value & 0xFF00U) >> 8U);
}

std::array<std::uint8_t,3> expected_masks(
    std::int32_t column, std::uint16_t count) {
    std::array<std::uint8_t,3> masks{};
    const auto phase = static_cast<std::uint32_t>(column) & 3U;
    for(std::uint32_t i=0U; i<count; ++i) {
        const auto position=phase+i;
        if(position/4U>=masks.size()) {
            throw std::out_of_range(
                "original CODE.2 VGA run exceeds its three mask passes");
        }
        masks[position/4U] |= static_cast<std::uint8_t>(
            1U << (position&3U));
    }
    return masks;
}

} // namespace

void Type7VgaTraceValidator::validate(
    const Type7VgaSceneRun& run) {
    if(run.width<1U || run.width>7U ||
       run.viewport_x>static_cast<std::size_t>(
           std::numeric_limits<std::int32_t>::max()-7)) {
        throw std::out_of_range(
            "type-7 VGA trace has unverified width or column");
    }
    const auto start=static_cast<std::int32_t>(run.viewport_x);
    const auto& calls=run.dispatch.calls;
    if(calls.empty()) {
        throw std::invalid_argument(
            "visible type-7 run has no CODE.2 raster calls");
    }

    std::vector<CallGroup> groups;
    for(std::size_t at=0U;at<calls.size();) {
        std::size_t end=at+1U;
        while(end<calls.size() &&
              calls[end].destination_column==
                  calls[at].destination_column) {
            ++end;
        }
        groups.push_back(
            CallGroup{calls[at].destination_column,at,end});
        at=end;
    }
    const bool individual=run.individual_columns_mode;
    const auto required_groups=individual
        ? static_cast<std::size_t>(run.width)
        : 1U;
    if(groups.size()!=required_groups) {
        throw std::invalid_argument(
            "type-7 VGA call trace does not match the CODE.2 C029 mode");
    }
    if(groups.size()==1U && groups[0].column!=start) {
        throw std::invalid_argument(
            "type-7 VGA grouped dispatch starts at the wrong column");
    }
    const auto first_lookup=calls.front().ecx_lookup;
    const auto original_lookup=
        (static_cast<std::uint32_t>(start)&1U)!=0U &&
        individual
            ? swap_low_bytes(first_lookup)
            : first_lookup;

    for(std::size_t group_index=0U;
        group_index<groups.size();++group_index) {
        const auto& group=groups[group_index];
        const auto expected_column=start+
            static_cast<std::int32_t>(
                individual?group_index:0U);
        if(group.column!=expected_column) {
            throw std::invalid_argument(
                "type-7 VGA destination columns are missing or reordered");
        }
        const auto width=individual?1U:run.width;
        const auto masks=expected_masks(group.column,width);
        const auto expected_lookup=individual &&
            ((static_cast<std::uint32_t>(group.column)&1U)!=0U)
            ? swap_low_bytes(original_lookup)
            : original_lookup;

        std::size_t call_index=group.first;
        const auto base=calls[call_index].physical_address;
        for(std::size_t pass=0U;pass<masks.size();++pass) {
            if(masks[pass]==0U) {
                continue;
            }
            if(call_index>=group.last ||
               calls[call_index].pass_index!=pass ||
               calls[call_index].sequencer_mask!=masks[pass] ||
               calls[call_index].ecx_lookup!=expected_lookup ||
               calls[call_index].physical_address!=
                   base+static_cast<std::uint32_t>(pass)) {
                throw std::invalid_argument(
                    "type-7 VGA trace mask, lookup, pass or address is inconsistent");
            }
            ++call_index;
        }
        if(call_index!=group.last) {
            throw std::invalid_argument(
                "type-7 VGA trace contains extra or duplicate passes");
        }
    }
    const auto expected_after=individual
        ? original_lookup
        : swap_low_bytes(original_lookup);
    if(run.dispatch.lookup_after_dispatch!=expected_after) {
        throw std::invalid_argument(
            "type-7 CODE.2 lookup word was not restored/swapped");
    }
}

} // namespace rloft
