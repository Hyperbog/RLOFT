#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_entity_collision.hpp"
#include "rloft/exploration_locomotion.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {
void word(std::vector<std::uint8_t>& b,std::size_t at,std::uint16_t v) {
    b.at(at)=static_cast<std::uint8_t>(v);
    b.at(at+1)=static_cast<std::uint8_t>(v>>8U);
}
rloft::NpcCatalog npcs() {
    std::vector<std::uint8_t> bytes(
        4U+10U*rloft::NpcDefinition::kSerializedSize,0U);
    bytes[0]=10U;
    word(bytes,4U+9U*rloft::NpcDefinition::kSerializedSize+0x16U,0x2500U);
    return rloft::NpcCatalog::parse(bytes);
}
rloft::PlacedSceneCatalog trees() {
    constexpr std::size_t count=0x80U;
    std::vector<std::uint8_t> bytes(0x26U+7U*count,0U);
    word(bytes,0x24U,static_cast<std::uint16_t>(count));
    word(bytes,0x26U+7U*62U+2U,0x4800U);
    word(bytes,0x26U+7U*63U+2U,1U); // original 2CC88 skips <=1
    word(bytes,0x26U+7U*0x54U+2U,0x4800U);
    word(bytes,0x26U+7U*0x7AU+2U,0x4800U);
    return rloft::PlacedSceneCatalog::parse_init(bytes);
}
rloft::LiveObjectRecord object(
    std::uint16_t id,std::uint32_t x,std::uint32_t y) {
    rloft::LiveObjectRecord r{};
    r.object_type_id=id;
    r.x_position_16_16=x;
    r.y_position_16_16=y;
    return r;
}
}
int main() {
    using namespace rloft;
    const auto catalog=npcs();
    const auto placed=trees();
    LevelWorldState world{};
    constexpr std::uint32_t x=0x00028000U,y=0x00018000U;

    // 2C800: 0x2500 + 0x03E8 radius, inclusive, in live 16.16 A/B;
    // skip only runtime death flag byte +0x0B bit 0x02.
    LiveNpcRecord npc{};
    npc.npc_type_id=9U;
    npc.x_position_16_16=x;
    npc.y_position_16_16=y;
    world.npcs.push_back(npc);
    assert(OriginalEntityCollisionModel::party_contact(
        world,catalog,placed,22U,x,y)==OriginalEntityContact::Blocked);
    assert(OriginalEntityCollisionModel::party_contact(
        world,catalog,placed,22U,x+0x28E8U,y)==OriginalEntityContact::Blocked);
    assert(OriginalEntityCollisionModel::party_contact(
        world,catalog,placed,22U,x+0x28E9U,y)==OriginalEntityContact::Clear);
    world.npcs[0].runtime_flags=0x02000000U;
    assert(OriginalEntityCollisionModel::party_contact(
        world,catalog,placed,22U,x,y)==OriginalEntityContact::Clear);

    // 2CC88: original placed-object footprint comes from INIT class-1
    // record +2; a tree is solid even in an otherwise empty level cell.
    world.placed_objects.push_back(object(62U,x,y));
    assert(OriginalEntityCollisionModel::party_contact(
        world,catalog,placed,22U,x,y)==OriginalEntityContact::Blocked);
    assert(OriginalEntityCollisionModel::party_contact(
        world,catalog,placed,22U,x+0x4800U,y)==OriginalEntityContact::Blocked);
    assert(OriginalEntityCollisionModel::party_contact(
        world,catalog,placed,22U,x+0x4801U,y)==OriginalEntityContact::Clear);
    world.placed_objects.clear();
    world.placed_objects.push_back(object(63U,x,y));
    assert(OriginalEntityCollisionModel::party_contact(
        world,catalog,placed,22U,x,y)==OriginalEntityContact::Clear);
    world.placed_objects[0].object_type_id=0x54U;
    // At Level 22 even a colliding 54h object is not a transition:
    // original 2CD99 accepts only level 0x17 or 0x0A with state 0x40.
    assert(OriginalEntityCollisionModel::party_contact(
        world,catalog,placed,22U,x,y)==OriginalEntityContact::Clear);
    world.placed_objects[0].packed_runtime_state=0x40U;
    assert(OriginalEntityCollisionModel::party_contact(
        world,catalog,placed,22U,x,y)==OriginalEntityContact::Clear);
    assert(OriginalEntityCollisionModel::party_contact(
        world,catalog,placed,0x17U,x,y)==
        OriginalEntityContact::RequiresSideEffect);
    assert(OriginalEntityCollisionModel::party_contact(
        world,catalog,placed,0x0AU,x,y)==
        OriginalEntityContact::RequiresSideEffect);
    world.placed_objects[0].packed_runtime_state=0U;
    assert(OriginalEntityCollisionModel::party_contact(
        world,catalog,placed,0x17U,x,y)==OriginalEntityContact::Clear);
    world.placed_objects[0].object_type_id=0x7AU;
    assert(OriginalEntityCollisionModel::party_contact(
        world,catalog,placed,22U,x,y)==OriginalEntityContact::RequiresSideEffect);

    // The real host 2DBAC movement retry must use the same entity
    // probe as the world runtime, not only open cell words.
    ExplorationSessionState s{};
    s.level.width=4;
    s.level.height=4;
    s.level.cell_words.assign(16U,0U);
    s.coordinate_a=0x00018000U;
    s.coordinate_b=0x00018000U;
    ExplorationMotionStep move{};
    move.state.heading=270U;
    move.translation=ExplorationMotionVector{0x00010000U,0x00010000U};
    world.placed_objects.clear();
    world.placed_objects.push_back(object(
        62U,0x00028000U,0x00028000U));
    const auto probe=[&](std::uint32_t a,std::uint32_t b) {
        return OriginalEntityCollisionModel::party_contact(
            world,catalog,placed,22U,a,b);
    };
    const auto adjusted=ExplorationLocomotionModel::apply(
        s,move,probe);
    assert(adjusted.acceptance==LocomotionAcceptance::YOnly);
    assert(s.coordinate_a==0x00018000U);
    assert(s.coordinate_b==0x00028000U);
    // Special side effects are NOT silently played as normal walking.
    s.coordinate_a=0x00018000U;
    s.coordinate_b=0x00018000U;
    world.placed_objects[0].object_type_id=0x7AU;
    const auto special=ExplorationLocomotionModel::apply(
        s,move,probe);
    assert(special.suppressed_side_effect);
    assert(s.coordinate_a==0x00018000U);
}
