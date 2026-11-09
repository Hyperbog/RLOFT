#include "rloft/GM2.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {

Gm2Bank::Gm2Bank(std::vector<std::uint8_t> original)
    : opaque_bank_(std::move(original)) {
    if (opaque_bank_.size() != kRetailFileBytes) {
        throw std::runtime_error(
            "GM2.BNK does not match the retail 1-MiB-ROM bank extent");
    }
}

std::size_t Gm2Bank::size() const noexcept {
    return opaque_bank_.size();
}

const std::vector<std::uint8_t>& Gm2Bank::bytes() const noexcept {
    return opaque_bank_.bytes();
}

std::vector<std::uint8_t> Gm2Bank::encode() const {
    return opaque_bank_.encode();
}

std::vector<std::uint8_t> Gm2Bank::read_range(
    std::size_t offset, std::size_t length) const {
    return opaque_bank_.read_range(offset, length);
}

void Gm2Bank::apply_patches(const std::vector<Gm2BytePatch>& patches) {
    opaque_bank_.apply_patches(patches);
}

}  // namespace rloft
