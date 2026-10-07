#include "rloft/character_palette_remap.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {
namespace {

constexpr std::int16_t kNil = 0x0101;

struct PaletteNode {
    // function_5880 stores the tree key as G,R,B.
    std::array<std::uint8_t, 3> key{};
    std::uint16_t source_index{};
    std::int16_t left{kNil};
    std::int16_t right{kNil};
};

class PaletteTree final {
public:
    explicit PaletteTree(const Palette6& palette)
        : palette_(palette) {
        nodes_[0].left = kNil;
        nodes_[0].right = kNil;

        // function_59AC: choose the source entry nearest (31,31,31), using
        // the encoded signed-low-word distance comparison and 0x200 seed.
        int first = 0;
        std::int16_t best = 0x0200;
        for (int i = 0; i < 256; ++i) {
            const auto r = component(i, 0);
            const auto g = component(i, 1);
            const auto b = component(i, 2);
            const std::uint32_t dr =
                static_cast<std::uint32_t>(r) - 31U;
            const std::uint32_t dg =
                static_cast<std::uint32_t>(g) - 31U;
            const std::uint32_t db =
                static_cast<std::uint32_t>(b) - 31U;
            const auto raw =
                dr * dr + dg * dg + db * db;
            const auto distance =
                static_cast<std::int16_t>(
                    static_cast<std::uint16_t>(raw));
            if (distance < best) {
                best = distance;
                first = i;
            }
        }

        int serial = 1;
        for (int i = first; i >= 0; --i) {
            insert(i, serial++);
        }
        for (int i = first + 1; i < 256; ++i) {
            insert(i, serial++);
        }

        if (serial != 257) {
            throw std::logic_error(
                "CODE.1 palette tree did not insert 256 entries");
        }
    }

    [[nodiscard]] std::uint8_t nearest(
        std::uint8_t r,
        std::uint8_t g,
        std::uint8_t b,
        std::uint16_t radius) const {
        // Exact function_5B64 traversal. Stack stores alternating split-axis
        // and parent-node words. Popping a parent resumes at its right child.
        std::array<std::int16_t, 514> stack{};
        int sp = 0;
        stack[sp++] = -1;
        stack[sp++] = 0;

        const std::array<std::int16_t, 6> bounds{{
            static_cast<std::int16_t>(
                static_cast<std::uint16_t>(g) - radius),
            static_cast<std::int16_t>(
                static_cast<std::uint16_t>(g) + radius),
            static_cast<std::int16_t>(
                static_cast<std::uint16_t>(r) - radius),
            static_cast<std::int16_t>(
                static_cast<std::uint16_t>(r) + radius),
            static_cast<std::int16_t>(
                static_cast<std::uint16_t>(b) - radius),
            static_cast<std::int16_t>(
                static_cast<std::uint16_t>(b) + radius)
        }};

        std::int32_t best_distance = 0x2E83;
        std::int16_t best_node = kNil;

        while (sp != 0 &&
               static_cast<std::int16_t>(best_distance) != 0) {
            const auto popped = stack[--sp];
            auto node = right_child(popped);
            auto axis = stack[--sp];

            while (node != kNil &&
                   static_cast<std::int16_t>(best_distance) != 0) {
                axis = axis == 2
                    ? 0
                    : static_cast<std::int16_t>(axis + 1);

                const auto value = static_cast<std::int16_t>(
                    nodes_[static_cast<std::size_t>(node)]
                        .key[static_cast<std::size_t>(axis)]);

                if (value <= bounds[
                        static_cast<std::size_t>(axis) * 2U]) {
                    node = nodes_[
                        static_cast<std::size_t>(node)].right;
                    continue;
                }
                if (value > bounds[
                        static_cast<std::size_t>(axis) * 2U + 1U]) {
                    node = nodes_[
                        static_cast<std::size_t>(node)].left;
                    continue;
                }

                if (sp + 2 > static_cast<int>(stack.size())) {
                    throw std::runtime_error(
                        "CODE.1 palette tree stack overflow");
                }
                stack[sp++] = axis;
                stack[sp++] = node;

                const auto& n =
                    nodes_[static_cast<std::size_t>(node)];
                const int dr =
                    static_cast<int>(n.key[1]) - r;
                const int dg =
                    static_cast<int>(n.key[0]) - g;
                const int db =
                    static_cast<int>(n.key[2]) - b;
                const int distance =
                    dr * dr + dg * dg + db * db;

                if (static_cast<std::int16_t>(distance) <
                    static_cast<std::int16_t>(best_distance)) {
                    best_distance = distance;
                    best_node = node;
                }

                node = n.left;
            }
        }

        if (best_node == kNil) {
            throw std::runtime_error(
                "CODE.1 palette nearest search found no node");
        }

        return static_cast<std::uint8_t>(
            nodes_[static_cast<std::size_t>(best_node)]
                .source_index);
    }

private:
    [[nodiscard]] std::uint8_t component(
        int index, int rgb) const noexcept {
        return palette_.rgb[
            static_cast<std::size_t>(index) * 3U +
            static_cast<std::size_t>(rgb)];
    }

    [[nodiscard]] std::int16_t right_child(
        std::int16_t node) const {
        if (node < 0 ||
            node >= static_cast<std::int16_t>(nodes_.size())) {
            throw std::out_of_range(
                "CODE.1 palette tree node index");
        }
        return nodes_[static_cast<std::size_t>(node)].right;
    }

    void insert(int source_index, int serial) {
        if (serial <= 0 ||
            serial >= static_cast<int>(nodes_.size())) {
            throw std::out_of_range(
                "CODE.1 palette tree serial");
        }

        auto& inserted =
            nodes_[static_cast<std::size_t>(serial)];
        inserted.key = {{
            component(source_index, 1),
            component(source_index, 0),
            component(source_index, 2)
        }};
        inserted.source_index =
            static_cast<std::uint16_t>(source_index);
        inserted.left = kNil;
        inserted.right = kNil;

        std::int16_t node = nodes_[0].right;
        std::int16_t parent = 0;
        std::int16_t axis = -1;
        bool right = true;

        while (node != kNil) {
            parent = node;
            axis = axis == 2
                ? 0
                : static_cast<std::int16_t>(axis + 1);
            right = false;

            const auto& current =
                nodes_[static_cast<std::size_t>(node)];
            if (inserted.key[static_cast<std::size_t>(axis)] >=
                current.key[static_cast<std::size_t>(axis)]) {
                node = current.right;
                right = true;
            } else {
                node = current.left;
            }
        }

        auto& parent_node =
            nodes_[static_cast<std::size_t>(parent)];
        if (right) {
            parent_node.right =
                static_cast<std::int16_t>(serial);
        } else {
            parent_node.left =
                static_cast<std::int16_t>(serial);
        }
    }

    const Palette6& palette_;
    std::array<PaletteNode, 257> nodes_{};
};

int div4_toward_zero(int value) noexcept {
    return value / 4;
}

std::uint8_t luminance_bucket(
    const Palette6& palette,
    int target) {
    int bucket = 0;
    while (bucket < 0x1C &&
           static_cast<int>(
               palette.rgb[
                   static_cast<std::size_t>(bucket) * 3U]) <=
               target) {
        ++bucket;
    }
    --bucket;
    return static_cast<std::uint8_t>(bucket);
}

}  // namespace

CharacterPaletteRemaps CharacterPaletteRemaps::build(
    const Palette6& palette) {
    CharacterPaletteRemaps out;
    const PaletteTree tree(palette);

    // function_4A0C -> 5FF8..64F7.
    for (int row = 0; row < 5; ++row) {
        for (int i = 0; i < 256; ++i) {
            const auto at = static_cast<std::size_t>(i) * 3U;
            const int r = palette.rgb[at];
            const int g = palette.rgb[at + 1U];
            const int b = palette.rgb[at + 2U];

            const auto rr = static_cast<std::uint8_t>(
                r + div4_toward_zero((63 - r) * row + 2));
            const auto gg = static_cast<std::uint8_t>(
                g + div4_toward_zero((63 - g) * row + 2));
            const auto bb = static_cast<std::uint8_t>(
                b + div4_toward_zero((63 - b) * row + 2));

            out.bright[static_cast<std::size_t>(row)]
                      [static_cast<std::size_t>(i)] =
                tree.nearest(rr, gg, bb, 0x0F);
        }
    }

    // function_4B18 -> 5AF8..5FF7.
    for (int i = 0; i < 256; ++i) {
        const auto at = static_cast<std::size_t>(i) * 3U;
        const int r = palette.rgb[at];
        const int g = palette.rgb[at + 1U];
        const int b = palette.rgb[at + 2U];

        const int luminance =
            (r * 300) / 1000 +
            (g * 590) / 1000 +
            (b * 110) / 1000;
        out.luminance[0][static_cast<std::size_t>(i)] =
            luminance_bucket(palette, luminance);
    }

    for (int row = 1; row < 5; ++row) {
        for (int i = 0; i < 256; ++i) {
            const auto source =
                out.luminance[0][static_cast<std::size_t>(i)];
            const int red =
                palette.rgb[
                    static_cast<std::size_t>(source) * 3U];
            const int target =
                red +
                div4_toward_zero(
                    (63 - red) * row + 2);
            out.luminance[static_cast<std::size_t>(row)]
                         [static_cast<std::size_t>(i)] =
                luminance_bucket(palette, target);
        }
    }

    return out;
}

}  // namespace rloft
