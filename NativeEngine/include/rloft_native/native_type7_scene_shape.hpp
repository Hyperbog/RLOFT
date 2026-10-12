#pragma once

#include "rloft_native/native_assets.hpp"
#include "rloft_native/native_media.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace rloft_native {

// Original CODE.2 36CEC/369D8: these are offsets into *scale-specific
// SINTAB-generated x86 rasterizer tables*, NOT RGB spans or bitmap pixels.
// Keep signed adjustment and both selector offsets unchanged.
struct NativeType7RasterSpan {
    std::uint16_t stop_selector{};
    std::int16_t signed_adjustment{};
    std::uint16_t entry_selector{};
};

struct NativeType7Column {
    std::uint16_t original_list_offset{};
    std::vector<NativeType7RasterSpan> descriptors;
};

struct NativeType7SceneShape {
    std::uint16_t source_width{};
    std::int16_t x_min{};
    std::int16_t x_max{};
    std::int16_t y_min{};
    std::int16_t y_max{};
    std::vector<NativeType7Column> columns;
    std::size_t unique_lists{};
    std::size_t total_descriptors{};
    std::size_t decoded_body_bytes{};
    std::string original_entry_sha256;
    std::string decoded_body_sha256;
    // True means structurally validated, never original visual parity.
    bool structure_verified{};
};

// Decodes EH7 original LZSS 4KiB into a complete *read-only* display list.
// All column offsets are relative to the decompressed body; offsets may be
// odd, may be shared and may refer to empty zero-terminated lists.
// No original DOS x86 machine code is interpreted, patched or executed.
[[nodiscard]] NativeType7SceneShape decode_native_type7_scene_shape(
    const std::vector<std::uint8_t>& res_entry);

// Scene source class 0=NPCS, 1=INIT family-B, 2=OBJECTS, 3=INIT transient.
// CODE.2 looks up each source-class base from original INIT +0x06..+0x0C.
struct NativeSceneSourceDescriptor {
    std::uint16_t shape_base_local{};
    std::uint16_t extent_raw{};
    std::uint8_t shape_stride{};
    std::uint8_t variant_count{};
    std::uint8_t scene_influence_step{};
};

struct NativeRes3SceneBanks {
    std::array<std::uint16_t,4> starts{};
    std::array<std::size_t,4> lengths{};
    std::array<std::vector<NativeSceneSourceDescriptor>,2> init_descriptors;
    std::size_t res3_entries{};

    [[nodiscard]] std::size_t absolute_id(unsigned source_class,
                                           std::size_t local_shape_id) const;
    // Validates source class + local index with explicit source boundaries.
    [[nodiscard]] bool contains(unsigned source_class,
                                std::size_t local_shape_id) const noexcept;
};

// Bounded INIT/RES3 index validation; requires only the original INIT
// header+descriptor tables and RES3 entry count (not original x86).
[[nodiscard]] NativeRes3SceneBanks decode_native_res3_scene_banks(
    const std::vector<std::uint8_t>& original_init,
    std::size_t res3_entry_count);

// Exact original NPCS/OBJECTS type-definition shape namespaces.
// NPCS record size 0x45, local base +0x14; OBJECTS record size 0x41,
// local base +0x28, optional stride +0x2A. Preserve type-index identity,
// but leave live animation/view-subframe selection to later CODE.2 work.
struct NativeNpcShapeBase {
    std::uint16_t local_shape_base{};
    std::size_t absolute_shape_base{};
    // Original NPCS.DAT definition bytes +0x18..+0x1A, used by
    // original CODE.2 2B77A..2BA4F animation/orientation logic.
    // Values retained as source metadata, not assumed state*stride.
    std::uint8_t animation_stride{};
    std::uint8_t cycle_divisor{};
    std::uint8_t animation_span{};
};
struct NativeObjectShapeBase {
    std::uint16_t local_shape_base{};
    std::uint8_t shape_stride{};
    std::size_t absolute_shape_base{};
};
struct NativeSceneDefinitionDirectory {
    NativeRes3SceneBanks banks;
    std::vector<NativeNpcShapeBase> npc_definitions;
    std::vector<NativeObjectShapeBase> object_definitions;
    std::size_t max_npc_local_base{};
    std::size_t max_object_allocation_end{};

    [[nodiscard]] std::size_t npc_shape_base(unsigned type_id) const;
    [[nodiscard]] std::size_t object_shape_base(unsigned type_id) const;
    [[nodiscard]] NativeSceneSourceDescriptor init_scene_descriptor(
        unsigned source_class, unsigned type_id) const;
    // An explicit input variant only. Does not silently assign facing or
    // original perspective subframes; caller must have source proof.
    [[nodiscard]] std::size_t init_base_variant(unsigned source_class,
                                                unsigned type_id,
                                                unsigned variant_index) const;
};

[[nodiscard]] NativeSceneDefinitionDirectory decode_native_scene_definitions(
    const NativeRes3SceneBanks& banks,
    const std::vector<std::uint8_t>& npc_dat,
    const std::vector<std::uint8_t>& objects_dat);

struct NativeRes3ShapeFingerprint {
    std::size_t id{};
    std::string eh7_entry_sha256;
    std::string eh7_body_sha256;
    std::size_t decoded_body_bytes{};
    unsigned source_width{};
    std::size_t source_columns{};
    std::size_t unique_column_lists{};
    std::size_t raster_descriptors_with_column_references{};
    std::size_t raster_descriptors_unique_lists{};
};

struct NativeRes3SceneCorpus {
    NativeRes3SceneBanks banks;
    std::size_t type7_shapes{};
    std::size_t unique_lists{};
    std::size_t raster_descriptors{};
    std::size_t decoded_body_total_bytes{};
    std::vector<NativeRes3ShapeFingerprint> shapes;
    std::array<std::size_t,3> width_histogram{};
    std::array<std::size_t,4> source_class_shapes{};
    // All 4234 original records are parsed, or rejects the entire audit.
    bool original_sintab_raster_execution_verified{false};
    bool original_scene_visual_parity_verified{false};
};

// Full read-only RES3 validation using the *same* native typed decoder,
// constrained to the original four INIT-defined source-bank ranges.
[[nodiscard]] NativeRes3SceneCorpus audit_native_res3_scene_corpus(
    const std::vector<std::uint8_t>& init,
    const NativeResArchive& res3);

[[nodiscard]] std::string native_res3_scene_corpus_json(
    const NativeRes3SceneCorpus& corpus);

} // namespace rloft_native
