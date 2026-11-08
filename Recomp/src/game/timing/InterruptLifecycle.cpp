#include "InterruptLifecycle.hpp"
#include "../core/GuestCallbacks.hpp"
#include <cstdint>

namespace rloft::host_interrupts {
void seed_source_fixture(guest_callbacks::VectorTable& ivt) noexcept {
    ivt[9u]={0x00009009u,0x0020u};
    ivt[8u]={0x00009008u,0x0020u};
    ivt[0u]={0x00009000u,0x0020u};
}
bool source_irq9_installed(const guest_callbacks::VectorTable& ivt,
                          std::uint32_t cs) noexcept {
    return ivt[9u].offset==0x308F8u && ivt[9u].selector==cs;
}
bool source_vectors_installed(const guest_callbacks::VectorTable& ivt,
                              std::uint32_t cs) noexcept {
    return source_irq9_installed(ivt,cs) &&
           ivt[8u].offset==0x30708u && ivt[8u].selector==cs &&
           ivt[0u].offset==0x30B54u && ivt[0u].selector==cs;
}
} // namespace rloft::host_interrupts
