#include "games/ravenloft/data/RavenloftDataSource.h"
#include "games/ravenloft/data/Iso9660Image.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <stdexcept>

namespace ravenloft::data {
namespace {

bool fileAny(const std::filesystem::path& root, const char* a, const char* b = nullptr) {
    if (std::filesystem::is_regular_file(root / a)) return true;
    return b && std::filesystem::is_regular_file(root / b);
}

std::string lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
    return s;
}

std::filesystem::path findGogImageIn(const std::filesystem::path& dir) {
    if (!std::filesystem::is_directory(dir)) return {};
    for (const auto& e : std::filesystem::directory_iterator(dir)) {
        if (!e.is_regular_file()) continue;
        if (lower(e.path().filename().string()) == "game.gog") return e.path();
    }
    return {};
}

void writeSourceMarker(const std::filesystem::path& cache,
                       const std::filesystem::path& image) {
    std::ofstream out(cache / ".ravenloft_gog_source", std::ios::trunc);
    if (!out) return;
    std::error_code ec;
    out << std::filesystem::absolute(image, ec).string() << '\n';
    out << std::filesystem::file_size(image, ec) << '\n';
}

} // namespace

bool looksLikeRloftDirectory(const std::filesystem::path& root) {
    return std::filesystem::is_directory(root) &&
           std::filesystem::is_regular_file(root / "LEVELS.DAT") &&
           std::filesystem::is_regular_file(root / "SAVE.DAT") &&
           fileAny(root, "INIT", "INIT.") &&
           fileAny(root, "SINTAB", "SINTAB.") &&
           fileAny(root, "RES0", "RES0.");
}

bool looksLikeGogImage(const std::filesystem::path& image) {
    if (!std::filesystem::is_regular_file(image)) return false;
    if (lower(image.filename().string()) != "game.gog") return false;
    try {
        Iso9660Image iso(image);
        const auto e = iso.find("/RLOFT/LEVELS.DAT");
        return !e.directory && e.size > 0;
    } catch (...) {
        return false;
    }
}

ResolvedRavenloftData resolveRavenloftData(const std::filesystem::path& candidate,
                                           const std::filesystem::path& cacheRoot) {
    std::filesystem::path image;
    std::filesystem::path directRoot;

    if (std::filesystem::is_regular_file(candidate)) {
        if (looksLikeGogImage(candidate)) image = candidate;
    } else if (std::filesystem::is_directory(candidate)) {
        // Prefer game.gog whenever the user points at a GOG installation. It is
        // the complete CD authority and prevents the installed subset from
        // silently omitting RES9/RES10/cinematics.
        image = findGogImageIn(candidate);
        if (image.empty() && lower(candidate.filename().string()) == "rloft")
            image = findGogImageIn(candidate.parent_path());
        if (image.empty() && looksLikeRloftDirectory(candidate)) directRoot = candidate;
        if (image.empty() && directRoot.empty() && looksLikeRloftDirectory(candidate / "RLOFT"))
            directRoot = candidate / "RLOFT";
    }

    if (!image.empty()) {
        Iso9660Image iso(image);
        const auto extracted = cacheRoot / "gog_cd" / "RLOFT";
        iso.extractDirectory("/RLOFT", extracted);
        writeSourceMarker(extracted, image);
        if (!looksLikeRloftDirectory(extracted))
            throw std::runtime_error("game.gog extraction did not produce a valid Ravenloft RLOFT directory");
        return {extracted, image, true};
    }

    if (!directRoot.empty()) return {directRoot, directRoot, false};

    throw std::runtime_error(
        "Could not find Ravenloft data. Select game.gog, your GOG Ravenloft installation folder, "
        "or a complete RLOFT directory.");
}

} // namespace ravenloft::data
