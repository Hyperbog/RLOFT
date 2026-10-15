#pragma once
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include "rloft/renderer_tables.hpp"
#include "rloft/type7_column_traversal.hpp"

namespace rloft {
// Internal native color plan; RES numbers remain original-media provenance.
struct EnhancedType7ShadePlan final {
    std::vector<std::uint16_t> lookup_by_column;
    bool high_on_even_row{};
};
class EnhancedType7ShadeModel final {
    static std::uint16_t swap(std::uint16_t v) noexcept {
        return static_cast<std::uint16_t>((v>>8U)|(v<<8U));
    }
public:
    // Decode only fixed original SINTAB instruction grammar to identify
    // row AL/AH phase. This does NOT execute self-modifying x86 code.
    static bool high_on_even(const RendererScaleBlock& block) {
        const auto& code=block.rasterizer_x86;
        std::size_t pc=0U;
        bool saw_store=false, phase=false, ret=false;
        while(pc<code.size()) {
            if(code[pc]==0xC3U && pc+1U==code.size()) {
                ret=true;break;
            }
            if(pc+1U>=code.size())
                throw std::invalid_argument("truncated native RES3 shade opcode");
            const auto a=code[pc],b=code[pc+1U];
            if(a==0x8AU && b==0x86U) {
                if(code.size()-pc<6U)
                    throw std::invalid_argument("truncated SINTAB shade source");
                pc+=6U;
            } else if((a==0x88U && b==0xC4U) ||
                      (a==0x29U && b==0xC8U)) {
                pc+=2U;
            } else if(a==0x88U && (b==0x87U || b==0xA7U)) {
                if(code.size()-pc<6U)
                    throw std::invalid_argument("truncated SINTAB shade store");
                const auto off=
                    static_cast<std::uint32_t>(code[pc+2U]) |
                    (static_cast<std::uint32_t>(code[pc+3U])<<8U) |
                    (static_cast<std::uint32_t>(code[pc+4U])<<16U) |
                    (static_cast<std::uint32_t>(code[pc+5U])<<24U);
                if(off%80U!=0U)
                    throw std::invalid_argument("SINTAB shade address off row grid");
                const bool this_phase=(b==0xA7U)!=(((off/80U)&1U)!=0U);
                if(saw_store && this_phase!=phase)
                    throw std::invalid_argument("inconsistent SINTAB shade phase");
                phase=this_phase;saw_store=true;pc+=6U;
            } else {
                throw std::invalid_argument("invalid native SINTAB shade opcode");
            }
        }
        if(!ret || !saw_store)
            throw std::invalid_argument("SINTAB shader lacks valid terminal store");
        return phase;
    }

    static EnhancedType7ShadePlan build(
        const std::vector<Type7VisibleRun>& runs,
        const std::vector<Type7VisibleColumn>& columns,
        std::uint16_t lookup, bool individual,
        const RendererScaleBlock& block) {
        EnhancedType7ShadePlan out{};
        out.high_on_even_row=high_on_even(block);
        out.lookup_by_column.reserve(columns.size());
        auto carried=lookup;
        std::size_t index=0U;
        for(const auto& run:runs) {
            if(run.width==0U || run.viewport_x>=256U ||
               run.width>256U-run.viewport_x ||
               index>columns.size() || run.width>columns.size()-index)
                throw std::invalid_argument("native RES3 shade run outside viewport");
            for(std::size_t i=0U;i<run.width;++i) {
                const auto& col=columns[index++];
                if(col.viewport_x!=run.viewport_x+i ||
                   col.source_x!=run.source_x ||
                   col.source_column_index!=run.source_column_index)
                    throw std::invalid_argument("native RES3 shade column mismatch");
                out.lookup_by_column.push_back(
                    individual && ((run.viewport_x+i)&1U)
                    ? swap(carried):carried);
            }
            // Original CODE.2 36B14 carries XCHG AL,AH between groups.
            // C029's individual-column mode restores lookup per run.
            if(!individual) carried=swap(carried);
        }
        if(index!=columns.size())
            throw std::invalid_argument("unmatched native RES3 shade columns");
        return out;
    }

    // Original generated code: MOV AL,src; MOV AH,AL; SUB EAX,ECX.
    // Preserve low-byte borrow into AH; enhanced PC clamps underflow.
    static std::uint8_t shade(std::uint8_t src,std::uint16_t packed,
                              bool high,bool* underflow=nullptr) noexcept {
        const unsigned lo=packed&0xFFU, hi=(packed>>8U)&0xFFU;
        const unsigned sub=high
            ? hi+(static_cast<unsigned>(src)<lo ? 1U:0U) : lo;
        const bool clipped=static_cast<unsigned>(src)<sub;
        if(underflow) *underflow=clipped;
        return clipped ? std::uint8_t{0U}
                       : static_cast<std::uint8_t>(src-sub);
    }
};
} // namespace rloft
