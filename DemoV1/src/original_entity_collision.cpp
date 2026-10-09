#include "rloft/original_entity_collision.hpp"

#include <cstdint>

namespace rloft {
namespace {

// Original CODE.2 2C800/2CC88 use unsigned JB/JBE with 32-bit
// wraparound arithmetic for their inclusive axis-aligned overlap.
bool source_overlap(
    std::uint32_t source_a, std::uint32_t source_b,
    std::uint32_t target_a, std::uint32_t target_b,
    std::uint32_t half_extent) noexcept {
    return target_a >= source_a - half_extent &&
           target_a <= source_a + half_extent &&
           target_b >= source_b - half_extent &&
           target_b <= source_b + half_extent;
}
} // namespace

OriginalEntityContact OriginalEntityCollisionModel::party_contact(
    const LevelWorldState& world,
    const NpcCatalog& npcs,
    const PlacedSceneCatalog& placed,
    std::uint32_t current_level_id,
    std::uint32_t coordinate_a,
    std::uint32_t coordinate_b) {
    // 2C800: skip dead/inactive +0x0B bit 0x02; party index -1
    // excludes no otherwise-live NPC. The footprint is original
    // NPCS.DAT +0x16 plus an exact 0x03E8 signed-unit padding.
    for (const auto& npc:world.npcs) {
        if ((npc.runtime_flags & 0x02000000U) != 0U) continue;
        const auto& definition=npcs.at(npc.npc_type_id);
        const auto extent=static_cast<std::uint32_t>(
            definition.collision_half_extent)+0x03E8U;
        if (source_overlap(
                npc.x_position_16_16,npc.y_position_16_16,
                coordinate_a,coordinate_b,extent)) {
            return OriginalEntityContact::Blocked;
        }
    }

    // 2CC88: INIT class-1 descriptor +2 gives the source-authored
    // overlap half-extent. Zero and one are nonblocking. On overlap
    // ordinary class-1 types return one (collision blocked) in
    // party mode -1, but special 0x54/0x7A objects enter authored
    // transition/action branches; never silently treat those as an
    // ordinary immovable tree without their event conditions.
    for (const auto& object:world.placed_objects) {
        const auto& definition=placed.at(object.object_type_id);
        const auto extent=static_cast<std::uint32_t>(
            definition.spatial_extent);
        if (extent <= 1U) continue;
        if (source_overlap(
                object.x_position_16_16,object.y_position_16_16,
                coordinate_a,coordinate_b,extent)) {
            if (object.object_type_id == 0x54U) {
                // Original CODE.2 2CD76..2CE39: type 54h is an
                // authored transition only when the live packed state
                // has bits 1C0h == 040h AND C6C8 is 0x17 or 0x0A.
                // Otherwise the original loops to the NEXT object;
                // it does not return an ordinary block or event.
                if ((object.packed_runtime_state & 0x01C0U)!=0x0040U ||
                    (current_level_id!=0x17U &&
                     current_level_id!=0x0AU)) {
                    continue;
                }
                return OriginalEntityContact::RequiresSideEffect;
            }
            if (object.object_type_id == 0x7AU) {
                // Original CODE.2 2CE3B..2CE57 executes 3CAD4.
                return OriginalEntityContact::RequiresSideEffect;
            }
            return OriginalEntityContact::Blocked;
        }
    }
    return OriginalEntityContact::Clear;
}

} // namespace rloft
