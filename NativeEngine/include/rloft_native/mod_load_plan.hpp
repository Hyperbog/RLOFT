#pragma once

// Pure planning contract: no asset override or gameplay scripting is enabled.
// It is intentionally independent from the renderer, platform and GameSource.
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace rloft_native {

inline std::string canonical_mod_id(std::string_view raw) {
    if (raw.empty() || raw.size() > 96) throw std::invalid_argument("invalid mod ID length");
    std::string out;
    out.reserve(raw.size());
    for (char ch : raw) {
        const auto c = static_cast<unsigned char>(ch);
        if ((c >= 'A' && c <= 'Z')) {
            out += static_cast<char>(c - 'A' + 'a');
        } else if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') ||
                   c == '_' || c == '-' || c == '.') {
            out += static_cast<char>(c);
        } else {
            throw std::invalid_argument("non-ASCII or unsafe mod ID");
        }
    }
    if (out == "original" || out == "." || out == "..")
        throw std::invalid_argument("reserved mod ID");
    return out;
}

struct ModManifest final {
    std::string id;
    std::string version;                  // nonempty human-readable data version
    std::uint32_t minimum_api{1};
    std::uint32_t maximum_api{1};
    std::vector<std::string> requires_mod; // IDs, not filesystem paths
};

struct PlannedMod final {
    std::string id;
    std::string version;
    std::size_t original_priority{};
};

// Topologically order dependents after dependencies. Among simultaneously
// ready mods preserve the owner's input priority deterministically.
// Since ReadOnlyVfs is last-mounted-wins, dependencies are lower priority.
inline std::vector<PlannedMod> plan_mod_mounts(
    const std::vector<ModManifest>& requested, std::uint32_t engine_api = 1) {
    if (requested.size() > 256U) throw std::invalid_argument("too many mods");
    std::map<std::string, std::size_t> names;
    for (std::size_t i = 0; i < requested.size(); ++i) {
        const auto& mod = requested[i];
        auto id = canonical_mod_id(mod.id);
        if (!names.emplace(id, i).second)
            throw std::invalid_argument("duplicate canonical mod ID");
        if (mod.version.empty() || mod.version.size() > 64U ||
            mod.minimum_api > engine_api || mod.maximum_api < engine_api ||
            mod.minimum_api > mod.maximum_api)
            throw std::invalid_argument("unsupported mod manifest version/API");
        for (const auto c : mod.version) {
            const auto ch = static_cast<unsigned char>(c);
            if (!((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z') ||
                  (ch >= 'A' && ch <= 'Z') || ch == '.' || ch == '-' || ch == '+'))
                throw std::invalid_argument("unsafe version identifier");
        }
        if (mod.requires_mod.size() > 128U)
            throw std::invalid_argument("too many dependencies");
    }
    std::vector<std::size_t> incoming(requested.size(), 0);
    std::vector<std::vector<std::size_t>> dependents(requested.size());
    for (std::size_t i = 0; i < requested.size(); ++i) {
        std::set<std::string> unique;
        for (const auto& dep : requested[i].requires_mod) {
            const auto id = canonical_mod_id(dep);
            if (!unique.insert(id).second)
                throw std::invalid_argument("duplicate mod dependency");
            const auto it = names.find(id);
            if (it == names.end())
                throw std::invalid_argument("missing required mod");
            if (it->second == i)
                throw std::invalid_argument("self dependency");
            ++incoming[i];
            dependents[it->second].push_back(i);
        }
    }
    std::set<std::size_t> ready;
    for (std::size_t i = 0; i < incoming.size(); ++i)
        if (incoming[i] == 0) ready.insert(i);
    std::vector<PlannedMod> plan;
    plan.reserve(requested.size());
    while (!ready.empty()) {
        const auto index = *ready.begin();
        ready.erase(ready.begin());
        plan.push_back({
            canonical_mod_id(requested[index].id),
            requested[index].version,
            index});
        for (const auto dependent : dependents[index])
            if (--incoming[dependent] == 0) ready.insert(dependent);
    }
    if (plan.size() != requested.size())
        throw std::invalid_argument("dependency cycle detected");
    return plan;
}

} // namespace rloft_native
