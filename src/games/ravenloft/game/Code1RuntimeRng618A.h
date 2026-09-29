#pragma once

#include <cstdint>

namespace ravenloft::game {

// Faithful reconstruction of the CODE.1 runtime RNG helpers:
//   0x6184 - return address of the RNG state storage
//   0x618A - advance the 32-bit LCG and return a 15-bit value
//   0x61AC - replace the RNG state
//
// The original state lived at CODE.1 data offset 0x23CC. The source port keeps
// the same 32-bit state semantics without exposing an emulated flat address.
class Code1RuntimeRng618A {
public:
    explicit Code1RuntimeRng618A(std::uint32_t state = 1u) noexcept : state_(state) {}

    // CODE.1 0x6184.
    std::uint32_t* stateStorage() noexcept { return &state_; }
    const std::uint32_t* stateStorage() const noexcept { return &state_; }

    // CODE.1 0x618A.
    std::uint16_t next15() noexcept;

    // CODE.1 0x61AC.
    void seed(std::uint32_t state) noexcept;

    std::uint32_t state() const noexcept { return state_; }

private:
    std::uint32_t state_ = 1u;
};

} // namespace ravenloft::game
