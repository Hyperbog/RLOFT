#include "games/ravenloft/game/InventoryGridClick11FB0.h"

namespace ravenloft::game {
namespace {
std::uint16_t rawU16(const data::CharacterRecord& c,std::size_t p){
    return static_cast<std::uint16_t>(c.raw[p]) |
           static_cast<std::uint16_t>(static_cast<std::uint16_t>(c.raw[p+1])<<8u);
}
}

std::optional<std::size_t> InventoryGridClick11FB0::mapSlot(std::int32_t x,std::int32_t localY){
    // Caller 0x12F9B..0x12FB3 owns these inclusive bounds.
    if(x<kLeft || x>kRight || localY<kTop || localY>kBottom)return std::nullopt;

    // 0x11FCA..0x12014. The second IDIV in each axis leaves the first
    // quotient's remainder in EDX. Within the caller-owned rectangle the
    // quotients are only 0..2 and 0..3, but retain the literal %32/%16 form.
    const std::int32_t column=((x-kLeft)/kColumnPitch)%0x20;
    const std::int32_t row=((localY-kTop)/kRowPitch)%0x10;
    return static_cast<std::size_t>(column*4+row);
}

InventoryGridClickResult11FB0 InventoryGridClick11FB0::execute(
    std::int32_t screenX,
    std::int32_t screenY,
    std::uint32_t clickFlags,
    HeldObjectState1A344& held,
    std::array<data::CharacterRecord,4>& party,
    const InventoryGridClickContext11FB0& context){

    InventoryGridClickResult11FB0 result{};

    // 0x12E7B..0x12E9C: global Y selects one of four 80-pixel party panels,
    // then becomes panel-local Y. The platform caller keeps screenY within
    // the original four-panel range; reject anything else instead of indexing
    // outside the reconstructed party array.
    if(screenY<0)return result;
    const std::int32_t character=screenY/kPanelHeight;
    if(character<0 || character>=static_cast<std::int32_t>(party.size()))return result;
    const std::int32_t localY=screenY-character*kPanelHeight;
    const auto ci=static_cast<std::size_t>(character);
    result.characterIndex=ci;

    // 0x12E9E..0x12EB5: inactive character panels (status bit 15) do not
    // process any of the normal inventory UI branches.
    if((rawU16(party[ci],0x10)&0x8000u)!=0)return result;

    // 0x12F9B..0x12FBA: inclusive inventory rectangle and click-bit-0 gate.
    if((clickFlags&0x1u)==0)return result;
    const auto slot=mapSlot(screenX,localY);
    if(!slot)return result;
    result.slotIndex=*slot;

    // 0x11FB1..0x11FC9: button bit 1 or a nonzero DS:6F90 panel mode makes
    // 0x11FB0 return zero so the outer dispatcher can continue elsewhere.
    if((clickFlags&0x2u)!=0 || context.panelMode6F90[ci]!=0)return result;

    // 0x12012..0x12020: PUSH 1, PUSH slot, PUSH character; CALL 0x117A0.
    // 0x11FB0 returns one after the call even if 0x117A0 itself performs no
    // exchange, so expose both the consumed-click result and exchange result.
    const auto exchange=InventorySlotExchange117A0::executeOpeningEmptySlot(
        ci,*slot,1,held,party,context.exchangeContext);
    result.handled=true;
    result.exchangeHandled=exchange.handled;
    return result;
}

} // namespace ravenloft::game
