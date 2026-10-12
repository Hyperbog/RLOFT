#pragma once

#include "rloft_native/native_type7_scene_shape.hpp"
#include "rloft_native/native_world_runtime.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace rloft_native {

// Immutable original compact placement translated through the exact
// NPC/INIT/OBJECTS source-class mapping, not a runtime sprite frame.
struct NativeAuthoredSceneInstance {
    unsigned level{};
    unsigned source_class{}; // 0=family A; 1=family B; 2=family C.
    std::size_t compact_record_index{};
    std::uint16_t original_type_id{};
    std::uint16_t runtime_type_id{};
    std::int32_t x_position_16_16{};
    std::int32_t y_position_16_16{};
    unsigned cell_column{};
    unsigned cell_row{};
    std::uint8_t compact_flags{};
    std::uint8_t compact_facing_0_to_7{};
    // Original CODE.2 shape source class + definition-local shape base.
    std::size_t absolute_res3_base{};
    // Current frame/view suffix is deliberately UNRESOLVED.
    bool actual_view_subframe_verified{false};
};

struct NativeAuthoredSceneDirectory {
    std::string original_compact_families_sha256;
    // Every resolved class/base/type/position is fingerprinted,
    // without serializing original source records to diagnostic output.
    std::string resolved_source_bases_sha256;
    std::vector<std::string> per_level_resolved_sha256;
    std::vector<std::vector<NativeAuthoredSceneInstance>> levels;
    std::size_t original_npc_instances{};
    std::size_t original_placed_instances{};
    std::size_t original_loose_instances{};
    std::size_t original_legacy_object_aliases{};
    bool original_instance_data_provenance_preserved{true};
    bool native_scene_frame_selection_verified{false};
};

// Reads 11-byte original compact family-A/B/C world records. Explicit
// original family-C alias 0x54->0x58 is applied only to the runtime type;
// the original record and original type ID are retained unchanged.
// Does not generate original live 22-byte actor structs, combat state,
// event VM, draw visibility or object animation.
[[nodiscard]] NativeAuthoredSceneDirectory
derive_native_authored_scene_instances(
    const NativeWorldPayload& world,
    const NativeSceneDefinitionDirectory& definitions);

// Consumes explicit current-view mark 0x0040 in the mutable runtime,
// returning a bounded *candidate* list. It does not infer the original
// multi-pass rendering order, occlusion or animated shape-frame IDs.
[[nodiscard]] std::vector<NativeAuthoredSceneInstance>
native_marked_scene_instance_candidates(
    const NativeWorldRuntime& runtime,
    const NativeAuthoredSceneDirectory& directory,
    unsigned level, std::size_t limit);

} // namespace rloft_native
