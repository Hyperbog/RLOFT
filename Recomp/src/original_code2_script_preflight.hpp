#pragma once
// Live CODE.2 002A8 source script entry guard. The original 181B4 loader
// reads exactly 0x514 bytes (50 x 0x1A) into CF44. Validate *all* commands
// before running compiled 002A8, so an unsupported later opcode cannot
// leave an earlier command's original guest-state mutation half-committed.
// This is a bounded event bridge, NOT complete scripted gameplay.
#include <cstddef>
#include <cstdint>

namespace rloft { namespace source_script {
struct Validation {
    bool accepted;
    const char* reason;
    unsigned commands;
};
inline Validation validate(const std::uint8_t* guest, std::size_t size,
                           std::uint32_t base, std::int32_t index) noexcept {
    if(index==-1)return {true,"SOURCE_NO_SCRIPT",0u};
    if(index<0 || index>=50)return {false,"SOURCE_SCRIPT_INDEX_OUT_OF_TABLE",0u};
    if(!guest || base==0u ||
       std::uint64_t(base)+0x514u>size)
        return {false,"SOURCE_SCRIPT_TABLE_NOT_LOADED",0u};

    const std::uint32_t entry=base+static_cast<std::uint32_t>(index)*0x1Au;
    const unsigned count=guest[entry];
    if(count>5u)return {false,"SOURCE_SCRIPT_COMMAND_COUNT_INVALID",count};

    if(size<0x2561Du)
        return {false,"SOURCE_SCRIPT_WORLD_DIMENSIONS_INVALID",count};
    const std::int32_t width=static_cast<std::int16_t>(
        std::uint16_t(guest[0x25619u]) |
        (std::uint16_t(guest[0x2561Au])<<8u));
    const std::int32_t height=static_cast<std::int16_t>(
        std::uint16_t(guest[0x2561Bu]) |
        (std::uint16_t(guest[0x2561Cu])<<8u));
    if(size<0x2561Du || width<=0 || height<=0 ||
       std::uint64_t(width)*std::uint64_t(height)>32768u)
        return {false,"SOURCE_SCRIPT_WORLD_DIMENSIONS_INVALID",count};

    // 08A80 appends 0x16-byte records at 248EC, up to original cap 150.
    // Reserve across ALL commands: a later spawn must not overrun the
    // guest after earlier spawns have already changed its state.
    const std::uint32_t initial_moving=
        std::uint32_t(guest[0xC038u]) |
        (std::uint32_t(guest[0xC039u])<<8u) |
        (std::uint32_t(guest[0xC03Au])<<16u) |
        (std::uint32_t(guest[0xC03Bu])<<24u);
    if(initial_moving>150u)
        return {false,"SOURCE_MOVING_RECORD_COUNT_INVALID",count};
    std::uint32_t reserved_moving=initial_moving;
    for(unsigned i=0;i<count;++i) {
        const std::uint32_t cmd=entry+i*5u;
        const unsigned opcode=guest[cmd+1u];
        switch(opcode) {
            // Original map-object toggles / opening animated tile.
            case 0u: case 1u: case 2u:
            // Exact compiled 28110 sound queue, world event flags, BF44.
            case 5u: case 8u: case 9u: case 10u: case 12u:
                break;
            // Source 002A8 opcode 3 invokes ORIGINAL 08A80. All types 0..8
            // use the original source, but 0..7 are guarded against
            // entering the unresolved 22E90 collision-reposition loop.
            // The source's packed 16.16 coordinates are row/column bytes.
            case 3u: {
                const std::uint32_t type=guest[cmd+4u];
                const std::uint32_t row=guest[cmd+2u];
                const std::uint32_t col=guest[cmd+3u];
                if(type>8u)
                    return {false,"SOURCE_MOVING_ACTOR_TYPE_OUT_OF_SOUND_TABLE",count};
                if(row>=static_cast<std::uint32_t>(height) ||
                   col>=static_cast<std::uint32_t>(width))
                    return {false,"SOURCE_MOVING_RECORD_TILE_OUT_OF_WORLD",count};
                if(type!=8u && reserved_moving<150u) {
                    // Original 08A80 calls 2C9DC(packed_row,packed_col)
                    // and needs 22E90 repositioning if the projectile
                    // overlaps the current party. Admit only the already
                    // collision-free original branch.
                    const auto word32=[&](std::uint32_t at)->std::uint32_t {
                        return std::uint32_t(guest[at]) |
                          (std::uint32_t(guest[at+1u])<<8u) |
                          (std::uint32_t(guest[at+2u])<<16u) |
                          (std::uint32_t(guest[at+3u])<<24u);
                    };
                    const auto cam_x=word32(0x255D0u);
                    const auto cam_y=word32(0x255D4u);
                    const std::uint32_t raw_y=(row<<16u)|0x8000u;
                    const std::uint32_t raw_x=(col<<16u)|0x8000u;
                    if(raw_x>=cam_x-0x8FA0u && raw_x<=cam_x+0x8FA0u &&
                       raw_y>=cam_y-0x8FA0u && raw_y<=cam_y+0x8FA0u)
                        return {false,"SOURCE_MOVING_ACTOR_NEEDS_22E90_REPOSITION",count};
                }
                if(reserved_moving<150u) {
                    if(std::uint64_t(0x248ECu)+
                       std::uint64_t(reserved_moving+1u)*0x16u>size)
                        return {false,"SOURCE_MOVING_RECORD_OUT_OF_GUEST",count};
                    ++reserved_moving;
                }
                break;
            }
            // Original tile's low six bits, with checked map addressing.
            case 4u: {
                const std::uint32_t row=guest[cmd+2u];
                const std::uint32_t col=guest[cmd+3u];
                if(row>=static_cast<std::uint32_t>(height) ||
                   col>=static_cast<std::uint32_t>(width) ||
                   std::uint64_t(0x198E0u)+
                    2u*(std::uint64_t(width)*row+col)+2u>size)
                    return {false,"SOURCE_SCRIPT_TILE_OUT_OF_WORLD",count};
                break;
            }
            // Narrow original 11h early-handler branches that return
            // from source before requesting actor selection or UI.
            // Require opcode 11 to be first: earlier commands can change
            // the latch bits and invalidate the preflight guarantee.
            case 11u: {
                const std::uint32_t sub=guest[cmd+4u];
                if(i!=0u)
                    return {false,"SOURCE_EARLY_SCRIPT_DEPENDS_ON_PRIOR_MUTATION",count};
                const std::uint32_t bit=
                    sub==0x0Cu?0x3Du:(sub==0x0Du?0x46u:0xFFFFFFFFu);
                if(bit==0xFFFFFFFFu)
                    return {false,"SOURCE_EARLY_SCRIPT_UNCONNECTED_SUBTYPE",count};
                const std::uint32_t addr=0x22200u+(bit/32u)*4u;
                const std::uint32_t value=
                    std::uint32_t(guest[addr]) |
                    (std::uint32_t(guest[addr+1u])<<8u) |
                    (std::uint32_t(guest[addr+2u])<<16u) |
                    (std::uint32_t(guest[addr+3u])<<24u);
                if((value&(1u<<(bit%32u)))==0u)
                    return {false,"SOURCE_EARLY_SCRIPT_REQUIRES_UNCONNECTED_UI",count};
                break;
            }
            // 6 dialogue and 7 synchronized display are not connected.
            default:
                return {false,"SOURCE_SCRIPT_OPCODE_NEEDS_UNCONNECTED_SERVICE",count};
        }
        // Compiled 002A8 reads LE32 at cmd+4 for opcode 5, including
        // bytes from the next descriptor if the source does so.
        if(opcode==5u && std::uint64_t(cmd)+8u>size)
            return {false,"SOURCE_SCRIPT_SOUND_WORD_OUT_OF_GUEST",count};
    }
    return {true,"SOURCE_SCRIPT_SUPPORTED_BOUNDED",count};
}
}} // namespace
