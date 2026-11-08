#include "rloft/SAMPLE.hpp"

#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {

void require(bool ok, const char* reason) {
    if (!ok) throw std::runtime_error(reason);
}

template <class Func>
void rejects(Func&& fn) {
    bool rejected = false;
    try { fn(); } catch (const std::exception&) { rejected = true; }
    require(rejected, "malformed SAMPLE.AD was accepted");
}

void supplied_file(const std::vector<std::uint8_t>& bytes) {
    require(bytes.size() == 4662U, "unexpected original SAMPLE.AD size");
    const auto library = rloft::parse_sample_ad(bytes);
    require(library.timbres.size() == 233U, "wrong timbre count");
    for (std::size_t i = 0; i < library.timbres.size(); ++i) {
        const auto& t = library.timbres[i];
        const auto bank = i < 128U ? 0U : (i < 181U ? 127U : 1U);
        const auto patch = i < 128U ? i : (i < 181U ? i-128U+35U : i-181U);
        require(t.bank == bank && t.patch == patch, "bank/patch order differs");
        require(library.find(t.bank, t.patch) == &t, "lookup differs");
    }
    require(!library.find(2, 0), "unknown bank unexpectedly found");
    require(rloft::encode_sample_ad(library) == bytes,
            "source bytes were not reproduced exactly");
}

void malformed_and_edit_tests() {
    rloft::SampleAdLibrary library{};
    rloft::SampleAdTimbre t{};
    t.patch = 42; t.bank = 127; t.transpose = -12;
    t.mod_ws = 3; t.car_ws = 2; t.fb_c = 0x0E;
    library.timbres.push_back(t);
    auto bytes = rloft::encode_sample_ad(library);
    require(bytes.size() == 22U, "single-record geometry");
    auto rebuilt = rloft::parse_sample_ad(bytes);
    require(rebuilt.timbres[0].transpose == -12, "signed transpose");
    require(rebuilt.find(127, 42) != nullptr, "bank/patch lookup");
    require(rloft::encode_sample_ad(rebuilt) == bytes, "single-record roundtrip");
    library.timbres[0].bank = 0xFFU;
    library.timbres[0].patch = 0xFFU;
    rejects([&] { (void)rloft::encode_sample_ad(library); });
    library.timbres.clear();
    rejects([&] { (void)rloft::encode_sample_ad(library); });
    rejects([&] { (void)rloft::parse_sample_ad({}); });
    rejects([&] { (void)rloft::parse_sample_ad({0xFFU,0xFFU}); });
    auto corrupted = bytes; corrupted[2] = 0; // invalid absolute offset
    rejects([&] { (void)rloft::parse_sample_ad(corrupted); });
    corrupted = bytes; corrupted[8] = 0; // invalid record length
    rejects([&] { (void)rloft::parse_sample_ad(corrupted); });
    corrupted = bytes; corrupted.pop_back();
    rejects([&] { (void)rloft::parse_sample_ad(corrupted); });
    corrupted = bytes; corrupted.push_back(0);
    rejects([&] { (void)rloft::parse_sample_ad(corrupted); });
    corrupted = bytes; corrupted[6] = 0; // destroys directory terminator
    rejects([&] { (void)rloft::parse_sample_ad(corrupted); });
}
} // namespace

int main(int argc, char** argv) {
    malformed_and_edit_tests();
    if (argc > 2) throw std::runtime_error("usage: SAMPLE_tests [SAMPLE.AD]");
    if (argc == 2) {
        std::ifstream in(argv[1], std::ios::binary);
        if (!in) throw std::runtime_error("cannot open SAMPLE.AD");
        const std::vector<std::uint8_t> data(
            (std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        supplied_file(data);
    }
}
