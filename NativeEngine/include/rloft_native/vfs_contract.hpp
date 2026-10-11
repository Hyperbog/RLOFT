#pragma once

// Read-only, in-memory precedence proof. Not an implementation of file-backed
// mods, archive ingestion, scripting, or source game resource decoding.
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <functional>
#include <map>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace rloft_native {

inline std::string normalize_virtual_path(std::string_view value) {
    if (value.empty() || value.size() > 240 || value.front() == '/' ||
        value.front() == '\\')
        throw std::invalid_argument("invalid virtual path");
    std::string canonical;
    std::string part;
    for (std::size_t i = 0; i <= value.size(); ++i) {
        const char ch = i == value.size() ? '/' : value[i];
        if (ch == '/' || ch == '\\') {
            if (part.empty() || part == "." || part == "..")
                throw std::invalid_argument("empty/dot virtual path component");
            if (!canonical.empty()) canonical += '/';
            canonical += part;
            part.clear();
        } else {
            const auto byte = static_cast<unsigned char>(ch);
            if (!(std::isalnum(byte) && byte < 128) &&
                ch != '.' && ch != '_' && ch != '-' && ch != '+')
                throw std::invalid_argument("unsupported virtual path character");
            part += static_cast<char>(std::toupper(byte));
        }
    }
    return canonical;
}

struct ResolvedFile final {
    std::string provider;
    std::vector<std::uint8_t> data;
};

struct Overlay final {
    std::string id;
    std::map<std::string, std::vector<std::uint8_t>> files;
};

class ReadOnlyVfs final {
public:
    using Exists = std::function<bool(std::string_view)>;
    using Read = std::function<std::vector<std::uint8_t>(std::string_view)>;

    ReadOnlyVfs(Exists exists, Read read)
        : original_exists_(std::move(exists)), original_read_(std::move(read)) {
        if (!original_exists_ || !original_read_)
            throw std::invalid_argument("original media callbacks required");
    }

    void mount(Overlay layer) {
        if (layer.id.empty() || layer.id == "original")
            throw std::invalid_argument("mod ID required");
        for (const auto& previous : layers_)
            if (previous.id == layer.id)
                throw std::invalid_argument("duplicate mod ID");
        Overlay normalized{std::move(layer.id), {}};
        for (auto& file : layer.files) {
            auto key = normalize_virtual_path(file.first);
            // Writable original/runtime state must never be modded this way.
            if (key == "CHARS" || key == "SAVE.DAT" || key == "GAME.OPT")
                throw std::invalid_argument("protected runtime file");
            if (!normalized.files.emplace(std::move(key), std::move(file.second)).second)
                throw std::invalid_argument("case-insensitive duplicate in mod");
        }
        layers_.push_back(std::move(normalized));
    }

    ResolvedFile read(std::string_view virtual_path) const {
        const auto key = normalize_virtual_path(virtual_path);
        for (auto layer = layers_.rbegin(); layer != layers_.rend(); ++layer) {
            const auto it = layer->files.find(key);
            if (it != layer->files.end()) return {layer->id, it->second};
        }
        if (!original_exists_(key)) throw std::out_of_range("missing original/media file");
        return {"original", original_read_(key)};
    }

    std::vector<std::string> providers(std::string_view virtual_path) const {
        const auto key = normalize_virtual_path(virtual_path);
        std::vector<std::string> owners;
        for (auto layer = layers_.rbegin(); layer != layers_.rend(); ++layer)
            if (layer->files.find(key) != layer->files.end()) owners.push_back(layer->id);
        if (original_exists_(key)) owners.emplace_back("original");
        return owners;
    }

private:
    Exists original_exists_;
    Read original_read_;
    std::vector<Overlay> layers_;
};

} // namespace rloft_native
