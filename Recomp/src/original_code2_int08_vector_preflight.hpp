#pragma once
// Original CODE.2 30BB4 -> 30708 -> 30C38 retained INT 08h contract.
// Include after Code2.cpp. This verifier does not implement a DOS PIC,
// an RTC, or the previous interrupt-vector handler.
#include <cstddef>
#include <cstdint>
namespace rloft { namespace source_int08 {
struct Readiness { bool ready; const char* reason; };
template<class Vectors>
inline Readiness validate(const code2::Machine& guest,
                          std::size_t size, const Vectors& ivt,
                          const code2::Application71Boundary& irq) noexcept {
    if(!guest.data || size<0xF58Eu)
        return {false,"ORIGINAL_INT08_GUEST_MEMORY_INVALID"};
    const auto* const data=guest.data;
    if(data[0xF575u]!=1u)
        return {false,"ORIGINAL_INT08_SOURCE_VECTORS_NOT_INSTALLED"};
    if(ivt[8u].offset!=0x30708u ||
       ivt[9u].offset!=0x308F8u ||
       ivt[0u].offset!=0x30B54u ||
       ivt[8u].selector!=guest.cpu.cs ||
       ivt[9u].selector!=guest.cpu.cs ||
       ivt[0u].selector!=guest.cpu.cs)
        return {false,"ORIGINAL_INT08_RETAINED_IVT_MISMATCH"};
    const auto old_offset=code2::load_le32(data+0xF582u);
    const auto old_selector=code2::load_le16(data+0xF586u);
    if(!old_offset || !old_selector)
        return {false,"ORIGINAL_INT08_PREVIOUS_DOS_VECTOR_UNAVAILABLE"};
    if(old_offset==0x30708u && old_selector==guest.cpu.cs)
        return {false,"ORIGINAL_INT08_PREVIOUS_VECTOR_RECURSIVE"};
    if(!irq.call_19c80)
        return {false,"ORIGINAL_INT08_SOURCE_19C80_NOT_CONNECTED"};
    if(!irq.far_chain)
        return {false,"ORIGINAL_INT08_PREVIOUS_VECTOR_CHAIN_UNCONNECTED"};
    return {true,"ORIGINAL_INT08_SOURCE_VECTOR_CONTRACT_READY"};
}
}} // namespace rloft::source_int08
