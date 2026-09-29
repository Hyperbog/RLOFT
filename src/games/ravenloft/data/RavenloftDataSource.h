#pragma once

#include <filesystem>
#include <string>

namespace ravenloft::data {

struct ResolvedRavenloftData {
    std::filesystem::path root;
    std::filesystem::path source;
    bool fromGogImage{};
};

// Resolves either a complete/extracted RLOFT directory, a GOG installation
// directory containing game.gog, or game.gog itself into the ordinary flat
// RLOFT filesystem view expected by the faithful-port parsers.
ResolvedRavenloftData resolveRavenloftData(const std::filesystem::path& candidate,
                                           const std::filesystem::path& cacheRoot);

bool looksLikeRloftDirectory(const std::filesystem::path& root);
bool looksLikeGogImage(const std::filesystem::path& image);

} // namespace ravenloft::data
