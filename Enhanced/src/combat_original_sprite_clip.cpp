#include "rloft/combat_original_sprite_clip.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {
namespace {
std::int64_t signed32(std::uint32_t bits) noexcept {
    return bits<=0x7FFFFFFFU
        ? static_cast<std::int64_t>(bits)
        : static_cast<std::int64_t>(bits)-0x100000000LL;
}
std::size_t scratch_offset(
    const CombatOriginalDgroupCapture& memory,
    std::uint32_t address) {
    if(address<memory.captured_base_dgroup_offset)
        throw std::logic_error("uncaptured 29414 scratch record");
    const auto offset=static_cast<std::uint64_t>(
        address-memory.captured_base_dgroup_offset);
    if(offset>memory.bytes.size() ||
       memory.bytes.size()-offset<24U)
        throw std::logic_error("incomplete 24-byte original 29414 record");
    return static_cast<std::size_t>(offset);
}
std::uint32_t get(const CombatOriginalDgroupCapture& memory,
                  std::size_t offset,unsigned word) noexcept {
    const auto start=offset+static_cast<std::size_t>(word)*4U;
    return static_cast<std::uint32_t>(memory.bytes[start])
        |(static_cast<std::uint32_t>(memory.bytes[start+1U])<<8U)
        |(static_cast<std::uint32_t>(memory.bytes[start+2U])<<16U)
        |(static_cast<std::uint32_t>(memory.bytes[start+3U])<<24U);
}
void put(CombatOriginalDgroupCapture& memory,
         std::size_t offset,unsigned word,std::uint32_t value) noexcept {
    const auto start=offset+static_cast<std::size_t>(word)*4U;
    for(unsigned i=0;i<4U;++i)
        memory.bytes[start+i]=static_cast<std::uint8_t>(
            value>>(i*8U));
}
std::uint16_t resource_rows(
    const CombatOriginalSpriteClipServices& s,
    std::uint32_t resource) {
    if(!s.read_sprite_word)
        throw std::logic_error("missing original sprite header WORD reader");
    const auto value=s.read_sprite_word(resource+4U);
    if(!value)throw std::logic_error("uncaptured original sprite row count");
    return *value;
}
std::uint32_t resource_columns(
    const CombatOriginalSpriteClipServices& s,
    std::uint32_t resource) {
    if(!s.read_sprite_byte)
        throw std::logic_error("missing original sprite header BYTE reader");
    const auto value=s.read_sprite_byte(resource+6U);
    if(!value)throw std::logic_error("uncaptured original sprite column pitch");
    return static_cast<std::uint32_t>(*value)*8U;
}
} // namespace

std::uint32_t CombatOriginalSpriteClipModel::draw_29414(
    CombatOriginalSpriteClipGlobals& g,
    CombatOriginalDgroupCapture& dgroup,
    std::uint32_t mode,std::uint32_t row,std::uint32_t column,
    std::uint32_t resource,std::uint32_t scratch,
    const CombatOriginalSpriteClipServices& calls) {
    if(!calls.refresh_2e758)
        throw std::logic_error("missing original 2E758 refresh callback");
    // Original 29423: REFRESH before even testing resource pointer.
    calls.refresh_2e758();
    if(resource==0U)return 0U;

    const auto base=scratch_offset(dgroup,scratch);
    // Scratch DWORD indices: [top skip, bottom skip, visible rows,
    // left skip, right skip, visible columns] at offsets 0..20.
    put(dgroup,base,2U,resource_rows(calls,resource));
    auto excess=static_cast<std::uint32_t>(
        g.viewport_first_row_ef55-row);
    put(dgroup,base,0U,0U);
    put(dgroup,base,1U,0U);
    if(signed32(excess)>0) {
        put(dgroup,base,2U,get(dgroup,base,2U)-excess);
        const auto byte_pitch=resource_columns(calls,resource);
        const auto top_skip=static_cast<std::uint32_t>(
            excess*byte_pitch);
        row=g.viewport_first_row_ef55;
        put(dgroup,base,0U,top_skip);
    }
    excess=static_cast<std::uint32_t>(
        get(dgroup,base,2U)+row-1U-g.viewport_last_row_ef59);
    if(signed32(excess)>0) {
        put(dgroup,base,2U,get(dgroup,base,2U)-excess);
        const auto pitch=resource_columns(calls,resource);
        put(dgroup,base,1U,static_cast<std::uint32_t>(excess*pitch));
    }

    put(dgroup,base,5U,resource_columns(calls,resource));
    excess=static_cast<std::uint32_t>(
        g.viewport_first_column_ef5d-column);
    put(dgroup,base,3U,0U);
    put(dgroup,base,4U,0U);
    if(signed32(excess)>0) {
        put(dgroup,base,3U,excess);
        put(dgroup,base,5U,get(dgroup,base,5U)-excess);
        column=g.viewport_first_column_ef5d;
    }
    excess=static_cast<std::uint32_t>(
        column+get(dgroup,base,5U)-1U-
        g.viewport_last_column_ef61);
    if(signed32(excess)>0) {
        put(dgroup,base,4U,excess);
        put(dgroup,base,5U,get(dgroup,base,5U)-excess);
    }

    // 294E0..294FE: FOUR WORD outputs are updated even when
    // the eventual sprite ends up clipped to an empty region.
    g.output_column_ef6b=static_cast<std::uint16_t>(column);
    g.output_rows_ef6d=static_cast<std::uint16_t>(get(dgroup,base,2U));
    g.output_columns_ef6f=static_cast<std::uint16_t>(get(dgroup,base,5U));
    const auto original_height=resource_rows(calls,resource);
    g.output_row_ef69=static_cast<std::uint16_t>(row);

    if(static_cast<std::uint32_t>(original_height)==get(dgroup,base,2U)
       && resource_columns(calls,resource)==get(dgroup,base,5U)) {
        if(!calls.full_sprite_2a380)
            throw std::logic_error("missing original 2A380 full sprite");
        calls.full_sprite_2a380(mode,row,column,resource);
        return 2U;
    }
    if(signed32(get(dgroup,base,2U))>0 &&
       signed32(get(dgroup,base,5U))>0) {
        if(!calls.clipped_sprite_2a6b0)
            throw std::logic_error("missing original 2A6B0 clipped sprite");
        calls.clipped_sprite_2a6b0(mode,row,column,scratch,resource);
        return 1U;
    }
    return 0U;
}

} // namespace rloft
