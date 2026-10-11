#pragma once

// Framework-only VFS proof: read-only original media, in-memory mods and lazy
// local-folder overlays. Not a sandbox for actively malicious/racing processes.
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <map>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
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

inline void check_mod_path(std::string_view path) {
    if (path == "CHARS" || path == "SAVE.DAT" || path == "GAME.OPT")
        throw std::invalid_argument("protected original runtime file");
}

struct ResolvedFile final {
    std::string provider;
    std::vector<std::uint8_t> data;
};

struct Overlay final {
    std::string id;
    std::map<std::string, std::vector<std::uint8_t>> files;
};

struct DiskOverlay final {
    std::string id;
    std::filesystem::path root;
    // Canonical virtual key -> path relative to root, never source bytes.
    std::map<std::string, std::filesystem::path> files;
};

class ReadOnlyVfs final {
public:
    using Exists = std::function<bool(std::string_view)>;
    using Read = std::function<std::vector<std::uint8_t>(std::string_view)>;
    static constexpr std::uintmax_t kMaxDiagnosticFileBytes = 256U * 1024U * 1024U;

    ReadOnlyVfs(Exists exists, Read read)
        : original_exists_(std::move(exists)), original_read_(std::move(read)) {
        if (!original_exists_ || !original_read_)
            throw std::invalid_argument("original media callbacks required");
    }

    void mount(Overlay layer) {
        validate_mod_id(layer.id);
        Overlay normalized{std::move(layer.id), {}};
        for (auto& file : layer.files) {
            auto key = normalize_virtual_path(file.first);
            check_mod_path(key);
            if (!normalized.files.emplace(std::move(key), std::move(file.second)).second)
                throw std::invalid_argument("case-insensitive duplicate in mod");
        }
        layers_.emplace_back(std::move(normalized));
    }

    // Files stay on disk until requested. Mounting never reads/mutates the
    // original installation, and rejects symlink/junction-like entries where
    // exposed as filesystem symlinks. Security-sensitive race-free mounts
    // require OS file-descriptor-relative APIs, not this proof adapter.
    void mount_directory(std::string id, const std::filesystem::path& root) {
        namespace fs = std::filesystem;
        validate_mod_id(id);
        std::error_code ec;
        if (fs::is_symlink(fs::symlink_status(root, ec)) || ec ||
            !fs::is_directory(root))
            throw std::invalid_argument("mod root must be an ordinary directory");
        DiskOverlay layer{std::move(id), fs::canonical(root), {}};
        fs::recursive_directory_iterator it(layer.root, fs::directory_options::none, ec), end;
        if (ec) throw std::runtime_error("cannot traverse mod root");
        while (it != end) {
            const auto status = it->symlink_status(ec);
            if (ec || fs::is_symlink(status))
                throw std::invalid_argument("symbolic link not allowed in mod");
            if (fs::is_regular_file(status)) {
                const auto relative = it->path().lexically_relative(layer.root);
                if (relative.empty()) throw std::invalid_argument("mod relative path failed");
                const auto key = normalize_virtual_path(relative.generic_string());
                check_mod_path(key);
                const auto bytes = fs::file_size(it->path(), ec);
                if (ec || bytes > kMaxDiagnosticFileBytes)
                    throw std::invalid_argument("mod file exceeds diagnostic size limit");
                if (!layer.files.emplace(key, relative).second)
                    throw std::invalid_argument("case-insensitive duplicate in mod");
            } else if (!fs::is_directory(status)) {
                throw std::invalid_argument("unsupported mod filesystem entry");
            }
            it.increment(ec);
            if (ec) throw std::runtime_error("mod directory enumeration failed");
        }
        layers_.emplace_back(std::move(layer));
    }

    ResolvedFile read(std::string_view virtual_path) const {
        const auto key = normalize_virtual_path(virtual_path);
        for (auto layer = layers_.rbegin(); layer != layers_.rend(); ++layer) {
            if (const auto* mem = std::get_if<Overlay>(&*layer)) {
                const auto it = mem->files.find(key);
                if (it != mem->files.end()) return {mem->id, it->second};
            } else if (const auto* disk = std::get_if<DiskOverlay>(&*layer)) {
                const auto it = disk->files.find(key);
                if (it != disk->files.end())
                    return {disk->id, read_disk(*disk, it->second)};
            }
        }
        if (!original_exists_(key)) throw std::out_of_range("missing original/media file");
        return {"original", original_read_(key)};
    }

    std::vector<std::string> providers(std::string_view virtual_path) const {
        const auto key = normalize_virtual_path(virtual_path);
        std::vector<std::string> owners;
        for (auto layer = layers_.rbegin(); layer != layers_.rend(); ++layer) {
            if (const auto* mem = std::get_if<Overlay>(&*layer)) {
                if (mem->files.find(key) != mem->files.end()) owners.push_back(mem->id);
            } else if (const auto* disk = std::get_if<DiskOverlay>(&*layer)) {
                if (disk->files.find(key) != disk->files.end()) owners.push_back(disk->id);
            }
        }
        if (original_exists_(key)) owners.emplace_back("original");
        return owners;
    }

private:
    void validate_mod_id(const std::string& id) const {
        if (id.empty() || id == "original")
            throw std::invalid_argument("mod ID required");
        for (const auto& layer : layers_) {
            const auto& previous = std::visit([](const auto& value) -> const std::string& {
                return value.id;
            }, layer);
            if (previous == id) throw std::invalid_argument("duplicate mod ID");
        }
    }

    static std::vector<std::uint8_t> read_disk(
        const DiskOverlay& disk, const std::filesystem::path& relative) {
        namespace fs = std::filesystem;
        // Reject components replaced with symlinks since mount. This check is
        // NOT atomic with opening the file; distrust/races need openat-style APIs.
        auto current = disk.root;
        std::error_code ec;
        if (fs::is_symlink(fs::symlink_status(current, ec)) || ec)
            throw std::runtime_error("mod root changed to symlink");
        for (const auto& component : relative) {
            current /= component;
            if (fs::is_symlink(fs::symlink_status(current, ec)) || ec)
                throw std::runtime_error("mod path changed to symlink");
        }
        if (!fs::is_regular_file(current, ec) || ec)
            throw std::runtime_error("mod file disappeared");
        const auto count = fs::file_size(current, ec);
        if (ec || count > kMaxDiagnosticFileBytes)
            throw std::runtime_error("mod file size invalid");
        std::ifstream in(current, std::ios::binary);
        if (!in) throw std::runtime_error("cannot open mod file");
        std::vector<std::uint8_t> bytes(static_cast<std::size_t>(count));
        if (!bytes.empty()) in.read(reinterpret_cast<char*>(bytes.data()),
                                   static_cast<std::streamsize>(bytes.size()));
        if (!in || in.peek() != std::char_traits<char>::eof())
            throw std::runtime_error("mod file changed during read");
        return bytes;
    }

    Exists original_exists_;
    Read original_read_;
    std::vector<std::variant<Overlay, DiskOverlay>> layers_;
};

} // namespace rloft_native
