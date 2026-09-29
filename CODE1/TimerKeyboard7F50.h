#pragma once

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct InterruptVector7F50 {
    std::uint32_t offset = 0;
    std::uint16_t selector = 0;
};

struct TimerKeyboardState7F50 {
    std::uint32_t tick2934 = 0;

    std::uint8_t port61Latch2918 = 0;
    std::uint8_t previousScan2919 = 0;
    std::uint8_t currentScan291A = 0;
    std::uint8_t modifiers291B = 0;
    std::uint8_t spaceEvent291C = 0;
    std::uint8_t handlersInstalled291D = 0;

    InterruptVector7F50 oldInt9{};
    InterruptVector7F50 oldInt0{};
    InterruptVector7F50 oldInt8{};

    std::uint8_t queueCount2930 = 0;
    std::array<std::uint8_t, 256> queue89D0{};
    std::array<std::uint32_t, 256> keyMap2938{};

    std::uint8_t flags2B01 = 0;
};

struct TimerKeyboardOps7F50 {
    std::function<void()> callCEC0;
    std::function<void(std::uint32_t)> callDDE0;
    std::function<void()> chainOldInt8;

    std::function<void()> enterCriticalDA18;
    std::function<void()> leaveCriticalDA1A;

    std::function<std::uint8_t(std::uint16_t)> inPortDDE7;
    std::function<void(std::uint16_t, std::uint8_t)> outPortDDF2;
    std::function<void()> service9AE8;

    std::function<void(std::int32_t)> display64DC;
    std::function<void()> callCF98;

    std::function<InterruptVector7F50(std::uint8_t)> getVectorDE35;
    std::function<void(std::uint8_t, const InterruptVector7F50&)> setVectorDDFE;
    std::function<std::uint16_t()> currentCodeSegment;
    std::function<void()> chainOldInt0;
};

class TimerKeyboard7F50 {
public:
    // CODE.1 0x7F41..0x7F4F is zero/data padding.
    // CODE.1 0x7F50..0x7F5B.
    static std::uint32_t ticks(const TimerKeyboardState7F50& state) noexcept;

    // Portable observable body of CODE.1 timer ISR 0x7F5C..0x7F89.
    // CODE.1 0x7F8A..0x7F8B is alignment.
    static void timerInterrupt(TimerKeyboardState7F50& state,
                               const TimerKeyboardOps7F50& ops);

    // CODE.1 0x7F8C..0x7FA4. 0x7FA5..0x7FA7 is alignment.
    static void setTicks(std::uint32_t value,
                         TimerKeyboardState7F50& state,
                         const TimerKeyboardOps7F50& ops);

    // CODE.1 0x7FA8..0x8028. 0x8029..0x802B is alignment.
    static void removeQueuedScan(std::uint8_t scan,
                                 TimerKeyboardState7F50& state) noexcept;

    // CODE.1 0x802C..0x8089. 0x808A..0x808B is alignment.
    static void queueScan(std::uint8_t scan,
                          TimerKeyboardState7F50& state) noexcept;

    // CODE.1 0x808C..0x80B9. 0x80BA..0x80BB is alignment.
    static std::int32_t queued(std::uint8_t scan,
                               const TimerKeyboardState7F50& state) noexcept;

    // Portable observable body of CODE.1 keyboard ISR 0x80BC..0x826D.
    // CODE.1 0x826E..0x826F is alignment.
    static void keyboardInterrupt(TimerKeyboardState7F50& state,
                                  const TimerKeyboardOps7F50& ops);

    // CODE.1 0x8270..0x8276. 0x8277 is NOP alignment.
    static void noop8270() noexcept;

    // CODE.1 0x8278..0x8288. 0x8289..0x828B is alignment.
    static std::uint32_t modifiers(std::uint32_t mask,
                                   const TimerKeyboardState7F50& state) noexcept;

    // CODE.1 0x828C..0x82A0. 0x82A1..0x82A3 is alignment.
    static std::uint32_t takeSpaceEvent(TimerKeyboardState7F50& state) noexcept;

    // CODE.1 0x82A4..0x82BC. 0x82BD..0x82BF is alignment.
    static std::int32_t keyPending(TimerKeyboardState7F50& state,
                                   const TimerKeyboardOps7F50& ops);

    // CODE.1 0x82C0..0x82EB.
    static std::uint32_t readKey(TimerKeyboardState7F50& state,
                                 const TimerKeyboardOps7F50& ops);

    // CODE.1 0x82EC..0x8300. 0x8301..0x8303 is alignment.
    static void clearQueue(TimerKeyboardState7F50& state) noexcept;

    // CODE.1 0x8304..0x831B.
    static void restoreCurrent(TimerKeyboardState7F50& state) noexcept;

    // CODE.1 0x831C..0x8335. 0x8336..0x8337 is alignment.
    static std::uint32_t peekKey(TimerKeyboardState7F50& state,
                                 const TimerKeyboardOps7F50& ops);

    // Portable observable body of CODE.1 INT 0 handler 0x8338..0x8397.
    static void divideErrorInterrupt(TimerKeyboardState7F50& state,
                                     const TimerKeyboardOps7F50& ops);

    // CODE.1 0x8398..0x8420. 0x8421..0x8423 is alignment.
    static void installHandlers(TimerKeyboardState7F50& state,
                                const TimerKeyboardOps7F50& ops);

    // CODE.1 0x8424..0x8482. 0x8483 is NOP alignment.
    static void restoreHandlers(TimerKeyboardState7F50& state,
                                const TimerKeyboardOps7F50& ops);

    // CODE.1 0x8484..0x8491. 0x8492..0x849F is zero/data padding.
    static std::int32_t handlersInstalled(const TimerKeyboardState7F50& state) noexcept;

    // CODE.1 0x84A0..0x84C1 is the already reconstructed memset helper.
    // CODE.1 0x84C2..0x84CF is data/alignment padding.
};

} // namespace ravenloft::game
