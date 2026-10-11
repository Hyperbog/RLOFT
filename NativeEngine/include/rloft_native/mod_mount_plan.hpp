#pragma once

// Transactional diagnostic application of a validated mod dependency plan.
// This deliberately does not enable modded original-game execution.
#include "rloft_native/mod_load_plan.hpp"
#include "rloft_native/vfs_contract.hpp"
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

namespace rloft_native {

struct ModDirectoryRequest final {
    ModManifest manifest;
    std::filesystem::path directory;
};

// All-or-nothing replacement of the VFS *mount table*. The filesystem can
// change later, and this operation is not an OS-security sandbox.
inline std::vector<PlannedMod> apply_mod_directory_plan(
    ReadOnlyVfs& vfs, const std::vector<ModDirectoryRequest>& requested,
    std::uint32_t engine_api = 1) {
    std::vector<ModManifest> manifests;
    manifests.reserve(requested.size());
    for (const auto& item : requested) manifests.push_back(item.manifest);
    const auto plan = plan_mod_mounts(manifests, engine_api);
    auto candidate = vfs; // copy only overlay metadata/base callbacks
    for (const auto& item : plan)
        candidate.mount_directory(item.id, requested[item.original_priority].directory);
    vfs = std::move(candidate); // commit only after all mounts validated
    return plan;
}

} // namespace rloft_native
