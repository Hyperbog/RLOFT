#pragma once

#include "rloft_native/native_type7_scene_shape.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace rloft_native {

// Structurally and instruction-granularly validated original SINTAB block.
// The x86 bytes are original data only; executable memory is NEVER allocated.
struct NativeSintabProgram {
    std::array<std::uint16_t,129> patch_offsets{};
    std::array<std::uint16_t,129> scale_steps{};
    std::vector<std::uint8_t> original_x86_bytes;
    std::string original_block_sha256;
    std::string generated_code_sha256;
    std::uint32_t summed_scale_steps{};
    std::size_t instruction_count{};
    std::size_t source_reads{};
    std::size_t destination_writes{};
};
struct NativeSintabPrograms {
    std::array<std::uint16_t,509> original_runtime_lookup{};
    std::vector<NativeSintabProgram> blocks;
    std::string original_sintab_sha256;
    std::size_t total_payload_bytes{};
    std::size_t total_generated_code_bytes{};
    std::size_t total_read_opcodes{};
    std::size_t total_write_opcodes{};
    bool x86_executed{false};
    bool original_frame_parity_verified{false};
};

[[nodiscard]] NativeSintabPrograms decode_native_sintab_programs(
    const std::vector<std::uint8_t>& original_sintab);

// Original EH7 selector bytes are offsets into a 129-word SINTAB patch
// table, not literal framebuffer positions. A plan resolves those offsets
// to validated instruction boundaries without mutating or executing x86.
struct NativeEh7SpanPlan {
    unsigned scale_block{};
    unsigned source_column{};
    unsigned descriptor_index{};
    std::int16_t source_adjustment{};
    std::uint16_t entry_selector{};
    std::uint16_t stop_selector{};
    std::size_t entry_code_offset{};
    std::size_t stop_code_offset{};
    std::uint32_t summed_steps{};
    bool visual_frame_parity_verified{false};
};
[[nodiscard]] NativeEh7SpanPlan resolve_native_eh7_span(
    const NativeType7SceneShape& shape,
    const NativeSintabPrograms& tables,
    unsigned scale_index,unsigned column_index,unsigned descriptor_index);

// Four distinct 64 KiB VGA planes. Writes are addressed by *byte* within a
// single plane and combined only under the explicit original sequencer mask.
// No host graphics API or licensed source files are required.
class NativeIndexedVgaPlanes {
public:
    static constexpr std::size_t kPlaneSize=65536;
    static constexpr std::size_t kRowBytes=80;
    NativeIndexedVgaPlanes();
    void masked_write(std::size_t destination_byte,std::uint8_t mask,
                      std::uint8_t indexed_color);
    [[nodiscard]] std::uint8_t indexed_pixel(unsigned x,unsigned y,
                                             unsigned page=0) const;
    [[nodiscard]] std::string planes_sha256() const;
private:
    std::array<std::vector<std::uint8_t>,4> planes_;
};

struct NativeSafeRasterExecution {
    std::size_t executed_instructions{};
    std::size_t source_reads{};
    std::size_t masked_destination_writes{};
    std::size_t individual_plane_writes{};
    bool x86_executed{false};
    bool original_frame_parity_verified{false};
};

// Reimplements ONLY the six original 32-bit generated SINTAB opcodes that
// have been exhaustively seen on original blocks: 8A86, 88C4, 29C8,
// 8887, 88A7, C3. No DOS code is called. No EB patching is executed:
// the source descriptor's validated stop offset is an exclusive end.
// This is a narrowly bounded primitive, not the complete original
// 369D8/36B14 pipeline, geometry projection, shading or palette.
// Invalid source/destination/descriptor aborts before ANY plane mutation.
[[nodiscard]] NativeSafeRasterExecution draw_native_eh7_span(
    const NativeSintabPrograms& tables,
    const NativeEh7SpanPlan& plan,
    const std::vector<std::uint8_t>& original_decoded_eh7_body,
    std::int64_t source_base,
    std::uint32_t ecx_lookup,
    std::size_t destination_byte,
    std::uint8_t sequencer_mask,
    NativeIndexedVgaPlanes& target);

} // namespace rloft_native
