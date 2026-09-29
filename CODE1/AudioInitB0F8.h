#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>

namespace ravenloft::game {

struct AudioBackendRecordB0F8 {
    std::uint32_t field0 = 0;
    std::int32_t field4 = 0;
};

struct AudioInitStateB0F8 {
    bool initialized2B84 = false;

    std::int32_t object2B90 = -1;

    std::uint32_t mainLinear2B9C = 0;
    std::uint32_t mainFar2BA0 = 0;
    std::uint16_t mainSelector2BA4 = 0;

    std::array<std::uint32_t, 8> blockLinear2BA8{};
    std::array<std::uint32_t, 8> blockFar2BC8{};
    std::array<std::uint16_t, 8> blockSelector2BE8{};

    std::uint32_t extraSize2B8C = 0;
    std::uint32_t extraLinear2BF8 = 0;
    std::uint32_t extraFar2BFC = 0;
    std::uint16_t extraSelector2C00 = 0;

    std::uint32_t blockSize2C04 = 0;

    std::uint32_t backendFlag2F48 = 0;

    std::array<std::uint8_t, 0x100>* config2AA0 = nullptr;
};

struct AudioInitOpsB0F8 {
    std::function<std::uint32_t(
        std::int32_t, std::uint32_t&, std::uint16_t&)> allocateDos6F10;
    std::function<std::int32_t(std::uint16_t)> freeDos6F74;

    std::function<std::int32_t(const std::string&)> openRead62A8;
    std::function<std::int32_t(const std::string&)> querySize627C;

    std::function<std::uintptr_t(std::size_t)> heapAllocate71C8;
    std::function<void(std::uintptr_t)> heapFree7240;

    std::function<std::int32_t(
        void*, std::int32_t, std::int32_t)> read6304;
    std::function<std::int32_t(std::int32_t)> close62F0;

    std::function<std::uintptr_t(
        std::uintptr_t, std::int32_t, std::int32_t)> callF8C4;

    std::function<void()> callE680;
    std::function<std::int32_t(std::uintptr_t)> callEA0B;
    std::function<std::int32_t(std::int32_t)> callE6EA;

    std::function<const AudioBackendRecordB0F8*(
        std::int32_t)> callEAC8;

    std::function<std::int32_t(
        std::int32_t, std::int32_t, std::int32_t,
        std::int32_t, std::int32_t)> callEAE7;

    std::function<void(
        std::int32_t, std::int32_t, std::int32_t,
        std::int32_t, std::int32_t)> callEAF1;

    std::function<std::uint32_t(std::int32_t)> callEC89;
    std::function<std::uint32_t(std::int32_t)> callECA7;

    std::function<void(
        std::int32_t, std::uint32_t, std::uint32_t)> callECB1;

    std::function<void(
        std::array<std::uint8_t, 0x100>*)> bindConfig8A18;

    std::function<void()> loadSampleB060;
};

class AudioInitB0F8 {
public:
    // Direct reconstruction of CODE.1 0xB0F8..0xB42F.
    static void initialize(
        const std::string& fileName,
        std::int32_t arg0C,
        std::int32_t arg10,
        std::int32_t arg14,
        std::int32_t arg18,
        AudioInitStateB0F8& state,
        const AudioInitOpsB0F8& ops);
};

} // namespace ravenloft::game
