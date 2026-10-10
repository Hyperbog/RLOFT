#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace rloft {

// A record that passed through original CODE.2 materializer
// 330E7,33278,3337F,33487. Native empty_record() placeholders
// MUST NOT be silently claimed to be original materialized rays.
struct OriginalGroundMaterializedRay final {
    std::int32_t depth_at_15a18{};
    std::uint32_t main_resource_at_15a20{};
    bool original_materializer_executed{};
};

struct OriginalGroundNearest22220 final {
    std::int32_t nearest_depth{};
    std::size_t sentinel_zero_resets{};
    std::size_t signed_min_updates{};
    std::size_t materialized_ray_count{};
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
