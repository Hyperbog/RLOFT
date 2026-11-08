#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace rloft {

// One original 28-byte ray record in producer order. It is EITHER
// a 330E7/33278/3337F/33487 source materializer OR a source 32F4C
// sentinel writer (NO nearest 22220 update). Unknown native fixture
// provenance must not be treated as either original execution path.
struct OriginalGroundMaterializedRay final {
    std::int32_t depth_at_15a18{};
    std::uint32_t main_resource_at_15a20{};
    bool original_materializer_executed{};
    bool original_sentinel_initializer_executed{};
};

struct OriginalGroundNearest22220 final {
    std::int32_t nearest_depth{};
    std::size_t sentinel_zero_resets{};
    std::size_t signed_min_updates{};
    std::size_t materialized_ray_count{};
    std::size_t source_sentinel_fill_count{};
};

class OriginalGroundNearest22220SourceModel final {
public:
    static constexpr std::int32_t kInitialNearest=0x800;
    static constexpr std::uint32_t kEmptyMainResource=0x93U;

    // Isolated, source-order 335C8 initialization and per-materializer
    // stores. Does NOT infer which native rays were materialized, the
    // original dynamic C026/C027 state or exact live DOS 22220 RAM.
    [[nodiscard]] static std::optional<OriginalGroundNearest22220>
    replay(const std::vector<OriginalGroundMaterializedRay>& rays,
           std::size_t original_source_c6d8_ray_count) noexcept {
        if(original_source_c6d8_ray_count==0U ||
           rays.size()!=original_source_c6d8_ray_count)
            return std::nullopt;
        OriginalGroundNearest22220 out{};
        out.nearest_depth=kInitialNearest;
        for(const auto& ray:rays) {
            if(ray.original_sentinel_initializer_executed) {
                // Original CODE.2 32F4C fills 28-byte ray slot with
                // depth 0, main/outer 0x93 and zero remaining words.
                // It DOES NOT read or write nearest global 22220.
                if(ray.original_materializer_executed ||
                   ray.depth_at_15a18!=0 ||
                   ray.main_resource_at_15a20!=kEmptyMainResource)
                    return std::nullopt;
                ++out.source_sentinel_fill_count;
                continue;
            }
            if(!ray.original_materializer_executed)
                return std::nullopt;
            // Original record+8 (main), not dormant alternate/prepass.
            // 0x93 explicitly STORES ZERO to 22220, not just leaving
            // the initialized 0x800 value untouched.
            if(ray.main_resource_at_15a20==kEmptyMainResource) {
                out.nearest_depth=0;
                ++out.sentinel_zero_resets;
            }
            // Original EAX=record+0; CMP/JGE is SIGNED.
            if(ray.depth_at_15a18<out.nearest_depth) {
                out.nearest_depth=ray.depth_at_15a18;
                ++out.signed_min_updates;
            }
            ++out.materialized_ray_count;
        }
        return out;
    }
};

} // namespace rloft
