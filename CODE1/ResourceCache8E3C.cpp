#include "ResourceCache8E3C.h"

#include <cstddef>

namespace ravenloft::game {
namespace {

ResourceRecord8978& recordAt(
    ResourceCacheState8E3C& state,
    std::int32_t item,
    std::int32_t group) {
    return state.index->groups[static_cast<std::size_t>(group)]
        .records[static_cast<std::size_t>(item)];
}

std::string outOfRangeMessage(
    std::int32_t item,
    std::int32_t group) {
    return "Attempt to load out-of-range: " +
        std::to_string(item) + "/" +
        std::to_string(static_cast<std::uint32_t>(group));
}

} // namespace

void ResourceCache8E3C::load(
    std::int32_t item,
    std::int32_t group,
    ResourceCacheState8E3C& state,
    const ResourceCacheOps8E3C& ops) {
    auto& groupState =
        state.index->groups[static_cast<std::size_t>(group)];

    // 0x8E45..0x8EBF.
    if (item < 0 ||
        item >= static_cast<std::int32_t>(groupState.count)) {
        ops.fatal9814(outOfRangeMessage(item, group));

        // Original execution falls through after the fatal call. The shipped
        // fatal service is non-returning; returning here avoids manufacturing
        // an out-of-range host access if a replacement backend returns.
        return;
    }

    auto& record = recordAt(state, item, group);

    // 0x8EC2..0x9185.
    if (record.loaded2() == 0) {
        if (ops.beforeLoad2B1C) {
            ops.beforeLoad2B1C(item, group);
        }

        ResourceHeader8D0C header{};
        ops.readHeader8D0C(
            header,
            item,
            groupState.handle,
            static_cast<std::uint32_t>(group));

        const std::uint8_t type = header.byte2;

        if (type < 1 || type > 7) {
            // Original string at CODE.1 data DS:02D4.
            ops.fatal9814("Unknown resource type");
        } else {
            const std::uint32_t resource =
                ops.loadByType[static_cast<std::size_t>(type - 1u)](
                    header,
                    groupState.handle);
            record.setDword5(resource);
        }

        record.loaded2() = 1;

        // Low nibble is the resource type; bit 4 is cleared.
        record.flags0() = static_cast<std::uint8_t>(
            (record.flags0() & 0xF0u) |
            (type & 0x0Fu));
        record.flags0() &= 0xEFu;

        if (header.payloadAllocation != 0) {
            ops.heapFree7240(header.payloadAllocation);
        }

        ++groupState.dword4;
    }

    // 0x9189..0x91AF: every load/access refreshes the age stamp.
    record.setWord3(
        static_cast<std::uint16_t>(
            ops.ticks7F50() >> 5u));
}

void ResourceCache8E3C::release(
    std::int32_t item,
    std::int32_t group,
    ResourceCacheState8E3C& state,
    const ResourceCacheOps8E3C& ops) {
    auto& groupState =
        state.index->groups[static_cast<std::size_t>(group)];
    auto& record = recordAt(state, item, group);

    // 0x91DD..0x921E.
    if ((record.flags0() & 0x10u) != 0 ||
        record.loaded2() == 0) {
        return;
    }

    const std::uint32_t resource = record.dword5();
    const std::uint8_t type =
        static_cast<std::uint8_t>(record.flags0() & 0x0Fu);

    // 0x9254..0x92F8. Types 1, 6, and 7 repeat the exact byte2==1
    // check present in the original before invoking their destructor.
    if (type >= 1 && type <= 7) {
        const bool requiresStateOne =
            type == 1 || type == 6 || type == 7;

        if (!requiresStateOne ||
            record.loaded2() == 1) {
            ops.releaseByType[
                static_cast<std::size_t>(type - 1u)](
                    resource);
        }
    }

    // 0x92F8..0x936C.
    record.loaded2() = 0;
    record.setWord3(0);
    record.setDword5(0);
    --groupState.dword4;
}

std::int32_t
ResourceCache8E3C::evictOldestByLoadedValue(
    std::int32_t loadedValue,
    ResourceCacheState8E3C& state,
    const ResourceCacheOps8E3C& ops) {
    std::int32_t bestItem = -1;
    std::int32_t bestGroup = 0;
    std::uint32_t bestAge = 0;

    // 0x9380..0x9417.
    for (std::int32_t group = 0;
         group < state.index->groupCount2B04;
         ++group) {
        auto& groupState =
            state.index->groups[
                static_cast<std::size_t>(group)];

        if (groupState.dword4 == 0) {
            continue;
        }

        for (std::int32_t item = 0;
             item < static_cast<std::int32_t>(groupState.count);
             ++item) {
            auto& record =
                groupState.records[
                    static_cast<std::size_t>(item)];

            if (static_cast<std::int32_t>(
                    record.loaded2()) != loadedValue ||
                (record.flags0() & 0x10u) != 0) {
                continue;
            }

            const std::uint32_t age = record.word3();

            if (bestItem == -1 ||
                age < bestAge) {
                bestItem = item;
                bestGroup = group;
                bestAge = age;
            }
        }
    }

    // 0x941D..0x9437.
    if (bestItem == -1) {
        return 0;
    }

    release(bestItem, bestGroup, state, ops);
    return 1;
}

void ResourceCache8E3C::pin(
    std::int32_t item,
    std::int32_t group,
    ResourceCacheState8E3C& state) noexcept {
    auto& record = recordAt(state, item, group);

    if (record.loaded2() != 0) {
        record.flags0() |= 0x10u;
    }
}

void ResourceCache8E3C::unpin(
    std::int32_t item,
    std::int32_t group,
    ResourceCacheState8E3C& state) noexcept {
    auto& record = recordAt(state, item, group);

    if (record.loaded2() != 0) {
        record.flags0() &= 0xEFu;
    }
}

void ResourceCache8E3C::unpinAllExceptSpecial(
    ResourceCacheState8E3C& state) noexcept {
    for (std::int32_t group = 0;
         group < state.index->groupCount2B04;
         ++group) {
        auto& groupState =
            state.index->groups[
                static_cast<std::size_t>(group)];

        for (std::int32_t item = 0;
             item < static_cast<std::int32_t>(groupState.count);
             ++item) {
            // Preserve the original quirk: DS:2B10 is compared only against
            // the item index, so the same item number is skipped in every
            // group, not only group zero.
            if (item != state.specialItem2B10) {
                unpin(item, group, state);
            }
        }
    }
}

void ResourceCache8E3C::reloadAndPinSpecial(
    ResourceCacheState8E3C& state,
    const ResourceCacheOps8E3C& ops) {
    // CODE.1 0x9523 calls 0x8B94: unpin then release every item.
    for (std::int32_t group = 0;
         group < state.index->groupCount2B04;
         ++group) {
        auto& groupState =
            state.index->groups[
                static_cast<std::size_t>(group)];

        for (std::int32_t item = 0;
             item < static_cast<std::int32_t>(groupState.count);
             ++item) {
            unpin(item, group, state);
            release(item, group, state, ops);
        }
    }

    load(state.specialItem2B10, 0, state, ops);
    pin(state.specialItem2B10, 0, state);
}

std::uint32_t ResourceCache8E3C::ensureAndGet(
    std::int32_t item,
    std::int32_t group,
    ResourceCacheState8E3C& state,
    const ResourceCacheOps8E3C& ops) {
    load(item, group, state, ops);

    const auto& record =
        state.index->groups[
            static_cast<std::size_t>(group)]
        .records[static_cast<std::size_t>(item)];

    return record.raw[2] == 1
        ? record.dword5()
        : 0u;
}

} // namespace ravenloft::game
