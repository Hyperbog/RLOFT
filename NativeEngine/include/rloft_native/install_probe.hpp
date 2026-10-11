#pragma once

// Read-only format-presence diagnostic. Does not execute, decode or validate
// the original copyrighted game, nor assume all optional files exist.
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace rloft_native {

struct InstallFileStatus final {
    std::string filename;
    bool required_for_probe{};
    bool present{};
    std::size_t byte_count{}; // only known for the three read-checked files
    bool size_known{};
};

struct InstallProbeResult final {
    bool minimum_media_present{};
    std::vector<InstallFileStatus> files;
};

// These three are used by the current authoritative GameSource loose-media
// signature; they are NOT a sufficient full-original-game start contract.
// Other entries are candidate-source inventory: presence does not certify
// game version, resource integrity or code compatibility.
inline InstallProbeResult inspect_original_install(
    const std::function<bool(std::string_view)>& exists,
    const std::function<std::vector<std::uint8_t>(std::string_view)>& read) {
    if (!exists || !read) throw std::invalid_argument("media reader required");
    struct Entry { const char* name; bool minimum; };
    constexpr std::array<Entry, 18> requested{{
        {"INIT", true}, {"RES0", true}, {"T", true},
        {"RES1", false}, {"RES2", false}, {"RES3", false},
        {"RES4", false}, {"RES5", false}, {"RES6", false},
        {"RES7", false}, {"RES8", false}, {"RES10", false},
        {"COLORS", false}, {"LEVELS.DAT", false},
        {"OBJECTS.DAT", false}, {"NPCS.DAT", false},
        {"SINTAB", false}, {"CINE00.ANM", false}
    }};
    InstallProbeResult output{true, {}};
    output.files.reserve(requested.size());
    for (const auto& source : requested) {
        InstallFileStatus record{source.name, source.minimum, false, 0, false};
        record.present = exists(source.name);
        if (source.minimum) {
            if (!record.present) {
                output.minimum_media_present = false;
            } else {
                const auto bytes = read(source.name);
                record.byte_count = bytes.size();
                record.size_known = true;
                if (bytes.empty()) output.minimum_media_present = false;
            }
        }
        output.files.push_back(std::move(record));
    }
    return output;
}

} // namespace rloft_native
