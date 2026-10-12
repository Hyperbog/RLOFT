#pragma once

#include "rloft_native/native_world_payload.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace rloft_native {

// These bit masks are instruction-proven from original CODE.2.
// Unknown bit 0x2000 and all other original values remain unmodified.
constexpr std::uint16_t kCellCode = 0x003f;
constexpr std::uint16_t kCellCurrentVisibility = 0x0040;
constexpr std::uint16_t kCellSpecialCenter = 0x0080;
constexpr std::uint16_t kCellSpecialFrame = 0x0100;
constexpr std::uint16_t kCellExtraRenderPass = 0x0200;
constexpr std::uint16_t kCellExplored = 0x0400;
constexpr std::uint16_t kCellHiddenTopology = 0x0800;
constexpr std::uint16_t kCellStateResource = 0x1000;
constexpr std::uint16_t kCellLegacyAuthoring = 0x2000;
constexpr std::uint16_t kCellShade = 0xc000;

enum class NativeCollision { Passable, OrdinaryBlocked, SpecialBlocked };

// Original CELL_WORD/CELL_RECORDS evidence. No perspective, AI, interaction,
// unseen-geometry or original DOS runtime equivalence is asserted.
struct NativeSceneCell {
    unsigned level{};
    unsigned column{};
    unsigned row{};
    std::uint16_t word{};
    std::uint8_t code{};
    std::uint8_t shade_decrement{};
    bool transient_visible{};
    bool explored{};
    bool special_center{};
    bool special_frame{};
    bool extra_render_pass{};
    bool hidden_topology{};
    bool authoring_marker{};
    // -1 means no linked record / no known state offset.
    int resource_variant_offset{-1};
    int dynamic_position{-1};
    NativeCollision normal_collision{NativeCollision::Passable};
};

struct NativeRuntimeSnapshot {
    std::string source_fingerprint_sha256;
    NativeMutableWorld maps;
    std::vector<std::vector<NativeOriginalControl>> controls;
    unsigned active_level{};
};

// Keeps immutable source plus separate mutable per-level words, automap,
// original ten-byte controls. This is an engine-side simulation foundation,
// NOT an implementation of the game's original save-file schema.
class NativeWorldRuntime {
public:
    explicit NativeWorldRuntime(NativeWorldPayload original);

    [[nodiscard]] const NativeWorldPayload& original() const noexcept {
        return original_;
    }
    [[nodiscard]] const NativeMutableWorld& maps() const noexcept {
        return mutable_maps_;
    }
    [[nodiscard]] unsigned active_level() const noexcept {
        return active_level_;
    }
    void select_level(unsigned level);

    [[nodiscard]] NativeSceneCell cell(unsigned level, unsigned column,
                                       unsigned row) const;
    [[nodiscard]] NativeCollision normal_collision(unsigned level,
                                                    unsigned column,
                                                    unsigned row) const;

    // Matches CODE.2 cell.low_byte replacement of bits 0..5.
    void replace_cell_code(unsigned level, unsigned column,
                           unsigned row, std::uint8_t six_bit_code);
    // Matches original visibility OR 0x0440 / later map-wide AND ~0x0040.
    void mark_reached_by_view(unsigned level, unsigned column, unsigned row);
    void clear_transient_visibility(unsigned level);

    // Source-visible debug inputs only: no collision/placement/action
    // eligibility, event dispatcher or sound side effects are simulated.
    // The controller sets a kind-1 record in motion from an endpoint.
    void debug_begin_dynamic_transition(unsigned level,
                                        std::size_t control_index);
    // Exact proven group-2 +6/+7 stepping and endpoint normalization.
    [[nodiscard]] std::size_t advance_dynamic_animations(unsigned level);

    [[nodiscard]] NativeRuntimeSnapshot snapshot() const;
    void restore(const NativeRuntimeSnapshot& saved);

    // Inventory of original current-view-marked cells, in row-major order;
    // these are renderer INPUTS, not projected geometry or occlusion proof.
    [[nodiscard]] std::vector<NativeSceneCell> marked_scene_cells(
        unsigned level, std::size_t budget) const;

    // Metadata only: safe to put in automated regression artifacts.
    [[nodiscard]] std::string evidence_json() const;

private:
    NativeWorldPayload original_;
    NativeMutableWorld mutable_maps_;
    std::vector<std::vector<NativeOriginalControl>> mutable_controls_;
    unsigned active_level_{};

    [[nodiscard]] const NativeMutableLevel& map(unsigned level) const;
    [[nodiscard]] NativeMutableLevel& map(unsigned level);
    [[nodiscard]] std::size_t index(unsigned level, unsigned column,
                                    unsigned row) const;
    [[nodiscard]] std::string source_fingerprint() const;
};
} // namespace rloft_native
