#pragma once
// Original Menzo/Stone CODE.2 resource/CHARS transition, immediately before
// unconverted title-specific game/UI initialization. Not a DOS guest.
#include "rloft/original_resource_startup.hpp"
#include "rloft/original_code2_global_state.hpp"
#include <cstdint>
#include <functional>
#include <map>
#include <optional>
#include <string_view>
#include <vector>

namespace rloft {

struct Code2MediaHandoff {
    std::uint32_t source_next{}; // exact first instruction outside this lift
    std::uint32_t chars_handle{}; // original file-handle return, not file bytes
    bool has_chars{};
    bool secondary_initializer_ran{};
    std::uint32_t upcoming_call{}; // next possible title-specific call, branch dependent
};

// Continue from execute_code2_resource_prefix() without skipping any
// original resource/CHARS branch. All unconverted callees are explicit.
[[nodiscard]] Code2MediaHandoff execute_code2_media_handoff(
    GameId game, const OriginalStartupCheckpoint& prefix,
    OriginalResourceStartupPort& port);

// Checks instruction/call witnesses against extracted original CODE.2 bytes.
[[nodiscard]] bool verify_code2_media_handoff_witness(
    GameId game, const std::vector<std::uint8_t>& code);


 // The next source-confirmed instructions after the CHARS/resource split.
 // A pending call remains a callback into unconverted original application
 // code, not a completed game subsystem or a second native player.
struct Code2GameplayBoundary {
    std::uint32_t source_boundary{};
    std::uint32_t pending_original_call{};
    bool resource_initializer_returned{};
    bool opening_ui_drawn{};
};

// original_stack_argument is Menzo resource-initializer [EBP+0x10],
// supplied by its CALLER rather than synthesized by the adapter.
[[nodiscard]] Code2GameplayBoundary execute_code2_gameplay_boundary(
    GameId game, const Code2MediaHandoff& prior,
    OriginalResourceStartupPort& port, std::uint32_t original_stack_argument);

// Checks immediate source continuation after Phase 6; callers must separately
// authenticate the complete original retail CODE.2 executable.
[[nodiscard]] bool verify_code2_gameplay_boundary_witness(
    GameId game, const std::vector<std::uint8_t>& code);

using OriginalCharsReader =
    std::function<std::optional<std::vector<std::uint8_t>>() >;

// Native data boundary for all three games. Caches original RES containers,
// preserves original bytes and never applies Strahd's 0x920 CHARS schema to
// Menzo/Stone without independent record verification. CHARS is transient.
class OriginalNativeMediaSession final {
public:
    OriginalNativeMediaSession(GameId game, OriginalResourceFileReader resource,
                               OriginalCharsReader chars);
    [[nodiscard]] const std::vector<std::uint8_t>& res(unsigned archive);
    [[nodiscard]] std::optional<std::vector<std::uint8_t>> chars() const;
    [[nodiscard]] GameId game() const noexcept { return game_; }
private:
    GameId game_;
    OriginalResourceFileReader resource_;
    OriginalCharsReader chars_;
    std::map<unsigned, std::vector<std::uint8_t>> archives_;
};

} // namespace rloft
