#include "rloft/INFO.hpp"

#include <array>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
using rloft::InfoDat;

void require(bool value, const char* message) {
    if (!value) { throw std::runtime_error(message); }
}

void reject(const std::vector<std::uint8_t>& bytes) {
    bool rejected = false;
    try { (void)rloft::parse_info_dat(bytes); }
    catch (const std::invalid_argument&) { rejected = true; }
    require(rejected, "malformed INFO.DAT accepted");
}

void roundtrip(const std::vector<std::uint8_t>& bytes, const std::string& path) {
    const auto decoded = rloft::parse_info_dat(bytes);
    require(decoded.resource_path == path, "path differs from expected bytes");
    require(decoded.resource_locations ==
            (std::array<std::uint8_t, 13>{0,0,0,0,0,0,0,0,0,1,1,1,1}),
            "source selectors differ");
    require(rloft::encode_info_dat(decoded) == bytes, "INFO.DAT round-trip differs");
    require(rloft::info_legacy_res_path(decoded, 0) == "RES0", "RES0 route");
    require(rloft::info_legacy_res_path(decoded, 8) == "RES8", "RES8 route");
    require(rloft::info_legacy_res_path(decoded, 9) == path + "\\RES9", "RES9 route");
    require(rloft::info_legacy_res_path(decoded, 10) == path + "\\RES10", "RES10 route");
    require(rloft::info_legacy_numbered_path(decoded, 7) == path + "\\07",
            "event 7 route");
    require(rloft::info_legacy_numbered_path(decoded, 99) == path + "\\99",
            "event 99 route");
    bool rejected = false;
    try { (void)rloft::info_legacy_res_path(decoded, 11); }
    catch (const std::out_of_range&) { rejected = true; }
    require(rejected, "inactive RES11 accepted");
}

void tests() {
    const std::vector<std::uint8_t> installed = {
        '0','0','0','0','0','0','0','0','0','1','1','1','1',
        8,0,'D',':','\\','R','L','O','F','T'
    };
    roundtrip(installed, "D:\\RLOFT");
    const std::vector<std::uint8_t> archival = {
        '0','0','0','0','0','0','0','0','0','1','1','1','1',
        2,0,'D',':'
    };
    roundtrip(archival, "D:");

    // Non-boolean decimal selectors and arbitrary trailing bytes survive.
    InfoDat modified = rloft::parse_info_dat(installed);
    modified.resource_locations[3] = 9;
    modified.resource_locations[12] = 7;
    modified.resource_path = std::string("C:\\X\0Y", 6);
    const auto encoded = rloft::encode_info_dat(modified);
    const auto decoded = rloft::parse_info_dat(encoded);
    require(decoded.resource_locations == modified.resource_locations,
            "non-boolean selectors lost");
    require(decoded.resource_path == modified.resource_path, "non-text path bytes lost");
    require(rloft::info_legacy_res_path(decoded, 3) ==
            std::string("C:\\X\0Y", 6) + "\\RES3", "nonzero selector route");

    auto invalid = installed;
    invalid[0] = 'A'; reject(invalid);
    invalid = installed; invalid[13] = 9; reject(invalid);
    invalid = installed; invalid[14] = 0x80; reject(invalid);
    invalid = installed; invalid.push_back(0); reject(invalid);
    reject({}); reject(std::vector<std::uint8_t>(14, 0));
    modified.resource_locations[0] = 10;
    bool rejected = false;
    try { (void)rloft::encode_info_dat(modified); }
    catch (const std::invalid_argument&) { rejected = true; }
    require(rejected, "invalid selector encoded");
    modified.resource_locations[0] = 0;
    modified.resource_path.resize(0x8000);
    rejected = false;
    try { (void)rloft::encode_info_dat(modified); }
    catch (const std::invalid_argument&) { rejected = true; }
    require(rejected, "oversized path encoded");
}
} // namespace

int main(int argc, char** argv) {
    tests();
    if (argc == 2) {
        std::ifstream file(argv[1], std::ios::binary);
        if (!file) { throw std::runtime_error("INFO.DAT fixture not found"); }
        const std::vector<std::uint8_t> raw(
            std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{});
        roundtrip(raw, "D:\\RLOFT");
    }
    return 0;
}
