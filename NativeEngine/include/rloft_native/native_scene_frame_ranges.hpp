#pragma once

#include "rloft_native/native_scene_instances.hpp"
#include "rloft_native/native_type7_scene_shape.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace rloft_native {

// NativeEngine authoring-to-draw preparation, NOT a guessed live frame.
// Every candidate is tied to one original compact 11-byte placement and a
// source-owned RES3 class; unproved camera/animation decisions remain absent.
struct NativeAuthoredFrameRange {
    unsigned level{};
    unsigned source_class{};
    std::size_t compact_record_index{};
    std::uint16_t original_type_id{};
    std::uint16_t runtime_type_id{};
    std::size_t first_absolute_res3_id{};
    std::size_t resource_slot_count{};
    std::uint8_t state_variant_count{};
    std::uint8_t source_shape_stride{};
    // Original NPCS definition bytes +18, +19, +1A, as-is.
    std::uint8_t npc_animation_stride{};
    std::uint8_t npc_cycle_divisor{};
    std::uint8_t npc_animation_span{};
    bool view_subframe_selected{false};
    bool dos_draw_order_verified{false};
};

struct NativeAuthoredFrameRangeDirectory {
    std::vector<std::vector<NativeAuthoredFrameRange>> levels;
    std::string source_resolved_instances_sha256;
    std::string frame_ranges_sha256;
    std::vector<std::string> per_level_frame_ranges_sha256;
    std::size_t instances{};
    std::size_t bounded_resource_slots{};
    std::size_t npc_source_metadata_records{};
    std::size_t init_state_subranges{};
    std::size_t loose_item_subranges{};
    bool dos_view_subframe_parity_verified{false};
    bool dos_depth_sort_parity_verified{false};
};

// All authored source placements: establish possible original frame
// allocations without inventing a missing 22-byte LIVE world record.
// Class 0: only its authenticated base is currently selectable;
//           NPC animation policy still requires CODE.2 live state.
// Class 1: INIT local base + explicitly selected variant*stride +
//           caller-chosen subframe; class1 allocation owns all slots.
// Class 2: source OBJECTS local base + caller-chosen subframe < stride.
//           No unproved 3-bit *multiple stride* is inferred.
// Class 3: not archive-authored; a later transient event API must
//           provide original live effect instance state.
[[nodiscard]] NativeAuthoredFrameRangeDirectory
derive_native_authored_frame_ranges(
    const NativeAuthoredSceneDirectory& authored,
    const NativeSceneDefinitionDirectory& definitions);

// Selection is legal only inside the authenticated allocation.
// This verifies legal RES3 ID formation, NOT how original gameplay
// chooses a state variant or view-dependent subframe.
[[nodiscard]] std::size_t select_native_authored_res3_frame(
    const NativeAuthoredFrameRange& range,
    const NativeRes3SceneBanks& banks,
    unsigned state_variant, unsigned view_subframe);

// Source-authenticated *base EH7* inspection for the native first-level
// session. The resolved frame is always the first legal slot (0,0).
// Crucially, it is NOT necessarily the source runtime's current view,
// animation frame, source plane shade or draw order.
struct NativeAuthoredBaseShapeEvidence {
    std::size_t absolute_res3_id{};
    unsigned source_width{};
    std::int16_t x_min{},x_max{},y_min{},y_max{};
    std::size_t raster_descriptors{};
    std::size_t decoded_body_bytes{};
    std::string original_entry_sha256;
    std::string original_body_sha256;
    bool live_view_subframe_verified{false};
};

[[nodiscard]] NativeAuthoredBaseShapeEvidence
probe_native_authored_base_eh7(
    const NativeAuthoredFrameRange& source_range,
    const NativeRes3SceneBanks& banks,
    const NativeResArchive& original_res3);

// Metadata-only, original-free ledger used by the independent oracle.
[[nodiscard]] std::string native_authored_frame_ranges_json(
    const NativeAuthoredFrameRangeDirectory& directory);

} // namespace rloft_native
