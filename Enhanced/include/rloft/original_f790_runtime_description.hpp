#pragma once

#include "rloft/inventory_runtime.hpp"
#include "rloft/object_catalog.hpp"
#include "rloft/original_f790_resolved_description.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace rloft {

// Source-only gateway from ACTUAL MUTABLE inventory and a RAW original
// OBJECTS.DAT image into the decoded CODE.2 F790 item text builder.
// No UI-owned inventory cache, RNG calls, state mutation or 70-Hz redraw.
struct OriginalF790RuntimeDescription final {
    [[nodiscard]] static std::string compose(
        const std::vector<std::uint8_t>& original_objects_bytes,
        const ObjectCatalog& objects,
        const InventoryRuntimeState& live_inventory,
        std::int32_t held_id,
        bool known,
        const OriginalF790StaticResources& original_code2,
        const OriginalTTextArchive& original_t) {
        // Original F790's empty hand is a restore-only event; it
        // never attempts to format an object description.
        if(held_id==-1)return {};
        if(held_id<0 ||
           static_cast<std::uint64_t>(held_id)>=objects.size())
            throw std::out_of_range("F790 held item outside object catalog");
        if(original_objects_bytes.size()<4U)
            throw std::invalid_argument("missing original OBJECTS.DAT count");
        const auto original_count=
            static_cast<std::uint32_t>(original_objects_bytes[0]) |
            (static_cast<std::uint32_t>(original_objects_bytes[1])<<8U) |
            (static_cast<std::uint32_t>(original_objects_bytes[2])<<16U) |
            (static_cast<std::uint32_t>(original_objects_bytes[3])<<24U);
        if(original_count!=objects.size() ||
           original_count >
             (std::numeric_limits<std::size_t>::max()-4U)/
                OriginalF790ItemDescriptionSource::Record{}.size() ||
           original_objects_bytes.size()!=
               4U+static_cast<std::size_t>(original_count)*0x41U)
            throw std::invalid_argument(
                "F790 original OBJECTS.DAT / object catalog mismatch");

        OriginalF790ItemDescriptionSource::Record raw{};
        const auto start=4U+static_cast<std::size_t>(held_id)*raw.size();
        std::copy_n(original_objects_bytes.begin()+start,raw.size(),raw.begin());
        const auto& parsed=objects.at(static_cast<std::size_t>(held_id));
        // A catalog constructed from another OBJECTS image is NOT a
        // valid basis for F790. Verify the actual NUL-terminated name
        // as well as the record fields that the display consumes.
        std::size_t name_bytes=0U;
        while(name_bytes<40U && raw[name_bytes]!=0U)++name_bytes;
        if(name_bytes==40U ||
           parsed.name!=std::string(
               reinterpret_cast<const char*>(raw.data()),name_bytes))
            throw std::invalid_argument(
                "F790 source item name and catalog diverged");
        if(parsed.item_category!=raw[0x2f] ||
           parsed.weight_half_units!=raw[0x30] ||
           static_cast<std::uint8_t>(parsed.container_state_index)!=
               raw[0x31] ||
           parsed.enchantment!=raw[0x37])
            throw std::invalid_argument(
                "F790 raw object and parsed catalog diverged");

        return OriginalF790ResolvedDescription::compose(
            raw,held_id,known,original_code2,original_t,
            [&](int subtype)->std::uint32_t {
                if(subtype<0 ||
                   subtype>=static_cast<int>(
                     InventoryRuntimeState::kRuntimeDescriptorCount))
                    throw std::out_of_range(
                        "F790 signed current container subtype");
                return live_inventory.container_weight_half_units(
                    objects,static_cast<std::size_t>(subtype));
            });
    }
};
} // namespace rloft
