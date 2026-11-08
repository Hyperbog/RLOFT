#pragma once
// Host IVT test fixtures and original 30BB4 IRQ vector lifecycle checks.
// Never installs physical host IRQs, emulates PIC/BIOS or changes CPU state.
#include "../core/GuestCallbacks.hpp"
#include <cstdint>

namespace rloft::host_interrupts {
// Three distinct sentinel pre-existing vectors in original source fixtures.
void seed_source_fixture(guest_callbacks::VectorTable& ivt) noexcept;
// Source 308F8 keyboard handler ownership, checked before original IRQ9.
[[nodiscard]] bool source_irq9_installed(
    const guest_callbacks::VectorTable& ivt,
    std::uint32_t code_selector) noexcept;
// Original 30BB4 expected keyboard, IRQ8 timer and divide-error vectors.
[[nodiscard]] bool source_vectors_installed(
    const guest_callbacks::VectorTable& ivt,
    std::uint32_t code_selector) noexcept;
} // namespace rloft::host_interrupts
