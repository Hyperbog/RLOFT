#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>

namespace ravenloft::game {

struct TextExport2474State {
    std::span<const std::uint8_t> records5760;
    std::uint8_t value5F60 = 0;
    std::uint8_t value5F61 = 0;
};

struct TextExport2474Ops {
    std::function<void(std::uint8_t,std::int32_t)> call31107;
    std::function<void(std::uint8_t*,std::int32_t,std::int32_t)> call30D30;
    std::function<void(const std::uint8_t*,std::int32_t,std::int32_t)> call31304;
};

class TextExport2474 {
public:
    // Direct reconstruction of CODE.2 0x2474..0x26E5.
    static void run(const TextExport2474State& state,
                    const TextExport2474Ops& ops,
                    std::int32_t outputHandle,
                    std::int32_t alternateMode);
};

} // namespace ravenloft::game
