#include "rloft/automap_annotations.hpp"

#include <stdexcept>
#include <utility>

namespace rloft {
namespace {

std::uint16_t u16(const std::uint8_t* p) noexcept {
    return static_cast<std::uint16_t>(p[0]) |
           (static_cast<std::uint16_t>(p[1]) << 8U);
}
std::int16_t s16(const std::uint8_t* p) noexcept {
    return static_cast<std::int16_t>(u16(p));
}
AutomapAnnotationLabel advance(AutomapAnnotationLabel label) noexcept {
    if (label.first == 'Z') {
        label.first = 'A';
        if (label.second == ' ') label.second = 'A';
        else ++label.second;
    } else {
        ++label.first;
    }
    return label;
}
}  // namespace

CellPosition AutomapAnnotation::map_cell() const noexcept {
    return CellPosition{
        static_cast<int>(raw_x) / 8,
        static_cast<int>(raw_y) / 5,
    };
}

std::vector<AutomapAnnotation> AutomapAnnotationModel::parse(
    const std::vector<std::uint8_t>& blob) {
    std::vector<AutomapAnnotation> out;
    std::size_t pos = 0;
    while (pos < blob.size()) {
        if (blob.size() - pos < 6U) {
            throw std::runtime_error(
                "automap annotation blob ends inside record header");
        }
        AutomapAnnotation annotation{};
        annotation.raw_x = s16(blob.data() + pos + 0U);
        annotation.raw_y = s16(blob.data() + pos + 2U);
        const auto signed_length = s16(blob.data() + pos + 4U);
        pos += 6U;
        if (signed_length <= 0) {
            throw std::runtime_error(
                "serialized automap annotation has nonpositive payload");
        }
        const auto length = static_cast<std::size_t>(signed_length);
        if (length > blob.size() - pos) {
            throw std::runtime_error(
                "automap annotation payload exceeds serialized blob");
        }
        annotation.payload.assign(
            blob.begin() + static_cast<std::ptrdiff_t>(pos),
            blob.begin() + static_cast<std::ptrdiff_t>(pos + length));
        pos += length;
        out.push_back(std::move(annotation));
    }
    return out;
}

AutomapAnnotationLabel AutomapAnnotationModel::label_for_index(
    std::size_t index) {
    AutomapAnnotationLabel label{};
    for (std::size_t i = 0; i < index; ++i) {
        label = advance(label);
        if (label.second > 'Z') {
            throw std::out_of_range("automap annotation label space");
        }
    }
    return label;
}

std::optional<AutomapAnnotationLabel> AutomapAnnotationModel::label_at(
    const std::vector<AutomapAnnotation>& annotations,
    CellPosition cell) {
    for (std::size_t i = 0; i < annotations.size(); ++i) {
        if (annotations[i].map_cell() == cell) {
            return label_for_index(i);
        }
    }
    return std::nullopt;
}

}  // namespace rloft
