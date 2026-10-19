#pragma once
// Bounded portable interpreter for the exact instruction forms used by the
// original SINTAB generated scale blocks; no host execution of DOS x86 bytes.
// Caller must supply an authenticated SINTAB arena and mapped RES4 texture.
#include <cstddef>
#include <cstdint>

namespace rloft_recomp {
enum class SintabRasterFailure : std::uint8_t {
    none, invalid_entry, invalid_ip, truncated_instruction,
    invalid_jump, source_read_out_of_range, vga_write_out_of_range,
    unknown_instruction, instruction_budget
};
struct SintabRasterResult {
    bool ok=false;
    std::uint32_t pixels=0;
    std::uint32_t instructions=0;
    SintabRasterFailure failure=SintabRasterFailure::none;
    std::uint32_t failure_ip=0;
    std::uint64_t failure_address=0;
    std::uint8_t failure_opcode=0;
    std::uint8_t failure_modrm=0;
};
template<class Read, class Write>
SintabRasterResult execute_sintab_scale(
    std::uint32_t target,std::uint32_t source,std::uint32_t destination,
    std::uint32_t subtract_pair,std::uint32_t code_begin,
    std::uint32_t code_end,std::uint32_t source_begin,
    std::uint32_t source_end,Read read,Write write) noexcept {
    SintabRasterResult result{};
    // Source is the ESI base, not necessarily a valid dereference.
    // Generated x86 loads [ESI+disp32]; validate those effective
    // read addresses individually, not ESI itself.
    if(target<code_begin || target>=code_end ||
       source_begin>=source_end) {
        result.failure=SintabRasterFailure::invalid_entry;
        result.failure_ip=target;
        return result;
    }
    std::uint8_t al=0,ah=0;
    std::uint32_t ip=target;
    for(std::uint32_t budget=0;budget<32768u;++budget) {
        result.failure_ip=ip;
        if(ip>=code_end || ip<code_begin) {
            result.failure=SintabRasterFailure::invalid_ip;
            return result;
        }
        const std::uint8_t opcode=read(ip);
        result.failure_opcode=opcode;
        result.failure_modrm=0u;
        ++result.instructions;
        if(opcode==0xC3u) {
            result.ok=true;
            result.failure=SintabRasterFailure::none;
            return result;
        }
        if(opcode==0xEBu) {
            if(ip+2u>code_end) {
                result.failure=SintabRasterFailure::truncated_instruction;
                return result;
            }
            const std::int32_t delta=static_cast<std::int8_t>(read(ip+1u));
            const std::int64_t next=static_cast<std::int64_t>(ip)+2+delta;
            if(next<code_begin || next>=code_end) {
                result.failure=SintabRasterFailure::invalid_jump;
                result.failure_address=static_cast<std::uint64_t>(next);
                return result;
            }
            ip=static_cast<std::uint32_t>(next);
            continue;
        }
        if(ip+2u>code_end) {
            result.failure=SintabRasterFailure::truncated_instruction;
            return result;
        }
        const std::uint8_t modrm=read(ip+1u);
        result.failure_modrm=modrm;
        if(opcode==0x88u && modrm==0xC4u) {
            ah=al;
            ip+=2u;
            continue;
        }
        if(opcode==0x29u && modrm==0xC8u) {
            const auto value=static_cast<std::uint16_t>(
                (static_cast<std::uint32_t>(ah)<<8u)|al);
            const auto reduced=static_cast<std::uint16_t>(
                static_cast<std::uint32_t>(value)-subtract_pair);
            al=static_cast<std::uint8_t>(reduced);
            ah=static_cast<std::uint8_t>(reduced>>8u);
            ip+=2u;
            continue;
        }
        if(ip+6u>code_end) {
            result.failure=SintabRasterFailure::truncated_instruction;
            return result;
        }
        std::uint32_t offset=0;
        for(std::uint32_t i=0;i<4u;++i)
            offset|=static_cast<std::uint32_t>(read(ip+2u+i))<<(i*8u);
        if(opcode==0x8Au && modrm==0x86u) {
            const auto addr=static_cast<std::uint64_t>(source)+offset;
            if(addr<source_begin || addr>=source_end) {
                result.failure=SintabRasterFailure::source_read_out_of_range;
                result.failure_address=addr;
                return result;
            }
            al=read(static_cast<std::uint32_t>(addr));
            ip+=6u;
            continue;
        }
        if(opcode==0x88u && (modrm==0x87u || modrm==0xA7u)) {
            const auto addr=static_cast<std::uint64_t>(destination)+offset;
            // Original planar destination is the 64K VGA window shadow.
            if(addr<0xA0000u || addr>=0xB0000u) {
                result.failure=SintabRasterFailure::vga_write_out_of_range;
                result.failure_address=addr;
                return result;
            }
            write(static_cast<std::uint32_t>(addr),
                  modrm==0x87u?al:ah);
            ++result.pixels;
            ip+=6u;
            continue;
        }
        result.failure=SintabRasterFailure::unknown_instruction;
        return result; // Fail closed for all other generated x86 forms.
    }
    result.failure=SintabRasterFailure::instruction_budget;
    return result;
}
} // namespace rloft_recomp
