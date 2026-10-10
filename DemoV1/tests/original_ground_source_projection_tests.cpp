#ifdef NDEBUG
#undef NDEBUG
#endif

#include "rloft/original_ground_source_projection.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

int main() {
    using namespace rloft;
    constexpr std::size_t kOriginalC6D8=256U;
    constexpr std::uint32_t kOriginalC738=127U;
    ViewSweepRecord full{};
    full.depth=3000U;
    full.resource_inner=42U;
    full.resource_outer=17U;
    full.original_materializer_executed=true;
    std::vector<ViewSweepRecord> rays(kOriginalC6D8,full);

    const auto open=OriginalGroundSourceProjectionModel::evaluate(
        rays,kOriginalC6D8,kOriginalC738);
    assert(open.source_replay_available());
    assert(open.native_ray_count==256U);
    assert(open.source_materializers==256U);
    assert(open.source_sentinel_writers==0U);
    assert(open.unknown_origins==0U);
    assert(open.nearest->nearest_depth==0x800);
    assert(open.nearest->materialized_ray_count==256U);
    assert(open.ff50->upper_ff50==128);
    assert(open.ff50->lower_ff50==128);
    assert(open.ff50->lower_four_plane_rows()==80U);
    assert(open.ff50->lower_paired_plane_rows()==48U);

    // A real original materializer, not a 32F4C filler, drives
    // source 22220 and the truncated lookup index.
    rays[2U].depth=408U;
    auto clipped=OriginalGroundSourceProjectionModel::evaluate(
        rays,kOriginalC6D8,kOriginalC738);
    assert(clipped.nearest && clipped.nearest->nearest_depth==408);
    assert(clipped.ff50 && clipped.ff50->lower_ff50==1);
    assert(clipped.ff50->lower_four_plane_rows()==1U);
    assert(clipped.ff50->lower_paired_plane_rows()==0U);
    rays[2U].depth=416U;
    clipped=OriginalGroundSourceProjectionModel::evaluate(
        rays,kOriginalC6D8,kOriginalC738);
    assert(clipped.nearest && clipped.nearest->nearest_depth==416);
    assert(clipped.ff50 && clipped.ff50->lower_ff50==-2);
    assert(!clipped.ff50->lower_has_textured_rows());

    // This original CODE.2 32F4C sentinel writer is not a
    // CODE.2 330E7/33278/3337F/33487 depth-materializer.
    rays[3U]=ViewSweepRecord{};
    rays[3U].resource_outer=0x93U;
    rays[3U].resource_inner=0x93U;
    rays[3U].original_sentinel_initializer_executed=true;
    const auto filler=OriginalGroundSourceProjectionModel::evaluate(
        rays,kOriginalC6D8,kOriginalC738);
    assert(filler.source_replay_available());
    assert(filler.source_sentinel_writers==1U);
    assert(filler.source_materializers==255U);
    assert(filler.nearest->source_sentinel_fill_count==1U);
    assert(filler.nearest->nearest_depth==416);
    assert(filler.ff50->lower_ff50==-2);

    // 0x93 after actual materialization DOES write 22220=0
    // ahead of the original signed min comparison.
    rays[4U].resource_inner=0x93U;
    const auto reset=OriginalGroundSourceProjectionModel::evaluate(
        rays,kOriginalC6D8,kOriginalC738);
    assert(reset.source_replay_available());
    assert(reset.nearest->nearest_depth==0);
    assert(reset.nearest->sentinel_zero_resets==1U);
    assert(reset.ff50->lower_ff50==128);

    // Signed x86 record DWORD -7, not an unsigned huge depth.
    rays[5U].depth=0xFFFFFFF9U;
    const auto negative=OriginalGroundSourceProjectionModel::evaluate(
        rays,kOriginalC6D8,kOriginalC738);
    assert(negative.nearest && negative.nearest->nearest_depth==-7);
    assert(!negative.ff50); // outside verified original C738 domain

    // Unknown source provenance and invalid original 32F4C record
    // both fail closed. Never silently guess the nearest wall.
    rays[6U]=ViewSweepRecord{};
    const auto unknown=OriginalGroundSourceProjectionModel::evaluate(
        rays,kOriginalC6D8,kOriginalC738);
    assert(unknown.unknown_origins==1U);
    assert(!unknown.source_replay_available());
    assert(!unknown.nearest);
    rays[6U].original_materializer_executed=true;
    rays[3U].depth=1U;
    const auto invalid_sentinel=OriginalGroundSourceProjectionModel::evaluate(
        rays,kOriginalC6D8,kOriginalC738);
    assert(!invalid_sentinel.nearest);

    rays[3U].depth=0U;
    assert(!OriginalGroundSourceProjectionModel::evaluate(
        rays,kOriginalC6D8-1U,kOriginalC738).nearest);
    assert(!OriginalGroundSourceProjectionModel::evaluate(
        {},kOriginalC6D8,kOriginalC738).nearest);
    // An actual wall depth 408 requires a source C738 table slot
    // 51. A fake zero-length C738 cannot authorize that lookup.
    for(auto& ray:rays) {
        ray.depth=3000U;
        ray.resource_inner=42U;
        ray.original_materializer_executed=true;
        ray.original_sentinel_initializer_executed=false;
    }
    rays[2U].depth=408U;
    const auto missing_lookup=OriginalGroundSourceProjectionModel::evaluate(
        rays,kOriginalC6D8,0U);
    assert(missing_lookup.nearest);
    assert(missing_lookup.nearest->nearest_depth==408);
    assert(!missing_lookup.ff50);
    assert(!missing_lookup.source_replay_available());

    // The projection must not mutate the original ray metadata or
    // the original host camera, 70Hz counters, RNG or indexed VGA.
    assert(rays[4U].resource_inner==0x93U);
    assert(rays[5U].depth==0xFFFFFFF9U);
}
