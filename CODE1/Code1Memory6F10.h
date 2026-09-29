#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct Code1DosMemoryResult6F10 {
    bool carry = false;
    std::uint16_t segment = 0;
    std::uint16_t selector = 0;
    std::uint16_t largestParagraphs = 0;
};

struct Code1LinearMemoryResult709C {
    bool carry = false;
    std::uint32_t address = 0;
    std::uint32_t handle = 0;
};

struct Code1MemoryOps6F10 {
    std::function<Code1DosMemoryResult6F10(std::uint16_t)> allocateDosMemory;
    std::function<bool(std::uint16_t)> freeDosMemory;
    std::function<std::int32_t(std::uint16_t)> callDA6C;
    std::function<std::uint32_t(std::uint16_t)> selectorBase;
    std::function<std::int32_t(std::int32_t, std::uint32_t&)> callDA48;
    std::function<bool(std::uint16_t, std::uint16_t)> resizeLinear0502;
    std::function<Code1LinearMemoryResult709C(std::uint16_t, std::uint16_t)> allocateLinear0501;
    std::function<std::uint32_t()> freeMemoryInfoFirstDword;
    std::function<std::uint8_t*(std::uint32_t)> resolveLinear;

    std::function<void(std::int32_t)> callDisplay64DC;
    std::function<void(std::int32_t)> callPrint61D0;
    std::function<void()> callCF98;
    std::function<void()> call8424;
    std::function<void(std::int32_t)> call97D4;

    std::function<std::int32_t()> callDAA6;
    std::function<std::uintptr_t(std::size_t)> callDAC9;
    std::function<std::int32_t(std::int32_t)> call9378;
    std::function<void()> call94D0;
    std::function<void()> call9520;
    std::function<void()> call7184;
    std::function<void(std::int32_t, std::uintptr_t)> call9874;
    std::function<void(std::int32_t)> call9814;
    std::function<void(std::uintptr_t)> callDB4B;
};

class Code1Memory6F10 {
public:
    // CODE.1 0x6F05..0x6F0F is zero/data padding.
    // CODE.1 0x6F10..0x6F71.
    static std::uint32_t allocateDos(std::int32_t bytes,
                                     std::uint32_t& farPointer,
                                     std::uint16_t& selector,
                                     const Code1MemoryOps6F10& ops);

    // CODE.1 0x6F74..0x6FAB.
    static std::int32_t freeDos(std::uint16_t selector,
                                const Code1MemoryOps6F10& ops);

    // CODE.1 0x6FAC..0x6FBF.
    static std::int32_t call6FAC(std::uint16_t value,
                                 const Code1MemoryOps6F10& ops);

    // CODE.1 0x6FC0..0x6FF1.
    static std::uint32_t selectorBase(std::uint16_t selector,
                                      const Code1MemoryOps6F10& ops);

    // CODE.1 0x6FF4..0x705A.
    static std::uint32_t allocateSelectorBlock(std::int32_t bytes,
                                               const Code1MemoryOps6F10& ops);

    // CODE.1 0x705C..0x7098.
    static std::int32_t resizeLinear(std::uint32_t value,
                                     const Code1MemoryOps6F10& ops);

    // CODE.1 0x709C..0x70FB.
    static std::uint32_t allocateLinear(std::uint32_t size,
                                        const Code1MemoryOps6F10& ops);

    // CODE.1 0x70FC..0x713A.
    static std::uint32_t queryLargestDos(const Code1MemoryOps6F10& ops);

    // CODE.1 0x713C..0x7181.
    static std::uint32_t queryFreeMemory(const Code1MemoryOps6F10& ops);

    // CODE.1 0x7184..0x71B8.
    static void fatal7184(const Code1MemoryOps6F10& ops);

    // CODE.1 0x71BC..0x71C7.
    static std::int32_t heapInit71BC(const Code1MemoryOps6F10& ops);

    // CODE.1 0x71C8..0x723C.
    static std::uintptr_t heapAllocate71C8(std::size_t bytes,
                                           const Code1MemoryOps6F10& ops);

    // CODE.1 0x7240..0x729F.
    static void heapFree7240(std::uintptr_t pointer,
                             const Code1MemoryOps6F10& ops);

    // CODE.1 0x72A0..0x72C8 is the already reconstructed strcpy helper.
};

} // namespace ravenloft::game
