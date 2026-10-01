#include "NameScreen3020.h"

#include <array>
#include <cstdint>

namespace ravenloft::game {
namespace {
std::uint32_t add32(std::uint32_t a, std::uint32_t b) { return a + b; }
std::uint32_t mul32(std::uint32_t a, std::uint32_t b) { return a * b; }
}

void NameScreen3020::run(NameScreenState3020& state,
                         const NameScreenOps3020& ops) {
    (void)state;
    const std::array<std::int32_t,9> slots{
        -1,-1,-1,-1,-1,-1,-1,0x14,-1
    };
    ops.call1464(6, slots, -1, -1, 0x2E);

    ops.call6588(ops.readByte(0x7138u));
    ops.draw.call6C60(0, 0x5B, 0x88, 0x14, 7);
    ops.call53E4(ops.readDword(0x7138u));
    ops.writeByte(0x7138u, static_cast<std::uint8_t>(ops.readByte(0x7138u) ^ 1u));

    TextEntryState2DFC textState{};
    TextEntry2DFC::run(0x93, 0x0F, textState, ops.textEntry);

    const std::uint32_t oldHandle = ops.readDword(0x0958u);
    if (oldHandle != 0) ops.call7830(oldHandle);

    ops.call6588(ops.readByte(0x7138u));
    ops.draw.call6C60(0, 0x5B, 0x88, 0x14, 7);

    const std::uint32_t selected = ops.readDword(0x04CCu);
    const std::uint32_t nameAddress = add32(mul32(selected, 0x248u), 0x67F8u);
    ops.draw.call7D80(0x93, 0x88, 0x30,
                      reinterpret_cast<const char*>(static_cast<std::uintptr_t>(nameAddress)), 0);

    ops.writeDword(0x0958u, ops.call7868(0x5B, 0x88, 0x7A, 0x30, 0xFF));
    ops.writeDword(0x092Cu, 0u);
    ops.writeDword(0x0930u, 1u);
    ops.call15B0(-1, -1, -1, -1);
}

} // namespace ravenloft::game
