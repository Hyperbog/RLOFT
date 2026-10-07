#include "rloft/npc_scene.hpp"

#include "rloft/first_person_entity_projection.hpp"
#include "rloft/scene_shape_selector.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {
namespace {

std::uint16_t add_word(
    std::uint16_t value,
    std::uint32_t addend) noexcept {
    return static_cast<std::uint16_t>(
        static_cast<std::uint32_t>(value) + addend);
}

std::uint8_t definition_direction_byte(
    const NpcDefinition& definition,
    std::size_t index) {
    if (index >= 8U) {
        throw std::out_of_range(
            "NPC directional byte index");
    }
    const auto word =
        definition.directional_offset[index / 2U];
    return static_cast<std::uint8_t>(
        index % 2U == 0U
            ? word
            : word >> 8U);
}

std::uint16_t seeded_random15(
    std::size_t npc_index) noexcept {
    // CODE.2 30CC4(npc_index) followed by one 30CA2 call.
    const auto seed =
        static_cast<std::uint32_t>(npc_index) *
            0x41C64E6DU +
        0x00003039U;
    return static_cast<std::uint16_t>(
        (seed >> 16U) & 0x7FFFU);
}

std::uint16_t select_resource(
    const LiveNpcRecord& npc,
    const NpcDefinition& definition,
    std::size_t npc_index,
    std::uint16_t screen_x,
    const ExplorationSessionState& session,
    const RendererTables& tables,
    const NpcSceneModel::Random15& shared_random15) {
    auto resource = definition.local_shape_base;
    const auto packed = npc.packed_runtime_state;
    const auto state =
        static_cast<std::uint16_t>(
            (packed >> 1U) & 3U);
    const auto frame =
        static_cast<std::uint16_t>(
            (packed >> 6U) & 7U);
    const auto high_state =
        static_cast<std::uint16_t>(
            packed >> 14U);

    if (npc.npc_type_id == 8U) {
        if ((packed & 0xC000U) == 0xC000U) {
            resource = add_word(
                resource,
                static_cast<std::uint32_t>(
                    frame + 12U));
        } else {
            switch (state) {
                case 0:
                    resource =
                        add_word(resource, frame);
                    break;
                case 1:
                case 2:
                    resource = add_word(
                        resource,
                        static_cast<std::uint32_t>(
                            high_state * 2U +
                            (state - 1U) +
                            4U));
                    break;
                case 3: {
                    if (!shared_random15) {
                        throw std::logic_error(
                            "NPC type 8 state-3 scene frame requires shared game RNG");
                    }
                    const auto random_bit =
                        static_cast<std::uint16_t>(
                            (shared_random15() &
                             0x7FFFU) >>
                            14U);
                    resource = add_word(
                        resource,
                        static_cast<std::uint32_t>(
                            random_bit + 10U));
                    break;
                }
            }
        }
    } else if (state == 3U) {
        resource = add_word(
            resource,
            static_cast<std::uint32_t>(
                definition.animation_cycle_divisor) *
                definition.scene_frame_stride);

        const auto random_bit =
            static_cast<std::uint16_t>(
                seeded_random15(npc_index) >>
                14U);
        resource = add_word(
            resource,
            static_cast<std::uint32_t>(
                random_bit) *
                (static_cast<std::uint32_t>(
                     definition.animation_span) +
                 1U));
        resource = add_word(
            resource,
            definition.animation_span);
    } else if (state != 0U) {
        resource = add_word(
            resource,
            static_cast<std::uint32_t>(
                definition.animation_cycle_divisor) *
                definition.scene_frame_stride);
        resource = add_word(
            resource,
            (static_cast<std::uint32_t>(
                 definition.animation_span) +
             definition.damage_reaction_animation_flag) *
                static_cast<std::uint32_t>(
                    state - 1U));
    } else if (
        (npc.runtime_flags &
         0x08000000U) != 0U) {
        resource = add_word(
            resource,
            static_cast<std::uint32_t>(
                definition_direction_byte(
                    definition, frame)) *
                definition.scene_frame_stride);
    } else if (
        (npc.runtime_flags &
         0x04000000U) != 0U) {
        if (npc.npc_type_id == 0x51U) {
            resource = add_word(
                resource,
                static_cast<std::uint32_t>(
                    definition.animation_cycle_divisor) *
                    high_state +
                    frame);
        } else {
            resource = add_word(
                resource,
                static_cast<std::uint32_t>(
                    definition.scene_frame_stride) *
                    frame);
        }
    } else {
        resource = add_word(
            resource,
            static_cast<std::uint32_t>(
                definition.scene_frame_stride) *
                frame);
    }

    // State 3 bypasses 2B2F4 in the original builder.
    if (state != 3U) {
        resource =
            SceneShapeSelectorModel::select_bank_local(
                resource,
                screen_x,
                npc.packed_runtime_state,
                npc.npc_type_id,
                0U,
                definition.scene_frame_stride,
                session,
                tables);
    }
    return resource;
}

}  // namespace

std::vector<NpcSceneEmission>
NpcSceneModel::build(
    const LevelWorldState& world,
    const ExplorationSessionState& session,
    const NpcCatalog& catalog,
    const RendererTables& tables,
    const SceneSourceBanks& banks,
    std::uint8_t render_threshold,
    const Random15& shared_random15) {
    std::vector<NpcSceneEmission> out;
    out.reserve(
        world.npcs.size() < kMaxSceneRecords
            ? world.npcs.size()
            : kMaxSceneRecords);

    for (std::size_t npc_index = 0;
         npc_index < world.npcs.size();
         ++npc_index) {
        const auto& npc = world.npcs[npc_index];

        // CODE.2 tests live byte +0x0B bit 0x02, i.e. dword bit 25.
        if ((npc.runtime_flags &
             0x02000000U) != 0U) {
            continue;
        }

        const auto cell_index =
            static_cast<std::size_t>(
                npc.cached_cell_index);
        if (cell_index >=
            session.level.cell_words.size()) {
            throw std::out_of_range(
                "NPC cached cell leaves level");
        }
        if ((session.level.cell_words[cell_index] &
             0x0040U) == 0U) {
            continue;
        }

        const auto& definition =
            catalog.at(npc.npc_type_id);

        if (definition.scene_influence_step != 0U) {
            // CODE.2 calls 2B088 here after successful projection. That
            // side effect is distinct from scene-record construction and is
            // not silently omitted.
            throw std::logic_error(
                "NPC scene influence path is not integrated");
        }

        const auto projection =
            FirstPersonEntityProjectionModel::project(
                npc.y_position_16_16,
                npc.x_position_16_16,
                session,
                tables);
        if (!projection.visible ||
            projection.projected_extent == 0U ||
            projection.projected_extent <=
                render_threshold) {
            continue;
        }

        // Valid shipped scenes stay below the encoded 0x190 capacity.
        // Prevent a malformed/custom world from reproducing the original
        // one-record-overrun into the adjacent scene buffer.
        if (out.size() >= kMaxSceneRecords) {
            break;
        }

        NpcSceneEmission emission{};
        emission.screen_x =
            projection.screen_x;
        emission.projected_extent =
            projection.projected_extent;
        emission.resource_id =
            static_cast<std::uint16_t>(
                banks.at(0U) +
                select_resource(
                    npc,
                    definition,
                    npc_index,
                    projection.screen_x,
                    session,
                    tables,
                    shared_random15));
        emission.source_index =
            static_cast<std::uint16_t>(
                npc_index);
        emission.source_class = 0U;
        emission.render_scale_lookup_bias =
            definition.render_scale_lookup_bias;
        out.push_back(emission);
    }

    return out;
}

}  // namespace rloft
