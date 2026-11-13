#pragma once
// Original Menzo/Stone CODE.2 application global-state initialization.
// Source: verified original LE instructions following the env gate.
// No DOS guest, game logic guessing, or resource asset duplication.
#include "rloft/original_application_main.hpp"
#include "rloft/original_resource_startup.hpp"
#include <cstdint>
#include <functional>
#include <string_view>
#include <vector>

namespace rloft {

struct Code2GlobalCheckpoint {
    std::uint32_t final_source_offset{};
    std::uint32_t resource_entry{};
    std::uint32_t program_initialization_entry{};
    std::uint32_t exit_helper{};
};

// Replays the actual stores and three calls, in encoded order.
// The port owns memory and unconverted original callees; the exit helper's
// original nonreturning behavior cannot be inferred from this wrapper.
[[nodiscard]] Code2GlobalCheckpoint execute_code2_post_environment(
    GameId game, const OriginalCode2Environment& environment,
    OriginalResourceStartupPort& port);

// Verifies each store instruction, its width and address, and direct call
// destinations in the authenticated extracted original CODE.2 object.
// Separate whole-file SHA-256 validation is required for original identity.
[[nodiscard]] bool verify_code2_global_state_witness(
    GameId game, const std::vector<std::uint8_t>& original_code);

using OriginalResourceFileReader =
    std::function<std::vector<std::uint8_t>(std::string_view)>;

// Shared file-loading boundary for RES0..RES12 across the three titles.
// Returns the ORIGINAL archive bytes unchanged. Callers use the EXISTING
// ResArchive type for indexed extraction and decoding; no duplicate codec.
// File reading is injected so Menzo/Stone can be analyzed without routing
// them through the currently Strahd-specific GameSource::open.
[[nodiscard]] std::vector<std::uint8_t> load_shared_original_res(
    GameId game, unsigned archive_number,
    const OriginalResourceFileReader& read_original);

} // namespace rloft
