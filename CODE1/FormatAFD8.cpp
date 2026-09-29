#include "FormatAFD8.h"

#include <array>

namespace ravenloft::game {

void FormatAFD8::emit(
    void*,
    FormatSinkAFD8& sink,
    std::uint8_t value) noexcept {
    char* destination = sink.cursor;
    ++sink.cursor;

    *destination =
        static_cast<char>(value);

    ++sink.count;
}

std::int32_t FormatAFD8::format(
    char* destination,
    const char* formatString,
    const void* argumentCursor,
    const FormatOpsAFD8& ops) {
    const auto callback =
        [](void* ignored,
           FormatSinkAFD8& sink,
           std::uint8_t value) {
            emit(ignored, sink, value);
        };

    const std::int32_t length =
        ops.callEDE7(
            destination,
            formatString,
            argumentCursor,
            callback);

    destination[length] = '\0';
    return length;
}

void SampleDataB060::load(
    SampleDataStateB060& state,
    const SampleDataOpsB060& ops) {
    // Exact ten bytes copied from CODE.1 data DS:030C.
    std::array<char, 10> fileName{{
        'S','A','M','P','L','E','.','A','D','\0'
    }};

    const std::int32_t size =
        ops.querySize627C(
            fileName.data());

    state.sampleData2B98 =
        ops.heapAllocate71C8(
            static_cast<std::uint32_t>(size));

    const std::int32_t handle =
        ops.openRead62A8(
            fileName.data());

    if (handle == -1) {
        std::array<char, 256> message{};

        // Exact CODE.1 data DS:0318:
        // "Timber file \"%s\" not found\n"
        ops.formatAFEE(
            message.data(),
            "Timber file \"%s\" not found\n",
            fileName.data());

        // Original code falls through after this call because 0x9814 is
        // expected to terminate execution.
        ops.fatal9814(
            message.data());
    }

    (void)ops.read6304(
        reinterpret_cast<void*>(
            state.sampleData2B98),
        size,
        handle);

    (void)ops.close62F0(handle);

    state.loaded2B94 = 1;
}

} // namespace ravenloft::game
