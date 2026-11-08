#pragma once

#include <cstdint>
#include <string>
#include "rloft/object_catalog.hpp"

namespace rloft {

// CODE.2 item-description input: (object ID, identified-state byte).
// OBJECTS +00 supplies its exact name; +37 supplies the enchantment
// revealed only if identified; +30 is displayed in half pounds.
class InventoryItemDescriptionModel final {
public:
    [[nodiscard]] static std::string describe(
        const ObjectCatalog& objects,
        std::int16_t object_id,std::uint8_t known);
    [[nodiscard]] static std::string weight_text(
        std::uint32_t half_pounds);
};

} // namespace rloft
