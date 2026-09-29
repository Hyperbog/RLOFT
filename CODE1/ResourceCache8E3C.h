#pragma once

#include "ResourceIndex8978.h"

#include <array>
#include <cstdint>
#include <functional>
#include <string>

namespace ravenloft::game {

struct ResourceCacheState8E3C {
    ResourceIndexState8978* index = nullptr;

    // Original global DS:2B10.
    std::int32_t specialItem2B10 = -1;
};

struct ResourceCacheOps8E3C {
    // CODE.1 0x8D0C.
    std::function<void(ResourceHeader8D0C&,
                       std::int32_t, std::int32_t, std::uint32_t)> readHeader8D0C;

    // Loaders selected by the seven-entry jump table at CODE.1 0x8E20.
    // They correspond to original routines:
    // 767C, 79C8, B51C, C5B0, E0D0, E190, E208.
    std::array<std::function<std::uint32_t(
        const ResourceHeader8D0C&, std::int32_t)>, 7> loadByType{};

    // Destructors selected by the seven-entry jump table at CODE.1 0x91BC.
    // They correspond to original routines:
    // 7830, 7A64, B574, C608, E174, E1F4, E26C.
    std::array<std::function<void(std::uint32_t)>, 7> releaseByType{};

    std::function<void(std::uintptr_t)> heapFree7240;
    std::function<std::uint32_t()> ticks7F50;

    // Original optional callback at DS:2B1C.
    std::function<void(std::int32_t, std::int32_t)> beforeLoad2B1C;

    std::function<void(const std::string&)> fatal9814;
};

class ResourceCache8E3C {
public:
    // Direct reconstruction of CODE.1 0x8E3C..0x91BB.
    static void load(std::int32_t item,
                     std::int32_t group,
                     ResourceCacheState8E3C& state,
                     const ResourceCacheOps8E3C& ops);

    // CODE.1 0x91BC..0x91D7 is the seven-DWORD release jump table.
    // Direct reconstruction of CODE.1 0x91D8..0x9376.
    // CODE.1 0x9377 is alignment.
    static void release(std::int32_t item,
                        std::int32_t group,
                        ResourceCacheState8E3C& state,
                        const ResourceCacheOps8E3C& ops);

    // Direct reconstruction of CODE.1 0x9378..0x943D.
    // CODE.1 0x943E..0x943F is alignment.
    static std::int32_t evictOldestByLoadedValue(
        std::int32_t loadedValue,
        ResourceCacheState8E3C& state,
        const ResourceCacheOps8E3C& ops);

    // Direct reconstruction of CODE.1 0x9440..0x9487.
    static void pin(std::int32_t item,
                    std::int32_t group,
                    ResourceCacheState8E3C& state) noexcept;

    // Direct reconstruction of CODE.1 0x9488..0x94CF.
    static void unpin(std::int32_t item,
                      std::int32_t group,
                      ResourceCacheState8E3C& state) noexcept;

    // Direct reconstruction of CODE.1 0x94D0..0x951C.
    // CODE.1 0x951D..0x951F is alignment.
    static void unpinAllExceptSpecial(
        ResourceCacheState8E3C& state) noexcept;

    // Direct reconstruction of CODE.1 0x9520..0x954B.
    static void reloadAndPinSpecial(
        ResourceCacheState8E3C& state,
        const ResourceCacheOps8E3C& ops);

    // Direct reconstruction of CODE.1 0x954C..0x95A7.
    static std::uint32_t ensureAndGet(
        std::int32_t item,
        std::int32_t group,
        ResourceCacheState8E3C& state,
        const ResourceCacheOps8E3C& ops);
};

} // namespace ravenloft::game
