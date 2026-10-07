#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "rloft/game_source.hpp"

namespace rloft {

struct Palette6 {
    std::array<std::uint8_t, 768> rgb{};
};

struct Type1Raster {
    int count{};
    int width{};
    int height{};
    std::uint8_t transparent_index{0xFF};
    std::vector<std::uint8_t> pixels;
};

struct Type2Font {
    std::array<std::uint8_t, 128> widths{};
    Type1Raster raster;
};

class ResArchive final {
public:
    explicit ResArchive(std::vector<std::uint8_t> bytes);

    [[nodiscard]] int entry_count() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> entry(int index) const;
    [[nodiscard]] Type1Raster decode_type1(int index) const;
    [[nodiscard]] Type2Font decode_type2(int index) const;
    [[nodiscard]] std::vector<std::uint8_t> decode_type3(int index) const;
    [[nodiscard]] std::vector<std::uint8_t> decode_type4(int index) const;

private:
    std::vector<std::uint8_t> bytes_;
    std::vector<std::uint32_t> offsets_;
};

class RavenloftBootAssets final {
public:
    explicit RavenloftBootAssets(GameSource source);

    [[nodiscard]] const GameSource& source() const noexcept { return source_; }
    [[nodiscard]] Type1Raster type1(int archive, int index) const;
    [[nodiscard]] Type1Raster res0_type1(int index) const;
    [[nodiscard]] Type2Font res0_font(int index) const;
    [[nodiscard]] std::vector<std::uint8_t> music_sequence(int index) const;
    [[nodiscard]] std::vector<std::uint8_t> sampled_audio(int archive, int index) const;
    [[nodiscard]] std::string text(int index) const;
    [[nodiscard]] std::uint8_t code1_u8(std::size_t dgroup_offset) const;
    [[nodiscard]] std::uint16_t code1_u16(std::size_t dgroup_offset) const;
    [[nodiscard]] std::int32_t code1_s32(std::size_t dgroup_offset) const;
    [[nodiscard]] std::string code1_cstring(std::size_t dgroup_offset) const;
    [[nodiscard]] Palette6 colors_palette(int index, int brightness = 0) const;

private:
    GameSource source_;
    ResArchive res0_;
    ResArchive res1_;
    ResArchive res5_;
    ResArchive res6_;
    ResArchive res7_;
    ResArchive res10_;
    std::vector<std::uint8_t> colors_;
    std::vector<std::uint8_t> text_;
    std::vector<std::uint8_t> code1_;
};

}  // namespace rloft
