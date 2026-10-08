#include "rloft/combat_original_sprite_raster.hpp"
#include "rloft/combat_original_vga_address.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>

namespace rloft {
namespace {
std::uint16_t word(const std::vector<std::uint8_t>& b,
                   std::size_t at) noexcept {
    return static_cast<std::uint16_t>(
        static_cast<std::uint16_t>(b[at]) |
        (static_cast<std::uint16_t>(b[at+1U])<<8U));
}

std::uint8_t rotate_left_one(std::uint8_t value) noexcept {
    return static_cast<std::uint8_t>(
        (static_cast<unsigned>(value)<<1U) |
        (static_cast<unsigned>(value)>>7U));
}

bool inside_original_aperture(std::uint32_t physical) noexcept {
    return physical>=VgaPlanarMemory::kPhysicalBase &&
        physical-VgaPlanarMemory::kPhysicalBase<
            VgaPlanarMemory::kApertureBytes;
}
} // namespace

std::optional<CombatOriginalSpriteRasterResult>
CombatOriginalSpriteRasterModel::draw_2a380(
    VgaPlanarMemory& vga,
    const CombatOriginalSpriteMemory& source,
    std::uint32_t frame_bits,
    const CombatOriginalSpriteVideoState& video) {
    const auto header=static_cast<std::uint32_t>(
        source.resource_header_address-source.captured_base_address);
    if (header>=source.bytes.size() ||
        source.bytes.size()-header<8U)
        return std::nullopt;

    // 2A3AC..2A3C3 uses [ESI], [ESI+4], [ESI+6], [ESI+7].
    // Original unsigned MUL computes a wrapping 32-bit frame
    // displacement. Negative signed glyph indexes are therefore
    // legal IF the caller has captured the backing earlier memory.
    const auto stride=word(source.bytes,header);
    const auto height=word(source.bytes,header+4U);
    const auto width=source.bytes[header+6U];
    const bool transparent=(source.bytes[header+7U]&0x80U)!=0U;
    if (height==0U) return std::nullopt; // DOS DEC/JNE underflow

    const auto frame_address=static_cast<std::uint32_t>(
        source.resource_header_address+8U+
        static_cast<std::uint32_t>(frame_bits*stride));
    const auto frame=static_cast<std::uint32_t>(
        frame_address-source.captured_base_address);
    if (frame>source.bytes.size()) return std::nullopt;

    // Opaque uses REP MOVSW, CL=BYTE[header+6], so two bytes per
    // iteration. Transparent first doubles EBX, then uses only BL
    // as its byte-loop counter: preserve the original 8-bit wrap.
    const auto full_bytes=static_cast<std::uint32_t>(width)*2U;
    const auto bytes_per_row=transparent
        ? static_cast<std::uint32_t>(
            static_cast<std::uint8_t>(full_bytes))
        : full_bytes;
    // A transparent zero byte counter enters a DEC/JNE loop
    // whose iterations cannot be bounded by the supplied capture.
    if (transparent && bytes_per_row==0U) return std::nullopt;

    const auto required=static_cast<std::uint64_t>(height)*
        static_cast<std::uint64_t>(bytes_per_row)*4U+
        static_cast<std::uint64_t>(transparent?1U:0U);
    if (required>source.bytes.size()-frame) return std::nullopt;

    const auto address=CombatOriginalVgaAddressModel::inspect_2ab38(
        video.row,video.column,video.first_row_shift_efed,
        video.second_row_shift_eff1,video.page_base_f001);
    // The original subtracts full 2*width from EFF9 even in
    // transparent mode where BL may have wrapped to fewer bytes.
    const auto row_remainder=static_cast<std::uint32_t>(
        video.row_pitch_eff9-full_bytes);
    const auto starting_plane=address.original_plane_ecx;
    auto sequencer_ah=static_cast<std::uint8_t>(
        0x11U<<starting_plane);
    auto plane_start=address.original_linear_address_ebx;

    // Preflight every destination byte before the FIRST write.
    // DOS has no aperture bounds guard; unsupported captures
    // must not leave a partially modified native framebuffer.
    for(std::uint32_t plane=0U;plane<4U;++plane) {
        auto dst=plane_start;
        for(std::uint32_t y=0U;y<height;++y) {
            for(std::uint32_t x=0U;x<bytes_per_row;++x) {
                if(!inside_original_aperture(dst))
                    return std::nullopt;
                ++dst;
            }
            dst+=row_remainder;
        }
        if(plane!=3U) {
            const bool carry=(sequencer_ah&0x80U)!=0U;
            sequencer_ah=rotate_left_one(sequencer_ah);
            plane_start+=static_cast<std::uint32_t>(carry);
        }
    }

    CombatOriginalSpriteRasterResult out{};
    out.used_transparent_branch=transparent;
    std::size_t position=frame;
    if(transparent) {
        // 2A422 MOV BH,[ESI], then INC ESI once for the frame.
        out.sentinel=source.bytes[position++];
    }
    // 2A3A5 starts with AH=11h and shifts by initial plane.
    sequencer_ah=static_cast<std::uint8_t>(0x11U<<starting_plane);
    plane_start=address.original_linear_address_ebx;
    for(std::uint32_t plane=0U;plane<4U;++plane) {
        const auto mask=static_cast<std::uint8_t>(
            sequencer_ah&0x0FU);
        auto dst=plane_start;
        for(std::uint32_t y=0U;y<height;++y) {
            for(std::uint32_t x=0U;x<bytes_per_row;++x) {
                const auto pixel=source.bytes[position++];
                // 2A43F..2A44C compares the sentinel PER BYTE.
                // Transparent pixels advance EDI without writing.
                if(transparent && pixel==out.sentinel) {
                    ++out.transparent_bytes_skipped;
                } else {
                    vga.write(dst,mask,pixel);
                    ++out.vga_bytes_written;
                }
                ++dst;
            }
            dst+=row_remainder;
        }
        out.final_sequencer_plane_mask=mask;
        if(plane!=3U) {
            const bool carry=(sequencer_ah&0x80U)!=0U;
            sequencer_ah=rotate_left_one(sequencer_ah);
            plane_start+=static_cast<std::uint32_t>(carry);
        }
    }
    out.source_bytes_consumed=static_cast<std::uint32_t>(
        position-frame);
    return out;
}

} // namespace rloft
