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


struct OriginalCharsLayout {
    std::size_t record_count;
    std::size_t record_bytes;
    std::size_t image_bytes;
    std::size_t status_word_offset;
};
[[nodiscard]] OriginalCharsLayout original_chars_layout(GameId game);

// CHARS is an opaque, precisely sized transfer. Only independently verified
// common fields are exposed; this is NOT a shared character/rules codec.
class OriginalCharsImage final {
public:
    OriginalCharsImage(GameId game, std::vector<std::uint8_t> bytes);
    [[nodiscard]] GameId game() const noexcept { return game_; }
    [[nodiscard]] OriginalCharsLayout layout() const;
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept {
        return bytes_;
    }
    [[nodiscard]] std::vector<std::uint8_t> record(std::size_t index) const;
    [[nodiscard]] std::uint16_t status_word(std::size_t index) const;
private:
    GameId game_;
    std::vector<std::uint8_t> bytes_;
};

// Authenticates four-record/stride/transfer-size evidence from the original
// producer CODE.1 and consumer CODE.2 LE code objects. Whole-file identity
// checks are separate and required before trusting retail input.
[[nodiscard]] bool verify_cross_game_chars_geometry_witness(
    GameId game, const std::vector<std::uint8_t>& code1,
    const std::vector<std::uint8_t>& code2);

// SOURCE IMPLEMENTATIONS of the first real CODE.2 game/UI callees.
// Menzo 204DC: status-gated four-party resource selection and drawing.
// Stone 25468: original UI state setup/first backdrop, conditional on 400E0.
// Other unconverted calls are still explicit on the injected port.
void execute_menzo_first_gameplay_callee(
    OriginalResourceStartupPort& port, std::uint32_t redraw_flag);
[[nodiscard]] bool execute_stone_first_gameplay_callee(
    OriginalResourceStartupPort& port);
[[nodiscard]] Code2GameplayBoundary continue_stone_code2_present_startup(
    const Code2GameplayBoundary& prior, OriginalResourceStartupPort& port);
[[nodiscard]] bool verify_first_gameplay_callee_witness(
    GameId game, const std::vector<std::uint8_t>& code2);

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
    [[nodiscard]] std::optional<OriginalCharsImage> validated_chars() const;
    [[nodiscard]] GameId game() const noexcept { return game_; }
private:
    GameId game_;
    OriginalResourceFileReader resource_;
    OriginalCharsReader chars_;
    std::map<unsigned, std::vector<std::uint8_t>> archives_;
};

} // namespace rloft
