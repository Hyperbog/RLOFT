#include "rloft/CHARS.hpp"

#include <algorithm>
#include <cstdint>
#include <stdexcept>

namespace rloft {
namespace {

constexpr std::size_t kNameSize = 16U;
constexpr std::size_t kItemsOffset = 0x40U;
constexpr std::size_t kItemCount = 21U;

std::size_t base_for(std::size_t actor) {
    if (actor >= PartyImage::kActorCount) {
        throw std::out_of_range("CHARS actor index outside 0..3");
    }
    return actor * PartyImage::kActorSize;
}

std::uint16_t u16(const PartyImage& image, std::size_t pos) noexcept {
    return static_cast<std::uint16_t>(
        static_cast<unsigned>(image.bytes[pos]) |
        (static_cast<unsigned>(image.bytes[pos + 1U]) << 8U));
}

std::int8_t s8(std::uint8_t value) noexcept {
    const int signed_value = value <= 0x7FU
        ? static_cast<int>(value) : static_cast<int>(value) - 0x100;
    return static_cast<std::int8_t>(signed_value);
}

std::int16_t s16(const PartyImage& image, std::size_t pos) noexcept {
    const auto value = u16(image, pos);
    const int signed_value = value <= 0x7FFFU
        ? static_cast<int>(value) : static_cast<int>(value) - 0x10000;
    return static_cast<std::int16_t>(signed_value);
}

std::int32_t s32(const PartyImage& image, std::size_t pos) noexcept {
    const auto value = static_cast<std::uint32_t>(image.bytes[pos]) |
        (static_cast<std::uint32_t>(image.bytes[pos + 1U]) << 8U) |
        (static_cast<std::uint32_t>(image.bytes[pos + 2U]) << 16U) |
        (static_cast<std::uint32_t>(image.bytes[pos + 3U]) << 24U);
    const std::int64_t signed_value = value <= 0x7FFFFFFFU
        ? static_cast<std::int64_t>(value)
        : static_cast<std::int64_t>(value) - 0x100000000LL;
    return static_cast<std::int32_t>(signed_value);
}

void put16(PartyImage& image, std::size_t pos, std::uint16_t value) noexcept {
    image.bytes[pos] = static_cast<std::uint8_t>(value & 0xFFU);
    image.bytes[pos + 1U] = static_cast<std::uint8_t>(value >> 8U);
}

} // namespace

PartyImage CharsCodec::decode(const std::vector<std::uint8_t>& file) {
    if (file.size() != PartyImage::kSize) {
        throw std::invalid_argument("CHARS must be exactly 0x920 bytes");
    }
    PartyImage image{};
    std::copy(file.begin(), file.end(), image.bytes.begin());
    return image;
}

std::vector<std::uint8_t> CharsCodec::encode(const PartyImage& image) {
    return {image.bytes.begin(), image.bytes.end()};
}

CharsCharacterCore CharsCodec::core(const PartyImage& image,
                                    std::size_t actor) {
    const auto base = base_for(actor);
    CharsCharacterCore out{};
    const auto start = image.bytes.begin() + static_cast<std::ptrdiff_t>(base);
    const auto terminator = std::find(start, start + kNameSize, 0U);
    out.name.assign(start, terminator);

    out.status = u16(image, base + 0x10U);
    out.occupied = (out.status & 0x8000U) == 0;
    out.dead = out.occupied && (out.status & 0x4000U) != 0;
    out.portrait = image.bytes[base + 0x12U];
    out.sex = image.bytes[base + 0x13U];
    out.race = image.bytes[base + 0x14U];
    for (std::size_t i = 0; i < out.class_codes.size(); ++i) {
        out.class_codes[i] = s8(image.bytes[base + 0x15U + i]);
    }
    out.age = u16(image, base + 0x18U);
    out.alignment = image.bytes[base + 0x1AU];
    out.base_armor_class = s8(image.bytes[base + 0x1BU]);
    out.current_hp = s16(image, base + 0x1CU);
    out.max_hp = s16(image, base + 0x1EU);
    for (std::size_t i = 0; i < out.class_levels.size(); ++i) {
        out.class_levels[i] = s16(image, base + 0x20U + 2U * i);
        out.class_progress[i] = s32(image, base + 0x26U + 4U * i);
    }
    for (std::size_t i = 0; i < out.abilities.size(); ++i) {
        out.abilities[i].base = image.bytes[base + 0x32U + i * 2U];
        out.abilities[i].current = image.bytes[base + 0x33U + i * 2U];
    }
    for (std::size_t i = 0; i < out.items.size(); ++i) {
        out.items[i] = s16(image, base + kItemsOffset + i * 2U);
    }
    return out;
}

void CharsCodec::set_status(PartyImage& image, std::size_t actor,
                            std::uint16_t status) {
    put16(image, base_for(actor) + 0x10U, status);
}

void CharsCodec::set_current_hp(PartyImage& image, std::size_t actor,
                                std::int16_t hp) {
    put16(image, base_for(actor) + 0x1CU, static_cast<std::uint16_t>(hp));
}

void CharsCodec::set_item(PartyImage& image, std::size_t actor,
                          std::size_t slot, std::int16_t object_id) {
    const auto base = base_for(actor);
    if (slot >= kItemCount) {
        throw std::out_of_range("CHARS item index outside 0..20");
    }
    put16(image, base + kItemsOffset + slot * 2U,
          static_cast<std::uint16_t>(object_id));
}

} // namespace rloft
