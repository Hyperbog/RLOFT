#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>

namespace ravenloft::game {

struct SystemState95A8 {
    std::uint32_t callbackValue23D0 = 0;

    std::uint8_t displayMode23D4 = 0xFF;
    std::uint8_t displayRows23D6 = 0x0C;
    std::int16_t word23DF = 0;
    std::int16_t word23EB = 0;
    std::uint8_t activePage23E5 = 0;
    std::uint8_t drawPage23E6 = 0;
    std::uint8_t byte2400 = 0;

    std::uint8_t flags2B01 = 0;
    std::uint8_t busy2B02 = 0;
    std::int32_t value2B0C = 0;
    std::int32_t specialItem2B10 = -1;
    std::uint8_t color2B14 = 0;

    std::uint32_t flags2B84 = 0;
    std::uint32_t flags2F48 = 0;

    std::int32_t mode309C = 0;
    std::uint32_t externalConfigSource30A4 = 0;

    std::array<std::uint8_t, 0x100>* config2AA0 = nullptr;
    std::uintptr_t input2A9C = 0;

    bool resourceIndexPresent = false;
};

struct SystemOps95A8 {
    std::function<void(std::uint32_t, std::uint16_t)> registerFarCallbackE280;
    std::function<std::uint16_t()> currentCodeSegment;

    std::function<void()> installHandlers8398;
    std::function<void(std::int32_t)> seed64B0;

    std::function<std::array<std::uint8_t, 0x100>*()> allocateConfig256;
    std::function<std::array<std::uint8_t, 0x100>*(std::uint32_t)> copyConfigE360;

    std::function<std::int32_t(const std::string&)> openRead62A8;
    std::function<std::int32_t(void*, std::int32_t, std::int32_t)> read6304;
    std::function<std::int32_t(std::int32_t)> close62F0;

    std::function<std::uintptr_t()> callCF98;

    std::function<void(std::uint8_t*, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> callC2D0;
    std::function<void(std::uint8_t*, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> callB0F8;

    std::function<void(std::int32_t, std::int32_t)> callD10C;
    std::function<void(std::int32_t, std::int32_t)> callD130;

    std::function<void(std::uint32_t, std::int32_t)> openGroups8A04;

    std::function<void(std::int32_t, std::int32_t)> unpin9488;
    std::function<void(std::int32_t, std::int32_t)> load8E3C;
    std::function<void(std::int32_t, std::int32_t)> pin9440;

    std::function<void()> callB430;
    std::function<void()> callC4A0;
    std::function<void(std::int32_t)> callE6EA;
    std::function<void()> shutdownIndex8C40;
    std::function<void()> restoreHandlers8424;

    std::function<void()> callEDE0;
    std::function<void(std::int32_t)> callCBF0;

    std::function<void(std::int32_t)> display64DC;
    std::function<void(const std::string&)> dosPrint61D0;

    std::function<std::uintptr_t(std::size_t)> heapAllocate71C8;
    std::function<void(std::uintptr_t)> heapFree7240;

    std::function<void(std::uintptr_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> capture66C0;
    std::function<void(std::uintptr_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> restore6744;

    std::function<void(std::int32_t, std::int32_t, std::int32_t,
                       std::int32_t, std::uint8_t)> lineA05C;

    std::function<std::int32_t(const std::string&, std::int32_t)> textWidth7AB4;
    std::function<void(std::int32_t, std::int32_t)> setTextPos7D68;
    std::function<void(const std::string&, std::int32_t)> drawText7C0C;
    std::function<std::int32_t()> getTextX7DDC;
    std::function<std::int32_t()> getTextY7DE8;

    std::function<void(std::uint8_t)> setActivePage6588;
    std::function<void(std::int32_t)> setDisplayParamA474;

    std::function<std::uint32_t(std::uint32_t, std::uint32_t)> wait87D8;
    std::function<std::uint32_t()> readKey82C0;
};

class System95A8 {
public:
    // Portable observable reconstruction of the far callback at
    // CODE.1 0x95A8..0x95BA. 0x95BB is alignment.
    static std::int32_t farCallback(
        std::uint32_t thirdArgument,
        SystemState95A8& state) noexcept;

    // Direct reconstruction of CODE.1 0x95BC..0x9723.
    static void initialize(
        std::uint32_t resourcePrefix,
        std::int32_t groupCount,
        SystemState95A8& state,
        const SystemOps95A8& ops);

    // Direct reconstruction of CODE.1 0x9724..0x9776.
    // CODE.1 0x9777 is alignment.
    static void setSpecial(
        std::int32_t value2B0C,
        std::int32_t specialItem,
        std::uint8_t color,
        SystemState95A8& state,
        const SystemOps95A8& ops);

    // Direct reconstruction of CODE.1 0x9778..0x97D3.
    static void shutdown(
        SystemState95A8& state,
        const SystemOps95A8& ops);

    // Direct reconstruction of CODE.1 0x97D4..0x97E4.
    // CODE.1 0x97E5..0x97E7 is alignment.
    static void exitWithCode(
        std::int32_t code,
        const SystemOps95A8& ops);

    // Direct reconstruction of CODE.1 0x97E8..0x9810.
    // CODE.1 0x9811..0x9813 is alignment.
    static void shutdownDisplay(
        std::int32_t exitAfter,
        SystemState95A8& state,
        const SystemOps95A8& ops);

    // Direct reconstruction of CODE.1 0x9814..0x9870.
    // CODE.1 0x9871..0x9873 is alignment.
    static void fatal(
        const std::string& message,
        SystemState95A8& state,
        const SystemOps95A8& ops);

    // Direct reconstruction of CODE.1 0x9874..0x9AB1.
    // CODE.1 0x9AB2..0x9AB3 is alignment.
    static std::int32_t prompt(
        const std::string& message,
        std::int32_t mode,
        SystemState95A8& state,
        const SystemOps95A8& ops);

    // CODE.1 0x9AB4..0x9AC1. 0x9AC2..0x9AC3 alignment.
    static void suspendUi(SystemState95A8& state) noexcept;

    // CODE.1 0x9AC4..0x9AD1. 0x9AD2..0x9AD3 alignment.
    static void resumeUi(SystemState95A8& state) noexcept;

    // CODE.1 0x9AD4..0x9AE7.
    static std::int32_t uiActive(
        const SystemState95A8& state) noexcept;
};

} // namespace ravenloft::game
