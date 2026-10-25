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

    for(unsigned i=0;i<count;++i) {
        const std::uint32_t cmd=entry+i*5u;
        const unsigned opcode=guest[cmd+1u];
        switch(opcode) {
            // Original map-object toggles / opening animated tile.
            case 0u: case 1u: case 2u:
            // Exact compiled 28110 sound queue, world event flags, BF44.
            case 5u: case 8u: case 9u: case 10u: case 12u:
                break;
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
            // 3 actor teleport, 6 dialog, 7 synchronized display and
            // 11 nested interaction/early handlers need source services
            // not yet connected. Never execute them with null callbacks.
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
