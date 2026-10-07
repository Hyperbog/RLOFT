#include "rloft/object_catalog.hpp"

#include <cstddef>
#include <stdexcept>

namespace rloft {
namespace {

std::uint16_t u16(const std::uint8_t* p) noexcept {
    return static_cast<std::uint16_t>(p[0]) |
           (static_cast<std::uint16_t>(p[1]) << 8U);
}
std::uint32_t u32(const std::uint8_t* p) noexcept {
    return static_cast<std::uint32_t>(p[0]) |
           (static_cast<std::uint32_t>(p[1]) << 8U) |
           (static_cast<std::uint32_t>(p[2]) << 16U) |
           (static_cast<std::uint32_t>(p[3]) << 24U);
}
std::string fixed_name(const std::uint8_t* p) {
    std::size_t n = 0;
    while (n < 40U && p[n] != 0) ++n;
    return std::string(
        reinterpret_cast<const char*>(p), n);
}

}  // namespace

ObjectCatalog ObjectCatalog::parse(
    const std::vector<std::uint8_t>& bytes) {
    if (bytes.size() < 4U) {
        throw std::runtime_error("OBJECTS.DAT is truncated");
    }
    const auto count =
        static_cast<std::size_t>(u32(bytes.data()));
    const auto expected =
        4U + count * ObjectDefinition::kSerializedSize;
    if (bytes.size() != expected) {
        throw std::runtime_error(
            "OBJECTS.DAT size does not match counted 0x41 records");
    }

    ObjectCatalog out{};
    out.objects_.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        const auto* rec =
            bytes.data() + 4U +
            i * ObjectDefinition::kSerializedSize;
        ObjectDefinition object{};
        object.name = fixed_name(rec);
        object.scene_shape_base_local =
            u16(rec + 0x28);
        object.scene_shape_stride =
            rec[0x2A];
        object.scene_influence_step =
            rec[0x2B];
        object.ui_graphic_index = u16(rec + 0x2D);
        object.item_category = rec[0x2F];
        object.weight_half_units = rec[0x30];
        object.container_state_index =
            static_cast<std::int8_t>(rec[0x31]);
        object.enchantment = rec[0x37];
        out.objects_.push_back(std::move(object));
    }
    return out;
}

ObjectCatalog ObjectCatalog::load(
    const GameSource& source) {
    return parse(source.read("OBJECTS.DAT"));
}

const ObjectDefinition& ObjectCatalog::at(
    std::size_t object_id) const {
    if (object_id >= objects_.size()) {
        throw std::out_of_range("OBJECTS.DAT object id");
    }
    return objects_[object_id];
}

}  // namespace rloft
