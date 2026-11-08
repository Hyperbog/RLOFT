#include "rloft/CODE1.hpp"
#include "rloft/character_creation_flow.hpp"

#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
void require(bool check, const char* reason) {
    if (!check) throw std::runtime_error(reason);
}
template<class F> void rejects(F f) {
    bool failed = false;
    try { f(); } catch (const std::exception&) { failed = true; }
    require(failed, "invalid CODE.1 was accepted");
}
void original_file_tests(const std::vector<std::uint8_t>& bytes) {
    auto rules = rloft::Code1ClassRules::from_code1(bytes);
    const auto vanilla = rloft::Code1ClassRules::vanilla();
    require(rules.first == vanilla.first && rules.second == vanilla.second,
            "original CODE.1 class tables differ from builtin defaults");
    require(!rules.allows_first(-1, 0) && !rules.allows_second(6, 0, 0),
            "out-of-range class indices");
    rloft::CharacterCreationFlow flow(rules);
    require(flow.apply_event(3) && flow.apply_event(3) && flow.apply_event(2),
            "cannot enter class choice");
    require(flow.available_choices()[0] && !flow.available_choices()[1],
            "first-class rules not used by gameplay");
    require(flow.apply_event(1), "cannot select first class");
    require(!flow.available_choices()[0] && flow.available_choices()[3],
            "second-class rules not used by gameplay");

    // Mod a single original table byte, then prove a different gameplay
    // outcome without changing code or touching the rest of the executable.
    auto modified = bytes;
    modified[0x3E724U + 0x06D8U + 1U * 6U + 1U] = 1U;
    auto changed = rloft::Code1ClassRules::from_code1(modified);
    require(changed.allows_first(1, 1), "modified class eligibility lost");
    rloft::CharacterCreationFlow custom(changed);
    require(custom.apply_event(3) && custom.apply_event(3) &&
            custom.apply_event(2) && custom.available_choices()[1],
            "native flow ignored CODE.1 data change");
    auto corrupted = bytes;
    corrupted[0x27BDCU] = 'Q';
    rejects([&] { (void)rloft::Code1ClassRules::from_code1(corrupted); });
    corrupted = bytes;
    corrupted[0x3E724U + 0x06D8U] = 2;
    rejects([&] { (void)rloft::Code1ClassRules::from_code1(corrupted); });
    corrupted = bytes;
    for (unsigned i = 0; i < 4U; ++i)
        corrupted[0x25D24U + 0x3CU + i] = 0xFFU;
    rejects([&] { (void)rloft::Code1ClassRules::from_code1(corrupted); });
    corrupted = bytes; corrupted.resize(17);
    rejects([&] { (void)rloft::Code1ClassRules::from_code1(corrupted); });
}
}
int main(int argc, char** argv) {
    const auto vanilla = rloft::Code1ClassRules::vanilla();
    require(vanilla.allows_first(1, 0) && !vanilla.allows_first(1, 1) &&
            vanilla.allows_second(1, 0, 3), "default CODE.1 class rules");
    rloft::CharacterCreationFlow baseline(vanilla);
    require(baseline.apply_event(3) && baseline.apply_event(3) &&
            baseline.apply_event(2) && !baseline.available_choices()[1],
            "default rules not installed into native character flow");
    if (argc > 2) throw std::runtime_error("usage: CODE1_tests [CODE.1]");
    if (argc == 1) return 0;
    std::ifstream file(argv[1], std::ios::binary);
    if (!file) throw std::runtime_error("cannot open CODE.1");
    const std::vector<std::uint8_t> data(
        (std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    original_file_tests(data);
}
