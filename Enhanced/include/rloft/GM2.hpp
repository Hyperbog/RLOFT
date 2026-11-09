#pragma once

#include "rloft/GM1.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

// Original Sierra Semiconductor ARIA General MIDI bank for the 1-MiB
// hardware ROM profile. The proprietary vendor record grammar is not
// established: all payload bytes remain opaque and byte-exact.
//
// GM1 and GM2 share their 16-byte ARIA signature and patch safety rules.
// Reuse GM1's already verified opaque-bank implementation, NOT a second
// instrument decoder or sound engine. The GM2 type verifies the retail
// bank extent so GM1 cannot be accidentally supplied as GM2.
using Gm2BytePatch = Gm1BytePatch;

class Gm2Bank final {
public:
    static constexpr std::size_t kHardwareRomBytes = 1024U * 1024U;
    static constexpr std::size_t kRetailFileBytes = 24273U;
    static constexpr std::size_t kSignatureBytes = Gm1Bank::kSignatureBytes;

    explicit Gm2Bank(std::vector<std::uint8_t> original);

    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
    [[nodiscard]] std::vector<std::uint8_t> read_range(
        std::size_t offset, std::size_t length) const;

    // Equal-length nonoverlapping edits, with expected-byte and signature
    // checks. Does not pretend to edit unknown ARIA instrument fields.
    void apply_patches(const std::vector<Gm2BytePatch>& patches);

private:
    Gm1Bank opaque_bank_;
};

}  // namespace rloft
