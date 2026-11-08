#include "rloft/CHARS.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
void require(bool success, const char* why) {
    if (!success) throw std::runtime_error(why);
}

template <typename Func>
void must_reject(Func action) {
    bool caught = false;
    try { action(); }
    catch (const std::exception&) { caught = true; }
    require(caught, "expected CHARS operation to reject invalid input");
}

void test_image(const std::vector<std::uint8_t>& bytes) {
    using rloft::CharsCodec;
    require(bytes.size() == rloft::PartyImage::kSize, "incorrect fixture size");
    auto party = CharsCodec::decode(bytes);
    require(CharsCodec::encode(party) == bytes, "CHARS binary roundtrip changed");
    const auto first = CharsCodec::core(party, 0);
    const auto second = CharsCodec::core(party, 1);
    const auto third = CharsCodec::core(party, 2);
    const auto fourth = CharsCodec::core(party, 3);
    require(first.occupied && second.occupied, "two expected occupied slots");
    require(!third.occupied && !fourth.occupied, "two expected empty slots");
    require(!third.dead && !fourth.dead, "empty is not dead");
    require(first.status == 0 && second.status == 0, "occupied status bytes");
    require(third.status == 0xFFFF && fourth.status == 0xFFFF, "empty status bytes");
    require(first.age == 78 && second.age == 24, "age offsets");
    require(first.race == 3 && second.race == 2, "race offsets");
    require(first.class_codes[0] == 3 && second.class_codes[0] == 3,
            "class offsets");
    require(first.class_codes[1] == -1 && second.class_codes[1] == -1,
            "signed absent class sentinel");
    require(first.current_hp == 30 && first.max_hp == 30,
            "hit-point offsets");
    require(first.items[0] == 49 && third.items[0] == -1,
            "signed inventory sentinel");

    // Original bytes remain immutable; only requested field bytes may change.
    CharsCodec::set_status(party, 0, 0x4000U);
    CharsCodec::set_current_hp(party, 0, 0);
    CharsCodec::set_item(party, 0, 0, 123);
    require(CharsCodec::core(party, 0).dead, "death flag edit");
    require(CharsCodec::core(party, 0).current_hp == 0, "HP edit");
    require(CharsCodec::core(party, 0).items[0] == 123, "item edit");
    auto changed = CharsCodec::encode(party);
    std::size_t changed_bytes = 0;
    for (std::size_t i = 0; i < bytes.size(); ++i) {
        if (changed[i] != bytes[i]) {
            require(i == 0x11U || i == 0x1CU || i == 0x40U || i == 0x41U,
                    "unrelated CHARS bytes changed");
            ++changed_bytes;
        }
    }
    require(changed_bytes == 3, "unexpected edit byte count");
    require(std::equal(bytes.begin() + 0x248U, bytes.end(),
                       changed.begin() + 0x248U), "other actor records changed");

    must_reject([&] { (void)CharsCodec::core(party, 4); });
    must_reject([&] { CharsCodec::set_status(party, 4, 0); });
    must_reject([&] { CharsCodec::set_item(party, 0, 21, 0); });
    must_reject([&] { (void)CharsCodec::decode({}); });
    auto short_file = bytes;
    short_file.pop_back();
    must_reject([&] { (void)CharsCodec::decode(short_file); });
    auto long_file = bytes;
    long_file.push_back(0);
    must_reject([&] { (void)CharsCodec::decode(long_file); });
}
} // namespace

int main(int argc, char** argv) {
    // Source-only CI tests do not require distribution of original media.
    std::vector<std::uint8_t> synthetic(rloft::PartyImage::kSize, 0xFF);
    const auto put16 = [&](std::size_t at, unsigned value) {
        synthetic[at] = static_cast<std::uint8_t>(value);
        synthetic[at + 1U] = static_cast<std::uint8_t>(value >> 8U);
    };
    for (std::size_t actor = 0; actor < 2; ++actor) {
        const auto base = actor * rloft::PartyImage::kActorSize;
        std::fill_n(synthetic.begin() + static_cast<std::ptrdiff_t>(base), 16, 0);
        synthetic[base] = 'T';
        put16(base + 0x10U, 0);
        synthetic[base + 0x14U] = static_cast<std::uint8_t>(actor == 0 ? 3 : 2);
        synthetic[base + 0x15U] = 3;
        put16(base + 0x18U, actor == 0 ? 78 : 24);
        put16(base + 0x1CU, actor == 0 ? 30 : 35);
        put16(base + 0x1EU, actor == 0 ? 30 : 35);
        put16(base + 0x40U, 49);
    }
    test_image(synthetic);

    if (argc > 2) throw std::runtime_error("usage: CHARS_tests [CHARS]");
    if (argc == 2) {
        // Locally supplied original file; never checked into public Git.
        std::ifstream stream(argv[1], std::ios::binary);
        if (!stream) throw std::runtime_error("unable to open CHARS fixture");
        const std::vector<std::uint8_t> bytes(
            (std::istreambuf_iterator<char>(stream)),
            std::istreambuf_iterator<char>());
        test_image(bytes);
    }
}
