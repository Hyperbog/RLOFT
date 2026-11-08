#pragma once
// Read-only page-ledger validation for the original 20294 presentation.
// The retained guest owns all counters, DGROUP and the actual VGA device.
#include <cstdint>
namespace rloft::presentation {
struct PageLedger { unsigned pushes=0, pops=0, fills=0; };
[[nodiscard]] bool can_select_page(std::uint32_t page, bool ready) noexcept;
[[nodiscard]] bool can_push_20294(std::uint32_t page, bool worker,
    bool vga, bool palette, PageLedger ledger, std::uint8_t depth) noexcept;
[[nodiscard]] bool can_clear_20294(std::uint32_t row, std::uint32_t x,
    std::uint32_t rows, std::uint32_t pixels, std::uint32_t color,
    bool vga, PageLedger ledger) noexcept;
[[nodiscard]] bool can_pop_20294(bool vga, PageLedger ledger,
    std::uint8_t depth) noexcept;
} // namespace rloft::presentation
