#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

// Sierra Semiconductor ARIA General MIDI bank for the 512-KiB ROM profile.
// Not an AdLib BNK, an OPL2 SAMPLE.AD library or a native soundfont.
// The original vendor record grammar remains unverified. Store every byte;
// do not assign unsupported instrument/sample names to opaque fields.
struct Gm1BytePatch final {
    std::size_t offset{};
    std::vector<std::uint8_t> expected;
    std::vector<std::uint8_t> replacement;
};

class Gm1Bank final {
public:
    static constexpr std::size_t kHardwareRomBytes = 512U * 1024U;
    static constexpr std::size_t kSignatureBytes = 16U;

    explicit Gm1Bank(std::vector<std::uint8_t> original);

    [[nodiscard]] std::size_t size() const noexcept { return bytes_.size(); }
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept {
        return bytes_;
    }
    [[nodiscard]] std::vector<std::uint8_t> encode() const { return bytes_; }
    [[nodiscard]] std::vector<std::uint8_t> read_range(
        std::size_t offset, std::size_t length) const;

    // Applies equal-length, nonoverlapping edits atomically, with expected
    // original bytes as a stale-edit guard. The 16-byte signature cannot be
    // changed. These are byte patches, NOT semantic instrument editing.
    void apply_patches(const std::vector<Gm1BytePatch>& patches);

private:
    std::vector<std::uint8_t> bytes_;
};

}  // namespace rloft
