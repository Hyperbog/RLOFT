#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>

namespace ravenloft::game {

// Semantic representation of the output context consumed by CODE.1 0xAFD8.
// The original 32-bit structure stores its write pointer at +0 and emitted
// character count at +0x10.
struct Code1FormatSinkAFD8 {
    char* cursor = nullptr;
    std::array<std::uint8_t, 12> opaque4_15{};
    std::uint32_t count = 0;
};

struct Code1FormatOpsAFD8 {
    // CODE.1 runtime formatter at 0xEDE7. Its implementation is reconstructed
    // later in the sequential pass; AFEE passes destination, format string,
    // pointer to the first variadic argument, and callback 0xAFD8.
    std::function<std::int32_t(
        char*,
        const char*,
        const void*,
        const std::function<void(
            void*, Code1FormatSinkAFD8&, std::uint8_t)>&)> callEDE7;
};

class Code1FormatAFD8 {
public:
    // Direct reconstruction of callback CODE.1 0xAFD8..0xAFED.
    static void emit(
        void* ignored,
        Code1FormatSinkAFD8& sink,
        std::uint8_t value) noexcept;

    // Direct reconstruction of CODE.1 0xAFEE..0xB019.
    static std::int32_t format(
        char* destination,
        const char* formatString,
        const void* argumentCursor,
        const Code1FormatOpsAFD8& ops);

    // CODE.1 0xB01A..0xB054 is the already reconstructed strcat helper.
    // CODE.1 0xB055..0xB05F is zero/data padding.
};

struct Code1SampleDataStateB060 {
    std::uint8_t loaded2B94 = 0;
    std::uintptr_t sampleData2B98 = 0;
};

struct Code1SampleDataOpsB060 {
    std::function<std::int32_t(const std::string&)> querySize627C;
    std::function<std::uintptr_t(std::size_t)> heapAllocate71C8;
    std::function<std::int32_t(const std::string&)> openRead62A8;

    // AFEE with the single %s argument used by B060.
    std::function<std::int32_t(
        char*, const char*, const char*)> formatAFEE;

    std::function<void(const std::string&)> fatal9814;
    std::function<std::int32_t(
        void*, std::int32_t, std::int32_t)> read6304;
    std::function<std::int32_t(std::int32_t)> close62F0;
};

class Code1SampleDataB060 {
public:
    // Direct reconstruction of CODE.1 0xB060..0xB0F5.
    // CODE.1 0xB0F6..0xB0F7 is alignment.
    static void load(
        Code1SampleDataStateB060& state,
        const Code1SampleDataOpsB060& ops);
};

} // namespace ravenloft::game
