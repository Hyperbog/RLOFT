#include "rloft_native/install_probe.hpp"
#include <cassert>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

using namespace rloft_native;
namespace {
void probe_tests() {
    std::map<std::string, std::vector<std::uint8_t>> files{
        {"INIT", {1, 2, 3}}, {"RES0", {4}}, {"T", {5, 6}},
        {"RES4", {7, 8, 9}}};
    unsigned read_calls = 0;
    auto exists = [&](std::string_view path) {
        return files.find(std::string(path)) != files.end();
    };
    auto read = [&](std::string_view path) {
        ++read_calls;
        return files.at(std::string(path));
    };
    const auto result = inspect_original_install(exists, read);
    assert(result.minimum_media_present);
    assert(result.files.size() == 18);
    assert(read_calls == 3); // optional originals never eagerly loaded
    assert(result.files[0].filename == "INIT");
    assert(result.files[0].size_known && result.files[0].byte_count == 3);
    assert(result.files[6].filename == "RES4" && result.files[6].present);
    assert(!result.files[6].size_known);
    assert(result.files[17].filename == "CINE00.ANM");
    files.erase("T");
    const auto missing = inspect_original_install(exists, read);
    assert(!missing.minimum_media_present);
    assert(!missing.files[2].present);
    files["T"] = {};
    const auto zero = inspect_original_install(exists, read);
    assert(!zero.minimum_media_present);
    files["T"] = {5};
    assert(inspect_original_install(exists, read).minimum_media_present);
    bool rejected = false;
    try {
        inspect_original_install({}, read);
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    assert(rejected);
}
}
int main() { probe_tests(); }
