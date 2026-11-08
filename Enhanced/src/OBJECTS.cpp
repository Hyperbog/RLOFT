#include "rloft/OBJECTS.hpp"

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
constexpr std::size_t kNameBytes = 40U;
constexpr std::size_t kRecordBytes = ObjectDefinition::kSerializedSize;

std::uint16_t read16(const std::uint8_t* p) noexcept {
    return static_cast<std::uint16_t>(
        static_cast<unsigned>(p[0]) | (static_cast<unsigned>(p[1]) << 8U));
}
std::uint32_t read32(const std::uint8_t* p) noexcept {
    return static_cast<std::uint32_t>(p[0]) |
           (static_cast<std::uint32_t>(p[1]) << 8U) |
           (static_cast<std::uint32_t>(p[2]) << 16U) |
           (static_cast<std::uint32_t>(p[3]) << 24U);
}
std::int8_t signed8(std::uint8_t b) noexcept {
    const int n = b <= 127U ? static_cast<int>(b) : static_cast<int>(b) - 256;
    return static_cast<std::int8_t>(n);
}
void write16(std::uint8_t* p, std::uint16_t value) noexcept {
    p[0] = static_cast<std::uint8_t>(value);
    p[1] = static_cast<std::uint8_t>(value >> 8U);
}
void write32(std::uint8_t* p, std::uint32_t value) noexcept {
    for (unsigned i = 0; i != 4U; ++i)
        p[i] = static_cast<std::uint8_t>(value >> (8U * i));
}
} // namespace

std::vector<ObjectDefinition> decode_objects_dat(
    const std::vector<std::uint8_t>& bytes) {
    if (bytes.size() < 4U) throw std::runtime_error("OBJECTS.DAT count missing");
    const auto count = static_cast<std::size_t>(read32(bytes.data()));
    if ((bytes.size() - 4U) % kRecordBytes != 0U ||
        count != (bytes.size() - 4U) / kRecordBytes) {
        throw std::runtime_error("OBJECTS.DAT count/0x41 record geometry mismatch");
    }
    std::vector<ObjectDefinition> objects;
    objects.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        const auto* r = bytes.data() + 4U + i * kRecordBytes;
        ObjectDefinition o{};
        std::copy_n(r, kRecordBytes, o.original_bytes.begin());
        const auto end = std::find(r, r + kNameBytes, std::uint8_t{0});
        if (end == r + kNameBytes) throw std::runtime_error("OBJECTS.DAT name missing NUL");
        o.name.assign(reinterpret_cast<const char*>(r),
                      static_cast<std::size_t>(end - r));
        o.scene_shape_base_local = read16(r + 0x28U);
        o.scene_shape_stride = r[0x2AU];
        o.scene_influence_step = r[0x2BU];
        o.unresolved_2c = r[0x2CU];
        o.ui_graphic_index = read16(r + 0x2DU);
        o.item_category = r[0x2FU];
        o.weight_half_units = r[0x30U];
        o.container_state_index = signed8(r[0x31U]);
        o.intrinsic_magic_marker = read16(r + 0x32U);
        o.armor_class_adjustment = signed8(r[0x34U]);
        o.raw_word_35 = read16(r + 0x35U);
        o.raw_byte_35_low2 = static_cast<std::uint8_t>(o.raw_word_35 & 3U);
        o.enchantment = r[0x37U];
        o.damage_dice_count = r[0x38U];
        o.damage_dice_sides = r[0x39U];
        o.damage_per_die_bonus = r[0x3AU];
        o.damage_lg_dice = r[0x3BU];
        o.damage_lg_sides = r[0x3CU];
        o.damage_lg_bonus = r[0x3DU];
        o.range_short = r[0x3EU];
        o.range_medium = r[0x3FU];
        o.range_long = r[0x40U];
        objects.push_back(std::move(o));
    }
    return objects;
}

std::vector<std::uint8_t> encode_objects_dat(
    const std::vector<ObjectDefinition>& objects) {
    const auto count = objects.size();
    if (count > std::numeric_limits<std::uint32_t>::max() ||
        count > (std::vector<std::uint8_t>{}.max_size() - 4U) / kRecordBytes) {
        throw std::invalid_argument("OBJECTS.DAT count too large");
    }
    std::vector<std::uint8_t> bytes(4U + count * kRecordBytes);
    write32(bytes.data(), static_cast<std::uint32_t>(count));
    for (std::size_t i = 0; i < count; ++i) {
        const auto& o = objects[i];
        if (o.name.size() >= kNameBytes || o.name.find('\0') != std::string::npos)
            throw std::invalid_argument("OBJECTS.DAT name must fit with NUL");
        if (o.raw_byte_35_low2 != (o.raw_word_35 & 3U))
            throw std::invalid_argument("OBJECTS.DAT attack pair alias conflicts with word");
        auto* r = bytes.data() + 4U + i * kRecordBytes;
        std::copy(o.original_bytes.begin(), o.original_bytes.end(), r);
        std::copy(o.name.begin(), o.name.end(), r);
        r[o.name.size()] = 0;
        write16(r + 0x28U, o.scene_shape_base_local);
        r[0x2AU] = o.scene_shape_stride;
        r[0x2BU] = o.scene_influence_step;
        r[0x2CU] = o.unresolved_2c;
        write16(r + 0x2DU, o.ui_graphic_index);
        r[0x2FU] = o.item_category;
        r[0x30U] = o.weight_half_units;
        r[0x31U] = static_cast<std::uint8_t>(o.container_state_index);
        write16(r + 0x32U, o.intrinsic_magic_marker);
        r[0x34U] = static_cast<std::uint8_t>(o.armor_class_adjustment);
        write16(r + 0x35U, o.raw_word_35);
        r[0x37U] = o.enchantment;
        r[0x38U] = o.damage_dice_count;
        r[0x39U] = o.damage_dice_sides;
        r[0x3AU] = o.damage_per_die_bonus;
        r[0x3BU] = o.damage_lg_dice;
        r[0x3CU] = o.damage_lg_sides;
        r[0x3DU] = o.damage_lg_bonus;
        r[0x3EU] = o.range_short;
        r[0x3FU] = o.range_medium;
        r[0x40U] = o.range_long;
    }
    return bytes;
}

} // namespace rloft
