#include "Code1NearestColor5B64.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {

std::int32_t Code1NearestColor5B64::find(
    Code1ColorTreeState5880& state,
    std::uint8_t red,
    std::uint8_t green,
    std::uint8_t blue,
    std::int32_t radius) noexcept {
    std::int32_t bestDistance = 0x2E83;
    std::int16_t bestNode =
        Code1ColorTreeState5880::kSentinel;

    if (state.ready982 == 0) {
        return -1;
    }

    state.stackDepth980 = 0;
    state.visited984 = 0;

    // The original tree axis order is G,R,B. Bounds are stored as signed
    // WORDs and compared with signed JLE/JG.
    const std::array<std::int16_t, 3> low{{
        static_cast<std::int16_t>(
            static_cast<std::int32_t>(green) - radius),
        static_cast<std::int16_t>(
            static_cast<std::int32_t>(red) - radius),
        static_cast<std::int16_t>(
            static_cast<std::int32_t>(blue) - radius)
    }};
    const std::array<std::int16_t, 3> high{{
        static_cast<std::int16_t>(
            static_cast<std::int32_t>(green) + radius),
        static_cast<std::int16_t>(
            static_cast<std::int32_t>(red) + radius),
        static_cast<std::int16_t>(
            static_cast<std::int32_t>(blue) + radius)
    }};

    // 0x5BEF..0x5C11: seed the explicit traversal stack with axis -1 and
    // synthetic root node zero.
    state.traversalStack[0] = -1;
    state.stackDepth980 = 1;
    state.traversalStack[1] = 0;
    state.stackDepth980 = 2;

    while (state.stackDepth980 != 0 &&
           static_cast<std::int16_t>(bestDistance) != 0) {
        --state.stackDepth980;
        const std::int16_t parentIndex =
            state.traversalStack[
                static_cast<std::uint16_t>(state.stackDepth980)];

        std::int16_t current =
            state.nodes[
                static_cast<std::uint16_t>(parentIndex)].right;

        --state.stackDepth980;
        std::int16_t axis =
            state.traversalStack[
                static_cast<std::uint16_t>(state.stackDepth980)];

        while (current != Code1ColorTreeState5880::kSentinel &&
               static_cast<std::int16_t>(bestDistance) != 0) {
            ++state.visited984;

            axis = axis == 2
                ? 0
                : static_cast<std::int16_t>(axis + 1);

            const auto& node =
                state.nodes[static_cast<std::uint16_t>(current)];

            const std::array<std::uint8_t, 3> component{{
                node.axis0, node.axis1, node.axis2
            }};

            const std::int16_t value =
                static_cast<std::int16_t>(
                    component[static_cast<std::size_t>(axis)]);

            // Original lower bound is strict: value <= low searches only the
            // right branch. Upper bound is inclusive: value > high searches
            // only the left branch.
            if (value <= low[static_cast<std::size_t>(axis)]) {
                current = node.right;
                continue;
            }

            if (value > high[static_cast<std::size_t>(axis)]) {
                current = node.left;
                continue;
            }

            // 0x5CB7..0x5CFB: save the right-branch continuation as an
            // [axis,current-node] pair and descend left first.
            state.traversalStack[
                static_cast<std::uint16_t>(
                    state.stackDepth980)] = axis;
            ++state.stackDepth980;

            state.traversalStack[
                static_cast<std::uint16_t>(
                    state.stackDepth980)] = current;
            ++state.stackDepth980;

            // Node storage is G,R,B while callers pass R,G,B.
            const std::int32_t dr =
                static_cast<std::int32_t>(node.axis1) - red;
            const std::int32_t dg =
                static_cast<std::int32_t>(node.axis0) - green;
            const std::int32_t db =
                static_cast<std::int32_t>(node.axis2) - blue;

            const std::int32_t distance =
                dr * dr + dg * dg + db * db;

            // Original JGE preserves the first encountered node on ties.
            if (distance <
                static_cast<std::int16_t>(bestDistance)) {
                bestDistance = distance;
                bestNode = current;
            }

            current = node.left;
        }
    }

    // If no node qualified, bestNode remains 0x0101. That index maps to the
    // sentinel record at DS:88F9, whose payload was initialized to -1.
    return state.nodes[
        static_cast<std::uint16_t>(bestNode)].payload;
}

} // namespace ravenloft::game
