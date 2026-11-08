#include "rloft/INIT.hpp"

#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {

void require(bool value, const char* message) {
    if (!value) { throw std::runtime_error(message); }
}

template <typename F>
void expect_rejected(F&& action) {
    bool rejected = false;
    try { action(); }
    catch (const std::exception&) { rejected = true; }
    require(rejected, "expected validation exception");
}

void verify_original(const std::vector<std::uint8_t>& bytes) {
    const auto init = rloft::parse_init(bytes);
    constexpr std::uint16_t expected[18] = {
        15,168,336,0,2169,3316,4158,600,49,49,69,166,
        192,266,32,103,48,414
    };
    require(bytes.size() == 1879, "INIT codec contract failed");
    for (std::size_t i = 0; i < 18; ++i) {
        require(init.header_words[i] == expected[i], "INIT codec contract failed");
    }
    require(init.class1_descriptors.size() == 175, "INIT codec contract failed");
    require(init.class3_descriptors.size() == 12, "INIT codec contract failed");
    require(init.environment_profiles.size() == 2, "INIT codec contract failed");
    require(init.environment_profiles[0].res8_base == 8, "INIT codec contract failed");
    require(init.environment_profiles[0].stored_record_count == 38, "INIT codec contract failed");
    require(init.environment_profiles[0].placements.size() == 38, "INIT codec contract failed");
    require(init.environment_profiles[1].res8_base == 45, "INIT codec contract failed");
    require(init.environment_profiles[1].stored_record_count == 27, "INIT codec contract failed");
    require(init.environment_profiles[1].placements.size() == 27, "INIT codec contract failed");
    for (const auto& p : init.environment_profiles) {
        require(p.placements.back().terminator(), "INIT codec contract failed");
    }
    require(rloft::encode_init(init) == bytes, "INIT codec contract failed");
    const auto env = rloft::decode_init_environment_profiles_at(bytes, 0x545);
    require(env.size() == 2, "INIT codec contract failed");
    require(env[0].placements.size() == 38, "INIT codec contract failed");
}

void verify_variable_length() {
    rloft::InitFile init{};
    init.header_words[0] = 123;
    init.class1_descriptors.push_back({0xFFFF, 0x8000, 255, 0, 78});
    init.class3_descriptors.push_back({77, 12, 7, 2, 8});
    rloft::EnvironmentProfile p{};
    p.res8_base = 11;
    p.stored_record_count = 2;
    p.placements.push_back({-1, -35, 127, -12});
    p.placements.push_back({});
    init.environment_profiles.push_back(p);
    const auto bytes = rloft::encode_init(init);
    const auto recovered = rloft::parse_init(bytes);
    require(recovered.header_words[0] == 123, "INIT codec contract failed");
    require(recovered.class1_descriptors.size() == 1, "INIT codec contract failed");
    require(recovered.class1_descriptors[0].spatial_extent == 0x8000, "INIT codec contract failed");
    require(recovered.class3_descriptors.size() == 1, "INIT codec contract failed");
    require(recovered.environment_profiles[0].placements[0].azimuth_degrees == -35, "INIT codec contract failed");
    require(rloft::encode_init(recovered) == bytes, "INIT codec contract failed");
    auto truncated = bytes;
    truncated.pop_back();
    expect_rejected([&] { (void)rloft::parse_init(truncated); });
    auto extra = bytes;
    extra.push_back(42);
    expect_rejected([&] { (void)rloft::parse_init(extra); });
    auto invalid = init;
    invalid.environment_profiles[0].stored_record_count = 1;
    expect_rejected([&] { (void)rloft::encode_init(invalid); });
    invalid = init;
    invalid.environment_profiles[0].placements.back().screen_y = 4;
    expect_rejected([&] { (void)rloft::encode_init(invalid); });
    invalid = init;
    invalid.class3_descriptors.resize(65536);
    expect_rejected([&] { (void)rloft::encode_init(invalid); });
    expect_rejected([&] { (void)rloft::parse_init({}); });
}
} // namespace

int main(int argc, char** argv) {
    verify_variable_length();
    if (argc > 1) {
        std::ifstream input(argv[1], std::ios::binary);
        if (!input) { throw std::runtime_error("INIT fixture unavailable"); }
        const std::vector<std::uint8_t> bytes(
            (std::istreambuf_iterator<char>(input)),
            std::istreambuf_iterator<char>());
        verify_original(bytes);
    }
}
