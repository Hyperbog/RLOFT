#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

// CODE.1 function 2198: first- and second-class availability are read from
// CODE.1 data-object 2. Third-class rules remain the existing control flow.
// Only boolean availability bytes are represented; no DOS runtime is cloned.
struct Code1ClassRules final {
    std::array<std::uint8_t, 6U * 6U> first{};
    std::array<std::uint8_t, 6U * 6U * 6U> second{};

    [[nodiscard]] bool allows_first(int race, int candidate) const noexcept;
    [[nodiscard]] bool allows_second(int race, int primary,
                                     int candidate) const noexcept;

    // Built-in original rules for synthetic tests and asset-free callers.
    [[nodiscard]] static Code1ClassRules vanilla() noexcept;
    // Reads actual installed CODE.1, checking the nested MZ/LE object layout.
    // A mod may change eligibility bytes but not the executable geometry.
    [[nodiscard]] static Code1ClassRules from_code1(
        const std::vector<std::uint8_t>& bytes);
};

}  // namespace rloft
