#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace rloft {

// Original CODE.2 logical 1C21C..1C257 queues a presentation
// message using the full independent 30700 IRQ tick, NOT A7E4.
// These are observed original-memory inputs, not host-generated time.
struct CombatIrqMessageSnapshot final {
    std::uint32_t original_irq_tick{};
    std::vector<std::uint8_t> source_c_string;
    // Captured bytes at BSS buffer DGROUP:14580. No size is safely
    // recoverable from the original unbounded string-copy routine:
    // caller must provide a bounded writable snapshot.
    std::vector<std::uint8_t> destination_buffer;
};

struct CombatIrqMessageProjection final {
    std::uint32_t deadline_cf54{};
    std::uint8_t pending_flag_cf59{1U};
    std::size_t bytes_copied{};
    std::vector<std::uint8_t> destination_buffer;
};

class CombatIrqMessageModel final {
public:
    // Returns unsupported for missing NUL or an insufficient captured
    // destination span. The actual DOS routine does NO bounds checking.
    [[nodiscard]] static std::optional<CombatIrqMessageProjection> queue(
        const CombatIrqMessageSnapshot& observed);

    // CODE.2 1C2CC..1C2D7 uses *unsigned* EAX > CF54. Does not
    // correct for 32-bit wrap or substitute elapsed host time.
    [[nodiscard]] static bool deadline_passed(
        std::uint32_t observed_irq_tick,
        std::uint32_t deadline_cf54) noexcept;
};

} // namespace rloft
