#include "rloft/sintab_pixel_microcode.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace rloft {
namespace {

std::uint32_t immediate(const std::vector<std::uint8_t>& code,
                        std::size_t at) {
    if (at > code.size() || code.size() - at < 4U) {
        throw std::runtime_error("truncated SINTAB 32-bit displacement");
    }
    return static_cast<std::uint32_t>(code[at]) |
           (static_cast<std::uint32_t>(code[at+1U]) << 8U) |
           (static_cast<std::uint32_t>(code[at+2U]) << 16U) |
           (static_cast<std::uint32_t>(code[at+3U]) << 24U);
}

bool pair_at(const std::vector<std::uint8_t>& code,
             std::size_t pc,
             std::uint8_t b0,
             std::uint8_t b1) noexcept {
    return pc < code.size() &&
           code.size() - pc >= 2U &&
           code[pc] == b0 &&
           code[pc+1U] == b1;
}

} // namespace

SintabOpcodeAudit SintabPixelMicrocode::audit(
    const RendererScaleBlock& block) {
    const auto& code = block.rasterizer_x86;
    SintabOpcodeAudit out{};
    std::vector<bool> boundaries(code.size()+1U, false);
    std::size_t pc=0U;
    boundaries[0U]=true;
    while(pc<code.size()) {
        if (pair_at(code,pc,0x8AU,0x86U)) {
            (void)immediate(code,pc+2U);
            pc+=6U;
            ++out.source_reads;
        } else if (pair_at(code,pc,0x88U,0xC4U)) {
            pc+=2U;
            ++out.mov_ah_al;
        } else if (pair_at(code,pc,0x29U,0xC8U)) {
            pc+=2U;
            ++out.sub_eax_ecx;
        } else if (pair_at(code,pc,0x88U,0x87U) ||
                   pair_at(code,pc,0x88U,0xA7U)) {
            const auto offset=immediate(code,pc+2U);
            if (offset%VgaPlanarMemory::kRowBytes != 0U) {
                throw std::runtime_error(
                    "SINTAB pixel store is not on the original 80-byte row grid");
            }
            pc+=6U;
            ++out.destination_writes;
        } else if (code[pc] == 0xC3U) {
            ++pc;
            ++out.returns;
            if (pc!=code.size()) {
                throw std::runtime_error(
                    "SINTAB source contains a nonterminal RET");
            }
        } else {
            throw std::runtime_error(
                "unknown instruction in original SINTAB generated code");
        }
        boundaries[pc]=true;
        ++out.instruction_count;
    }
    if (out.returns!=1U ||
        out.source_reads!=out.mov_ah_al ||
        out.source_reads!=out.sub_eax_ecx) {
        throw std::runtime_error(
            "SINTAB rasterizer read/adjust/return structure is inconsistent");
    }

    for (auto physical : block.code_patch_offset) {
        if (physical<0x204U ||
            static_cast<std::size_t>(physical-0x204U)>=code.size() ||
            !boundaries[physical-0x204U]) {
            throw std::runtime_error(
                "SINTAB code patch offset is not an instruction boundary");
        }
    }
    return out;
}

SintabPixelExecution SintabPixelMicrocode::execute(
    const std::vector<std::uint8_t>& code,
    std::size_t entry_code_offset,
    const std::vector<std::uint8_t>& source,
    std::int64_t source_base,
    std::uint32_t ecx_lookup,
    std::uint32_t destination_physical_address,
    std::uint8_t sequencer_mask,
    VgaPlanarMemory& video) {
    if (entry_code_offset>=code.size()) {
        throw std::out_of_range(
            "SINTAB generated-code entry outside scale block");
    }
    SintabPixelExecution out{};
    std::uint32_t eax=0U;
    std::size_t pc=entry_code_offset;
    // Original SINTAB bodies are straight-line. Patched short jumps only
    // move forward in validated shipped blocks. A finite bound rejects
    // corrupt/backward jumps rather than hanging the demo.
    const auto limit=code.size()*2U+1U;
    while (out.instructions<limit) {
        ++out.instructions;
        if(pc>=code.size()) {
            throw std::runtime_error(
                "SINTAB generated-code execution ran off its body");
        }
        if(code[pc]==0xC3U) {
            return out;
        }
        if(code[pc]==0xEBU) {
            if(pc+1U>=code.size()) {
                throw std::runtime_error(
                    "truncated SINTAB patched short jump");
            }
            const auto displacement=static_cast<std::int8_t>(
                code[pc+1U]);
            const auto next=static_cast<std::int64_t>(pc)+2+
                static_cast<std::int64_t>(displacement);
            if(next<0 || static_cast<std::uint64_t>(next)>=code.size()) {
                throw std::out_of_range(
                    "SINTAB patched jump exits generated code");
            }
            pc=static_cast<std::size_t>(next);
            continue;
        }
        if(pair_at(code,pc,0x8AU,0x86U)) {
            const auto displacement=immediate(code,pc+2U);
            const auto source_at=source_base+
                static_cast<std::int64_t>(
                    static_cast<std::int32_t>(displacement));
            if (source_at<0 ||
                static_cast<std::uint64_t>(source_at)>=source.size()) {
                throw std::out_of_range(
                    "SINTAB generated code reads beyond decoded RES3 body");
            }
            eax=(eax&0xFFFFFF00U) |
                source[static_cast<std::size_t>(source_at)];
            ++out.source_reads;
            pc+=6U;
            continue;
        }
        if(pair_at(code,pc,0x88U,0xC4U)) {
            const auto al=eax&0xFFU;
            eax=(eax&0xFFFF00FFU) | (al<<8U);
            pc+=2U;
            continue;
        }
        if(pair_at(code,pc,0x29U,0xC8U)) {
            eax-=ecx_lookup;
            pc+=2U;
            continue;
        }
        if(pair_at(code,pc,0x88U,0x87U) ||
           pair_at(code,pc,0x88U,0xA7U)) {
            const auto offset=immediate(code,pc+2U);
            const auto value=code[pc+1U]==0x87U
                ? static_cast<std::uint8_t>(eax&0xFFU)
                : static_cast<std::uint8_t>((eax>>8U)&0xFFU);
            video.write(
                destination_physical_address+offset,
                sequencer_mask,
                value);
            ++out.destination_writes;
            pc+=6U;
            continue;
        }
        throw std::runtime_error(
            "unsupported instruction during SINTAB pixel execution");
    }
    throw std::runtime_error(
        "SINTAB generated-code instruction count exceeded guard");
}

} // namespace rloft
