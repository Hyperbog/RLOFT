#include "MouseLifecycle.hpp"
namespace rloft::input::mouse_lifecycle {
namespace {
constexpr std::uint32_t kRegisters=0xA7ACu, kOutput=0xA7C8u;
constexpr std::uint32_t kSegments=0xA79Eu, kSourceHandler=0x27894u;
std::uint16_t le16(const std::uint8_t* p) noexcept {
    return std::uint16_t(p[0]) | (std::uint16_t(p[1])<<8u);
}
std::uint32_t le32(const std::uint8_t* p) noexcept {
    return std::uint32_t(le16(p)) | (std::uint32_t(le16(p+2u))<<16u);
}
}
bool callback_frame_1a064(const std::uint8_t* guest, std::size_t size,
    std::uint32_t interrupt, std::uint32_t registers,
    std::uint32_t output, std::uint32_t segments) noexcept {
    return guest && size>=std::size_t(kOutput)+4u &&
           interrupt==0x33u && registers==kRegisters &&
           output==kOutput && segments==kSegments &&
           le16(guest+kRegisters)==0x000Cu &&
           le16(guest+kRegisters+8u)==0xFFFFu &&
           le32(guest+kRegisters+12u)==kSourceHandler;
}
bool may_install(bool source_mouse_started,
                 unsigned successful_installs) noexcept {
    return source_mouse_started && successful_installs==0u;
}
bool accepted_reply(bool source_mouse_started,
    unsigned successful_installs, std::uint32_t return_eax) noexcept {
    return may_install(source_mouse_started,successful_installs) &&
           return_eax==0u;
}
bool result_pointer(std::uint32_t output,
                    std::size_t guest_size) noexcept {
    return output==kOutput && guest_size>=std::size_t(kOutput)+4u;
}
} // namespace rloft::input::mouse_lifecycle
