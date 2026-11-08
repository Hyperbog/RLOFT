#include "Input.hpp"
namespace rloft::input {
void Irq9Ports::begin_scan(std::uint8_t scan) noexcept {
    scan_=scan; port61_=0u;
}
bool Irq9Ports::read8(std::uint16_t port,
                     std::uint8_t& value) const noexcept {
    if(port==0x60u) { value=scan_; return true; }
    if(port==0x61u) { value=port61_; return true; }
    return false;
}
bool Irq9Ports::write8(std::uint16_t port, std::uint8_t value) noexcept {
    if(port==0x61u) { port61_=value; return true; }
    if(port==0x20u && value==0x20u) { ++eoi_count_; return true; }
    return false;
}
unsigned Irq9Ports::eoi_count() const noexcept { return eoi_count_; }
MouseCommand classify_20440_mouse(std::uint32_t ax, std::uint32_t bx,
                                  std::uint32_t cx, std::uint32_t dx) noexcept {
    if(bx!=0u) return MouseCommand::Unsupported;
    if(ax==8u && cx==0u && dx==0x18Fu) return MouseCommand::RangeY;
    if(ax==7u && cx==0u && dx==0x27Eu) return MouseCommand::RangeX;
    if(ax==4u && cx==0x140u && dx==0xC8u) return MouseCommand::SetPosition;
    if(ax==3u && cx==0u && dx==0u) return MouseCommand::ReadPosition;
    return MouseCommand::Unsupported;
}
bool is_host_set1_key(std::uint32_t key) noexcept {
    return (key&0xFF00u)==0xE000u;
}
bool decode_host_set1_key(std::uint32_t key, std::size_t guest_size,
                          std::uint8_t& scan) noexcept {
    if(!is_host_set1_key(key)) return false;
    const std::uint32_t code=key&0xFFu;
    if(code==0u || code>0x58u ||
       std::size_t(0xF590u)+std::size_t(code)*4u+4u>guest_size)
        return false;
    scan=static_cast<std::uint8_t>(code);
    return true;
}
} // namespace rloft::input
