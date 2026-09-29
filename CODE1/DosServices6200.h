#pragma once

#include "RuntimeRng618A.h"

#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct DosState6200 {
    std::uint8_t byte8919 = 0;
    std::int32_t dword891E = 0;
    std::uint32_t dword046C = 0;
};

struct DosOps6200 {
    std::function<std::int32_t(std::int32_t, std::uint16_t)> callD23A;
    std::function<std::int32_t(std::int32_t, std::uint16_t, std::uint32_t)> callD280;
    std::function<std::int32_t(std::uint32_t)> callD2AD;

    std::function<std::int32_t(std::int32_t, std::int32_t)> callD313;
    std::function<std::int32_t(std::int32_t, std::int32_t)> callD55E;
    std::function<std::int32_t(std::int32_t)> callD573;

    std::function<std::int32_t(std::int32_t, void*, std::int32_t)> callD5AD;
    std::function<std::int32_t(std::int32_t, const void*, std::int32_t)> callD686;
    std::function<std::int32_t(std::int32_t, std::int32_t, std::int32_t)> callD85F;

    std::function<std::int32_t(std::int32_t)> callD8AA;
    std::function<void(std::int32_t, std::int32_t&)> callD8E8;
    std::function<void(std::int32_t&)> callD8D0;
    std::function<std::int32_t(std::uint32_t, std::int32_t)> callD906;

    std::function<std::int32_t(std::int32_t)> callD991;
    std::function<std::int32_t(const char*)> callD9A6;
    std::function<std::int32_t(std::int32_t)> callD9CA;
};

class DosServices6200 {
public:
    // CODE.1 0x6200..0x621F.
    static std::int32_t probe(std::int32_t value,
                              std::uint16_t word,
                              const DosOps6200& ops);

    // CODE.1 0x6220..0x624B.
    static std::uint32_t query(std::int32_t value,
                               std::uint16_t word,
                               const DosOps6200& ops);

    // CODE.1 0x624C..0x626E.
    static std::uint32_t queryDefault(const DosOps6200& ops);

    // CODE.1 0x6270..0x627B.
    static std::uint8_t byte6270(
        const DosState6200& state) noexcept;

    // CODE.1 0x627C..0x62A7.
    static std::int32_t queryHandle(
        std::int32_t value,
        const DosState6200& state,
        const DosOps6200& ops);

    // CODE.1 0x62A8..0x62BF.
    static std::int32_t openRead(
        std::int32_t value,
        const DosOps6200& ops);

    // CODE.1 0x62C0..0x62D7.
    static std::int32_t openWrite(
        std::int32_t value,
        const DosOps6200& ops);

    // CODE.1 0x62D8..0x62EF.
    static std::int32_t openExisting(
        std::int32_t value,
        const DosOps6200& ops);

    // CODE.1 0x62F0..0x6302.
    static std::int32_t close(
        std::int32_t handle,
        const DosOps6200& ops);

    // CODE.1 0x6304..0x631E.
    static std::int32_t read(void* destination,
                     std::int32_t count,
                     std::int32_t handle,
                     const DosOps6200& ops);

    // CODE.1 0x6320..0x633A.
    static std::int32_t write(const void* source,
                      std::int32_t count,
                      std::int32_t handle,
                      const DosOps6200& ops);

    // CODE.1 0x633C..0x6356.
    static std::int32_t seek(std::int32_t offset,
                     std::int32_t origin,
                     std::int32_t handle,
                     const DosOps6200& ops);

    // CODE.1 0x6358..0x63A1.
    static void seekIndexed(std::int32_t index,
                            std::int32_t handle,
                            const DosOps6200& ops);

    // CODE.1 0x63A4..0x63B6.
    static std::int32_t remove(std::int32_t value,
                       const DosOps6200& ops);

    // CODE.1 0x63B8..0x63D4.
    static std::int32_t setDrive(
        std::int32_t drive,
        const DosOps6200& ops);

    // CODE.1 0x63D8..0x63F0.
    static std::int32_t currentDrive(
        const DosOps6200& ops);

    // CODE.1 0x63F4..0x6408.
    static std::int32_t refreshDrive(
        const DosOps6200& ops);

    // CODE.1 0x640C..0x6421.
    static std::int32_t initPathBuffer(
        const DosOps6200& ops);

    // CODE.1 0x6424..0x648C.
    static std::int32_t changePath(
        char* path,
        const DosOps6200& ops);

    // CODE.1 0x6490..0x64A2.
    static std::int32_t call6490(
        std::int32_t value,
        const DosOps6200& ops);

    // CODE.1 0x64B0..0x64D8.
    // CODE.1 0x64A3..0x64AF is zero/alignment data.
    // CODE.1 0x64D9..0x64DB is alignment.
    static std::int32_t seedFromGlobal(
        std::int32_t value,
        DosState6200& state,
        RuntimeRng618A& rng) noexcept;
};

} // namespace ravenloft::game
