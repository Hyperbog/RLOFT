#include "BackendSupportC5B0.h"

namespace ravenloft::game {

std::uintptr_t BackendSupportC5B0::readLengthPrefixed(
    std::int32_t,
    std::int32_t handle,
    const BackendSupportStateC5B0& state,
    const BackendSupportOpsC5B0& ops) {
    if (state.active2F48 == 0) {
        return 0;
    }

    std::int32_t length = 0;
    (void)ops.read6304(&length, 4, handle);

    const std::uintptr_t block =
        ops.heapAllocate71C8(length + 4);

    auto* destination =
        reinterpret_cast<std::int32_t*>(block);
    destination[0] = length;

    (void)ops.read6304(
        reinterpret_cast<void*>(block + 4),
        length,
        handle);

    return block;
}

void BackendSupportC5B0::freeIfActive(
    std::uintptr_t pointer,
    const BackendSupportStateC5B0& state,
    const BackendSupportOpsC5B0& ops) {
    if (state.active2F48 != 0 &&
        pointer != 0) {
        ops.heapFree7240(pointer);
    }
}

void BackendSupportC5B0::setEnabled(
    std::uint8_t enabled,
    BackendSupportStateC5B0& state,
    const BackendSupportOpsC5B0& ops) {
    state.enabled2F78 = enabled;

    if (state.active2F48 != 0 &&
        enabled == 0) {
        ops.stopC57C();
    }
}

void BackendSupportC5B0::setLookupParam(
    std::int32_t value,
    BackendSupportStateC5B0& state) noexcept {
    state.lookupParam2F4C = value;
}

std::int32_t BackendSupportC5B0::type2Active(
    const BackendSupportStateC5B0& state,
    const BackendSupportOpsC5B0& ops) {
    if (state.active2F48 == 0) {
        return 0;
    }

    return ops.backendStateEC2F(
               state.backend2F74) == 2
        ? 1
        : 0;
}

void BackendSupportC5B0::queue(
    std::int32_t resourceId,
    std::int32_t value,
    std::int32_t slot,
    std::int32_t priority,
    BackendSupportStateC5B0& state) noexcept {
    if (state.active2F48 == 0) {
        return;
    }

    auto& entry =
        state.queue2F50[
            static_cast<std::size_t>(slot)];

    bool accept = true;

    if (entry.resourceId != -1 &&
        entry.priority < priority) {
        accept = false;
    }

    if (accept) {
        entry.resourceId = resourceId;
        entry.value = value;
        entry.priority = priority;
    }
}

void BackendSupportC5B0::noOpC6F0() noexcept {
}

void BackendSupportC5B0::flushQueue(
    BackendSupportStateC5B0& state,
    const BackendSupportOpsC5B0& ops) {
    if (state.active2F48 == 0) {
        return;
    }

    auto& first = state.queue2F50[0];
    auto& second = state.queue2F50[1];

    if (first.resourceId != -1 &&
        second.resourceId != -1) {
        if (first.priority < second.priority) {
            ops.submitC4E0(first.resourceId);
        } else {
            ops.submitC4E0(second.resourceId);
        }

        second.resourceId = -1;
        first.resourceId = -1;
        return;
    }

    if (first.resourceId != -1) {
        ops.submitC4E0(first.resourceId);
        first.resourceId = -1;
        return;
    }

    if (second.resourceId != -1) {
        ops.submitC4E0(second.resourceId);
        second.resourceId = -1;
    }
}

} // namespace ravenloft::game
