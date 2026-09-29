#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct Code1BackendQueueEntryC5B0 {
    std::int32_t resourceId = -1;
    std::int32_t value = 0;
    std::int32_t priority = 0;
};

struct Code1BackendSupportStateC5B0 {
    std::int32_t active2F48 = 0;
    std::array<Code1BackendQueueEntryC5B0, 2> queue2F50{{
        {-1, 0, 0},
        {-1, 0, 0}
    }};
    std::int32_t lookupParam2F4C = 0;
    std::uint8_t enabled2F78 = 0;
    std::int32_t backend2F74 = -1;
};

struct Code1BackendSupportOpsC5B0 {
    std::function<std::int32_t(void*, std::int32_t, std::int32_t)> read6304;
    std::function<std::uintptr_t(std::int32_t)> heapAllocate71C8;
    std::function<void(std::uintptr_t)> heapFree7240;
    std::function<std::int32_t(std::int32_t)> backendStateEC2F;
    std::function<void()> stopC57C;
    std::function<void(std::int32_t)> submitC4E0;
};

class Code1BackendSupportC5B0 {
public:
    // Direct reconstruction of CODE.1 0xC5B0..0xC607.
    static std::uintptr_t readLengthPrefixed(std::int32_t ignored,
                                             std::int32_t handle,
                                             const Code1BackendSupportStateC5B0& state,
                                             const Code1BackendSupportOpsC5B0& ops);

    // Direct reconstruction of CODE.1 0xC608..0xC629.
    // 0xC62A..0xC62B is alignment.
    static void freeIfActive(std::uintptr_t pointer,
                             const Code1BackendSupportStateC5B0& state,
                             const Code1BackendSupportOpsC5B0& ops);

    // Direct reconstruction of CODE.1 0xC62C..0xC64C.
    // 0xC64D..0xC64F is alignment.
    static void setEnabled(std::uint8_t enabled,
                           Code1BackendSupportStateC5B0& state,
                           const Code1BackendSupportOpsC5B0& ops);

    // Direct reconstruction of CODE.1 0xC650..0xC65E.
    // 0xC65F is alignment.
    static void setLookupParam(std::int32_t value,
                               Code1BackendSupportStateC5B0& state) noexcept;

    // Direct reconstruction of CODE.1 0xC660..0xC68D.
    // 0xC68E..0xC68F is alignment.
    static std::int32_t type2Active(const Code1BackendSupportStateC5B0& state,
                                    const Code1BackendSupportOpsC5B0& ops);

    // Direct reconstruction of CODE.1 0xC690..0xC6ED.
    // 0xC6EE..0xC6EF is alignment.
    static void queue(std::int32_t resourceId,
                      std::int32_t value,
                      std::int32_t slot,
                      std::int32_t priority,
                      Code1BackendSupportStateC5B0& state) noexcept;

    // Direct reconstruction of CODE.1 0xC6F0..0xC6F6.
    // 0xC6F7 is alignment.
    static void noOpC6F0() noexcept;

    // Direct reconstruction of CODE.1 0xC6F8..0xC79E.
    // 0xC79F onward is not part of this routine.
    static void flushQueue(Code1BackendSupportStateC5B0& state,
                           const Code1BackendSupportOpsC5B0& ops);
};

} // namespace ravenloft::game
