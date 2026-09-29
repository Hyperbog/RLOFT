#pragma once

#include "Code1System95A8.h"

#include <cstdint>
#include <functional>
#include <string>

namespace ravenloft::game {

struct Code1ServiceOps9AE8 {
    Code1SystemOps95A8 system;

    std::function<void()> call8960;
    std::function<void(std::int32_t)> call5014;
    std::function<std::string(std::int32_t)> stringFromToken;

    std::function<void(std::int32_t, std::int32_t, std::int32_t)> call86A4;

    std::function<void(std::int32_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> callC7A0;
    std::function<void(std::int32_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> callC8C4;

    std::function<void(std::int32_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t, std::int32_t,
                       std::int32_t)> callCACC;
    std::function<void(std::int32_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t, std::int32_t,
                       std::int32_t)> callC9D4;
};

class Code1Service9AE8 {
public:
    // Direct reconstruction of CODE.1 0x9AE8..0x9BC1.
    // CODE.1 0x9BC2..0x9BC3 is alignment.
    static void service(
        Code1SystemState95A8& state,
        const Code1ServiceOps9AE8& ops);

    // Direct reconstruction of CODE.1 0x9BC4..0x9BD6.
    // CODE.1 0x9BD7..0x9BDF is zero/data padding.
    static void requestQuit(
        Code1SystemState95A8& state,
        const Code1ServiceOps9AE8& ops);

    // Direct reconstruction of CODE.1 0x9BE0..0x9C43.
    static void dispatch5(
        std::int32_t mode,
        std::int32_t a,
        std::int32_t b,
        std::int32_t c,
        std::int32_t d,
        std::int32_t e,
        const Code1ServiceOps9AE8& ops);

    // Direct reconstruction of CODE.1 0x9C44..0x9C97.
    // CODE.1 0x9C98..0x9C9F is zero/data padding.
    static void dispatch7(
        std::int32_t mode,
        std::int32_t a,
        std::int32_t b,
        std::int32_t c,
        std::int32_t d,
        std::int32_t e,
        std::int32_t f,
        std::int32_t g,
        const Code1ServiceOps9AE8& ops);
};

} // namespace ravenloft::game
