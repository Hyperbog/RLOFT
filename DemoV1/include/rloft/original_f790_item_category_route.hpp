#pragma once

#include <cstdint>

namespace rloft {

// Literal source destinations from CODE.2 F8BB..F8D3 and the original
// ten DWORD CS jump slots at logical F768..F78F. This does NOT pretend
// to reconstruct the still-unmapped branch-specific description text.
struct OriginalF790ItemCategoryRoute final {
    static constexpr std::uint32_t kType2To4 = 0xFA85U;
    static constexpr std::uint32_t kType5To8AndDefault = 0xFB3DU;
    static constexpr std::uint32_t kType9To11 = 0xF8D4U;

    [[nodiscard]] static constexpr std::uint32_t destination(
        std::uint8_t category) noexcept {
        // SUB AL,2; CMP AL,9; JA FB3D is a BYTE operation. The
        // unsigned 8-bit wrapping category shift is intentional.
        const auto index = static_cast<std::uint8_t>(category-2U);
        if (index>9U) return kType5To8AndDefault;
        if (index<3U) return kType2To4;
        if (index<7U) return kType5To8AndDefault;
        return kType9To11;
    }
};

} // namespace rloft
