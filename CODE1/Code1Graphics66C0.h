#pragma once

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct Code1ResourceView66C0 {
    std::uintptr_t handle = 0;
    std::uint16_t width = 0;
    std::uint8_t heightUnits = 0;

    explicit operator bool() const noexcept { return handle != 0; }
    std::int32_t heightPixels() const noexcept {
        return static_cast<std::int32_t>(heightUnits) * 8;
    }
};

struct Code1ClipWork6854 {
    std::int32_t sourceLeadBytes = 0;
    std::int32_t sourceTailBytes = 0;
    std::int32_t width = 0;
    std::int32_t topSkip = 0;
    std::int32_t bottomSkip = 0;
    std::int32_t height = 0;
};

struct Code1GraphicsState66C0 {
    std::int16_t screenWidth23D7 = 0;
    std::int16_t screenHeight23D9 = 0;

    std::int32_t clipLeft23ED = 0;
    std::int32_t clipRight23F1 = 0;
    std::int32_t clipTop23F5 = 0;
    std::int32_t clipBottom23F9 = 0;

    std::array<std::int16_t, 4> words2401{};

    // CODE.1 reserves exactly four 24-byte clipping records at
    // DS:240C..246B, followed by nesting depth at DS:246C.
    std::array<Code1ClipWork6854, 4> scratch240C{};
    std::int32_t scratchDepth246C = 0;
};

struct Code1GraphicsOps66C0 {
    std::function<void(std::uintptr_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> callA484;
    std::function<void(std::uintptr_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> callA4E8;

    std::function<void(std::int32_t, std::int32_t, std::int32_t,
                       std::uintptr_t)> callA6F0;
    std::function<void(std::int32_t, std::int32_t, std::int32_t,
                       std::uintptr_t, std::uint8_t)> callA7E8;
    std::function<void(std::int32_t, std::int32_t, std::int32_t,
                       std::uintptr_t, std::int32_t)> callA8F4;

    std::function<void(std::int32_t, std::int32_t, std::int32_t,
                       const Code1ClipWork6854&, std::uintptr_t)> callAA20;
    std::function<void(std::int32_t, std::int32_t, std::int32_t,
                       const Code1ClipWork6854&, std::uintptr_t,
                       std::uint8_t)> callAC10;

    std::function<void(std::int32_t, std::int32_t,
                       std::uintptr_t)> call862C;

    std::function<Code1ResourceView66C0(
        std::int32_t, std::int32_t)> call954C;

    std::function<void()> call8960;
    std::function<void()> callDA18;
    std::function<void()> callDA1A;
};

class Code1Graphics66C0 {
public:
    // CODE.1 0x66C0..0x6742. 0x6743 is alignment.
    static void capture(std::uintptr_t buffer,
                        std::int32_t x, std::int32_t y,
                        std::int32_t width, std::int32_t height,
                        const Code1GraphicsState66C0& state,
                        const Code1GraphicsOps66C0& ops);

    // CODE.1 0x6744..0x67C6. 0x67C7 is alignment.
    static void restore(std::uintptr_t buffer,
                        std::int32_t x, std::int32_t y,
                        std::int32_t width, std::int32_t height,
                        const Code1GraphicsState66C0& state,
                        const Code1GraphicsOps66C0& ops);

    // CODE.1 0x67C8..0x67F1. 0x67F2..0x67F3 is alignment.
    static void drawDirect(std::int32_t page,
                           std::int32_t x, std::int32_t y,
                           const Code1ResourceView66C0& resource,
                           const Code1GraphicsOps66C0& ops);

    // CODE.1 0x67F4..0x6822. 0x6823 is alignment.
    static void drawDirectMasked(std::int32_t page,
                                 std::int32_t x, std::int32_t y,
                                 const Code1ResourceView66C0& resource,
                                 std::uint8_t mask,
                                 const Code1GraphicsOps66C0& ops);

    // CODE.1 0x6824..0x6851. 0x6852..0x6853 is alignment.
    static void drawDirectMode(std::int32_t page,
                               std::int32_t x, std::int32_t y,
                               const Code1ResourceView66C0& resource,
                               std::int32_t mode,
                               const Code1GraphicsOps66C0& ops);

    // CODE.1 0x6854..0x6A02. 0x6A03 is alignment.
    static std::int32_t drawClipped(
        std::int32_t page,
        std::int32_t x, std::int32_t y,
        const Code1ResourceView66C0& resource,
        Code1ClipWork6854& work,
        Code1GraphicsState66C0& state,
        const Code1GraphicsOps66C0& ops);

    // CODE.1 0x6A04..0x6BBC. 0x6BBD..0x6BBF is alignment.
    static std::int32_t drawClippedMasked(
        std::int32_t page,
        std::int32_t x, std::int32_t y,
        const Code1ResourceView66C0& resource,
        std::uint8_t mask,
        Code1ClipWork6854& work,
        Code1GraphicsState66C0& state,
        const Code1GraphicsOps66C0& ops);

    // CODE.1 0x6BC0..0x6C42. 0x6C43 is alignment.
    static void setClip(std::int32_t left,
                        std::int32_t top,
                        std::int32_t right,
                        std::int32_t bottom,
                        Code1GraphicsState66C0& state) noexcept;

    // CODE.1 0x6C44..0x6C5D. 0x6C5E..0x6C5F is alignment.
    static std::array<std::int32_t, 4> getClip(
        const Code1GraphicsState66C0& state) noexcept;

    // CODE.1 0x6C60..0x6C94. 0x6C95..0x6C97 is alignment.
    static void drawResource(std::int32_t page,
                             std::int32_t x, std::int32_t y,
                             std::int32_t resourceId,
                             std::int32_t bank,
                             const Code1GraphicsOps66C0& ops);

    // CODE.1 0x6C98..0x6CFC. 0x6CFD..0x6CFF is alignment.
    static std::int32_t drawResourceClipped(
        std::int32_t page,
        std::int32_t x, std::int32_t y,
        std::int32_t resourceId,
        std::int32_t bank,
        Code1GraphicsState66C0& state,
        const Code1GraphicsOps66C0& ops);

    // CODE.1 0x6D00..0x6D50. 0x6D51..0x6D53 is alignment.
    static void callResource862C(
        std::int32_t x, std::int32_t y,
        std::int32_t resourceId,
        std::int32_t bank,
        Code1GraphicsState66C0& state,
        const Code1GraphicsOps66C0& ops);

    // CODE.1 0x6D54..0x6DAB.
    static std::int32_t drawViewClipped(
        std::int32_t page,
        std::int32_t x, std::int32_t y,
        const Code1ResourceView66C0& resource,
        Code1GraphicsState66C0& state,
        const Code1GraphicsOps66C0& ops);

    // CODE.1 0x6DAC..0x6DE5. 0x6DE6..0x6DE7 is alignment.
    static void drawResourceMasked(
        std::int32_t page,
        std::int32_t x, std::int32_t y,
        std::int32_t resourceId,
        std::int32_t bank,
        std::uint8_t mask,
        const Code1GraphicsOps66C0& ops);

    // CODE.1 0x6DE8..0x6E4E. 0x6E4F is alignment.
    static std::int32_t drawResourceClippedMasked(
        std::int32_t page,
        std::int32_t x, std::int32_t y,
        std::int32_t resourceId,
        std::int32_t bank,
        std::uint8_t mask,
        Code1GraphicsState66C0& state,
        const Code1GraphicsOps66C0& ops);

    // CODE.1 0x6E50..0x6EAC. 0x6EAD..0x6EAF is alignment.
    static std::int32_t drawViewClippedMasked(
        std::int32_t page,
        std::int32_t x, std::int32_t y,
        const Code1ResourceView66C0& resource,
        std::uint8_t mask,
        Code1GraphicsState66C0& state,
        const Code1GraphicsOps66C0& ops);

    // CODE.1 0x6EB0..0x6EE8.
    static void drawResourceMode(
        std::int32_t page,
        std::int32_t x, std::int32_t y,
        std::int32_t resourceId,
        std::int32_t bank,
        std::int32_t mode,
        const Code1GraphicsOps66C0& ops);
};

} // namespace ravenloft::game
