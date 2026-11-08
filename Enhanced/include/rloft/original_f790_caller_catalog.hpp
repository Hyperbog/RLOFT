#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>

namespace rloft {

// Exact E8-rel32 call instructions in immutable CODE.2 object-1.
// This is a call ARGUMENT provenance catalog, NOT proof that an input
// operation invokes F790. Never equate host VGA presents to these sites.
enum class OriginalF790ArgumentOrigin : std::uint8_t {
    GlobalHeld8600_8602,   // push known BYTE 8602; held WORD 8600
    ExplicitEmpty,         // push 0; push -1
    StackLocal,            // push source-owned local known and item ID
    SelectedKnownZero      // push 0; push selected EBX item ID
};

struct OriginalF790DirectCall final {
    std::uint32_t e8_offset{};
    OriginalF790ArgumentOrigin argument_origin{};
};

class OriginalF790CallerCatalog final {
public:
    // Sorted by *original logical CODE.2 instruction offsets*. Original
    // binary verifier independently checks all 17 rel32 E8 targets.
    static constexpr std::array<OriginalF790DirectCall,17> kDirectCalls{{
        {0x1134cU,OriginalF790ArgumentOrigin::GlobalHeld8600_8602},
        {0x1173bU,OriginalF790ArgumentOrigin::GlobalHeld8600_8602},
        {0x1178eU,OriginalF790ArgumentOrigin::GlobalHeld8600_8602},
        {0x1184aU,OriginalF790ArgumentOrigin::ExplicitEmpty},
        {0x11882U,OriginalF790ArgumentOrigin::ExplicitEmpty},
        {0x118ffU,OriginalF790ArgumentOrigin::ExplicitEmpty},
        {0x119fcU,OriginalF790ArgumentOrigin::StackLocal},
        {0x11b63U,OriginalF790ArgumentOrigin::GlobalHeld8600_8602},
        {0x11f47U,OriginalF790ArgumentOrigin::StackLocal},
        {0x127ecU,OriginalF790ArgumentOrigin::GlobalHeld8600_8602},
        {0x128a3U,OriginalF790ArgumentOrigin::GlobalHeld8600_8602},
        {0x12afaU,OriginalF790ArgumentOrigin::GlobalHeld8600_8602},
        {0x12ba4U,OriginalF790ArgumentOrigin::GlobalHeld8600_8602},
        {0x12d50U,OriginalF790ArgumentOrigin::GlobalHeld8600_8602},
        {0x12dfdU,OriginalF790ArgumentOrigin::GlobalHeld8600_8602},
        {0x130deU,OriginalF790ArgumentOrigin::SelectedKnownZero},
        {0x131bbU,OriginalF790ArgumentOrigin::GlobalHeld8600_8602}
    }};

    [[nodiscard]] static constexpr
    std::optional<OriginalF790ArgumentOrigin> arguments_at(
        std::uint32_t source_e8_offset) noexcept {
        for(const auto& entry:kDirectCalls)
            if(entry.e8_offset==source_e8_offset)
                return entry.argument_origin;
        return std::nullopt;
    }
};
} // namespace rloft
