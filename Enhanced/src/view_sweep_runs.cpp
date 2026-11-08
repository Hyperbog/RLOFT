#include "rloft/view_sweep_runs.hpp"

#include <stdexcept>

namespace rloft {
namespace {

ViewSweepRun make_run(
    SweepRenderPass pass,
    std::size_t first,
    std::size_t count,
    const ViewSweepRecord& record) {
    ViewSweepRun out{};
    out.pass = pass;
    out.first_ray = first;
    out.ray_count = count;
    out.depth = record.depth;
    out.resource_id =
        pass == SweepRenderPass::Prepass
            ? record.resource_outer
            : record.resource_inner;
    out.texture_column_offset =
        record.texture_column_offset;
    out.texture_source_byte_offset =
        record.texture_column_offset << 1U;
    out.texture_column_index =
        record.texture_column_offset >> 6U;
    out.shade_decrement = record.shade_decrement;
    return out;
}

}  // namespace

std::vector<ViewSweepRun> ViewSweepRunModel::build(
    const std::vector<ViewSweepRecord>& records) {
    std::vector<ViewSweepRun> out;
    if (records.empty()) return out;

    // First pass: search/restart on +0C == 1.
    std::size_t index = 0;
    while (index < records.size()) {
        while (index < records.size() &&
               records[index].prepass != 1U) {
            ++index;
        }
        if (index >= records.size()) break;

        const auto first = index;
        const auto depth = records[index].depth;
        const auto column =
            records[index].texture_column_offset;
        std::size_t count = 1;
        ++index;

        while (index < records.size()) {
            const auto& current = records[index];
            if (current.prepass != 1U) break;

            const auto& previous = records[index - 1U];
            if (current.depth != previous.depth ||
                current.texture_column_offset !=
                    previous.texture_column_offset) {
                out.push_back(make_run(
                    SweepRenderPass::Prepass,
                    first,
                    count,
                    records[first]));
                const auto next_first = index;
                const auto next_depth = current.depth;
                const auto next_column =
                    current.texture_column_offset;
                (void)next_depth;
                (void)next_column;

                // Start a new run at the changed record and continue the
                // original linear scan.
                std::size_t changed_count = 1;
                ++index;
                while (index < records.size() &&
                       records[index].prepass == 1U &&
                       records[index].depth ==
                           records[index - 1U].depth &&
                       records[index].texture_column_offset ==
                           records[index - 1U].texture_column_offset) {
                    ++changed_count;
                    ++index;
                }
                out.push_back(make_run(
                    SweepRenderPass::Prepass,
                    next_first,
                    changed_count,
                    records[next_first]));

                // If this stopped on another changed prepass record, rewind
                // one logical step so the outer search can begin there.
                if (index < records.size() &&
                    records[index].prepass == 1U) {
                    continue;
                }
                count = 0;
                break;
            }
            ++count;
            ++index;
        }

        if (count != 0) {
            out.push_back(make_run(
                SweepRenderPass::Prepass,
                first,
                count,
                records[first]));
        }
    }

    // Second pass: record zero always initializes the first run. Every record
    // participates; resource 0x93 is filtered only by the downstream draw.
    std::size_t first = 0;
    std::size_t count = 1;
    for (std::size_t i = 1; i < records.size(); ++i) {
        if (records[i].depth != records[i - 1U].depth ||
            records[i].texture_column_offset !=
                records[i - 1U].texture_column_offset) {
            out.push_back(make_run(
                SweepRenderPass::Main,
                first,
                count,
                records[first]));
            first = i;
            count = 1;
        } else {
            ++count;
        }
    }
    out.push_back(make_run(
        SweepRenderPass::Main,
        first,
        count,
        records[first]));

    return out;
}

}  // namespace rloft
