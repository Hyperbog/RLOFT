#pragma once

#include <cstdint>
#include <functional>
#include <span>

namespace ravenloft::game {

struct DispatcherState02A8 {
    // Script block originally reached through DS:0xCF44. The selected block
    // begins at index * 0x1A; byte 0 is the entry count and each entry is
    // five bytes beginning at that same base.
    std::span<std::uint8_t> scripts;

    // Raw original data regions used directly by dispatcher cases.
    std::span<std::uint8_t> data198E0;
    std::int16_t value25619 = 0;
    std::int32_t valueBF44 = 0;
    std::uint8_t valueC050 = 0;
    std::int16_t value8600 = 0;
    std::uint8_t value8602 = 0;
};

struct DispatcherOps02A8 {
    std::function<std::uint8_t*(std::uint8_t, std::uint8_t)> call1E8C8;
    std::function<void(std::uint8_t, std::uint8_t)> call0010;
    std::function<void()> call0074;
    std::function<void()> call00B8;
    std::function<std::int32_t()> call01FC;

    std::function<void(std::uint8_t*, std::uint8_t, std::uint8_t)> call20C0C;
    std::function<void(std::uint8_t, std::int32_t,
                       std::uint32_t, std::uint32_t)> call8A80;
    std::function<void(std::int32_t)> call13EF8;
    std::function<void(std::uint32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call28110;
    std::function<void(std::int32_t)> call28DCC;
    std::function<void()> call28EF0;
    std::function<void(std::uint32_t, std::int32_t)> call28C54;
    std::function<void(std::int32_t)> call1C124;
    std::function<void(std::int32_t)> call1C160;
    std::function<void(std::int32_t)> call1C1A0;

    // Case 7 preserves the original presentation/input sequence as one
    // address-labelled operation until those callees are reconstructed.
    std::function<void(std::uint8_t)> case5B3Sequence;
};

class Dispatcher02A8 {
public:
    // Direct reconstruction of CODE.2 0x02A8..0x06B8.
    static void run(DispatcherState02A8& state,
                    const DispatcherOps02A8& ops,
                    std::int32_t scriptIndex);
};

} // namespace ravenloft::game
