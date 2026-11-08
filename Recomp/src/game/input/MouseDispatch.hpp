#pragma once
// Host physical mouse edge mailbox. Native UI publishes input; CODE.2 guest
// worker consumes it on its own suspended INT33 function 03h request.
#include <array>
#include <cstddef>
#include <cstdint>
#include <mutex>

namespace rloft::input::mouse_dispatch {
struct Sample {
    std::uint32_t buttons=0u, x=0x140u, y=0xC8u;
};
class Mailbox final {
    static constexpr std::size_t kCapacity=64u;
    std::mutex mutex_{};
    std::array<Sample,kCapacity> edges_{};
    std::size_t read_=0u,write_=0u,count_=0u;
    Sample latest_{};
    bool invalid_=false;
public:
    void publish(Sample sample) noexcept;
    void cancel_buttons() noexcept;
    [[nodiscard]] bool consume(Sample& sample) noexcept;
};
}
