// Isolated DOS interrupt-vector and private saved-level CODE.2 callbacks.
// Original CODE.2 Machine/DGROUP/IVT and 181B4 overlay are borrowed.
#include "GuestCallbacks.hpp"
#include "../save/Save.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft::guest_callbacks {
void get_vector(const VectorTable& table,std::uint8_t vector,
                std::uint32_t* offset,std::uint16_t* selector) noexcept {
    const auto& entry=table[vector];
    if(offset)*offset=entry.offset;
    if(selector)*selector=entry.selector;
}
void set_vector(VectorTable& table,std::uint8_t vector,
                std::uint32_t offset,std::uint16_t selector) noexcept {
    table[vector]={offset,selector};
}
bool guest_range(const std::vector<std::uint8_t>& guest,
                 std::uint32_t offset,std::uint32_t count) noexcept {
    return offset<=guest.size() && count<=guest.size()-offset;
}
OverlayIoResult write_level_from_guest(
    live_level_files::Overlay& files,
    const std::vector<std::uint8_t>& guest,
    std::uint32_t src,std::uint32_t size,std::uint32_t handle) noexcept {
    if(!guest_range(guest,src,size))
        return {OverlayIoStatus::GuestOutOfBounds,0u};
    if(handle!=live_level_files::Overlay::write_handle)
        return {OverlayIoStatus::InvalidHandle,0u};
    const auto* bytes=size?guest.data()+src:nullptr;
    const auto result=files.write(bytes,size);
    return result==size?OverlayIoResult{OverlayIoStatus::Ok,result}
                       :OverlayIoResult{OverlayIoStatus::ShortIo,result};
}
OverlayIoResult read_level_into_guest(
    live_level_files::Overlay& files,
    std::vector<std::uint8_t>& guest,
    std::uint32_t dst,std::uint32_t size,std::uint32_t handle) noexcept {
    if(!guest_range(guest,dst,size))
        return {OverlayIoStatus::GuestOutOfBounds,0u};
    if(handle!=live_level_files::Overlay::read_handle)
        return {OverlayIoStatus::InvalidHandle,0u};
    auto* bytes=size?guest.data()+dst:nullptr;
    const auto result=files.read(bytes,size);
    return result==size?OverlayIoResult{OverlayIoStatus::Ok,result}
                       :OverlayIoResult{OverlayIoStatus::ShortIo,result};
}
} // namespace rloft::guest_callbacks
