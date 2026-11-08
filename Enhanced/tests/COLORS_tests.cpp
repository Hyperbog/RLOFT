#include "rloft/COLORS.hpp"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {

void require(bool valid, const char* message) {
    if (!valid) throw std::runtime_error(message);
}

template <class Action>
void rejects(Action action) {
    bool failed = false;
    try { action(); } catch (const std::exception&) { failed = true; }
    require(failed, "malformed COLORS data was accepted");
}

void synthetic_tests() {
    rloft::ColorsFile file{};
    file.palettes.resize(2);
    auto& first = file.palettes[0].rgb;
    auto& second = file.palettes[1].rgb;
    first[0] = 10; first[1] = 20; first[2] = 30;
    second[0] = 63; second[1] = 31; second[2] = 0;
    auto bytes = rloft::encode_colors(file);
    require(bytes.size() == 1U + 2U * 768U && bytes[0] == 2U,
            "count and palette layout incorrect");
    const auto decoded = rloft::parse_colors(bytes);
    require(decoded.palettes.size() == 2U &&
            decoded.palettes[0].rgb == first &&
            decoded.palettes[1].rgb == second,
            "RGB channels or bank order changed");
    require(rloft::encode_colors(decoded) == bytes,
            "synthetic COLORS binary roundtrip differs");

    const auto p0 = rloft::brighten_colors(decoded.palettes[0], 0);
    require(p0.rgb == first, "zero intensity changed source palette");
    const auto p63 = rloft::brighten_colors(decoded.palettes[0], 63);
    require(p63.rgb[0] == 21U && p63.rgb[1] == 42U &&
            p63.rgb[2] == 63U, "brightness math differs from CODE.2");
    require(p63.rgb[3] == 0 && p63.rgb[4] == 0 && p63.rgb[5] == 0,
            "black must remain black");
    require(rloft::brighten_colors(decoded.palettes[0], -100).rgb == first,
            "brightness did not clamp below zero");
    require(rloft::brighten_colors(decoded.palettes[0], 999).rgb == p63.rgb,
            "brightness did not clamp above 63");
    require(rloft::encode_colors(decoded) == bytes,
            "runtime brightness mutated serialized palettes");

    auto truncated = bytes; truncated.pop_back();
    rejects([&] { (void)rloft::parse_colors(truncated); });
    auto extended = bytes; extended.push_back(0);
    rejects([&] { (void)rloft::parse_colors(extended); });
    auto bad_count = bytes; bad_count[0] = 3;
    rejects([&] { (void)rloft::parse_colors(bad_count); });
    auto high = bytes; high[1] = 64;
    rejects([&] { (void)rloft::parse_colors(high); });
    file.palettes[0].rgb[0] = 64;
    rejects([&] { (void)rloft::encode_colors(file); });
    file.palettes.clear();
    rejects([&] { (void)rloft::encode_colors(file); });
    file.palettes.resize(256);
    rejects([&] { (void)rloft::encode_colors(file); });
    rejects([] { (void)rloft::parse_colors({}); });
    rejects([] { (void)rloft::parse_colors({0}); });
}

void original_tests(const std::vector<std::uint8_t>& bytes) {
    const auto result = rloft::parse_colors(bytes);
    require(bytes.size() == 11521U && bytes[0] == 15U,
            "uploaded original COLORS geometry changed");
    require(result.palettes.size() == 15U, "expected 15 original palettes");
    require(result.palettes[0].rgb[0] == 0U &&
            result.palettes[0].rgb[3] == 1U,
            "first palette source sample differs");
    require(rloft::encode_colors(result) == bytes,
            "original COLORS was not reproduced byte-for-byte");
    for (const auto& p : result.palettes) {
        require(*std::max_element(p.rgb.begin(), p.rgb.end()) <= 63U,
                "invalid VGA channel in original file");
        const auto boosted = rloft::brighten_colors(p, 14);
        require(*std::max_element(boosted.rgb.begin(), boosted.rgb.end()) <= 63U,
                "brightness overflowed VGA range");
    }
}

} // namespace

int main(int argc, char** argv) {
    synthetic_tests();
    if (argc > 2) throw std::runtime_error("usage: COLORS_tests [COLORS]");
    if (argc == 2) {
        std::ifstream f(argv[1], std::ios::binary);
        if (!f) throw std::runtime_error("cannot read original COLORS");
        const std::vector<std::uint8_t> bytes(
            (std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
        original_tests(bytes);
    }
}
