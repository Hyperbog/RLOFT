#include "rloft/CODE2.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
void require(bool condition, const char* reason) {
    if (!condition) throw std::runtime_error(reason);
}
template <class Action> void rejects(Action action) {
    bool rejected = false;
    try { action(); } catch (const std::exception&) { rejected = true; }
    require(rejected, "invalid CODE.2 data was accepted");
}
void original(const std::vector<std::uint8_t>& bytes) {
    require(bytes.size() == 558689U, "original CODE.2 file size differs");
    const rloft::Code2Executable code2(bytes);
    require(code2.bytes() == bytes, "original file altered by loader");
    const auto maps = code2.portrait_maps();
    require(maps == rloft::Code2Executable::portrait_maps_from_raw(bytes),
            "portrait data differs between validated and raw readers");
    for (std::size_t i = 0; i < 256U; ++i) {
        require(maps[2][i] == i, "identity portrait map differs");
        for (std::size_t state = 0; state < 6U; ++state) {
            require(maps[state][i] == code2.data_u8(
                        0xCF70U + state * 256U + i),
                    "DGROUP map lookup differs from direct table");
        }
    }
    require(maps[3][1] == 0x92U && maps[4][1] == 2U &&
            maps[5][0] == 0x60U, "original palette mapping differs");
    require(code2.data_u16(0xCF70U + 2U * 256U) == 0x0100U,
            "little-endian data-object read mismatch");
    rejects([&] { (void)code2.data_bytes(0x1273DU, 1U); });
    rejects([&] { (void)code2.data_u16(0x1273CU); });
    rejects([&] { (void)code2.data_bytes(0, bytes.size()); });
    auto mod = bytes;
    mod[0x75F24U + 0xCF70U] = 0x5AU;
    require(rloft::Code2Executable(mod).portrait_maps()[0][0] == 0x5AU,
            "modded portrait remap ignored");
    mod = bytes;
    mod[0x27BDCU] = 'Q';
    rejects([&] { (void)rloft::Code2Executable(mod); });
    mod = bytes;
    mod[0x27CA0U + 16U] = 1U;
    rejects([&] { (void)rloft::Code2Executable(mod); });
    mod = bytes;
    for (unsigned i = 0; i < 4U; ++i)
        mod[0x25D24U + 0x3CU + i] = 0xFFU;
    rejects([&] { (void)rloft::Code2Executable(mod); });
    mod.resize(150U);
    rejects([&] { (void)rloft::Code2Executable(mod); });
}
void synthetic() {
    constexpr std::size_t base = 0x75F24U + 0xCF70U;
    std::vector<std::uint8_t> bytes(base + 6U * 256U, 0U);
    for (std::size_t state = 0; state < 6U; ++state)
        for (std::size_t i = 0; i < 256U; ++i)
            bytes[base + state * 256U + i] =
                static_cast<std::uint8_t>(i + state);
    const auto maps = rloft::Code2Executable::portrait_maps_from_raw(bytes);
    require(maps[2][100] == 102U && maps[5][255] == 4U,
            "synthetic palette remap differs");
    rejects([&] { (void)rloft::Code2Executable(bytes); });
    bytes.pop_back();
    rejects([&] {
        (void)rloft::Code2Executable::portrait_maps_from_raw(bytes);
    });
}
}  // namespace

int main(int argc, char** argv) {
    synthetic();
    if (argc > 2) throw std::runtime_error("usage: CODE2_tests [CODE.2]");
    if (argc == 2) {
        std::ifstream stream(argv[1], std::ios::binary);
        if (!stream) throw std::runtime_error("cannot open original CODE.2");
        const std::vector<std::uint8_t> bytes(
            (std::istreambuf_iterator<char>(stream)),
            std::istreambuf_iterator<char>());
        original(bytes);
    }
}
