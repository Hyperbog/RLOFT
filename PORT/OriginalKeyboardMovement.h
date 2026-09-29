#pragma once
#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct OriginalAxes {
    std::int32_t turn=0;
    std::int32_t forwardBack=0;
    std::int32_t strafe=0;
};

// Keyboard movement integrator recovered from CODE.2 0x1FC8C..0x20199.
// BIOS extended key words are retained as the external API so the SDL/platform
// binding can be a thin translation layer rather than a second movement model.
class OriginalKeyboardMovement {
public:
    using HeldQuery=std::function<bool(std::uint8_t)>; // original scan code state query

    // Processes one BIOS-style key word (0x147..0x151 movement family). The
    // held-query is used exactly where CODE.2 checks scan codes 47/48/49/50
    // to form the forward+turn combination channels.
    bool keyEvent(std::uint16_t biosKey,const HeldQuery& held={});
    void idleTick();
    OriginalAxes axes() const;
    const std::array<std::int16_t,6>& channels() const { return channels_; }

private:
    std::array<std::int16_t,6> channels_{{0,0,0,0,0,0}};
    static constexpr std::array<std::int16_t,6> kImpulse{{10,15,10,20,10,10}};
    static constexpr std::array<std::int16_t,6> kDecay{{25,80,20,20,25,25}};
    static constexpr std::array<std::int16_t,6> kMaximum{{160,80,160,160,80,80}};

    void apply(int channel,int sign);
    void decayExcept(int activeChannel);
};

} // namespace ravenloft::game
