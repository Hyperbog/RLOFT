#pragma once
#include <filesystem>
#include <optional>

namespace rloft {
class GameSource;

// Cache contains only locally imported original media; it is never shipped.
// The original GameSource interface remains the consumer-facing contract.
std::filesystem::path persistent_media_cache_directory();
std::optional<GameSource> open_persistent_media_cache();
// Reuse the cache only if it was imported from this explicitly selected source.
std::optional<GameSource> open_persistent_media_cache_for(
    const std::filesystem::path& original_media_path);
GameSource import_persistent_media_cache(GameSource original);
}
