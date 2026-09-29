#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct ResourceLoadState4C80 {
    std::int32_t global4BC = 0;
    std::int32_t global4C0 = 0;
    std::int32_t handle4C8 = 0;

    std::int32_t value7134 = 0;
    std::int32_t value713C = 0;
    std::int32_t value712C = 0;

    std::int32_t value7120 = 0;
    std::uintptr_t buffer7128 = 0;
};

struct ResourceLoadOps4C80 {
    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t)> call633C;
    std::function<void(void*, std::int32_t,
                       std::int32_t)> call6304;

    std::function<std::int32_t(std::int32_t)> call62A8;
    std::function<void(std::int32_t)> call9814;
    std::function<void(std::int32_t)> call62F0;
    std::function<void(std::int32_t)> callB74C;
    std::function<void(std::int32_t)> callC650;

    std::function<std::uintptr_t(std::size_t)> call71C8;
};

class IndexedRead4C80 {
public:
    // Direct reconstruction of CODE.1 0x4C80..0x4CAF.
    static std::int32_t read(std::int32_t index,
                             std::int32_t handle,
                             const ResourceLoadOps4C80& ops);
};

class ResourceOpen4CB0 {
public:
    // Direct reconstruction of CODE.1 0x4CB0..0x4D50.
    // CODE.1 0x4D51..0x4D53 is alignment.
    static void run(ResourceLoadState4C80& state,
                    const ResourceLoadOps4C80& ops);
};

class RecordBlock4D54 {
public:
    // Direct reconstruction of CODE.1 0x4D54..0x4DC3.
    static void load(ResourceLoadState4C80& state,
                     const ResourceLoadOps4C80& ops);
};

} // namespace ravenloft::game
