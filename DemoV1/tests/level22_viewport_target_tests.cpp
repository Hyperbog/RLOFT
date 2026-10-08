#include "rloft/level22_viewport_target.hpp"

#include "rloft/level22_story.hpp"

#include <cassert>
#include <optional>
#include <stdexcept>

int main() {
    rloft::LevelWorldState world{};
    world.npcs.resize(3U);
    world.npcs[0].npc_type_id=1U;
    world.npcs[1].npc_type_id=
        rloft::Level22Story::kAssassinNpcType;
    world.npcs[2].npc_type_id=2U;
    world.loose_items.resize(2U);

    // Render record index and native family record index are different.
    // Combat resolves the original source class 0 and source index 1.
    const std::optional<rloft::WorldSceneTarget> assassin(
        rloft::WorldSceneTarget{7U,0U,1U});
    assert(rloft::Level22ViewportTargetModel::assassin_index(
        assassin,world)==1U);
    assert(!rloft::Level22ViewportTargetModel::loose_item_index(
        assassin,world));

    const std::optional<rloft::WorldSceneTarget> npc(
        rloft::WorldSceneTarget{8U,0U,2U});
    assert(!rloft::Level22ViewportTargetModel::assassin_index(
        npc,world));

    const std::optional<rloft::WorldSceneTarget> relic(
        rloft::WorldSceneTarget{3U,2U,0U});
    assert(rloft::Level22ViewportTargetModel::loose_item_index(
        relic,world)==0U);
    assert(!rloft::Level22ViewportTargetModel::assassin_index(
        relic,world));

    const std::optional<rloft::WorldSceneTarget> placed(
        rloft::WorldSceneTarget{1U,1U,0U});
    assert(!rloft::Level22ViewportTargetModel::assassin_index(
        placed,world));
    assert(!rloft::Level22ViewportTargetModel::loose_item_index(
        placed,world));

    const std::optional<rloft::WorldSceneTarget> noninteractive(
        rloft::WorldSceneTarget{2U,3U,0U});
    assert(!rloft::Level22ViewportTargetModel::assassin_index(
        noninteractive,world));
    assert(!rloft::Level22ViewportTargetModel::loose_item_index(
        noninteractive,world));
    assert(!rloft::Level22ViewportTargetModel::assassin_index(
        std::nullopt,world));
    assert(!rloft::Level22ViewportTargetModel::loose_item_index(
        std::nullopt,world));

    // A cached viewport record can become stale after an Assassin death
    // or item removal. Fail explicitly instead of attacking/picking
    // another record through a silently clamped source index.
    const std::optional<rloft::WorldSceneTarget> stale_npc(
        rloft::WorldSceneTarget{0U,0U,99U});
    bool rejected=false;
    try {
        (void)rloft::Level22ViewportTargetModel::assassin_index(
            stale_npc,world);
    } catch(const std::out_of_range&) {rejected=true;}
    assert(rejected);

    const std::optional<rloft::WorldSceneTarget> stale_item(
        rloft::WorldSceneTarget{0U,2U,99U});
    rejected=false;
    try {
        (void)rloft::Level22ViewportTargetModel::loose_item_index(
            stale_item,world);
    } catch(const std::out_of_range&) {rejected=true;}
    assert(rejected);

    // A genuinely valid live index never depends on record_index.
    const std::optional<rloft::WorldSceneTarget> second_item(
        rloft::WorldSceneTarget{65535U,2U,1U});
    assert(rloft::Level22ViewportTargetModel::loose_item_index(
        second_item,world)==1U);
}
