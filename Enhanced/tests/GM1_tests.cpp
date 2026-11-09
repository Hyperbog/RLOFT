#include "rloft/GM1.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
void require(bool ok, const char* reason) {
    if (!ok) throw std::runtime_error(reason);
}
template <class Func> void rejects(Func fn) {
    bool failed = false;
    try { fn(); } catch (const std::exception&) { failed = true; }
    require(failed, "GM1 invalid input was accepted");
}
std::vector<std::uint8_t> minimal_bank() {
    return {0x02U,0xAFU,0x00U,0x32U,0x07U,0x02U,0x4DU,0x19U,
            0x71U,0x19U,0x95U,0x19U,0xC5U,0x19U,0xF5U,0x19U,
            0x81U,0x72U,0x63U,0x54U,0x45U};
}
void synthetic() {
    const auto bytes = minimal_bank();
    rloft::Gm1Bank bank(bytes);
    require(bank.size() == bytes.size() && bank.encode() == bytes,
            "GM1 synthetic round trip changed bank bytes");
    require(bank.read_range(16U, 3U) == std::vector<std::uint8_t>({0x81U,0x72U,0x63U}),
            "GM1 range read mismatch");
    bank.apply_patches({{16U,{0x81U,0x72U},{0x11U,0x22U}},
                        {20U,{0x45U},{0x55U}}});
    require(bank.read_range(16U, 2U) == std::vector<std::uint8_t>({0x11U,0x22U}) &&
            bank.read_range(20U, 1U)[0] == 0x55U,
            "GM1 transactional patches did not apply");
    const auto committed = bank.encode();
    rejects([&] { bank.apply_patches({{16U,{0x81U},{0x33U}}}); });
    rejects([&] { bank.apply_patches({{15U,{0x19U},{0x00U}}}); });
    rejects([&] { bank.apply_patches({{16U,{0x11U},{0x00U}},
                                       {16U,{0x11U},{0x01U}}}); });
    rejects([&] { bank.apply_patches({{16U,{0x11U},{0x00U,0x01U}}}); });
    rejects([&] { bank.apply_patches({{20U,{0x55U,0x00U},{0x00U,0x01U}}}); });
    rejects([&] { (void)bank.read_range(22U, 0U); });
    rejects([&] { (void)bank.read_range(20U, 2U); });
    rejects([&] { (void)rloft::Gm1Bank({}); });
    auto invalid = bytes; invalid[3U] ^= 1U;
    rejects([&] { (void)rloft::Gm1Bank(invalid); });
    require(bank.encode() == committed, "rejected patch changed GM1 data");
}
void original(const std::vector<std::uint8_t>& bytes) {
    require(bytes.size() == 23907U, "GM1 original byte count mismatch");
    rloft::Gm1Bank bank(bytes);
    require(bank.encode() == bytes, "GM1 original round trip is not lossless");
    require(bank.read_range(0x194DU, 16U) ==
            std::vector<std::uint8_t>(bytes.begin() + 0x194D,
                                      bytes.begin() + 0x194D + 16),
            "GM1 original vendor body range mismatch");
    const auto a = bank.read_range(0x194DU, 1U);
    const auto z = bank.read_range(bytes.size() - 1U, 1U);
    bank.apply_patches({{0x194DU, a, {static_cast<std::uint8_t>(a[0] ^ 0x5AU)}},
                        {bytes.size() - 1U, z,
                         {static_cast<std::uint8_t>(z[0] ^ 0x01U)}}});
    const auto edited = bank.encode();
    require(edited.size() == bytes.size() && edited != bytes,
            "GM1 byte patch did not serialize");
    rloft::Gm1Bank reopened(edited);
    require(reopened.encode() == edited,
            "GM1 edited bank failed to reload");
    for (std::size_t i = 0U; i < bytes.size(); ++i) {
        if (i != 0x194DU && i != bytes.size() - 1U) {
            require(bytes[i] == edited[i], "GM1 modified non-target vendor bytes");
        }
    }
    bank.apply_patches({{0x194DU,
                         {static_cast<std::uint8_t>(a[0] ^ 0x5AU)}, a},
                        {bytes.size() - 1U,
                         {static_cast<std::uint8_t>(z[0] ^ 0x01U)}, z}});
    require(bank.encode() == bytes, "GM1 original data not restored byte-exactly");
}
}  // namespace
int main(int argc, char** argv) {
    synthetic();
    if (argc > 2) throw std::invalid_argument("usage: GM1_tests [GM1.BNK]");
    if (argc == 2) {
        std::ifstream input(argv[1], std::ios::binary);
        if (!input) throw std::runtime_error("cannot read GM1.BNK");
        const std::vector<std::uint8_t> data(
            (std::istreambuf_iterator<char>(input)),
            std::istreambuf_iterator<char>());
        original(data);
    }
}
