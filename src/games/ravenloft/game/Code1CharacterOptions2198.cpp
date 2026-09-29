#include "games/ravenloft/game/Code1CharacterOptions2198.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

constexpr std::array<std::array<std::int8_t, 6>, 6> kTable06D8{{
    {{1, 1, 1, 1, 1, 1}},
    {{1, 0, 1, 1, 1, 1}},
    {{1, 0, 0, 1, 0, 1}},
    {{1, 0, 0, 1, 0, 1}},
    {{1, 0, 0, 1, 0, 1}},
    {{1, 0, 1, 1, 1, 1}},
}};

constexpr std::array<std::array<std::array<std::int8_t, 6>, 6>, 6> kTable06FC{{
    {{{{0,0,0,0,0,0}},{{0,0,0,0,0,0}},{{0,0,0,0,0,0}},{{0,0,0,0,0,0}},{{0,0,0,0,0,0}},{{0,0,0,0,0,0}}}},
    {{{{0,0,0,1,1,1}},{{0,0,0,0,0,0}},{{0,0,0,1,0,0}},{{1,0,1,0,1,0}},{{1,0,0,1,0,1}},{{1,0,0,0,1,0}}}},
    {{{{0,0,0,0,0,1}},{{0,0,0,0,0,0}},{{0,0,0,0,0,0}},{{0,0,0,0,0,0}},{{0,0,0,0,0,0}},{{1,0,0,0,0,0}}}},
    {{{{0,0,0,1,0,1}},{{0,0,0,0,0,0}},{{0,0,0,0,0,0}},{{1,0,0,0,0,1}},{{0,0,0,0,0,0}},{{1,0,0,1,0,0}}}},
    {{{{0,0,0,1,0,1}},{{0,0,0,0,0,0}},{{0,0,0,0,0,0}},{{1,0,0,0,0,0}},{{0,0,0,0,0,0}},{{1,0,0,0,0,0}}}},
    {{{{0,0,0,0,1,1}},{{0,0,0,0,0,0}},{{0,0,0,0,0,0}},{{0,0,0,0,0,0}},{{1,0,0,0,0,1}},{{1,0,0,0,1,0}}}},
}};

template <typename Row>
bool copyRow(std::array<std::int32_t, 6>& out, const Row& row) noexcept {
    bool any = false;
    for (std::size_t i = 0; i < 6; ++i) {
        out[i] = static_cast<std::int32_t>(row[i]);
        any = any || out[i] != 0;
    }
    return any;
}

bool anySet(const std::array<std::int32_t, 6>& out) noexcept {
    for (const auto value : out) if (value != 0) return true;
    return false;
}

} // namespace

bool Code1CharacterOptions2198::build(
    std::array<std::int32_t, 6>& out,
    std::int32_t mode,
    const Code1CharacterOptionState2198& state) noexcept {
    if (mode == 0) {
        if (state.field680C >= kTable06D8.size()) return false;
        return copyRow(out, kTable06D8[state.field680C]);
    }

    if (mode == 1) {
        const auto d = static_cast<std::int8_t>(state.field680D);
        if (state.field680C >= kTable06FC.size() || d < 0 || d >= 6) return false;
        return copyRow(out, kTable06FC[state.field680C][static_cast<std::size_t>(d)]);
    }

    if (mode != 2) {
        return false;
    }

    out.fill(0);

    const auto d = state.field680D;
    const auto e = state.field680E;

    if (state.field680C == 5u) {
        if (d == 0u) {
            if (e == 4u) out[5] = 1;
            if (e == 5u) out[4] = 1;
        }
        if (d == 4u) {
            if (e == 0u) out[5] = 1;
            if (e == 5u) out[0] = 1;
        }
        if (d == 5u && e == 4u) {
            out[0] = 1;
        }
        return anySet(out);
    }

    if (state.field680C == 1u) {
        if (d == 0u) {
            if (e == 4u) { out[5] = 1; out[3] = 1; }
            if (e == 5u) out[4] = 1;
            if (e == 3u) out[4] = 1;
        }
        if (d == 4u) {
            if (e == 0u) { out[5] = 1; out[3] = 1; }
            if (e == 5u) out[0] = 1;
            if (e == 3u) out[0] = 1;
        }
        if (d == 5u) {
            if (e == 4u) out[0] = 1;
            if (e == 0u) { out[4] = 1; out[3] = 1; }
        }
        if (d == 3u) {
            if (e == 4u) out[0] = 1;
            if (e == 0u) out[4] = 1;
        }
    }

    return anySet(out);
}

} // namespace ravenloft::game
