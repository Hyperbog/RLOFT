#include "rloft/GM2.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
void require(bool yes, const char* reason) {
    if (!yes) throw std::runtime_error(reason);
}

template <class Fn> void rejects(Fn&& action) {
    bool rejected = false;
    try { action(); } catch (const std::exception&) { rejected = true; }
    require(rejected, "GM2 invalid operation was accepted");
}

std::vector<std::uint8_t> synthetic_bank() {
    std::vector<std::uint8_t> bank(rloft::Gm2Bank::kRetailFileBytes, 0U);
    constexpr std::uint8_t signature[16]{
        0x02U,0xAFU,0x00U,0x32U,0x07U,0x02U,0x4DU,0x19U,
        0x71U,0x19U,0x95U,0x19U,0xC5U,0x19U,0xF5U,0x19U
    };
    std::copy(std::begin(signature), std::end(signature), bank.begin());
    bank[77U] = 0x81U;
    bank[780U] = 0x72U;
    bank.back() = 0x63U;
    return bank;
}

void synthetic() {
    const auto original = synthetic_bank();
    rloft::Gm2Bank bank(original);
    require(bank.size() == 24273U && bank.encode() == original,
            "GM2 synthetic bank changed on load");
    require(bank.read_range(77U, 1U)[0] == 0x81U,
            "GM2 checked range read failed");
    bank.apply_patches({{77U,{0x81U},{0x55U}},
                        {780U,{0x72U},{0x5AU}}});
    require(bank.bytes()[77U] == 0x55U && bank.bytes()[780U] == 0x5AU,
            "GM2 nonoverlapping edits failed");
    const auto committed = bank.encode();
    rejects([&] { bank.apply_patches({{77U,{0x81U},{0x34U}}}); });
    rejects([&] { bank.apply_patches({{15U,{0x19U},{0x02U}}}); });
    rejects([&] { bank.apply_patches({{77U,{0x55U},{0x01U}},
                                       {77U,{0x55U},{0x02U}}}); });
    rejects([&] { bank.apply_patches({{77U,{0x55U},{0x01U,0x02U}}}); });
    rejects([&] { bank.apply_patches({{24272U,{0x63U,0x01U},{0U,1U}}}); });
    rejects([&] { (void)bank.read_range(24273U, 1U); });
    rejects([&] { (void)bank.read_range(24274U, 0U); });
    rejects([&] { (void)rloft::Gm2Bank({}); });
    auto malformed = original;
    malformed[2U] ^= 1U;
    rejects([&] { (void)rloft::Gm2Bank(malformed); });
    malformed = original;
    malformed.resize(23907U);  // GM1-sized data must not pass as GM2.
    rejects([&] { (void)rloft::Gm2Bank(malformed); });
    require(bank.encode() == committed, "GM2 rejected edits changed source");
}

void original(const std::vector<std::uint8_t>& bytes) {
    require(bytes.size() == 24273U, "GM2 original byte count mismatch");
    rloft::Gm2Bank bank(bytes);
    require(bank.encode() == bytes, "GM2 original round trip changed bytes");
    const auto middle = bank.read_range(0x194DU, 16U);
    require(std::equal(middle.begin(), middle.end(), bytes.begin() + 0x194D),
            "GM2 original opaque range read differs");

    const auto before = bank.read_range(0x194DU, 1U);
    const auto last = bank.read_range(bytes.size() - 1U, 1U);
    bank.apply_patches({{0x194DU, before,
                         {static_cast<std::uint8_t>(before[0] ^ 0x5AU)}},
                        {bytes.size() - 1U, last,
                         {static_cast<std::uint8_t>(last[0] ^ 1U)}}});
    const auto edited = bank.encode();
    require(edited != bytes && edited.size() == bytes.size(),
            "GM2 patches did not persist");
    require(rloft::Gm2Bank(edited).encode() == edited,
            "GM2 modified bank did not reload");
    for (std::size_t i = 0U; i < bytes.size(); ++i) {
        if (i != 0x194DU && i != bytes.size() - 1U) {
            require(edited[i] == bytes[i],
                    "GM2 changed an unrelated vendor byte");
        }
    }
    bank.apply_patches({{0x194DU,
                         {static_cast<std::uint8_t>(before[0] ^ 0x5AU)}, before},
                        {bytes.size() - 1U,
                         {static_cast<std::uint8_t>(last[0] ^ 1U)}, last}});
    require(bank.encode() == bytes, "GM2 byte-exact restoration failed");
}
}  // namespace

int main(int argc, char** argv) {
    synthetic();
    if (argc > 2) throw std::invalid_argument("usage: GM2_tests [GM2.BNK]");
    if (argc == 2) {
        std::ifstream stream(argv[1], std::ios::binary);
        if (!stream) throw std::runtime_error("cannot open original GM2.BNK");
        const std::vector<std::uint8_t> data(
            (std::istreambuf_iterator<char>(stream)),
            std::istreambuf_iterator<char>());
        original(data);
    }
}
