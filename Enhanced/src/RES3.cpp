#include "rloft/RES3.hpp"
#include "rloft/resource_assets.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {

void validate_envelope(const std::vector<std::uint8_t>& raw) {
    if (raw.size() < 8U || raw[0] != 'E' || raw[1] != 'H' ||
        raw[2] != 7U || raw[3] != 0U || raw[4] != 0U) {
        throw std::runtime_error("RES3 entry must have an EH7 zero-length type header");
    }
    const auto length = static_cast<std::size_t>(raw[5U]) |
        (static_cast<std::size_t>(raw[6U]) << 8U);
    if (length < 10U) {
        throw std::runtime_error("RES3 type-7 shape lacks geometry header");
    }
}

std::vector<std::uint8_t> one_entry_archive(
    const std::vector<std::uint8_t>& raw) {
    std::vector<std::uint8_t> result;
    result.reserve(8U + raw.size());
    result.insert(result.end(), {1U, 0U, 0U, 0U, 8U, 0U, 0U, 0U});
    result.insert(result.end(), raw.begin(), raw.end());
    return result;
}

std::vector<std::uint8_t> encode_literals(
    const std::vector<std::uint8_t>& body) {
    if (body.size() < 10U ||
        body.size() > std::numeric_limits<std::uint16_t>::max()) {
        throw std::invalid_argument("RES3 body exceeds EH7 16-bit decoded length");
    }
    std::vector<std::uint8_t> raw;
    raw.reserve(7U + body.size() + (body.size() + 7U) / 8U);
    raw.insert(raw.end(), {'E', 'H', 7U, 0U, 0U,
        static_cast<std::uint8_t>(body.size()),
        static_cast<std::uint8_t>(body.size() >> 8U)});
    // CODE.2 37700: one low-bit-first flag byte precedes up to 8 literal
    // bytes. A trailing partial group is consumed exactly to decoded EOF.
    for (std::size_t pos = 0U; pos < body.size();) {
        const auto count = std::min<std::size_t>(8U, body.size() - pos);
        raw.push_back(static_cast<std::uint8_t>((1U << count) - 1U));
        raw.insert(raw.end(), body.begin() + static_cast<std::ptrdiff_t>(pos),
                   body.begin() + static_cast<std::ptrdiff_t>(pos + count));
        pos += count;
    }
    return raw;
}

} // namespace

Res3Archive::Res3Archive(std::vector<std::uint8_t> bytes)
    : archive_(std::make_unique<ResArchive>(std::move(bytes))) {
    if (archive_->entry_count() <= 0) {
        throw std::runtime_error("RES3 has no scene-shape entries");
    }
    // Eagerly validate headers without expanding 4,234 shapes at startup.
    // inspect_all() performs the stronger decoded corpus validation.
    for (int i = 0; i < archive_->entry_count(); ++i) {
        validate_envelope(archive_->entry(i));
    }
}

Res3Archive::~Res3Archive() = default;
Res3Archive::Res3Archive(Res3Archive&&) noexcept = default;
Res3Archive& Res3Archive::operator=(Res3Archive&&) noexcept = default;

int Res3Archive::entry_count() const noexcept { return archive_->entry_count(); }

std::vector<std::uint8_t> Res3Archive::entry(int index) const {
    return archive_->entry(index);
}

std::vector<std::uint8_t> Res3Archive::encode() const {
    return archive_->encode();
}

std::vector<std::uint8_t> Res3Archive::decode_body(int index) const {
    return archive_->decode_type7_body(index);
}

Type7Geometry Res3Archive::decode_geometry(int index) const {
    return archive_->decode_type7_geometry(index);
}

Type7SceneShape Res3Archive::decode_shape(int index) const {
    return archive_->decode_type7(index);
}

Type7NativeRaster Res3Archive::decode_native_raster(int index) const {
    return archive_->decode_type7_native_raster(index);
}

Res3ShapeInfo Res3Archive::inspect_shape(int index) const {
    const auto raw = entry(index);
    validate_envelope(raw);
    const auto shape = decode_shape(index);
    Res3ShapeInfo info{};
    info.source_width = shape.geometry.source_width;
    info.x_min = shape.geometry.x_min;
    info.x_max = shape.geometry.x_max;
    info.y_min = shape.geometry.y_min;
    info.y_max = shape.geometry.y_max;
    info.decoded_bytes = static_cast<std::size_t>(raw[5U]) |
        (static_cast<std::size_t>(raw[6U]) << 8U);
    info.column_lists = shape.columns.size();
    for (const auto& column : shape.columns) {
        info.draw_descriptors += column.descriptors.size();
    }
    return info;
}

Res3ArchiveStats Res3Archive::inspect_all() const {
    Res3ArchiveStats result{};
    for (int i = 0; i < entry_count(); ++i) {
        const auto info = inspect_shape(i);
        switch (info.source_width) {
            case 64U: ++result.widths_64; break;
            case 96U: ++result.widths_96; break;
            case 128U: ++result.widths_128; break;
            default:
                throw std::runtime_error("RES3 unsupported original scene-shape width");
        }
        ++result.shapes;
        result.decoded_bytes += static_cast<std::uint64_t>(info.decoded_bytes);
        result.draw_descriptors += static_cast<std::uint64_t>(info.draw_descriptors);
    }
    return result;
}

void Res3Archive::replace_body(
    int index, const std::vector<std::uint8_t>& body) {
    // Ensure the selected original index exists before any allocation.
    (void)archive_->entry(index);
    auto raw = encode_literals(body);
    ResArchive proof(one_entry_archive(raw));
    (void)proof.decode_type7(0); // Fully validate pointer/list/selector grammar.
    (void)proof.decode_type7_native_raster(0); // Validate packed source columns.
    if (proof.decode_type7_body(0) != body) {
        throw std::logic_error("RES3 LZSS round-trip mismatch");
    }
    archive_->replace_entry(index, raw);
}

} // namespace rloft
