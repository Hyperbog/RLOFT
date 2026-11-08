#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_ground_nearest_22220.hpp"

#include <cassert>
#include <vector>

int main() {
    using namespace rloft;
    using Model=OriginalGroundNearest22220SourceModel;
    using Ray=OriginalGroundMaterializedRay;
    // Original supplied SINTAB C6D8: 256 source rays.
    constexpr std::size_t count=256U;
    std::vector<Ray> rays(count,Ray{3000,42U,true});
    const auto untouched=Model::replay(rays,count);
    assert(untouched && untouched->nearest_depth==0x800);
    assert(untouched->sentinel_zero_resets==0U);
    assert(untouched->signed_min_updates==0U);
    assert(untouched->materialized_ray_count==count);

    rays[31U].depth_at_15a18=256;
    rays[43U].depth_at_15a18=512;
    const auto wall=Model::replay(rays,count);
    assert(wall && wall->nearest_depth==256);
    assert(wall->signed_min_updates==1U);

    // Original CODE.2 32F4C produces a *source-owned* ray with
    // depth=0 and main 0x93, but deliberately does not mutate 22220.
    rays[100U]=Ray{0,0x93U,false,true};
    const auto filler=Model::replay(rays,count);
    assert(filler && filler->nearest_depth==256);
    assert(filler->sentinel_zero_resets==0U);
    assert(filler->signed_min_updates==1U);
    assert(filler->materialized_ray_count==count-1U);
    assert(filler->source_sentinel_fill_count==1U);
    // Unknown or contradictory source flags fail closed, rather than
    // silently treating a native fixture as original DOS ray output.
    rays[100U].original_materializer_executed=true;
    assert(!Model::replay(rays,count));
    rays[100U].original_materializer_executed=false;
    rays[100U].depth_at_15a18=1;
    assert(!Model::replay(rays,count));
    rays[100U].depth_at_15a18=0;
    rays[100U].main_resource_at_15a20=42U;
    assert(!Model::replay(rays,count));
    rays[100U].main_resource_at_15a20=0x93U;

    // A materialized source 0x93 record RESETs global 22220 to 0.
    rays[100U]=Ray{700,0x93U,true};
    const auto empty=Model::replay(rays,count);
    assert(empty && empty->nearest_depth==0);
    assert(empty->sentinel_zero_resets==1U);
    assert(empty->signed_min_updates==1U);
    assert(empty->source_sentinel_fill_count==0U);

    // Signed original JGE: negative depth remains a negative min.
    rays[101U]=Ray{-7,42U,true};
    const auto negative=Model::replay(rays,count);
    assert(negative && negative->nearest_depth==-7);
    assert(negative->signed_min_updates==2U);

    // A native traversal placeholder is not independent evidence
    // of one of the four DOS materializer calls.
    rays[0U].original_materializer_executed=false;
    assert(!Model::replay(rays,count));
    rays[0U].original_materializer_executed=true;
    assert(!Model::replay(rays,count-1U));
    assert(!Model::replay(rays,0U));
    assert(!Model::replay({},count));
}
