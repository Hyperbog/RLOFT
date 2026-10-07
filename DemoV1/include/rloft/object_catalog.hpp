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
    std::uint16_t ui_graphic_index{};
    std::uint8_t item_category{};
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
