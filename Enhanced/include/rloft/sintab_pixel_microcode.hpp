#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "rloft/renderer_tables.hpp"
#include "rloft/vga_planar_memory.hpp"

namespace rloft {

// Only the six instruction forms present in all 127 original SINTAB
// generated code bodies are accepted; the run-time patcher additionally
// introduces RET (C3) and short JMP (EB xx) at verified code boundaries.
struct SintabOpcodeAudit final {
    std::size_t source_reads{};
    std::size_t mov_ah_al{};
    std::size_t sub_eax_ecx{};
    std::size_t destination_writes{};
    std::size_t returns{};
    std::size_t instruction_count{};
};

struct SintabPixelExecution final {
    std::size_t source_reads{};
    std::size_t destination_writes{};
    std::size_t instructions{};
};

// A deterministic interpreter for the original data-file rasterizer,
// not an x86 execution engine or a row-major scaling approximation.
// SOURCE_BASE is ESI relative to the start of the original decoded RES3
// type-7 body; DESTINATION is EDI as a physical VGA A0000 address.
class SintabPixelMicrocode final {
public:
    [[nodiscard]] static SintabOpcodeAudit audit(
        const RendererScaleBlock& block);

    // CODE is a private copy of rasterizer_x86 potentially modified by
    // 36C10/36C44 and 369D8's temporary RET. Never execute untrusted
    // arbitrary x86: only the exact six original forms plus EB/C3.
    [[nodiscard]] static SintabPixelExecution execute(
        const std::vector<std::uint8_t>& code,
        std::size_t entry_code_offset,
        const std::vector<std::uint8_t>& source,
        std::int64_t source_base,
        std::uint32_t ecx_lookup,
        std::uint32_t destination_physical_address,
        std::uint8_t sequencer_mask,
        VgaPlanarMemory& video);
};

} // namespace rloft
