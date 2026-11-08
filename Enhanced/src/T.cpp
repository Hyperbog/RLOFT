#include "rloft/T.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace rloft {
namespace {
void put16(std::vector<std::uint8_t>& b, std::uint16_t value) {
    b.push_back(static_cast<std::uint8_t>(value));
    b.push_back(static_cast<std::uint8_t>(value >> 8U));
}
void put32(std::vector<std::uint8_t>& b, std::size_t at,
           std::uint32_t value) noexcept {
    for (unsigned byte = 0; byte < 4U; ++byte) {
        b[at + byte] = static_cast<std::uint8_t>(value >> (8U * byte));
    }
}
} // namespace

void TArchive::validate_strict() const {
    const std::size_t table_end = 4U + 4U * count_;
    std::size_t previous_end = table_end;
    for (std::uint32_t i = 0; i < count_; ++i) {
        const std::size_t offset = read32(4U + 4U * i);
        if (offset < previous_end || offset > bytes_.size() ||
            bytes_.size() - offset < 2U) {
            throw std::out_of_range("T offset overlaps table/record or exceeds file");
        }
        const std::size_t length = read16(offset);
        if (length > bytes_.size() - offset - 2U) {
            throw std::out_of_range("T record text exceeds file");
        }
        previous_end = offset + 2U + length;
    }
}

void TArchive::replace(std::uint32_t index, std::string decoded_bytes) {
    if (index >= count_) throw std::out_of_range("T record index");
    if (decoded_bytes.size() > std::numeric_limits<std::uint16_t>::max() ||
        decoded_bytes.find('\0') != std::string::npos) {
        throw std::invalid_argument("T replacement exceeds u16 or contains NUL");
    }
    validate_strict();
    if (replacements_.empty()) replacements_.resize(count_);
    replacements_[index] = std::move(decoded_bytes);
}

std::vector<std::uint8_t> TArchive::encode() const {
    if (replacements_.empty()) return bytes_;
    validate_strict();
    const auto limit = static_cast<std::size_t>(
        std::numeric_limits<std::uint32_t>::max());
    const auto first = static_cast<std::size_t>(read32(4U));
    // Includes the original count, complete offset table and all 276 bytes
    // of original pre-record slack. Never reinterpret that residue as data.
    std::vector<std::uint8_t> out(bytes_.begin(),
                                  bytes_.begin() + static_cast<std::ptrdiff_t>(first));
    for (std::uint32_t i = 0; i < count_; ++i) {
        const auto at = static_cast<std::size_t>(read32(4U + 4U * i));
        const auto length = static_cast<std::size_t>(read16(at));
        const auto end = at + 2U + length;
        const auto next = static_cast<std::size_t>(i + 1U < count_
            ? read32(4U + 4U * (i + 1U)) : bytes_.size());
        if (out.size() > limit) throw std::overflow_error("T offset exceeds u32");
        put32(out, 4U + 4U * i, static_cast<std::uint32_t>(out.size()));
        if (replacements_[i].has_value()) {
            const auto& value = *replacements_[i];
            if (value.size() > limit - out.size() - 2U) {
                throw std::overflow_error("T replacement exceeds u32 offsets");
            }
            put16(out, static_cast<std::uint16_t>(value.size()));
            for (const unsigned char character : value) {
                out.push_back(static_cast<std::uint8_t>(character ^ 0xDCU));
            }
        } else {
            out.insert(out.end(), bytes_.begin() + static_cast<std::ptrdiff_t>(at),
                       bytes_.begin() + static_cast<std::ptrdiff_t>(end));
        }
        // The original indexed reader skips these bytes. Preserve them in
        // their original relative position after the corresponding record.
        if (next < end || next > bytes_.size()) {
            throw std::out_of_range("T inter-record gap overlaps record");
        }
        if (next - end > limit - out.size()) {
            throw std::overflow_error("T preserved gaps exceed u32 offsets");
        }
        out.insert(out.end(), bytes_.begin() + static_cast<std::ptrdiff_t>(end),
                   bytes_.begin() + static_cast<std::ptrdiff_t>(next));
    }
    return out;
}

} // namespace rloft
