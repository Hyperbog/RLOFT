#include "rloft/exploration_hit_regions.hpp"
#include <cassert>
int main(){
 const auto& r=rloft::ExplorationHitRegionModel::regions();assert(r.size()==21);
 assert(rloft::ExplorationHitRegionModel::region_at(4,32).value()==0);
 assert(rloft::ExplorationHitRegionModel::region_at(259,287).value()==0);
 assert(rloft::ExplorationHitRegionModel::inventory_actor_at(312,4).value()==0);
 assert(rloft::ExplorationHitRegionModel::inventory_actor_at(391,104).value()==1);
 assert(rloft::ExplorationHitRegionModel::inventory_actor_at(350,220).value()==2);
 assert(rloft::ExplorationHitRegionModel::inventory_actor_at(390,315).value()==3);
 assert(!rloft::ExplorationHitRegionModel::inventory_actor_at(300,20).has_value());
 assert(!rloft::ExplorationHitRegionModel::region_at(399,319).has_value());
 assert(rloft::ExplorationHitRegionModel::region_at(350,54).value()==17);
 assert(rloft::ExplorationHitRegionModel::region_at(350,265).value()==18);
 assert(rloft::ExplorationHitRegionModel::region_at(280,150).value()==19);
 assert(rloft::ExplorationHitRegionModel::region_at(330,150).value()==20);
}
