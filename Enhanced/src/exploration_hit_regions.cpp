#include "rloft/exploration_hit_regions.hpp"
namespace rloft { namespace {
constexpr std::array<ExplorationHitRect,ExplorationHitRegionModel::kRegionCount> kRegions{{
{4,32,259,287},{276,4,307,19},{276,21,307,35},{276,37,307,52},
{312,4,391,51},{276,57,307,72},{276,74,307,88},{276,90,307,105},
{312,57,391,104},{276,215,307,230},{276,232,307,246},{276,248,307,263},
{312,215,391,262},{276,268,307,283},{276,285,307,299},{276,301,307,316},
{312,268,391,315},{312,52,391,56},{312,263,391,267},{275,140,289,179},
{292,126,391,195}}};
}
const std::array<ExplorationHitRect,ExplorationHitRegionModel::kRegionCount>& ExplorationHitRegionModel::regions() noexcept{return kRegions;}
std::optional<int> ExplorationHitRegionModel::region_at(int row,int column) noexcept{
 for(std::size_t i=0;i<kRegions.size();++i)if(kRegions[i].contains(row,column))return static_cast<int>(i);
 return std::nullopt;
}
std::optional<std::size_t> ExplorationHitRegionModel::inventory_actor_at(int row,int column) noexcept{
 const auto r=region_at(row,column);if(!r)return std::nullopt;
 switch(*r){case 4:return 0U;case 8:return 1U;case 12:return 2U;case 16:return 3U;default:return std::nullopt;}
}
}  // namespace rloft
