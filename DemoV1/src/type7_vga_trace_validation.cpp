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
    if(run.width<1U || run.width>255U ||
       (!run.individual_columns_mode && run.width>7U) ||
       run.viewport_x>static_cast<std::size_t>(
           std::numeric_limits<std::int32_t>::max())-
           static_cast<std::size_t>(run.width-1U)) {
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
        if(individual) {
            // 36B80 increments horizontal 105E3 by one; 369D8
            // computes SAR(x,2). The physical byte destination must
            // advance only at the four-plane boundary, not arbitrarily
            // between successive individual-column calls.
            const auto physical_byte_delta=
                static_cast<std::uint32_t>(group.column/4) -
                static_cast<std::uint32_t>(start/4);
            if(base != calls.front().physical_address+
                           physical_byte_delta) {
                throw std::invalid_argument(
                    "type-7 VGA individual columns have wrong byte addresses");
            }
        }
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


void Type7VgaTraceValidator::validate_scene(
    const Type7VgaSceneTrace& trace,
    const WorldSceneRenderPlan& plan) {
    if(trace.runs.size()!=plan.visible_runs.size()) {
        throw std::invalid_argument(
            "type-7 scene trace has missing or additional source-column runs");
    }
    std::uint32_t current_lookup=plan.traversal.lookup_value;
    std::size_t expected_descriptor_index=0U;
    for(std::size_t index=0U;index<trace.runs.size();++index) {
        const auto& run=trace.runs[index];
        const auto& original=plan.visible_runs[index];
        if(run.source_x!=original.source_x ||
           run.source_column_index!=original.source_column_index ||
           run.viewport_x!=original.viewport_x ||
           run.width!=original.width) {
            throw std::invalid_argument(
                "type-7 scene trace changed original source-column geometry");
        }
        validate(run);
        // WorldSceneRenderPlanModel has already expanded each accepted
        // source-column run into one Type7MicrocodeDraw per *destination*
        // column and native 6-byte display-list descriptor. The VGA
        // trace intentionally stores that list only once per run, but
        // its contents and length must still match the original plan.
        // Otherwise an omitted descriptor could falsely count as a
        // fully drawn NPC or item even though zero pixels were written.
        for(std::size_t destination=0U;
            destination<run.width;
            ++destination) {
            for(const auto& actual:run.descriptors) {
                if(expected_descriptor_index>=plan.microcode_draws.size()) {
                    throw std::invalid_argument(
                        "type-7 VGA scene trace added a descriptor absent from the plan");
                }
                const auto& expected=
                    plan.microcode_draws[expected_descriptor_index++];
                if(expected.viewport_x!=run.viewport_x+destination ||
                   expected.source_x!=actual.source_x ||
                   expected.source_column_index!=actual.source_column_index ||
                   expected.descriptor_index!=actual.descriptor_index ||
                   expected.source_row_begin!=actual.source_row_begin ||
                   expected.source_row_end!=actual.source_row_end ||
                   expected.signed_source_adjustment!=
                       actual.signed_source_adjustment ||
                   expected.entry_selector!=actual.entry_selector ||
                   expected.stop_selector!=actual.stop_selector ||
                   expected.entry_patch_offset!=actual.entry_patch_offset ||
                   expected.stop_patch_offset!=actual.stop_patch_offset ||
                   expected.entry_code_offset!=actual.entry_code_offset ||
                   expected.stop_code_offset!=actual.stop_code_offset) {
                    throw std::invalid_argument(
                        "type-7 VGA scene descriptor differs from the original render plan");
                }
            }
        }
        const auto initial_pass_lookup=
            run.individual_columns_mode &&
            (run.viewport_x&1U)!=0U
                ? swap_low_bytes(current_lookup)
                : current_lookup;
        if(run.dispatch.calls.front().ecx_lookup !=
           initial_pass_lookup) {
            throw std::invalid_argument(
                "type-7 scene trace lost CODE.2 lookup state between runs");
        }
        current_lookup=run.dispatch.lookup_after_dispatch;
    }
    if(expected_descriptor_index!=plan.microcode_draws.size()) {
        throw std::invalid_argument(
            "type-7 VGA scene trace omitted original SINTAB descriptors");
    }
    if(trace.final_lookup_word!=current_lookup) {
        throw std::invalid_argument(
            "type-7 scene trace has inconsistent final lookup word");
    }
}

} // namespace rloft
