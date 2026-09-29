#pragma once

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct Code1InputDeviceSample5230 {
    std::int16_t word0 = 0;
    std::int16_t word4 = 0;
    std::int16_t word6 = 0;
};

struct Code1InputState5230 {
    std::array<std::uint8_t, 0x1C> block7190{};
    std::uint16_t word71AC = 0;
    std::uint16_t word71B4 = 0;
    std::uint32_t dword71B8 = 0;
    std::uint16_t word71C8 = 0;

    std::int16_t value71D8 = 0;
    std::int16_t value71D6 = 0;
    std::int16_t value71D4 = 0;
    std::int16_t word71DA = 0;
    std::array<std::uint8_t, 0x20A> block71DC{};

    std::int16_t mouseX73E6 = 0;
    std::int16_t mouseY73E8 = 0;
    std::int16_t word73EA = 0;
    std::int16_t word73EC = 0;
    std::uint8_t byte73EE = 0;
    std::uint8_t byte23E6 = 0;

    std::int32_t value7134 = 0;
};

struct Code1InputOps5230 {
    std::function<std::uint16_t()> currentCodeSegment;

    std::function<void(std::int32_t,
                       std::uint16_t*,
                       std::array<std::uint8_t, 0x1C>*,
                       std::uint16_t*)> callCEC8;

    std::function<std::uint16_t()> call84D0;
    std::function<void(std::int32_t, std::int32_t)> callD130;
    std::function<void(std::int32_t, std::int32_t)> callD10C;
    std::function<void(std::int32_t, std::int32_t)> callD08C;
    std::function<const Code1InputDeviceSample5230*()> callD0B4;

    std::function<std::uintptr_t(std::int32_t, std::int32_t)> call954C;
    std::function<void(std::array<std::uint8_t, 0x20A>*,
                       std::uintptr_t,
                       std::int32_t)> callD1E8;

    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call6BC0;

    std::function<void()> call5780;
    std::function<void()> callCF98;
    std::function<void()> call9AE8;
};

class Code1InstallHandler5230 {
public:
    // Direct reconstruction of CODE.1 0x5230..0x5279.
    // CODE.1 0x527A..0x527B is alignment.
    static void run(std::int32_t value,
                    Code1InputState5230& state,
                    const Code1InputOps5230& ops);
};

class Code1InputSnapshot527C {
public:
    // Direct reconstruction of CODE.1 0x527C..0x52A9.
    // CODE.1 0x52AA..0x52AB is alignment.
    static void get(std::int16_t& first,
                    std::int16_t& second,
                    std::int16_t& third,
                    const Code1InputState5230& state) noexcept;
};

class Code1InputInit52AC {
public:
    // Direct reconstruction of CODE.1 0x52AC..0x538B.
    static void run(Code1InputState5230& state,
                    const Code1InputOps5230& ops);
};

class Code1InputShutdown538C {
public:
    // Direct reconstruction of CODE.1 0x538C..0x53B0.
    // CODE.1 0x53B1..0x53B3 is alignment.
    static void run(Code1InputState5230& state,
                    const Code1InputOps5230& ops);
};

class Code1InputPoll53B4 {
public:
    // Direct reconstruction of CODE.1 0x53B4..0x53E1.
    // CODE.1 0x53E2..0x53E3 is alignment.
    static std::int32_t poll(Code1InputState5230& state,
                             const Code1InputOps5230& ops);
};

} // namespace ravenloft::game
