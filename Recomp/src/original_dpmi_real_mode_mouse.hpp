#pragma once
// DPMI 0.9 INT 31h AX=0300: simulate real-mode INT 33h using the
// 0x32-byte register image passed by the original 33AA0 -> 33A7C.
// The host supplies an EXPLICIT virtual mouse result. Never interpret
// this as an observed DOS driver or emulate arbitrary real-mode code.
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft { namespace dpmi_real_mode_mouse {
constexpr std::size_t kFrameBytes=0x32u;
constexpr std::size_t kOffsetEBX=0x10u;
constexpr std::size_t kOffsetEDX=0x14u;
constexpr std::size_t kOffsetECX=0x18u;
constexpr std::size_t kOffsetEAX=0x1Cu;
constexpr std::size_t kOffsetFlags=0x20u;
constexpr std::size_t kOffsetES=0x22u;
struct Result {
    std::uint32_t eax=0u,ebx=0u,ecx=0u,edx=0u;
};
inline std::uint32_t load32(const std::uint8_t* ptr) {
    return std::uint32_t(ptr[0])|(std::uint32_t(ptr[1])<<8u)|
           (std::uint32_t(ptr[2])<<16u)|(std::uint32_t(ptr[3])<<24u);
}
inline std::uint16_t load16(const std::uint8_t* ptr) {
    return std::uint16_t(ptr[0])|(std::uint16_t(ptr[1])<<8u);
}
inline void store32(std::uint8_t* ptr,std::uint32_t value) {
    for(unsigned i=0;i<4;++i)ptr[i]=static_cast<std::uint8_t>(value>>(i*8u));
}
inline Result before(const std::uint8_t* memory,std::size_t size,
                     std::size_t frame_offset,std::uint16_t dos_segment) {
    if(!memory || frame_offset>size || size-frame_offset<kFrameBytes)
        throw std::out_of_range("Original DPMI real-mode register frame is outside CODE.2 RAM");
    const auto* f=memory+frame_offset;
    if(load16(f+kOffsetES)!=dos_segment)
        throw std::runtime_error("Original 33AA0 DOS memory segment does not match tracked AX=0100 allocation");
    return {load32(f+kOffsetEAX),load32(f+kOffsetEBX),
            load32(f+kOffsetECX),load32(f+kOffsetEDX)};
}
inline void apply(std::uint8_t* memory,std::size_t size,
                  std::size_t frame_offset,std::uint16_t dos_segment,
                  const Result& returned) {
    // Validation must precede *any* guest write.
    (void)before(memory,size,frame_offset,dos_segment);
    auto* f=memory+frame_offset;
    store32(f+kOffsetEAX,returned.eax);
    store32(f+kOffsetEBX,returned.ebx);
    store32(f+kOffsetECX,returned.ecx);
    store32(f+kOffsetEDX,returned.edx);
    // AX=0300 returns carry as DPMI status in its outer register frame;
    // the real-mode frame flags are NOT an invented host return value.
    // All other register slots including flags and ES are unchanged.
}
}} // namespace rloft::dpmi_real_mode_mouse
