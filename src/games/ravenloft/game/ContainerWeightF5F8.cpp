#include "games/ravenloft/game/ContainerWeightF5F8.h"
#include <cstdint>
#include <stdexcept>

namespace ravenloft::game {
namespace {
int signedByte(std::uint8_t v){return static_cast<std::int8_t>(v);}
}

int ContainerWeightF5F8::evaluate(int containerIndex,
                                  const data::ObjectDatabase& objects,
                                  const data::QuickStartSave& save){
    std::uint32_t sum=0;
    // 0xF5FE..0xF603 / 0xF657..0xF663: descriptor index 0x14 is a literal
    // special case and returns OBJECTS[80].weight (+0x30), not descriptor contents.
    if(containerIndex==0x14){
        return objects.at(80).raw[0x30];
    }
    // The shipped data contains the original object-329 defect: persistent index 62,
    // immediately past the 62 serialized descriptors. CODE.2's adjacent initialized
    // bytes are zero, so its effective capacity is zero and F5F8 returns zero.
    if(containerIndex==62)return 0;
    if(containerIndex<0 || static_cast<std::size_t>(containerIndex)>=data::kContainerDescriptorCount)
        throw std::runtime_error("F5F8 container index outside reconstructed descriptor domain");

    const auto& items=save.containerItems[static_cast<std::size_t>(containerIndex)];
    for(const std::int16_t itemId:items){
        if(itemId==-1)continue;
        if(itemId<0 || static_cast<std::size_t>(itemId)>=objects.count())
            throw std::runtime_error("F5F8 contained object outside OBJECTS.DAT");
        const auto& r=objects.at(static_cast<std::size_t>(itemId)).raw;
        sum += r[0x30];
        if(r[0x2F]==1){
            sum += static_cast<std::uint32_t>(evaluate(signedByte(r[0x31]),objects,save));
        }
    }
    // Original returns MOVZX EAX,BX: accumulated weight truncates to 16 bits.
    return static_cast<int>(static_cast<std::uint16_t>(sum));
}

} // namespace ravenloft::game
