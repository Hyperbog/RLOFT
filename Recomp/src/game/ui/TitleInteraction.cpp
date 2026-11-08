#include "TitleInteraction.hpp"
namespace rloft::ui::title_interaction {
namespace {
constexpr std::size_t kButtons=0x2B7A8u;
constexpr std::size_t kDosX=0x2B7ACu;
constexpr std::size_t kDosY=0x2B7AEu;
std::uint16_t le16(const std::uint8_t* p) noexcept {
    return std::uint16_t(p[0]) | (std::uint16_t(p[1])<<8u);
}
}
bool read_source_mouse_event(const std::uint8_t* guest,
    std::size_t size, MouseEvent& event) noexcept {
    if(!guest || size<kDosY+2u)return false;
    // Original 19464 -> 1A1F4 -> 19D18(1,buttons,2*x,y,0).
    event={1u,le16(guest+kButtons),
           std::uint32_t(le16(guest+kDosX))*2u,
           le16(guest+kDosY),0u};
    return true;
}
std::uint32_t CallbackDispatch::event_mask(const MouseEvent& state) noexcept {
    const std::uint32_t buttons=state.buttons&3u;
    std::uint32_t flags=0u;
    if(state.dos_x_doubled!=previous_dos_x_ ||
       state.dos_y!=previous_dos_y_)flags|=1u;
    if((buttons^previous_buttons_)&1u)
        flags|=(buttons&1u)?2u:4u;
    if((buttons^previous_buttons_)&2u)
        flags|=(buttons&2u)?8u:16u;
    previous_buttons_=buttons;
    previous_dos_x_=state.dos_x_doubled;
    previous_dos_y_=state.dos_y;
    return flags;
}
bool source_title_presented(unsigned original_page_presentations) noexcept {
    return original_page_presentations!=0u;
}
} // namespace rloft::ui::title_interaction
