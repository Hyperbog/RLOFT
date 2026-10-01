#include "CharacterInit3884.h"

#include "CharacterHitRoll37C0.h"
#include "CharacterNormalize3404.h"
#include "CharacterProgress3208.h"
#include "CharacterRolls334C.h"
#include "CharacterVital32F4.h"

namespace ravenloft::game {

void CharacterInit3884::initialize(const CharacterInitOps3884& o) {
    // 0x3887, 0x388C, 0x3891, 0x3896, 0x389B: exact call order.
    CharacterProgress3208::run({o.readByte, o.readWord, o.readDword,
                                o.writeWord, o.writeDword});
    CharacterVital32F4::initialize({o.readByte, o.readWord, o.readDword,
                                    o.writeWord, o.call0010});
    CharacterRolls334C::initialize({o.readByte, o.readDword, o.writeByte,
                                    o.call0010, o.call84A0});
    CharacterHitRoll37C0::initialize({o.readByte, o.readWord, o.readDword,
                                      o.writeWord, o.call0010});
    CharacterNormalize3404::apply({o.readByte, o.readWord, o.readDword,
                                   o.writeByte, o.writeWord, o.writeDword});
}

} // namespace ravenloft::game
