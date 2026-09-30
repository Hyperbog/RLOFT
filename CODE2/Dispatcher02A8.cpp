#include "Dispatcher02A8.h"

#include <cstddef>

namespace ravenloft::game {
namespace {

std::uint16_t read16(std::span<const std::uint8_t> bytes,
                     std::size_t offset) noexcept {
    return static_cast<std::uint16_t>(bytes[offset]) |
           (static_cast<std::uint16_t>(bytes[offset + 1u]) << 8u);
}

void write16(std::span<std::uint8_t> bytes,
             std::size_t offset,
             std::uint16_t value) noexcept {
    bytes[offset] = static_cast<std::uint8_t>(value & 0xFFu);
    bytes[offset + 1u] = static_cast<std::uint8_t>(value >> 8u);
}

void replaceBit2(std::uint8_t* entity, std::uint8_t bit) noexcept {
    entity[1] = static_cast<std::uint8_t>(
        (entity[1] & 0xFBu) | ((bit & 1u) << 2u));
}

std::uint8_t oldBit7(const std::uint8_t* entity) noexcept {
    return static_cast<std::uint8_t>((entity[1] >> 7u) & 1u);
}

} // namespace

void Dispatcher02A8::run(DispatcherState02A8& state,
                         const DispatcherOps02A8& ops,
                         std::int32_t scriptIndex) {
    if (scriptIndex == -1) {
        return;
    }

    const std::size_t block =
        static_cast<std::size_t>(scriptIndex * 0x1A);
    std::size_t entry = block;
    std::int32_t index = 0;
    bool stop = false;

    while (index < state.scripts[block]) {
        const std::uint8_t type = state.scripts[entry + 1u];
        const std::uint8_t a = state.scripts[entry + 2u];
        const std::uint8_t b = state.scripts[entry + 3u];
        const std::uint8_t c = state.scripts[entry + 4u];

        std::uint8_t* entity = ops.call1E8C8(a, b);

        switch (type) {
        case 0:
            if (entity != nullptr) {
                if (entity[0] == 1u) {
                    if (entity[6] == 0u) {
                        break;
                    }
                    const std::uint8_t bit = oldBit7(entity);
                    entity[1] |= 0x04u;
                    ops.call20C0C(entity, a, b);
                    replaceBit2(entity, bit);
                } else {
                    entity[1] |= 0x04u;
                }
            } else {
                ops.call0010(a, b);
            }
            break;

        case 1:
            if (entity != nullptr) {
                if (entity[0] == 1u) {
                    if (entity[6] != 0u) {
                        break;
                    }
                    const std::uint8_t bit = oldBit7(entity);
                    entity[1] |= 0x04u;
                    ops.call20C0C(entity, a, b);
                    replaceBit2(entity, bit);
                } else {
                    entity[1] &= 0xFBu;
                }
            }
            break;

        case 2:
            if (entity != nullptr) {
                if (entity[0] == 1u) {
                    const std::uint8_t bit = oldBit7(entity);
                    entity[1] |= 0x04u;
                    ops.call20C0C(entity, a, b);
                    replaceBit2(entity, bit);
                } else if (entity[0] == 6u) {
                    replaceBit2(entity,
                                static_cast<std::uint8_t>(oldBit7(entity) ^ 1u));
                } else {
                    entity[1] = static_cast<std::uint8_t>(
                        (entity[1] & 0xFCu) |
                        (((entity[1] & 0x03u) ^ 1u) & 0x03u));
                }
            }
            break;

        case 3: {
            // 0x048D: the two 0x8000 words are pushed as DWORD locals by the
            // original. The angle is signed (8-c)/8 remainder * 45.
            const std::int32_t dividend = 8 - static_cast<std::int32_t>(c);
            const std::int32_t remainder = dividend % 8;
            ops.call8A80(c, remainder * 45, 0x00008000u, 0x00008000u);
            break;
        }

        case 4: {
            const std::int32_t cell =
                static_cast<std::int32_t>(state.value25619) * a + b;
            const std::size_t off = static_cast<std::size_t>(cell * 2);
            state.data198E0[off] &= 0xC0u;
            const std::uint16_t prior = read16(state.data198E0, off);
            write16(state.data198E0, off,
                    static_cast<std::uint16_t>(
                        prior | (static_cast<std::uint16_t>(c) & 0x003Fu)));
            break;
        }

        case 5:
            ops.call28110(
                static_cast<std::uint32_t>(state.scripts[entry + 4u]) |
                    (static_cast<std::uint32_t>(
                         state.scripts[entry + 5u]) << 8u) |
                    (static_cast<std::uint32_t>(
                         state.scripts[entry + 6u]) << 16u) |
                    (static_cast<std::uint32_t>(
                         state.scripts[entry + 7u]) << 24u),
                0xFF, 0, 0);
            break;

        case 6:
            ops.call28DCC(0x9C4);
            ops.call28EF0();
            ops.call28C54(
                static_cast<std::uint32_t>(state.scripts[entry + 4u]) |
                    (static_cast<std::uint32_t>(
                         state.scripts[entry + 5u]) << 8u) |
                    (static_cast<std::uint32_t>(
                         state.scripts[entry + 6u]) << 16u) |
                    (static_cast<std::uint32_t>(
                         state.scripts[entry + 7u]) << 24u),
                1);
            break;

        case 7:
            ops.case5B3Sequence(c);
            break;

        case 8:
            ops.call1C124(c);
            break;

        case 9:
            ops.call1C160(c);
            break;

        case 10:
            ops.call1C1A0(c);
            break;

        case 11:
            if (c == 0x0Bu) {
                ops.call0074();
            } else if (c == 0x0Cu) {
                ops.call00B8();
            } else if (c == 0x0Du) {
                if (ops.call01FC() != 0 && entity != nullptr) {
                    entity[1] &= 0xFBu;
                }
            } else {
                ops.call13EF8(c);
                stop = true;
            }
            break;

        case 12:
            state.valueBF44 = c;
            break;

        default:
            break;
        }

        if (stop) {
            return;
        }

        entry += 5u;
        ++index;
    }
}

} // namespace ravenloft::game
