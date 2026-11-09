#include "rloft/GM1.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {

// Original Ravenloft GM1 and GM2 share this ARIA samples-bank signature.
constexpr std::array<std::uint8_t, Gm1Bank::kSignatureBytes> kAriaSignature{
    0x02U, 0xAFU, 0x00U, 0x32U, 0x07U, 0x02U, 0x4DU, 0x19U,
    0x71U, 0x19U, 0x95U, 0x19U, 0xC5U, 0x19U, 0xF5U, 0x19U,
};

void check_range(std::size_t offset, std::size_t length, std::size_t size) {
    if (offset > size || length > size - offset) {
        throw std::out_of_range("ARIA GM1 byte range exceeds bank");
    }
}

}  // namespace

Gm1Bank::Gm1Bank(std::vector<std::uint8_t> original)
    : bytes_(std::move(original)) {
    if (bytes_.size() <= kSignatureBytes ||
        !std::equal(kAriaSignature.begin(), kAriaSignature.end(),
                    bytes_.begin())) {
        throw std::runtime_error("GM1.BNK lacks the certified ARIA bank signature");
    }
}

std::vector<std::uint8_t> Gm1Bank::read_range(
    std::size_t offset, std::size_t length) const {
    check_range(offset, length, bytes_.size());
    return {bytes_.begin() + static_cast<std::ptrdiff_t>(offset),
            bytes_.begin() + static_cast<std::ptrdiff_t>(offset + length)};
}

void Gm1Bank::apply_patches(const std::vector<Gm1BytePatch>& patches) {
    if (patches.empty()) return;

    std::vector<std::pair<std::size_t, std::size_t>> ranges;
    ranges.reserve(patches.size());
    for (const auto& patch : patches) {
        if (patch.offset < kSignatureBytes || patch.expected.empty() ||
            patch.expected.size() != patch.replacement.size()) {
            throw std::invalid_argument("GM1 patch changes ARIA identity or byte count");
        }
        check_range(patch.offset, patch.expected.size(), bytes_.size());
        if (!std::equal(patch.expected.begin(), patch.expected.end(),
                        bytes_.begin() + static_cast<std::ptrdiff_t>(patch.offset))) {
            throw std::runtime_error("GM1 patch expected bytes do not match source");
        }
        ranges.emplace_back(patch.offset, patch.offset + patch.expected.size());
    }
    std::sort(ranges.begin(), ranges.end());
    for (std::size_t i = 1U; i < ranges.size(); ++i) {
        if (ranges[i].first < ranges[i - 1U].second) {
            throw std::invalid_argument("GM1 patches overlap");
        }
    }

    // Build and swap: a rejected patch never leaves a partially edited bank.
    auto updated = bytes_;
    for (const auto& patch : patches) {
        std::copy(patch.replacement.begin(), patch.replacement.end(),
                  updated.begin() + static_cast<std::ptrdiff_t>(patch.offset));
    }
    bytes_.swap(updated);
}

}  // namespace rloft
