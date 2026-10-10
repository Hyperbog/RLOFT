#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/inventory_runtime.hpp"
#include "rloft/object_catalog.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {
void write32(std::vector<std::uint8_t>& b,std::size_t at,std::uint32_t n) {
    for(int i=0;i<4;++i)
        b[at+static_cast<std::size_t>(i)]=
            static_cast<std::uint8_t>(n>>(i*8));
}
}

int main() {
    // Synthetic OBJECTS.DAT: source special case returns object #80's
    // own weight byte (record offset 0x1480) even if descriptor20
    // contains unrelated heavy objects. Unmodified source object80
    // happens to have a weight of zero; set it to 4 here so the
    // special-address semantics are observable.
    std::vector<std::uint8_t> bytes(4U+165U*0x41U,0U);
    write32(bytes,0U,165U);
    const auto bag=4U+80U*0x41U;
    bytes[bag+0x2FU]=1U;
    bytes[bag+0x30U]=4U;
    bytes[bag+0x31U]=0x14U;
    bytes[4U+164U*0x41U+0x30U]=9U;
    const auto objects=rloft::ObjectCatalog::parse(bytes);
    rloft::InventoryRuntimeState inventory{};

    inventory.containers[20].slot_count=8U;
    inventory.containers[20].items.assign(8U,164);
    assert(inventory.container_weight_half_units(objects,20U)==4U);
    inventory.containers[20].items.assign(8U,-1);
    assert(inventory.container_weight_half_units(objects,20U)==4U);

    // Normal containers still recurse through source category-1 items.
    inventory.containers[0].slot_count=2U;
    inventory.containers[0].items={80,164};
    assert(inventory.container_weight_half_units(objects,0U)==17U);
    inventory.containers[1].slot_count=1U;
    inventory.containers[1].items={164};
    assert(inventory.container_weight_half_units(objects,1U)==9U);
}
