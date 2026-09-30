#pragma once
#include <cstdint>
#include <functional>
#include <span>

namespace ravenloft::game {

struct TextInputState1CE4 {
    std::span<std::uint8_t> records5760;
    std::span<const std::int8_t> table165C;
    std::int16_t value12740 = 0;
    std::int16_t value12744 = 0;
    std::uint32_t valueC050 = 0;
};

struct TextInputOps1CE4 {
    std::function<void(std::int32_t,std::int32_t,std::int32_t,std::int32_t)> call2A380;
    std::function<void(std::int32_t,std::int32_t)> call304F4;
    std::function<void(const std::uint8_t*,std::int32_t,std::int32_t)> call1C50;
    std::function<void(std::int32_t,std::int32_t)> call30354;
    std::function<void(std::uint32_t,std::int32_t,std::int32_t,std::int32_t,std::int32_t)> call1A8C8;
    std::function<std::int32_t()> call30AEC;
    std::function<std::int32_t(const std::uint8_t*,std::int32_t)> call1CA8;
};

class TextInput1CE4 {
public:
    // Direct reconstruction of CODE.2 0x1CE4..0x1F6A.
    static void run(TextInputState1CE4& state,
                    const TextInputOps1CE4& ops,
                    std::int32_t arg0,
                    std::int32_t arg1,
                    std::int32_t arg2,
                    std::int32_t arg3);
};

} // namespace ravenloft::game
