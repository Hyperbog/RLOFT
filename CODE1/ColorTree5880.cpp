#include "ColorTree5880.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {

void ColorTreeInsert5880::insert(
    ColorTreeState5880& state,
    std::uint8_t axis0,
    std::uint8_t axis1,
    std::uint8_t axis2,
    std::int16_t payload,
    std::int16_t nodeIndex) noexcept {
    auto& inserted =
        state.nodes[static_cast<std::uint16_t>(nodeIndex)];

    inserted.axis0 = axis0;
    inserted.axis1 = axis1;
    inserted.axis2 = axis2;
    inserted.payload = payload;
    inserted.right = ColorTreeState5880::kSentinel;
    inserted.left = inserted.right;

    std::int16_t current = state.nodes[0].right;
    std::int16_t previous = 0;
    std::int16_t axis = -1;
    bool attachRight = true;

    const std::array<std::uint8_t, 3> value{{
        axis0, axis1, axis2
    }};

    while (current != ColorTreeState5880::kSentinel) {
        previous = current;

        // 0x5926..0x5930.
        axis = axis == 2
            ? 0
            : static_cast<std::int16_t>(axis + 1);

        attachRight = false;

        const auto& node =
            state.nodes[static_cast<std::uint16_t>(current)];
        const std::array<std::uint8_t, 3> component{{
            node.axis0, node.axis1, node.axis2
        }};

        // Original unsigned byte comparison (JB).
        if (value[static_cast<std::size_t>(axis)] >=
            component[static_cast<std::size_t>(axis)]) {
            current = node.right;
            attachRight = true;
        } else {
            current = node.left;
        }
    }

    auto& parent =
        state.nodes[static_cast<std::uint16_t>(previous)];

    if (attachRight) {
        parent.right = nodeIndex;
    } else {
        parent.left = nodeIndex;
    }
}

std::int32_t PaletteMiddle59AC::find(
    std::span<const std::uint8_t> palette) noexcept {
    std::int32_t bestIndex = 0;
    std::int32_t bestDistance = 0x200;

    for (std::int32_t byteOffset = 0;
         byteOffset < 0x300;
         byteOffset += 3) {
        const auto offset =
            static_cast<std::size_t>(byteOffset);

        const std::int32_t c0 = palette[offset];
        const std::int32_t c1 = palette[offset + 1u];
        const std::int32_t c2 = palette[offset + 2u];

        const std::int32_t d0 = c0 - 0x1F;
        const std::int32_t d1 = c1 - 0x1F;
        const std::int32_t d2 = c2 - 0x1F;

        const std::int32_t distance =
            d0 * d0 + d1 * d1 + d2 * d2;

        // Original JGE leaves the earlier palette entry selected on ties.
        if (distance < bestDistance) {
            bestDistance = distance;
            bestIndex = byteOffset / 3;
        }
    }

    return bestIndex;
}

void ColorTreeBuild5A34::build(
    ColorTreeState5880& state,
    std::span<const std::uint8_t> palette) noexcept {
    // Synthetic root at DS:7FF0.
    state.nodes[0].payload = -1;
    state.nodes[0].left = ColorTreeState5880::kSentinel;
    state.nodes[0].right = ColorTreeState5880::kSentinel;

    // The second root record at DS:88F9 is exactly node index 0x101 from
    // DS:7FF0. It is the sentinel payload returned when no candidate wins.
    auto& sentinel =
        state.nodes[ColorTreeState5880::kSentinel];
    sentinel.payload = -1;
    sentinel.left = ColorTreeState5880::kSentinel;
    sentinel.right = ColorTreeState5880::kSentinel;

    std::int16_t nodeIndex = 1;

    std::int32_t paletteIndex =
        PaletteMiddle59AC::find(palette);
    std::int32_t ascendingIndex = paletteIndex + 1;

    // 0x5AA6..0x5AFA: middle entry down through palette entry zero.
    while (static_cast<std::int16_t>(paletteIndex) > -1) {
        const std::size_t offset =
            static_cast<std::size_t>(paletteIndex) * 3u;

        // The original 9-byte packed call payload is G,R,B,index,0,0.
        ColorTreeInsert5880::insert(
            state,
            palette[offset + 1u],
            palette[offset],
            palette[offset + 2u],
            static_cast<std::int16_t>(paletteIndex),
            nodeIndex);

        ++nodeIndex;
        --paletteIndex;
    }

    // 0x5AFE..0x5B51: middle+1 up through palette entry 255.
    while (static_cast<std::int16_t>(ascendingIndex) < 0x100) {
        const std::size_t offset =
            static_cast<std::size_t>(ascendingIndex) * 3u;

        ColorTreeInsert5880::insert(
            state,
            palette[offset + 1u],
            palette[offset],
            palette[offset + 2u],
            static_cast<std::int16_t>(ascendingIndex),
            nodeIndex);

        ++nodeIndex;
        ++ascendingIndex;
    }

    state.ready982 = 1;
}

} // namespace ravenloft::game
