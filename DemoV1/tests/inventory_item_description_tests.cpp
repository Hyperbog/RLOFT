#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/inventory_item_description.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
void set(std::vector<std::uint8_t>& data,std::size_t id,
         const char* name,std::uint8_t weight,std::uint8_t bonus=0) {
    const auto at=4U+id*65U;
    std::size_t i=0U;
    while(name[i]!=0) {data[at+i]=static_cast<std::uint8_t>(name[i]);++i;}
    data[at+0x30U]=weight;
    data[at+0x37U]=bonus;
}
}
int main() {
    std::vector<std::uint8_t> source(4U+4U*65U,0U);
    source[0]=4U;
    set(source,0U,"IVORY SCROLL CASE",1U);
    set(source,1U,"SLING POUCH",1U);
    set(source,2U,"BLUE ROBE",2U);
    set(source,3U,"RING OF PROTECTION",0U,3U);
    const auto objects=rloft::ObjectCatalog::parse(source);
    using rloft::InventoryItemDescriptionModel;
    assert(InventoryItemDescriptionModel::describe(objects,0,0U)==
           "IVORY SCROLL CASE: WEIGHT:0.5");
    assert(InventoryItemDescriptionModel::describe(objects,1,1U)==
           "SLING POUCH: WEIGHT:0.5");
    assert(InventoryItemDescriptionModel::describe(objects,2,0U)==
           "BLUE ROBE: WEIGHT:1");
    assert(InventoryItemDescriptionModel::describe(objects,3,0U)==
           "RING OF PROTECTION: WEIGHT:0");
    assert(InventoryItemDescriptionModel::describe(objects,3,1U)==
           "RING OF PROTECTION +3: WEIGHT:0");
    assert(InventoryItemDescriptionModel::describe(objects,-1,0U).empty());
    assert(InventoryItemDescriptionModel::weight_text(255U)=="127.5");
    bool threw=false;
    try {
        (void)InventoryItemDescriptionModel::describe(objects,4,0U);
    } catch(const std::out_of_range&) {threw=true;}
    assert(threw);
}
