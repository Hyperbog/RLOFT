#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <optional>

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

struct Type7Geometry {
    std::uint16_t source_width{};
    std::int16_t x_min{};
    std::int16_t x_max{};
    std::int16_t y_min{};
    std::int16_t y_max{};
};

struct Type7RasterDescriptor {
    std::int16_t raster_stop_selector{};
    std::int16_t signed_adjustment{};
    std::int16_t raster_entry_selector{};
};

struct Type7ColumnDisplayList {
    std::int16_t source_x{};
    std::uint16_t list_offset{};
    std::vector<Type7RasterDescriptor> descriptors;
};

struct Type7SceneShape {
    Type7Geometry geometry{};
    std::vector<Type7ColumnDisplayList> columns;
};

struct Type7NativeRaster {
    std::uint16_t width{};
    std::uint16_t height{128U};
    std::vector<std::uint8_t> pixels;
    std::vector<std::uint8_t> mask;
};

struct EnvironmentPlacement {
    std::int16_t resource_selector{};
    std::int16_t azimuth_degrees{};
    std::int16_t draw_width_pixels{};
    std::int16_t screen_y{};

    [[nodiscard]] bool terminator() const noexcept {
        return resource_selector == 0 && azimuth_degrees == 0 &&
               draw_width_pixels == 0 && screen_y == 0;
    }
};

struct EnvironmentProfile {
    std::uint16_t res8_base{};
    std::uint16_t stored_record_count{};
    std::vector<EnvironmentPlacement> placements;
};

[[nodiscard]] std::vector<EnvironmentProfile> decode_environment_profiles(
    const std::vector<std::uint8_t>& init_bytes);
[[nodiscard]] int resolve_environment_res8_id(
    const EnvironmentProfile& profile,
    const EnvironmentPlacement& placement);

class ResArchive final {
public:
    explicit ResArchive(std::vector<std::uint8_t> bytes);

    [[nodiscard]] int entry_count() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> entry(int index) const;
    [[nodiscard]] Type1Raster decode_type1(int index) const;
    [[nodiscard]] Type2Font decode_type2(int index) const;
    [[nodiscard]] std::vector<std::uint8_t> decode_type3(int index) const;
    [[nodiscard]] std::vector<std::uint8_t> decode_type4(int index) const;
    [[nodiscard]] Type7Geometry decode_type7_geometry(int index) const;
    [[nodiscard]] Type7SceneShape decode_type7(int index) const;
    // Original decoded RES3 body, including geometry, packed texels and
    // descriptor lists. This is the ESI base used by CODE.2 369D8.
    [[nodiscard]] std::vector<std::uint8_t> decode_type7_body(int index) const;
    [[nodiscard]] Type7NativeRaster decode_type7_native_raster(int index) const;

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
    [[nodiscard]] Type7Geometry res3_type7_geometry(int index) const;
    [[nodiscard]] Type7SceneShape res3_type7(int index) const;
    [[nodiscard]] std::vector<std::uint8_t> res3_type7_body(int index) const;
    [[nodiscard]] Type7NativeRaster res3_type7_native_raster(int index) const;
    [[nodiscard]] std::string text(int index) const;
    [[nodiscard]] std::uint16_t init_u16(std::size_t byte_offset) const;
    [[nodiscard]] std::vector<EnvironmentProfile> environment_profiles() const;
    [[nodiscard]] std::uint8_t code1_u8(std::size_t dgroup_offset) const;
    [[nodiscard]] std::uint16_t code1_u16(std::size_t dgroup_offset) const;
    [[nodiscard]] std::int32_t code1_s32(std::size_t dgroup_offset) const;
    [[nodiscard]] std::string code1_cstring(std::size_t dgroup_offset) const;
    [[nodiscard]] Palette6 colors_palette(int index, int brightness = 0) const;

private:
    GameSource source_;
    ResArchive res0_;
    ResArchive res1_;
    mutable std::optional<ResArchive> res3_;
    ResArchive res5_;
    ResArchive res6_;
    ResArchive res7_;
    ResArchive res8_;
    ResArchive res10_;
    std::vector<std::uint8_t> colors_;
    std::vector<std::uint8_t> text_;
    std::vector<std::uint8_t> init_;
    std::vector<std::uint8_t> code1_;
};

}  // namespace rloft
