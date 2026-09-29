#pragma once

#include "Vga9CA0.h"

#include <cstdint>

namespace ravenloft::game {

class VgaDrawA05C {
public:
    // Direct observable reconstruction of CODE.1 0xA05C..0xA219.
    // CODE.1 0xA21A..0xA21B is alignment.
    static void line(std::int32_t first1,
                     std::int32_t second1,
                     std::int32_t first2,
                     std::int32_t second2,
                     std::uint8_t color,
                     VgaState9CA0& state,
                     const VgaOps9CA0& ops);

    // Direct reconstruction of CODE.1 0xA21C..0xA2AD.
    // CODE.1 0xA2AE..0xA2AF is alignment.
    static void shiftPageMemory(std::int32_t amount,
                                VgaState9CA0& state,
                                const VgaOps9CA0& ops);

    // Direct reconstruction of CODE.1 0xA2B0..0xA358.
    // CODE.1 0xA359..0xA35B is alignment.
    static void copyRows(std::int32_t rowCount,
                         std::int32_t offset,
                         VgaState9CA0& state,
                         const VgaOps9CA0& ops);

    // Direct reconstruction of CODE.1 0xA35C..0xA369.
    // CODE.1 0xA36A..0xA36B is alignment.
    static void setActivePage(std::uint8_t page,
                              VgaState9CA0& state) noexcept;

    // Direct reconstruction of CODE.1 0xA36C..0xA38C.
    // CODE.1 0xA38D..0xA38F is alignment.
    static void setDisplayPage(std::uint8_t page,
                               const VgaState9CA0& state,
                               const VgaOps9CA0& ops);

    // Direct reconstruction of CODE.1 0xA390..0xA3E4.
    // CODE.1 0xA3E5..0xA3E7 is alignment.
    static void cloneCurrentPageTo(std::uint8_t page,
                                   VgaState9CA0& state,
                                   const VgaOps9CA0& ops);

    // Direct reconstruction of CODE.1 0xA3E8..0xA472.
    // CODE.1 0xA473 is alignment.
    static void copyRectBetweenPages(std::uint8_t destinationPage,
                                     std::int32_t sourceFirst,
                                     std::int32_t sourceSecond,
                                     std::int32_t destinationFirst,
                                     std::int32_t destinationSecond,
                                     std::int32_t firstCount,
                                     std::int32_t secondCount,
                                     VgaState9CA0& state,
                                     const VgaOps9CA0& ops);

    // Direct reconstruction of CODE.1 0xA474..0xA483.
    static void setDisplayParameter(std::int32_t value,
                                    const VgaOps9CA0& ops);
};

} // namespace ravenloft::game
