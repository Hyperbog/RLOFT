#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace rloft {

class ResArchive;
struct Type1Raster;

// A focused, editable RES8 facade over the game's existing RES archive
// decoder. The 73 original assets are EH type-1 LZSS environment images.
// Unchanged entries retain their *original compressed bytes*; replacing an
// image uses CODE.2's supported raw type-1 method 0, never a fake LZSS stream.
class Res8Archive final {
public:
    explicit Res8Archive(std::vector<std::uint8_t> bytes);
    ~Res8Archive();
    Res8Archive(Res8Archive&&) noexcept;
    Res8Archive& operator=(Res8Archive&&) noexcept;
    Res8Archive(const Res8Archive&) = delete;
    Res8Archive& operator=(const Res8Archive&) = delete;

    [[nodiscard]] int entry_count() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> entry(int index) const;
    [[nodiscard]] Type1Raster decode_type1(int index) const;
    void replace_pixels(int index, const std::vector<std::uint8_t>& pixels);
    [[nodiscard]] std::vector<std::uint8_t> encode() const;

private:
    std::unique_ptr<ResArchive> original_;
    // Empty entry means 'not overridden'; RES8 itself has no empty entries.
    std::vector<std::vector<std::uint8_t>> overrides_;
};

} // namespace rloft
