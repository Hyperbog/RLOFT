#pragma once
// Verified DOS/4GW Professional nested-module loader shared by the three games.
// Strictly rejects unsupported LE page mappings; never executes retail code.
#include "rloft/original_executable.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace rloft {

class OriginalBoundImage final {
public:
    explicit OriginalBoundImage(const std::vector<std::uint8_t>& file)
        : bytes_(file), layout_(inspect_original_executable(file)) {
        if (layout_.kind != OriginalExecutableKind::BoundLinearExecutable)
            throw std::invalid_argument("original image is not a DOS/4G LE program");
        const auto le = std::size_t(layout_.le_file_offset);
        // LE DATA PAGES OFFSET is relative to the *nested* DOS module, not
        // the outer DOS/4GW bound file and not the LE header itself.
        for (std::size_t i = layout_.mz_image_bytes; i + 0x40 <= le; ++i) {
            if (bytes_[i] != 'M' || bytes_[i + 1] != 'Z') continue;
            if (std::uint64_t(i) + word32(i + 0x3c) != le) continue;
            if (found_) throw std::invalid_argument("ambiguous nested DOS module");
            module_origin_ = i;
            found_ = true;
        }
        if (!found_) throw std::invalid_argument("nested MZ/LE origin not found");
        const auto page_origin = std::uint64_t(module_origin_) + word32(le + 0x80);
        const auto page_map = std::uint64_t(le) + word32(le + 0x48);
        const auto last = word32(le + 0x2c);
        if (last > layout_.le_page_size) throw std::invalid_argument("bad last-page size");
        const auto span = (std::uint64_t(layout_.le_pages) - 1) * layout_.le_page_size +
                          (last ? last : layout_.le_page_size);
        if (page_origin > bytes_.size() || span > bytes_.size() - page_origin)
            throw std::invalid_argument("LE data pages exceed original file");
        if (page_map > bytes_.size() ||
            std::uint64_t(layout_.le_pages) * 4 > bytes_.size() - page_map)
            throw std::invalid_argument("truncated LE page map");
        for (std::uint32_t n = 0; n < layout_.le_pages; ++n) {
            const auto p = std::size_t(page_map + std::uint64_t(n) * 4);
            const std::uint32_t number = (std::uint32_t(bytes_[p]) << 16) |
                (std::uint32_t(bytes_[p + 1]) << 8) | bytes_[p + 2];
            if (number != n + 1 || bytes_[p + 3] != 0)
                throw std::invalid_argument("unsupported LE page mapping/flags");
        }
        page_origin_ = std::size_t(page_origin);
        last_page_bytes_ = last ? last : layout_.le_page_size;
    }

    [[nodiscard]] std::size_t module_origin() const noexcept { return module_origin_; }
    [[nodiscard]] std::size_t page_origin() const noexcept { return page_origin_; }
    [[nodiscard]] const OriginalExecutableLayout& layout() const noexcept { return layout_; }
    [[nodiscard]] std::size_t object_physical_start(std::size_t index) const {
        if (index >= layout_.objects.size()) throw std::out_of_range("LE object index");
        return page_origin_ +
               std::size_t(layout_.objects[index].first_page - 1) * layout_.le_page_size;
    }
    // Object virtual bytes, with uninitialized/BSS capacity zero-filled.
    // This is an unpacker, not a decompiler or relocations/fixups loader.
    [[nodiscard]] std::vector<std::uint8_t> object_bytes(std::size_t index) const {
        if (index >= layout_.objects.size()) throw std::out_of_range("LE object index");
        const auto& obj = layout_.objects[index];
        if (obj.virtual_size > 64U * 1024U * 1024U)
            throw std::invalid_argument("LE virtual object exceeds safety limit");
        std::vector<std::uint8_t> out(obj.virtual_size, 0);
        const auto page_size = std::size_t(layout_.le_page_size);
        for (std::size_t j = 0; j < obj.page_count; ++j) {
            const auto dst = j * page_size;
            if (dst >= out.size()) break;
            const auto number = obj.first_page - 1 + j;
            const auto valid = number + 1 == layout_.le_pages ?
                std::size_t(last_page_bytes_) : page_size;
            const auto amount = std::min(valid, out.size() - dst);
            const auto offset = page_origin_ + std::size_t(number) * page_size;
            if (offset > bytes_.size() || amount > bytes_.size() - offset)
                throw std::invalid_argument("LE object page exceeds file");
            std::copy_n(bytes_.begin() + offset, amount, out.begin() + dst);
        }
        return out;
    }
private:
    [[nodiscard]] std::uint32_t word32(std::size_t offset) const {
        if (offset > bytes_.size() || bytes_.size() - offset < 4)
            throw std::invalid_argument("truncated LE field");
        return std::uint32_t(bytes_[offset]) | (std::uint32_t(bytes_[offset+1]) << 8) |
               (std::uint32_t(bytes_[offset+2]) << 16) |
               (std::uint32_t(bytes_[offset+3]) << 24);
    }
    const std::vector<std::uint8_t>& bytes_;
    OriginalExecutableLayout layout_;
    std::size_t module_origin_{};
    std::size_t page_origin_{};
    std::uint32_t last_page_bytes_{};
    bool found_{};
};

} // namespace rloft
