#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

#include "rloft/original_ground_ff50_source_span.hpp"
#include "rloft/original_ground_nearest_22220.hpp"
#include "rloft/view_sweep_record.hpp"

namespace rloft {

// Read-only native projection of CODE.2 335C8 -> 32954 / 32F4C /
// 32F8C / 33124 / 332B4 / 333BC -> 1F255.
// NOT a DOS RAM dump: original 32954 can reuse cached view records,
// whereas native ViewSweepTraversalModel re-materializes each frame.
// This reports source-ordered arithmetic on the exact native records,
// with explicit producer identity, for comparison in F12 field captures.
struct OriginalGroundSourceProjection final {
    std::size_t native_ray_count{};
    std::size_t source_materializers{};
    std::size_t source_sentinel_writers{};
    std::size_t unknown_origins{};
    std::optional<OriginalGroundNearest22220> nearest;
    std::optional<OriginalGroundFF50Span> ff50;

    [[nodiscard]] bool source_replay_available() const noexcept {
        return nearest.has_value() && ff50.has_value();
    }
};

class OriginalGroundSourceProjectionModel final {
public:
    [[nodiscard]] static OriginalGroundSourceProjection evaluate(
        const std::vector<ViewSweepRecord>& native_records,
        std::size_t original_c6d8_ray_count,
        std::uint32_t original_c738_scale_count) {
        OriginalGroundSourceProjection result{};
        result.native_ray_count=native_records.size();
        std::vector<OriginalGroundMaterializedRay> source_records;
        source_records.reserve(native_records.size());
        bool invalid_source_sentinel_fields=false;
        for(const auto& record:native_records) {
            if(record.original_materializer_executed)
                ++result.source_materializers;
            else if(record.original_sentinel_initializer_executed)
                ++result.source_sentinel_writers;
            else ++result.unknown_origins;
            // CODE.2 32F4C initializes ALL seven DWORD fields:
            // depth=0, both source materials=0x93, and +0C..+18=0.
            // The nearest replay alone sees only depth and inner,
            // so reject a corrupt/partially synthesized filler here.
            if(record.original_sentinel_initializer_executed &&
               (record.depth!=0U ||
                record.resource_outer!=0x93U ||
                record.resource_inner!=0x93U ||
                record.prepass!=0U ||
                record.texture_column_offset!=0U ||
                record.cell_index!=0U ||
                record.shade_decrement!=0U))
                invalid_source_sentinel_fields=true;
            // Signed x86 record depth: convert raw DWORD without
            // relying on implementation-defined uint32->int32 cast.
            const auto raw=record.depth;
            const auto signed_depth=raw<=0x7FFFFFFFU
                ? static_cast<std::int32_t>(raw)
                : static_cast<std::int32_t>(
                    static_cast<std::int64_t>(raw)-0x100000000LL);
            source_records.push_back(OriginalGroundMaterializedRay{
                signed_depth,record.resource_inner,
                record.original_materializer_executed,
                record.original_sentinel_initializer_executed
            });
        }
        // Preserve truthful provenance counts, but do not emit a
        // nearest-wall prediction from an invalid original filler.
        if(invalid_source_sentinel_fields) return result;
        // The original replay rejects wrong counts, unknowns, and
        // contradictory provenance; no guessed nearest-depth fallback.
        result.nearest=OriginalGroundNearest22220SourceModel::replay(
            source_records,original_c6d8_ray_count);
        if(result.nearest) {
            result.ff50=OriginalGroundFF50SourceModel::from_22220(
                result.nearest->nearest_depth,original_c738_scale_count);
        }
        return result;
    }
};

} // namespace rloft
