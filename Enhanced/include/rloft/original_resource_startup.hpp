#pragma once
// Byte-backed portions of Menzo/Stone CODE.1 and CODE.2 resource startup.
// Addresses below are offsets in the original LE code/DGROUP objects.
// Unconverted callees remain explicit callbacks; no DOS services are emulated.
#include "rloft/game_profile.hpp"
#include <cstdint>
#include <initializer_list>
#include <vector>

namespace rloft {

class OriginalResourceStartupPort {
public:
    virtual ~OriginalResourceStartupPort() = default;
    // Arguments are in source-language order, i.e. reverse of x86 PUSH order.
    virtual std::uint32_t call(std::uint32_t code_offset,
                               std::initializer_list<std::uint32_t> args) = 0;
    virtual std::uint8_t read8(std::uint32_t dgroup_offset) = 0;
    virtual std::uint32_t read32(std::uint32_t dgroup_offset) = 0;
    virtual std::uint8_t read_pointer_field8(std::uint32_t pointer,
                                             std::uint32_t field_offset) = 0;
    virtual void write8(std::uint32_t dgroup_offset, std::uint8_t value) = 0;
    virtual void write32(std::uint32_t dgroup_offset, std::uint32_t value) = 0;
};

struct OriginalStartupCheckpoint {
    std::uint32_t next_code_offset{};
    bool chars_examined{};
    bool chars_present{};
    bool optional_initialization_called{};
};

// CODE.1 first callee, through the original byte-table initialization.
// Menzo 0x1044..0x10CB and Stone 0x680C..0x68B5.
[[nodiscard]] OriginalStartupCheckpoint execute_code1_media_prefix(
    GameId game, OriginalResourceStartupPort& port);

// Stone CODE.1 second callee, 0x6134..0x618A, before the event loop.
[[nodiscard]] OriginalStartupCheckpoint execute_stone_code1_interaction_prefix(
    OriginalResourceStartupPort& port);

// CODE.2 first resource-initialization callee: Menzo 0x23934..0x23976,
// Stone 0x2D45C..0x2D4B2 (including its earlier CHARS open).
[[nodiscard]] OriginalStartupCheckpoint execute_code2_resource_prefix(
    GameId game, OriginalResourceStartupPort& port);

// Witness check against original, extracted first LE code/data object bytes.
// This is not equivalent to checking the whole retail binary SHA-256.
[[nodiscard]] bool verify_original_resource_startup_witness(
    GameId game, bool code2,
    const std::vector<std::uint8_t>& code,
    const std::vector<std::uint8_t>& data);

struct OriginalRes0Layout {
    std::uint32_t entries{};
    std::uint32_t first_payload{};
    std::uint32_t final_payload_end{};
};

// Strict generic RES0 container preflight. All observed RES0 entries are EH;
// the existing ResArchive decoder handles extraction/decompression separately.
[[nodiscard]] OriginalRes0Layout inspect_original_res0(
    const std::vector<std::uint8_t>& bytes);

} // namespace rloft
