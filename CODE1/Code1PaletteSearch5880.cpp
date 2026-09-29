#include "Code1PaletteSearch5880.h"

#include <cstddef>

namespace ravenloft::game {

void Code1PaletteInsert5880::insert(
    Code1PaletteSearchState5880& state,
    const Code1PaletteTreeNode5880& input,
    std::int16_t newIndex) noexcept {
    auto& node =
        state.tree7FF0[static_cast<std::size_t>(newIndex)];

    node.keyGRB = input.keyGRB;
    node.value = input.value;
    node.low = Code1PaletteSearchState5880::kSentinel;
    node.high = Code1PaletteSearchState5880::kSentinel;

    std::int16_t current =
        static_cast<std::int16_t>(state.tree7FF0[0].high);

    std::int16_t parent = 0;
    std::int16_t axis = -1;
    bool useHighLink = true;

    while (static_cast<std::uint16_t>(current) !=
           Code1PaletteSearchState5880::kSentinel) {
        parent = current;

        axis = axis == 2
            ? 0
            : static_cast<std::int16_t>(axis + 1);

        useHighLink = false;

        const auto component =
            static_cast<std::size_t>(axis);
        auto& currentNode =
            state.tree7FF0[static_cast<std::size_t>(current)];

        if (input.keyGRB[component] >=
            currentNode.keyGRB[component]) {
            current =
                static_cast<std::int16_t>(currentNode.high);
            useHighLink = true;
        } else {
            current =
                static_cast<std::int16_t>(currentNode.low);
        }
    }

    auto& parentNode =
        state.tree7FF0[static_cast<std::size_t>(parent)];

    if (useHighLink) {
        parentNode.high =
            static_cast<std::uint16_t>(newIndex);
    } else {
        parentNode.low =
            static_cast<std::uint16_t>(newIndex);
    }
}

std::int32_t Code1PaletteCenter59AC::find(
    const std::array<std::uint8_t, 0x300>& palette) noexcept {
    std::int32_t bestIndex = 0;
    std::int32_t bestDistance = 0x200;

    for (std::int32_t offset = 0;
         offset < 0x300;
         offset += 3) {
        const std::int32_t red =
            static_cast<std::int32_t>(palette[offset]);
        const std::int32_t green =
            static_cast<std::int32_t>(palette[offset + 1]);
        const std::int32_t blue =
            static_cast<std::int32_t>(palette[offset + 2]);

        const std::int32_t dr = red - 0x1F;
        const std::int32_t dg = green - 0x1F;
        const std::int32_t db = blue - 0x1F;

        const std::int32_t distance =
            dr * dr + dg * dg + db * db;

        if (distance < bestDistance) {
            bestDistance = distance;
            bestIndex = offset / 3;
        }
    }

    return bestIndex;
}

void Code1PaletteTreeBuild5A34::build(
    Code1PaletteSearchState5880& state,
    const std::array<std::uint8_t, 0x300>& palette) noexcept {
    state.tree7FF0[0].value = 0xFFFFu;
    state.tree7FF0[0].low =
        Code1PaletteSearchState5880::kSentinel;
    state.tree7FF0[0].high =
        Code1PaletteSearchState5880::kSentinel;

    state.tree88F9[0].value = 0xFFFFu;
    state.tree88F9[0].low =
        Code1PaletteSearchState5880::kSentinel;
    state.tree88F9[0].high =
        Code1PaletteSearchState5880::kSentinel;

    std::int16_t nextNode = 1;

    const std::int32_t center =
        Code1PaletteCenter59AC::find(palette);

    const auto add = [&](std::int32_t paletteIndex) {
        const std::size_t offset =
            static_cast<std::size_t>(paletteIndex) * 3u;

        Code1PaletteTreeNode5880 node{};

        // The original local nine-byte record is laid out G,R,B,index.
        node.keyGRB[0] = palette[offset + 1u];
        node.keyGRB[1] = palette[offset];
        node.keyGRB[2] = palette[offset + 2u];
        node.value =
            static_cast<std::uint16_t>(paletteIndex);

        Code1PaletteInsert5880::insert(
            state,
            node,
            nextNode);

        ++nextNode;
    };

    for (std::int32_t index = center;
         index >= 0;
         --index) {
        add(index);
    }

    for (std::int32_t index = center + 1;
         index < 0x100;
         ++index) {
        add(index);
    }

    state.word982 = 1;
}

std::int32_t Code1PaletteNearest5B64::find(
    Code1PaletteSearchState5880& state,
    std::uint8_t red,
    std::uint8_t green,
    std::uint8_t blue,
    std::int32_t margin) noexcept {
    if (state.word982 == 0) {
        return -1;
    }

    std::int32_t bestDistance = 0x2E83;
    std::uint16_t bestNode =
        Code1PaletteSearchState5880::kSentinel;

    // Locals are ordered by the tree's packed G,R,B component order.
    const std::array<std::int16_t, 3> low{{
        static_cast<std::int16_t>(
            static_cast<std::int32_t>(green) - margin),
        static_cast<std::int16_t>(
            static_cast<std::int32_t>(red) - margin),
        static_cast<std::int16_t>(
            static_cast<std::int32_t>(blue) - margin)
    }};

    const std::array<std::int16_t, 3> high{{
        static_cast<std::int16_t>(
            static_cast<std::int32_t>(green) + margin),
        static_cast<std::int16_t>(
            static_cast<std::int32_t>(red) + margin),
        static_cast<std::int16_t>(
            static_cast<std::int32_t>(blue) + margin)
    }};

    state.word980 = 0;
    state.word984 = 0;

    state.stack7BF0[0] = -1;
    state.word980 = 1;
    state.stack7BF0[1] = 0;
    state.word980 = 2;

    while (state.word980 != 0 &&
           bestDistance != 0) {
        --state.word980;

        const std::int16_t parent =
            state.stack7BF0[
                static_cast<std::size_t>(state.word980)];

        std::uint16_t current =
            state.tree7FF0[
                static_cast<std::size_t>(parent)].high;

        --state.word980;

        std::int16_t axis =
            state.stack7BF0[
                static_cast<std::size_t>(state.word980)];

        while (current !=
                   Code1PaletteSearchState5880::kSentinel &&
               bestDistance != 0) {
            ++state.word984;

            axis = axis == 2
                ? 0
                : static_cast<std::int16_t>(axis + 1);

            const auto& node =
                state.tree7FF0[current];

            const auto component =
                static_cast<std::size_t>(axis);

            const std::int32_t split =
                node.keyGRB[component];

            if (split <= low[component]) {
                current = node.high;
                continue;
            }

            if (split > high[component]) {
                current = node.low;
                continue;
            }

            state.stack7BF0[
                static_cast<std::size_t>(state.word980)] =
                axis;
            ++state.word980;

            state.stack7BF0[
                static_cast<std::size_t>(state.word980)] =
                static_cast<std::int16_t>(current);
            ++state.word980;

            const std::int32_t dr =
                static_cast<std::int32_t>(node.keyGRB[1]) -
                static_cast<std::int32_t>(red);
            const std::int32_t dg =
                static_cast<std::int32_t>(node.keyGRB[0]) -
                static_cast<std::int32_t>(green);
            const std::int32_t db =
                static_cast<std::int32_t>(node.keyGRB[2]) -
                static_cast<std::int32_t>(blue);

            const std::int32_t distance =
                dr * dr + dg * dg + db * db;

            if (distance < bestDistance) {
                bestDistance = distance;
                bestNode = current;
            }

            current = node.low;
        }
    }

    if (bestNode ==
        Code1PaletteSearchState5880::kSentinel) {
        // Sentinel index 257 aliases tree88F9[0] in original memory.
        return state.tree88F9[0].value;
    }

    return state.tree7FF0[bestNode].value;
}

} // namespace ravenloft::game
