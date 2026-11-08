#include "Presentation.hpp"
namespace rloft::presentation {
bool can_select_page(std::uint32_t page, bool ready) noexcept {
    return page<=1u && ready;
}
bool can_push_20294(std::uint32_t page, bool worker, bool vga,
    bool palette, PageLedger ledger, std::uint8_t depth) noexcept {
    return page==0u && worker && vga && palette &&
           ledger.pushes==ledger.pops && depth==0u;
}
bool can_clear_20294(std::uint32_t row, std::uint32_t x,
    std::uint32_t rows, std::uint32_t pixels, std::uint32_t color,
    bool vga, PageLedger ledger) noexcept {
    return row==0u && x==0u && rows==400u && pixels==320u &&
           color==0u && vga && ledger.pushes==ledger.pops+1u &&
           ledger.fills>=2u*ledger.pops &&
           ledger.fills<2u*ledger.pushes;
}
bool can_pop_20294(bool vga, PageLedger ledger,
    std::uint8_t depth) noexcept {
    return vga && ledger.pushes==ledger.pops+1u &&
           ledger.fills==2u*ledger.pushes && depth==1u;
}
} // namespace rloft::presentation
