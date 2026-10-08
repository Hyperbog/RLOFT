#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "rloft/game_source.hpp"

namespace rloft {

struct ObjectDefinition final {
    static constexpr std::size_t kSerializedSize = 0x41;

    std::string name;
    std::uint16_t scene_shape_base_local{};
    std::uint8_t scene_shape_stride{};
    std::uint8_t scene_influence_step{};
    std::uint16_t ui_graphic_index{};
    std::uint8_t item_category{};
    // Raw OBJECTS.DAT record +35h low two bits. CODE.2 240A8 /
    // 24478 tests this exact mask for hand-eligibility branches;
    // the remaining high bits are not interpreted as a modern flag.
    std::uint8_t raw_byte_35_low2{};
    // Original CODE.2 E5F0: recovery uses record WORD +35h bits 2..5.
    // Keep the complete little-endian source word, not only the
    // +35h qualifier mask consumed by 240A8/24478.
    std::uint16_t raw_word_35{};
    std::uint8_t weight_half_units{};
    std::int8_t container_state_index{};
    std::uint8_t enchantment{};
};

class ObjectCatalog final {
public:
    [[nodiscard]] static ObjectCatalog parse(
        const std::vector<std::uint8_t>& bytes);
    [[nodiscard]] static ObjectCatalog load(
        const GameSource& source);

    [[nodiscard]] std::size_t size() const noexcept {
        return objects_.size();
    }
    [[nodiscard]] const ObjectDefinition& at(
        std::size_t object_id) const;

private:
    std::vector<ObjectDefinition> objects_;
};

}  // namespace rloft
