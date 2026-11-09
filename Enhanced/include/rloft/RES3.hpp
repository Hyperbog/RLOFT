#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace rloft {

class ResArchive;
struct Type7Geometry;
struct Type7SceneShape;
struct Type7NativeRaster;

// RES3 stores CODE.2 EH type-7 scene-shape display lists, NOT bitmap sprites.
// The 6-byte draw descriptors reference scale-specific SINTAB rasterizer
// offsets. Keep source bytes and bank-local resource IDs in the native game.
struct Res3ShapeInfo final {
    std::uint16_t source_width{};
    std::int16_t x_min{}, x_max{}, y_min{}, y_max{};
    std::size_t decoded_bytes{};
    std::size_t column_lists{};
    std::size_t draw_descriptors{};
};

struct Res3ArchiveStats final {
    std::size_t shapes{};
    std::size_t widths_64{};
    std::size_t widths_96{};
    std::size_t widths_128{};
    std::uint64_t decoded_bytes{};
    std::uint64_t draw_descriptors{};
};

// One thin archive owner over the existing, original-source ResArchive.
// No second decompressor, renderer, VM, or duplicate scene-state model.
class Res3Archive final {
public:
    explicit Res3Archive(std::vector<std::uint8_t> bytes);
    ~Res3Archive();
    Res3Archive(Res3Archive&&) noexcept;
    Res3Archive& operator=(Res3Archive&&) noexcept;
    Res3Archive(const Res3Archive&) = delete;
    Res3Archive& operator=(const Res3Archive&) = delete;

    [[nodiscard]] int entry_count() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> entry(int index) const;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
    [[nodiscard]] std::vector<std::uint8_t> decode_body(int index) const;
    [[nodiscard]] Type7Geometry decode_geometry(int index) const;
    [[nodiscard]] Type7SceneShape decode_shape(int index) const;
    [[nodiscard]] Type7NativeRaster decode_native_raster(int index) const;
    [[nodiscard]] Res3ShapeInfo inspect_shape(int index) const;
    [[nodiscard]] Res3ArchiveStats inspect_all() const;

    // Replace a single uncompressed type-7 body with a standard literal-only
    // method-3 LZSS stream. The shared decoder validates the result before
    // replacing; other original compressed entries remain byte-identical.
    // This preserves all opaque packed source bytes inside the supplied body.
    void replace_body(int index, const std::vector<std::uint8_t>& body);

private:
    std::unique_ptr<ResArchive> archive_;
};

} // namespace rloft
