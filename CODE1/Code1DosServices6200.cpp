#include "Code1DosServices6200.h"

namespace ravenloft::game {

std::int32_t Code1DosServices6200::probe(
    std::int32_t value,
    std::uint16_t word,
    const Code1DosOps6200& ops) {
    return ops.callD23A(value, word) == 0 ? 1 : 0;
}

std::uint32_t Code1DosServices6200::query(
    std::int32_t value,
    std::uint16_t word,
    const Code1DosOps6200& ops) {
    return ops.callD280(value, word, 0x8904) != 0
        ? 0u
        : 0x8922u;
}

std::uint32_t Code1DosServices6200::queryDefault(
    const Code1DosOps6200& ops) {
    return ops.callD2AD(0x8904) != 0
        ? 0u
        : 0x8922u;
}

std::uint8_t Code1DosServices6200::byte6270(
    const Code1DosState6200& state) noexcept {
    return state.byte8919;
}

std::int32_t Code1DosServices6200::queryHandle(
    std::int32_t value,
    const Code1DosState6200& state,
    const Code1DosOps6200& ops) {
    return ops.callD280(value, 0, 0x8904) == 0
        ? state.dword891E
        : -1;
}

std::int32_t Code1DosServices6200::openRead(
    std::int32_t value,
    const Code1DosOps6200& ops) {
    return ops.callD313(value, 0x200);
}

std::int32_t Code1DosServices6200::openWrite(
    std::int32_t value,
    const Code1DosOps6200& ops) {
    return ops.callD313(value, 0x202);
}

std::int32_t Code1DosServices6200::openExisting(
    std::int32_t value,
    const Code1DosOps6200& ops) {
    return ops.callD55E(value, 0x180);
}

void Code1DosServices6200::close(
    std::int32_t handle,
    const Code1DosOps6200& ops) {
    ops.callD573(handle);
}

void Code1DosServices6200::read(
    void* destination,
    std::int32_t count,
    std::int32_t handle,
    const Code1DosOps6200& ops) {
    ops.callD5AD(handle, destination, count);
}

void Code1DosServices6200::write(
    const void* source,
    std::int32_t count,
    std::int32_t handle,
    const Code1DosOps6200& ops) {
    ops.callD686(handle, source, count);
}

void Code1DosServices6200::seek(
    std::int32_t offset,
    std::int32_t origin,
    std::int32_t handle,
    const Code1DosOps6200& ops) {
    ops.callD85F(handle, offset, origin);
}

void Code1DosServices6200::seekIndexed(
    std::int32_t index,
    std::int32_t handle,
    const Code1DosOps6200& ops) {
    seek(index * 4 + 4, 0, handle, ops);

    std::int32_t offset = 0;
    read(&offset, 4, handle, ops);

    seek(offset, 0, handle, ops);
}

void Code1DosServices6200::remove(
    std::int32_t value,
    const Code1DosOps6200& ops) {
    ops.callD8AA(value);
}

std::int32_t Code1DosServices6200::setDrive(
    std::int32_t drive,
    const Code1DosOps6200& ops) {
    std::int32_t result = 0;
    ops.callD8E8(drive, result);
    return result;
}

std::int32_t Code1DosServices6200::currentDrive(
    const Code1DosOps6200& ops) {
    std::int32_t result = 0;
    ops.callD8D0(result);
    return result;
}

std::int32_t Code1DosServices6200::refreshDrive(
    const Code1DosOps6200& ops) {
    return setDrive(currentDrive(ops), ops);
}

void Code1DosServices6200::initPathBuffer(
    const Code1DosOps6200& ops) {
    ops.callD906(0x892F, 0x50);
}

std::int32_t Code1DosServices6200::changePath(
    char* path,
    const Code1DosOps6200& ops) {
    const std::int32_t savedDrive =
        currentDrive(ops);

    if (path[1] == ':') {
        const std::int32_t drive =
            ops.callD991(
                static_cast<std::int8_t>(path[0])) -
            0x41;

        setDrive(drive, ops);

        if (currentDrive(ops) != drive) {
            return -1;
        }

        path += 2;
    }

    const std::int32_t result =
        ops.callD9A6(path);

    if (result == -1) {
        setDrive(savedDrive, ops);
    }

    return result;
}

std::int32_t Code1DosServices6200::call6490(
    std::int32_t value,
    const Code1DosOps6200& ops) {
    return ops.callD9CA(value);
}

std::int32_t Code1DosServices6200::seedFromGlobal(
    std::int32_t value,
    Code1DosState6200& state,
    Code1RuntimeRng618A& rng) noexcept {
    if (value == 0) {
        value =
            static_cast<std::uint16_t>(
                state.dword046C);
    }

    rng.seed(
        static_cast<std::uint16_t>(value));

    return value;
}

} // namespace ravenloft::game
