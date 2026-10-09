#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_pickup_ribbon_text.hpp"
#include "rloft/combat_irq_message.hpp"

#include <cassert>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

int main() {
    using rloft::OriginalPickupRibbonTextModel;
    const std::size_t base=0x75F24U;
    const std::size_t pointer_at=base+0xD67CU;
    const std::size_t prefix_at=base+0x11B4U;
    std::vector<std::uint8_t> source(pointer_at+4U,0U);
    source[pointer_at]=0xB4U;
    source[pointer_at+1U]=0x11U;
    const std::string original="You pick up ";
    for(std::size_t i=0U;i<original.size();++i)
        source[prefix_at+i]=static_cast<std::uint8_t>(original[i]);
    source[prefix_at+original.size()]=0U;
    const auto prefix=
        OriginalPickupRibbonTextModel::source_prefix_from_code2(source);
    assert(prefix.has_value());
    assert(std::string(prefix->begin(),prefix->end())==original);

    rloft::ObjectDefinition relic{};
    relic.name="FINGER OF COMMANDING";
    relic.enchantment=0U;
    const auto composed=OriginalPickupRibbonTextModel::compose_unenchanted(
        *prefix,relic,false);
    assert(composed.has_value());
    assert(composed->back()==0U);
    assert(std::string(composed->begin(),composed->end()-1)==
           "You pick up FINGER OF COMMANDING");
    assert(OriginalPickupRibbonTextModel::compose_unenchanted(
        *prefix,relic,true)==composed);

    // Reuse the EXACT CODE.2 1C21C +/- IRQ-byte service, not a
    // new timer based on render FPS or the gameplay A7E4 clock.
    const auto queued=rloft::CombatIrqMessageModel::queue({
        0x1234U,*composed,
        std::vector<std::uint8_t>(composed->size(),0xEEU)});
    assert(queued.has_value());
    assert(queued->deadline_cf54==0x128EU);
    assert(queued->pending_flag_cf59==1U);
    assert(queued->bytes_copied==composed->size());
    assert(queued->destination_buffer==*composed);
    assert(!rloft::CombatIrqMessageModel::deadline_passed(
        0x128EU,queued->deadline_cf54));
    assert(rloft::CombatIrqMessageModel::deadline_passed(
        0x128FU,queued->deadline_cf54));

    relic.enchantment=1U;
    assert(!OriginalPickupRibbonTextModel::compose_unenchanted(
        *prefix,relic,true));
    assert(OriginalPickupRibbonTextModel::compose_unenchanted(
        *prefix,relic,false)==composed);
    assert(!OriginalPickupRibbonTextModel::source_prefix_from_code2(
        std::vector<std::uint8_t>{}));
    source[pointer_at]=0xFFU; source[pointer_at+1U]=0xFFU;
    assert(!OriginalPickupRibbonTextModel::source_prefix_from_code2(source));
}
