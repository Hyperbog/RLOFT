#include "rloft/T.hpp"
#include "rloft/original_t_text_archive.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
void check(bool good, const char* why) {
    if (!good) throw std::runtime_error(why);
}
template <class Action> void rejects(Action action) {
    bool rejected = false;
    try { action(); } catch (const std::exception&) { rejected = true; }
    check(rejected, "invalid T archive operation accepted");
}
void put32(std::vector<std::uint8_t>& b, std::size_t at, unsigned x) {
    for (unsigned j = 0; j < 4U; ++j)
        b[at + j] = static_cast<std::uint8_t>(x >> (j * 8U));
}
std::vector<std::uint8_t> basic_fixture() {
    // T(count=3) plus 4 bytes pre-payload slack, a skipped 3-byte gap,
    // and one empty record. All non-record bytes must survive editing.
    std::vector<std::uint8_t> b(20U, 0U);
    put32(b, 0U, 3U);
    b[16] = 0x4FU; b[17] = 0x61U; b[18] = 0x00U; b[19] = 0xAAU;
    put32(b, 4U, static_cast<unsigned>(b.size()));
    b.push_back(3); b.push_back(0);
    for (const char c : std::string("CAT"))
        b.push_back(static_cast<std::uint8_t>(c) ^ 0xDCU);
    b.insert(b.end(), {0x39U, 0x81U, 0x00U});
    put32(b, 8U, static_cast<unsigned>(b.size()));
    b.push_back(0); b.push_back(0);
    put32(b, 12U, static_cast<unsigned>(b.size()));
    b.push_back(3); b.push_back(0);
    for (const char c : std::string("DOG"))
        b.push_back(static_cast<std::uint8_t>(c) ^ 0xDCU);
    return b;
}
void synthetic_tests() {
    auto b = basic_fixture();
    rloft::TArchive t(b);
    check(t.count() == 3U && t.decoded(0) == "CAT" &&
          t.decoded(1).empty() && t.decoded(2) == "DOG", "decode fixture");
    check(t.encode() == b, "untouched T lost original slack");
    t.validate_strict();
    check(rloft::OriginalTTextArchive(b).decoded(2) == "DOG",
          "CODE.2 compatibility reader changed");
    t.replace(0U, "FELINE");
    t.replace(1U, "MEOW");
    const auto edited = t.encode();
    rloft::TArchive fresh(edited);
    fresh.validate_strict();
    check(fresh.decoded(0) == "FELINE" && fresh.decoded(1) == "MEOW" &&
          fresh.decoded(2) == "DOG", "variable-length text edits");
    check(std::equal(b.begin() + 16, b.begin() + 20, edited.begin() + 16),
          "pre-record slack corrupted");
    check(std::equal(b.begin() + 25, b.begin() + 28, edited.begin() + 28),
          "inter-record slack corrupted");
    check(fresh.encode() == edited, "edited text does not re-encode");
    rejects([&] { t.replace(3U, "BAD"); });
    rejects([&] { t.replace(0U, std::string(65536U, 'X')); });
    rejects([&] { t.replace(0U, std::string("A\0B", 3U)); });
    rejects([&] { (void)t.decoded(3U); });
    auto bad = b;
    bad[4U] = 0xFFU; bad[5U] = 0xFFU;
    rejects([&] { rloft::TArchive(bad).validate_strict(); });
    bad = b; put32(bad, 8U, 20U); // shared/overlapping offsets
    rejects([&] { rloft::TArchive(bad).replace(0U, "no"); });
    rejects([] { (void)rloft::TArchive({}); });

    // Several existing source-only F790 test fixtures deliberately share
    // empty-record offsets. Preserve their old reader behavior, but refuse
    // lossless variable-length editing of ambiguous shared physical bytes.
    std::vector<std::uint8_t> shared(18U, 0U);
    put32(shared, 0U, 2U); put32(shared, 4U, 12U); put32(shared, 8U, 12U);
    rloft::OriginalTTextArchive legacy(shared);
    check(legacy.decoded(0).empty() && legacy.decoded(1).empty(),
          "source-only F790 alias compatibility");
    rejects([&] { legacy.replace(0U, "NEW"); });
}
void original_tests(const std::vector<std::uint8_t>& bytes) {
    rloft::TArchive t(bytes);
    t.validate_strict();
    check(t.count() == 1073U && bytes.size() == 133584U,
          "T source geometry changed");
    check(t.decoded(0U) == "FORESEE THIS CHARACTER", "first original text");
    check(t.encode() == bytes, "original T byte-exact round trip");
    std::size_t zero_count = 0U;
    for (std::uint32_t i = 0; i < t.count(); ++i)
        if (t.decoded(i).empty()) ++zero_count;
    check(zero_count == 25U, "T zero-length record count differs");
    t.replace(0U, "MODDED CHARACTER MESSAGE");
    const auto edited = t.encode();
    rloft::TArchive check_archive(edited);
    check_archive.validate_strict();
    check(check_archive.decoded(0U) == "MODDED CHARACTER MESSAGE",
          "original text modification failed");
    for (std::uint32_t i = 1; i < t.count(); ++i)
        check(check_archive.decoded(i) == t.decoded(i),
              "other indexed text changed");
    check(edited.size() == bytes.size() + 2U,
          "edited T record length didn't update correctly");
    check(std::equal(bytes.begin() + 4296, bytes.begin() + 4572,
                     edited.begin() + 4296), "original 276-byte slack changed");
}
} // namespace

int main(int argc, char** argv) {
    synthetic_tests();
    if (argc > 2) throw std::runtime_error("usage: T_tests [T]");
    if (argc == 2) {
        std::ifstream in(argv[1], std::ios::binary);
        if (!in) throw std::runtime_error("unable to read original T");
        const std::vector<std::uint8_t> bytes(
            (std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        original_tests(bytes);
    }
}
