#include "Code1Memory6F10.h"

namespace ravenloft::game {
namespace {

void write16(std::uint8_t* p, std::uint16_t value) {
    p[0] = static_cast<std::uint8_t>(value);
    p[1] = static_cast<std::uint8_t>(value >> 8);
}

void write32(std::uint8_t* p, std::uint32_t value) {
    for (int i = 0; i < 4; ++i) {
        p[i] = static_cast<std::uint8_t>(value >> (i * 8));
    }
}

std::uint32_t read32(const std::uint8_t* p) {
    return std::uint32_t(p[0]) |
           (std::uint32_t(p[1]) << 8) |
           (std::uint32_t(p[2]) << 16) |
           (std::uint32_t(p[3]) << 24);
}

} // namespace

std::uint32_t Code1Memory6F10::allocateDos(
    std::int32_t bytes,
    std::uint32_t& farPointer,
    std::uint16_t& selector,
    const Code1MemoryOps6F10& ops) {
    const std::int32_t paragraphs =
        (bytes + 0x10) / 0x10;

    const auto result =
        ops.allocateDosMemory(
            static_cast<std::uint16_t>(paragraphs));

    if (result.carry) {
        return 0;
    }

    selector = result.selector;

    farPointer =
        static_cast<std::uint32_t>(
            result.segment) << 16u;

    return static_cast<std::uint32_t>(
        result.segment) << 4u;
}

std::int32_t Code1Memory6F10::freeDos(
    std::uint16_t selector,
    const Code1MemoryOps6F10& ops) {
    return ops.freeDosMemory(selector)
        ? -1
        : 0;
}

std::int32_t Code1Memory6F10::call6FAC(
    std::uint16_t value,
    const Code1MemoryOps6F10& ops) {
    return ops.callDA6C(value);
}

std::uint32_t Code1Memory6F10::selectorBase(
    std::uint16_t selector,
    const Code1MemoryOps6F10& ops) {
    return ops.selectorBase(selector);
}

std::uint32_t
Code1Memory6F10::allocateSelectorBlock(
    std::int32_t bytes,
    const Code1MemoryOps6F10& ops) {
    const std::int32_t units =
        (bytes + 0x0F) / 0x10;

    if (units > 0xFFFF) {
        return 0;
    }

    std::uint32_t selectorToken = 0;

    if (ops.callDA48(
            units,
            selectorToken) != 0) {
        return 0;
    }

    const auto base =
        ops.selectorBase(
            static_cast<std::uint16_t>(
                selectorToken));

    auto* memory =
        ops.resolveLinear(base);

    write16(
        memory,
        static_cast<std::uint16_t>(
            selectorToken));

    write32(memory + 2, 0);

    return base;
}

std::int32_t Code1Memory6F10::resizeLinear(
    std::uint32_t value,
    const Code1MemoryOps6F10& ops) {
    const auto high =
        static_cast<std::uint16_t>(
            value >> 16u);

    const auto low =
        static_cast<std::uint16_t>(value);

    return ops.resizeLinear0502(high, low)
        ? 1
        : 0;
}

std::uint32_t Code1Memory6F10::allocateLinear(
    std::uint32_t size,
    const Code1MemoryOps6F10& ops) {
    const auto high =
        static_cast<std::uint16_t>(
            size >> 16u);

    const auto low =
        static_cast<std::uint16_t>(size);

    const auto result =
        ops.allocateLinear0501(high, low);

    if (result.carry) {
        return 0;
    }

    auto* memory =
        ops.resolveLinear(result.address);

    write16(memory, 0);
    write32(memory + 2, result.handle);

    return result.address;
}

std::uint32_t Code1Memory6F10::queryLargestDos(
    const Code1MemoryOps6F10& ops) {
    const auto result =
        ops.allocateDosMemory(0xFFFFu);

    return result.carry
        ? static_cast<std::uint32_t>(
              result.largestParagraphs) << 4u
        : 0u;
}

std::uint32_t Code1Memory6F10::queryFreeMemory(
    const Code1MemoryOps6F10& ops) {
    return ops.freeMemoryInfoFirstDword();
}

void Code1Memory6F10::fatal7184(
    const Code1MemoryOps6F10& ops) {
    ops.callDisplay64DC(0xFF);
    ops.callPrint61D0(0xB8);
    ops.callCF98();
    ops.call8424();
    ops.call97D4(0x0A);
}

std::int32_t Code1Memory6F10::heapInit71BC(
    const Code1MemoryOps6F10& ops) {
    return ops.callDAA6();
}

std::uintptr_t Code1Memory6F10::heapAllocate71C8(
    std::size_t bytes,
    const Code1MemoryOps6F10& ops) {
    std::uint8_t stage = 0;
    std::uintptr_t base = 0;

    while (base == 0) {
        base =
            ops.callDAC9(
                bytes + 0x0A);

        if (base != 0) {
            break;
        }

        if (ops.call9378(1) != 0) {
            continue;
        }

        if (stage == 0) {
            ops.call94D0();
            ++stage;
        } else if (stage == 1) {
            ops.call9520();
            ++stage;
        } else if (stage == 2) {
            ops.call7184();
        }
    }

    write32(
        reinterpret_cast<std::uint8_t*>(
            base + 6u),
        0x12345678u);

    return base + 0x0Au;
}

void Code1Memory6F10::heapFree7240(
    std::uintptr_t pointer,
    const Code1MemoryOps6F10& ops) {
    const std::uintptr_t base =
        pointer - 0x0Au;

    if (base == 0) {
        ops.call9874(0xE8, 0);
        return;
    }

    auto* memory =
        reinterpret_cast<std::uint8_t*>(
            base);

    const auto magic =
        read32(memory + 6);

    if (magic == 0xABCDEF98u) {
        ops.call9814(0x118);
        return;
    }

    if (magic != 0x12345678u) {
        ops.call9814(0x138);
        return;
    }

    write32(
        memory + 6,
        0xABCDEF98u);

    ops.callDB4B(base);
}

} // namespace ravenloft::game
