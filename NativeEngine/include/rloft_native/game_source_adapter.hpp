#pragma once

// Read-only bridge to the inherited verified loose-media/game.gog reader.
// The GameSource instance MUST outlive the returned VFS. No original writes.
#include "rloft/game_source.hpp"
#include "rloft_native/vfs_contract.hpp"

namespace rloft_native {

inline ReadOnlyVfs bind_original_media(const rloft::GameSource& source) {
    return ReadOnlyVfs(
        [&source](std::string_view file) { return source.contains(file); },
        [&source](std::string_view file) { return source.read(file); });
}

} // namespace rloft_native
