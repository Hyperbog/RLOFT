#pragma once
#include <filesystem>
#include <optional>

namespace rloft {
class GameSource;

// Cache contains only locally imported original media; it is never shipped.
// The original GameSource interface remains the consumer-facing contract.
std::filesystem::path persistent_media_cache_directory();
std::optional<GameSource> open_persistent_media_cache();
GameSource import_persistent_media_cache(GameSource original);
}
