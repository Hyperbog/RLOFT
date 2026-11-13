#pragma once

// Verified, bounded C++ lifts of selected original CODE application main
// instructions. The callbacks do not implement unconverted callees.
#include "rloft/game_profile.hpp"
#include <array>
#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>

namespace rloft {

enum class OriginalAppModule : std::uint8_t { Code1, Code2 };

struct OriginalMainIdentity {
    GameId game;
    OriginalAppModule module;
    std::uint32_t crt_call;
    std::uint32_t main_offset;
};

inline constexpr std::array<OriginalMainIdentity, 6> kOriginalMains{{
    {GameId::StrahdsPossession, OriginalAppModule::Code1, 0xFCD9, 0x508C},
    {GameId::StrahdsPossession, OriginalAppModule::Code2, 0x395CC, 0x20608},
    {GameId::Menzoberranzan, OriginalAppModule::Code1, 0xFA2C, 0x127C},
    {GameId::Menzoberranzan, OriginalAppModule::Code2, 0x41E36, 0x23AEC},
    {GameId::StoneProphet, OriginalAppModule::Code1, 0x14D71, 0x6C58},
    {GameId::StoneProphet, OriginalAppModule::Code2, 0x53317, 0x2DD04},
}};

[[nodiscard]] const OriginalMainIdentity& original_main(
    GameId game, OriginalAppModule module);

// Validates *original* LE code/data object bytes, including the CRT call
// displacement, application-owned CODE.1 call graph or CODE.2 environment
// startup calls. Separate SHA-256 verification is required for file identity.
[[nodiscard]] bool verify_original_application_main(
    GameId game, OriginalAppModule module,
    const std::vector<std::uint8_t>& code,
    const std::vector<std::uint8_t>& data);

class OriginalApplicationBoundary {
public:
    virtual ~OriginalApplicationBoundary() = default;
    virtual std::int32_t call0(std::uint32_t original_offset) = 0;
    virtual std::int32_t call1(std::uint32_t original_offset,
                               std::int32_t arg) = 0;
    virtual std::optional<std::string_view> getenv_call(
        std::uint32_t original_offset, std::string_view name) = 0;
    virtual std::int32_t atoi_call(
        std::uint32_t original_offset, std::string_view value) = 0;
};

struct OriginalCode1Outcome {
    enum class Kind { TailExit, Returned } kind;
    std::int32_t value{};   // status 10 for Menzo; EAX return for Stone
    std::uint32_t target{}; // Menzo's original exit tail, if any
};

// Menzo CODE.1 main: six original calls, followed by push 10/jmp 0xCC8E.
// Stone CODE.1 main: call A; return C(B()) via the original call boundary.
// Strahd retains its separately reconstructed canonical Code1.cpp.
[[nodiscard]] OriginalCode1Outcome execute_original_code1_main(
    GameId game, OriginalApplicationBoundary& boundary);

struct OriginalCode2Environment {
    std::int32_t ebx{}, esi{}, edi{}, local{};
    bool gate_present{};
    std::uint32_t continuation{}; // join after original conditional prefix
};

// First CODE.2 application-owned basic block, stopping at original join.
// The original first getenv lookup gates the other four lookups/atoi calls.
[[nodiscard]] OriginalCode2Environment execute_original_code2_env_prefix(
    GameId game, OriginalApplicationBoundary& boundary);

} // namespace rloft
