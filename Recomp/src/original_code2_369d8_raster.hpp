#pragma once
// Bounded CODE.2 36B14 -> 369D8 -> 369C0 renderer.
// Original CODE.2 SHA256 853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314.
// Requires 36CEC screen/ray traversal and 36C10/36C44 code-setup upstream.
#include "original_sintab_raster.hpp"
#include "original_vga_host.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace rloft_recomp {
struct Original369d8Context {
    std::uint32_t image_begin{}, image_end{}, source_body{}, list_pointer{};
    std::uint32_t sintab_begin{}, sintab_end{};
    std::int32_t screen_x{};
    std::uint16_t run_width{}, packed_lookup{};
    std::int32_t renderer_phase{};
    std::uint32_t row_base{};
    std::uint8_t video_page{};
    bool individual_columns{};
};
struct Original369d8Report {
    std::uint32_t descriptors{}, plane_passes{}, indexed_writes{};
    std::uint32_t off_aperture_cpu_writes{};
    std::uint16_t lookup_after{};
    std::uint8_t last_plane_mask{};
};
inline std::uint16_t original_369d8_word(
        const std::vector<std::uint8_t>& memory, std::uint64_t at) {
    if (at >= memory.size() || memory.size()-at < 2)
        throw std::out_of_range("369D8 original word outside guest");
    return static_cast<std::uint16_t>(memory[at] | (unsigned(memory[at+1])<<8));
}
inline std::uint16_t original_369d8_swap(std::uint16_t value) noexcept {
    return static_cast<std::uint16_t>((value<<8) | (value>>8));
}
// Authenticated CODE.2 C668/C688/C6A8 mask tables have one entry
// per ((screen_x&3)*8 + width), each spanning up to three planar bytes.
inline std::uint8_t original_369d8_mask(
        unsigned offset,unsigned width,unsigned pass) noexcept {
    std::uint8_t mask=0;
    for(unsigned i=0;i<width;++i)
        if((offset+i)/4==pass)
            mask=static_cast<std::uint8_t>(mask | (1u<<((offset+i)&3u)));
    return mask;
}
inline Original369d8Report execute_original_369d8(
        std::vector<std::uint8_t>& guest,
        rloft::graphics::OriginalVgaHost& video,
        const Original369d8Context& ctx) {
    if (ctx.run_width==0 || ctx.run_width>255 ||
        (!ctx.individual_columns && ctx.run_width>7))
        throw std::out_of_range("36B14 unvalidated grouped raster width");
    if (guest.size()<0xC0000u || ctx.video_page>1 ||
        ctx.sintab_begin>=ctx.sintab_end ||
        ctx.sintab_end>guest.size() ||
        ctx.image_begin>=ctx.image_end || ctx.image_end>guest.size() ||
        ctx.source_body<ctx.image_begin || ctx.source_body>=ctx.image_end ||
        ctx.list_pointer<ctx.image_begin || ctx.list_pointer>=ctx.image_end)
        throw std::out_of_range("369D8 original guest range invalid");
    // Original DOS has no rollback on malformed resource data.
    // Stage all plane writes privately to make the host execution bounded.
    // A four-plane VGA snapshot is >256 KiB: Windows defaults to a 1 MiB
    // thread stack. Allocate the transactional copy on the heap.
    auto staged_video=std::make_unique<rloft::graphics::OriginalVgaHost>(video);
    const auto code_begin=ctx.sintab_begin, code_end=ctx.sintab_end;
    std::vector<std::uint8_t> local_code(
        guest.begin()+code_begin,guest.begin()+code_end);
    struct Write {std::uint32_t address; std::uint8_t color;};
    constexpr std::size_t kMaxWrites=256u*400u;
    std::vector<Write> writes;
    writes.reserve(kMaxWrites);
    bool write_failed=false;
    Original369d8Report report{};
    report.lookup_after=ctx.packed_lookup;
    const unsigned invocations=ctx.individual_columns?ctx.run_width:1u;
    for(unsigned invocation=0;invocation<invocations;++invocation) {
        const std::int64_t x=static_cast<std::int64_t>(ctx.screen_x)+
                             (ctx.individual_columns?invocation:0u);
        if(x<0 || x>255)
            throw std::out_of_range("369D8 screen column outside viewport");
        const unsigned width=ctx.individual_columns?1u:ctx.run_width;
        const auto lookup=ctx.individual_columns && (x&1)
            ? original_369d8_swap(ctx.packed_lookup):ctx.packed_lookup;
        const std::int64_t row_offset=
            static_cast<std::int64_t>(ctx.renderer_phase)*80+
            static_cast<std::int64_t>(ctx.row_base)+(x>>2);
        const std::int64_t dest=
            0xA0000LL+ctx.video_page*0x8000LL+row_offset;
        // 369D8's retail MOV [EDI+disp32],AL/AH has no 64K
        // clipping instruction. Guest stores past AFFFF are legal
        // CPU writes into the adjacent bounded B0000-BFFFF region,
        // but are not evidence of pixels in the displayed VGA plane.
        // The initial retail EDI may precede A0000 when an actor is
        // vertically offscreen. Generated SINTAB MOV [EDI+disp32] writes
        // use effective addresses; those, not the EDI base, determine
        // whether a VGA write is legal. The interpreter below still
        // rejects every effective destination outside A0000-BFFFF.
        if(dest<0LL || dest+2>=0xC0000LL)
            throw std::out_of_range(
                "369D8 bounded EDI base outside mapped guest");
        const unsigned offset=static_cast<unsigned>(x)&3u;
        std::size_t at=ctx.list_pointer;
        while(true) {
            if(at+2>ctx.image_end)
                throw std::out_of_range("369D8 display-list terminator missing");
            const auto stop=original_369d8_word(guest,at);
            if(stop==0)break;
            if(at+6>ctx.image_end || stop>0x100 || (stop&1))
                throw std::out_of_range("369D8 bad stop selector");
            const auto adjustment=static_cast<std::int16_t>(
                original_369d8_word(guest,at+2));
            const auto entry=original_369d8_word(guest,at+4);
            if(entry>0x100 || (entry&1))
                throw std::out_of_range("369D8 bad entry selector");
            const std::uint64_t stop_slot=std::uint64_t(code_begin)+stop;
            const std::uint64_t entry_slot=std::uint64_t(code_begin)+entry;
            if(stop_slot+2>code_end || entry_slot+2>code_end)
                throw std::out_of_range("369D8 selector leaves SINTAB block");
            const auto stop_address=std::uint64_t(code_begin)+
                original_369d8_word(guest,stop_slot);
            const auto entry_address=std::uint64_t(code_begin)+
                original_369d8_word(guest,entry_slot);
            if(stop_address>=code_end || entry_address>=code_end)
                throw std::out_of_range("369D8 generated code offset invalid");
            const std::int64_t source=
                static_cast<std::int64_t>(ctx.source_body)+adjustment;
            // Original 369D8 computes ESI = coordinate + signed16
            // adjustment, then generated SINTAB x86 reads [ESI+disp32].
            // A valid ESI may precede the decoded EH7 sprite allocation:
            // only its eventual *effective* read must fall inside it.
            if(source<0 || source>=static_cast<std::int64_t>(guest.size()))
                throw std::out_of_range("369D8 adjusted guest pointer invalid");
            const auto stop_index=static_cast<std::size_t>(stop_address-code_begin);
            const auto previous=local_code[stop_index];
            local_code[stop_index]=0xC3u; // 36A75 temporary RET
            for(unsigned pass=0;pass<3;++pass) {
                const auto mask=original_369d8_mask(offset,width,pass);
                if(pass!=0 && !mask)continue;
                if(!staged_video->write_indexed_port(
                    0x3C4u,static_cast<std::uint16_t>(
                        0x0002u|(unsigned(mask)<<8))))
                    throw std::runtime_error("369D8 VGA mask register failed");
                report.last_plane_mask=mask;
                const auto result=execute_sintab_scale(
                    static_cast<std::uint32_t>(entry_address),
                    static_cast<std::uint32_t>(source),
                    static_cast<std::uint32_t>(dest+pass),lookup,
                    code_begin,code_end,ctx.image_begin,ctx.image_end,
                    [&](std::uint32_t address)->std::uint8_t {
                        return (address>=code_begin && address<code_end)
                            ? local_code[address-code_begin]:guest[address];
                    },
                    [&](std::uint32_t address,std::uint8_t color) {
                        // Never truncate address 0xB0032 to 0x0032:
                        // the old code's uint16_t cast would corrupt
                        // the visible top of the 64K planar window.
                        if(address<0xB0000u) {
                            if(!staged_video->write_aperture(
                                static_cast<std::uint16_t>(address),color))
                                write_failed=true;
                        } else {
                            ++report.off_aperture_cpu_writes;
                        }
                        if(writes.size()==kMaxWrites)write_failed=true;
                        else writes.push_back({address,color});
                    },0xC0000u);
                if(write_failed)
                    throw std::runtime_error("369D8 bounded write limit");
                if(!result.ok) {
                    // Return exact interpreter failure; historically this
                    // masked invalid EH7 effective reads as bad opcodes.
                    // Offsets and scalar diagnostics only, never asset bytes.
                    throw std::runtime_error(
                        "369D8 SINTAB execution rejected reason="+
                        std::to_string(static_cast<unsigned>(result.failure))+
                        " ip_offset="+std::to_string(
                            std::uint64_t(result.failure_ip)-code_begin)+
                        " opcode="+std::to_string(result.failure_opcode)+
                        " modrm="+std::to_string(result.failure_modrm)+
                        " address="+std::to_string(result.failure_address)+
                        " source="+std::to_string(source)+
                        " image_begin="+std::to_string(ctx.image_begin)+
                        " image_end="+std::to_string(ctx.image_end));
                }
                report.indexed_writes+=result.pixels;
                ++report.plane_passes;
            }
            local_code[stop_index]=previous; // 36B04 restore opcode
            ++report.descriptors;
            at+=6;
        }
    }
    // Grouped 36B39 swaps AL/AH; individual 36B42 restores ECX.
    if(!ctx.individual_columns)
        report.lookup_after=original_369d8_swap(ctx.packed_lookup);
    video=std::move(*staged_video);
    for(const auto& write:writes)guest[write.address]=write.color;
    return report;
}
} // namespace rloft_recomp
