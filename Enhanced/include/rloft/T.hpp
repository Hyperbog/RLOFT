#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace rloft {

// Original T: u32 count, count u32 absolute offsets, then u16-length
// strings XOR-encoded with 0xDC. All bytes not addressed by the index are
// opaque preserved source data, not extra records or padding to discard.
//
// The base reader intentionally accepts shared offsets used by legacy
// source-only fixtures. validate_strict() is required for the editable
// layout, where individual records and their inter-record gaps must be
// unambiguous. The CODE.2 reader itself does not validate count/overlap.
class TArchive final {
public:
    explicit TArchive(std::vector<std::uint8_t> bytes)
        : bytes_(std::move(bytes)) {
        if (bytes_.size() < 8U) {
            throw std::invalid_argument("T is missing the text index table");
        }
        count_ = read32(0U);
        if (count_ == 0U ||
            static_cast<std::size_t>(count_) > (bytes_.size() - 4U) / 4U) {
            throw std::invalid_argument("T count exceeds its index table");
        }
    }

    [[nodiscard]] std::uint32_t count() const noexcept { return count_; }

    // Return raw decoded bytes. Rendering/CP437 conversion is a separate
    // presentation concern; never normalize text through host Unicode.
    [[nodiscard]] std::string decoded(std::uint32_t index) const {
        if (index >= count_) throw std::out_of_range("T record index");
        const std::size_t start = read32(4U + 4U * index);
        if (start > bytes_.size() || bytes_.size() - start < 2U) {
            throw std::out_of_range("T record offset outside file");
        }
        const std::size_t length = read16(start);
        if (length > bytes_.size() - start - 2U) {
            throw std::out_of_range("T text payload truncated");
        }
        std::string result;
        result.reserve(length);
        for (std::size_t i = 0; i < length; ++i) {
            result.push_back(static_cast<char>(bytes_[start + 2U + i] ^ 0xDCU));
        }
        return result;
    }

    // Strictly increasing offsets, no overlap, and every record in bounds.
    // The original 1,073-record T file satisfies this, including 30 gaps.
    void validate_strict() const;

    // Replace one record with byte-preserving data. Variable-size edits
    // relocate later offsets but retain every original unreferenced gap.
    void replace(std::uint32_t index, std::string decoded_bytes);
    [[nodiscard]] std::vector<std::uint8_t> encode() const;

private:
    [[nodiscard]] std::uint16_t read16(std::size_t at) const {
        if (at > bytes_.size() || bytes_.size() - at < 2U)
            throw std::out_of_range("T read u16");
        return static_cast<std::uint16_t>(
            static_cast<unsigned>(bytes_[at]) |
            (static_cast<unsigned>(bytes_[at + 1U]) << 8U));
    }
    [[nodiscard]] std::uint32_t read32(std::size_t at) const {
        if (at > bytes_.size() || bytes_.size() - at < 4U)
            throw std::out_of_range("T read u32");
        return static_cast<std::uint32_t>(bytes_[at]) |
               (static_cast<std::uint32_t>(bytes_[at + 1U]) << 8U) |
               (static_cast<std::uint32_t>(bytes_[at + 2U]) << 16U) |
               (static_cast<std::uint32_t>(bytes_[at + 3U]) << 24U);
    }

    std::vector<std::uint8_t> bytes_;
    std::uint32_t count_{};
    std::vector<std::optional<std::string>> replacements_;
};

} // namespace rloft
