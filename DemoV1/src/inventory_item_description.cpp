#include "rloft/inventory_item_description.hpp"

#include <cstdint>
#include <stdexcept>
#include <string>

namespace rloft {

std::string InventoryItemDescriptionModel::weight_text(
    std::uint32_t half_pounds) {
    return std::to_string(half_pounds/2U) +
        ((half_pounds&1U)!=0U?".5":"");
}

std::string InventoryItemDescriptionModel::describe(
    const ObjectCatalog& objects,std::int16_t object_id,
    std::uint8_t known) {
    if(object_id==-1)return {};
    if(object_id<0)
        throw std::out_of_range("inventory description object ID");
    const auto& item=objects.at(
        static_cast<std::size_t>(object_id));
    std::string name=item.name;
    if(known!=0U && item.enchantment!=0U) {
        //  CODE.2 0FA9C reveals enchantment only on identified items.
        name+=" +"+std::to_string(item.enchantment);
    }
    return name+": WEIGHT:"+weight_text(item.weight_half_units);
}

} // namespace rloft
