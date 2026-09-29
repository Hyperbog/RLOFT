#pragma once

#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct BackendDescriptorC2D0 {
    std::int32_t field0 = 0;
    std::int32_t type4 = 0;
};

struct BackendResourceC4E0 {
    std::int32_t size = 0;
    const std::uint8_t* data = nullptr;
};

struct BackendStateC2D0 {
    std::int32_t active2F48 = 0;
    std::int32_t lookupParam2F4C = 0;
    std::uintptr_t dosLinear2F68 = 0;
    std::uint32_t dosFar2F6C = 0;
    std::uint16_t dosSelector2F70 = 0;
    std::int32_t backend2F74 = -1;
    std::uint8_t enabled2F78 = 0;
};

struct BackendOpsC2D0 {
    std::function<std::uintptr_t(std::int32_t, std::uint32_t&, std::uint16_t&)> allocateDos6F10;
    std::function<void(std::uint16_t)> freeDos6F74;

    std::function<std::int32_t(std::int32_t)> openRead62A8;
    std::function<std::int32_t(std::int32_t)> query627C;
    std::function<std::uintptr_t(std::int32_t)> heapAllocate71C8;
    std::function<void(std::uintptr_t)> heapFree7240;
    std::function<std::int32_t(void*, std::int32_t, std::int32_t)> read6304;
    std::function<std::int32_t(std::int32_t)> close62F0;

    std::function<std::uintptr_t(std::uintptr_t, std::int32_t, std::int32_t)> callF8C4;
    std::function<void()> callE680;
    std::function<std::int32_t(std::uintptr_t)> callEA0B;
    std::function<void(std::int32_t)> callE6EA;
    std::function<BackendDescriptorC2D0*(std::int32_t)> callEAC8;
    std::function<std::int32_t(std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t)> callEAE7;
    std::function<void(std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t)> callEAF1;

    std::function<void(std::int32_t)> callEA9A;
    std::function<std::int32_t(std::int32_t)> callEC2F;
    std::function<void(std::int32_t)> callEC43;

    std::function<BackendResourceC4E0(std::int32_t, std::int32_t)> lookup954C;
    std::function<void(std::uint8_t*, const std::uint8_t*, std::int32_t)> copyD9EE;
    std::function<void(std::int32_t, std::uintptr_t, std::uint16_t, std::int32_t)> callEC25;
    std::function<void(std::int32_t, std::uintptr_t, std::uint16_t, std::uint32_t, std::int32_t)> callEC1B;
    std::function<void(std::int32_t)> callEC39;
};

class BackendC2D0 {
public:
    // Direct reconstruction of CODE.1 0xC2D0..0xC49C.
    // 0xC49D..0xC49F is alignment.
    static void initialize(std::int32_t fileToken,
                           std::int32_t arg1,
                           std::int32_t arg2,
                           std::int32_t arg3,
                           std::int32_t arg4,
                           BackendStateC2D0& state,
                           const BackendOpsC2D0& ops);

    // Direct reconstruction of CODE.1 0xC4A0..0xC4DF.
    static void shutdown(BackendStateC2D0& state,
                         const BackendOpsC2D0& ops);

    // Direct reconstruction of CODE.1 0xC4E0..0xC57A.
    // 0xC57B is alignment.
    static void submit(std::int32_t resourceId,
                       BackendStateC2D0& state,
                       const BackendOpsC2D0& ops);

    // Direct reconstruction of CODE.1 0xC57C..0xC5AF.
    static void stopIfType2(BackendStateC2D0& state,
                            const BackendOpsC2D0& ops);
};

} // namespace ravenloft::game
