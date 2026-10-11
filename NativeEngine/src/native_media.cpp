#include "rloft_native/native_media.hpp"

#include <algorithm>
#include <array>
#include <fstream>
#include <functional>
#include <limits>
#include <stdexcept>
#include <system_error>
#include <utility>

namespace rloft_native {
namespace {
namespace fs = std::filesystem;
constexpr std::uint64_t kMaxAssetBytes = 256ULL * 1024ULL * 1024ULL;
constexpr std::uint32_t kLogicalSector = 2048;

std::uint32_t le32(const std::uint8_t* p) {
    return std::uint32_t(p[0]) | (std::uint32_t(p[1]) << 8) |
           (std::uint32_t(p[2]) << 16) | (std::uint32_t(p[3]) << 24);
}

std::string upper(std::string_view s) {
    std::string o;
    o.reserve(s.size());
    for (char c : s) {
        if (c >= 'a' && c <= 'z') c = char(c - 'a' + 'A');
        o.push_back(c);
    }
    return o;
}

bool has_name(const fs::path& p, std::string_view name) {
    return upper(p.filename().string()) == upper(name);
}

std::vector<fs::path> children(const fs::path& root) {
    std::vector<fs::path> entries;
    std::error_code error;
    for (fs::directory_iterator iter(root, fs::directory_options::skip_permission_denied, error);
         !error && iter != fs::directory_iterator(); iter.increment(error)) {
        entries.push_back(iter->path());
    }
    if (error) throw std::runtime_error("cannot index original directory");
    std::sort(entries.begin(), entries.end());
    return entries;
}

bool looks_loose(const fs::path& path) {
    bool init=false,res=false,t=false;
    for (const auto& file : children(path)) {
        if (!fs::is_regular_file(file) || fs::is_symlink(file)) continue;
        init |= has_name(file, "INIT");
        res |= has_name(file, "RES0");
        t |= has_name(file, "T");
    }
    return init && res && t;
}

fs::path find_iso(const fs::path& root, int depth) {
    if (depth < 0) return {};
    for (const auto& p : children(root)) {
        if (fs::is_symlink(p)) continue;
        if (fs::is_regular_file(p) && has_name(p, "game.gog")) return p;
    }
    if (depth == 0) return {};
    for (const auto& p : children(root)) {
        if (fs::is_symlink(p)) continue;
        if (fs::is_directory(p)) {
            auto candidate = find_iso(p, depth - 1);
            if (!candidate.empty()) return candidate;
        }
    }
    return {};
}

void read_exact(std::ifstream& stream, std::uint64_t at,
                std::uint8_t* dst, std::size_t length) {
    if (at > static_cast<std::uint64_t>(std::numeric_limits<std::streamoff>::max()))
        throw std::runtime_error("original media offset is too large");
    stream.clear();
    stream.seekg(static_cast<std::streamoff>(at));
    stream.read(reinterpret_cast<char*>(dst), static_cast<std::streamsize>(length));
    if (stream.gcount() != static_cast<std::streamsize>(length))
        throw std::runtime_error("short original media read");
}
} // namespace

std::string native_asset_key(std::string_view path) {
    if (path.empty() || path.size() > 256)
        throw std::invalid_argument("invalid original asset path length");
    std::string result;
    result.reserve(path.size());
    bool start = true;
    std::size_t component_start = 0;
    for (std::size_t i = 0; i <= path.size(); ++i) {
        if (i == path.size() || path[i] == '/' || path[i] == '\\') {
            if (i == component_start) throw std::invalid_argument("empty path component");
            const auto component = path.substr(component_start, i - component_start);
            if (component == "." || component == "..")
                throw std::invalid_argument("asset traversal denied");
            if (i == path.size()) break;
            result += '/';
            component_start = i + 1;
            start = false;
            continue;
        }
        const char c = path[i];
        if (!(c >= 'A' && c <= 'Z') && !(c >= 'a' && c <= 'z') &&
            !(c >= '0' && c <= '9') && c != '.' && c != '_' && c != '-' &&
            c != '$' && c != '~') {
            throw std::invalid_argument("unsupported original asset path character");
        }
        result.push_back((c >= 'a' && c <= 'z') ? char(c - 'a' + 'A') : c);
    }
    (void)start;
    if (result.empty() || result.front() == '/' || result.back() == '/')
        throw std::invalid_argument("invalid original asset name");
    return result;
}

NativeMedia NativeMedia::open(const fs::path& input) {
    if (input.empty()) throw std::invalid_argument("original install path missing");
    NativeMedia result;
    std::error_code error;
    const auto root = fs::absolute(input, error);
    if (error) throw std::runtime_error("unable to resolve original game path");
    if (fs::is_regular_file(root)) {
        result.index_iso(root);
    } else if (fs::is_directory(root)) {
        if (looks_loose(root)) {
            result.index_loose(root);
        } else {
            const auto iso = find_iso(root, 2);
            if (iso.empty()) throw std::runtime_error("original INIT/RES0/T or game.gog not found");
            result.index_iso(iso);
        }
    } else {
        throw std::runtime_error("original game path is not a file or directory");
    }
    if (!result.contains("INIT") || !result.contains("RES0") ||
        !result.contains("T")) {
        throw std::runtime_error("original media minimum signature missing");
    }
    return result;
}

void NativeMedia::index_loose(const fs::path& root) {
    source_ = root;
    kind_ = NativeMediaKind::LooseDirectory;
    std::function<void(const fs::path&, std::string, int)> visit;
    visit = [&](const fs::path& directory, std::string prefix, int depth) {
        if (depth > 5) return;
        for (const auto& child : children(directory)) {
            if (fs::is_symlink(child)) continue;
            if (fs::is_directory(child)) {
                visit(child, prefix + child.filename().string() + "/", depth + 1);
                continue;
            }
            if (!fs::is_regular_file(child)) continue;
            std::string name;
            try { name = native_asset_key(prefix + child.filename().string()); }
            catch (const std::invalid_argument&) { continue; } // unrelated host files
            const auto bytes = fs::file_size(child);
            if (files_.find(name) != files_.end())
                throw std::runtime_error("ambiguous original DOS filename collision");
            files_.emplace(name, File{child, bytes, 0, false});
            if (files_.size() > 32768) throw std::runtime_error("too many original files");
        }
    };
    visit(root, "", 0);
}

void NativeMedia::index_iso(const fs::path& image) {
    source_ = image;
    physical_length_ = fs::file_size(image);
    std::ifstream stream(image, std::ios::binary);
    if (!stream) throw std::runtime_error("cannot open original game.gog image");
    constexpr std::array<std::pair<std::uint32_t, std::uint32_t>, 3> layouts{{
        {2048, 0}, {2352, 16}, {2352, 24}
    }};
    bool found = false;
    for (const auto& layout : layouts) {
        const auto pos = 16ULL * layout.first + layout.second;
        if (pos + 7 > physical_length_) continue;
        std::array<std::uint8_t, 7> signature{};
        read_exact(stream, pos, signature.data(), signature.size());
        if (signature[0] == 1 &&
            std::equal(signature.begin() + 1, signature.begin() + 6, "CD001") &&
            signature[6] == 1) {
            sector_size_ = layout.first;
            user_offset_ = layout.second;
            kind_ = layout.first == 2048 ? NativeMediaKind::GogIso2048
                  : layout.second == 16 ? NativeMediaKind::GogIso2352_16
                                        : NativeMediaKind::GogIso2352_24;
            found = true;
            break;
        }
    }
    if (!found) throw std::runtime_error("unsupported ISO9660 game.gog sector layout");
    auto pvd = iso_read(16, kLogicalSector);
    if (pvd[0] != 1 || pvd[6] != 1 ||
        !std::equal(pvd.begin() + 1, pvd.begin() + 6, "CD001"))
        throw std::runtime_error("invalid original ISO primary descriptor");
    const auto* root = pvd.data() + 156;
    if (root[0] < 34 || (root[25] & 2) == 0)
        throw std::runtime_error("invalid original ISO root directory");
    std::vector<std::uint32_t> visited;
    index_iso_directory(le32(root + 2), le32(root + 10), "", 0, visited);
}

std::vector<std::uint8_t> NativeMedia::iso_read(
    std::uint32_t extent, std::uint64_t byte_count) const {
    if (byte_count > kMaxAssetBytes)
        throw std::runtime_error("original ISO resource exceeds bounded read size");
    const auto sector_count = (byte_count + 2047) / 2048;
    if (sector_count && (std::uint64_t(extent) + sector_count - 1) >
        (std::numeric_limits<std::uint64_t>::max() - user_offset_ - 2048) / sector_size_)
        throw std::runtime_error("original ISO extent arithmetic overflow");
    if (sector_count) {
        const auto final = (std::uint64_t(extent) + sector_count - 1) *
                           sector_size_ + user_offset_ + 2048;
        if (final > physical_length_)
            throw std::runtime_error("original ISO file extent exceeds image");
    }
    std::vector<std::uint8_t> output(static_cast<std::size_t>(byte_count));
    std::ifstream stream(source_, std::ios::binary);
    if (!stream) throw std::runtime_error("original ISO disappeared after indexing");
    std::size_t copied = 0;
    for (std::uint64_t i = 0; copied < output.size(); ++i) {
        std::array<std::uint8_t, 2048> sector{};
        const auto physical = (std::uint64_t(extent) + i) * sector_size_ +
                              user_offset_;
        read_exact(stream, physical, sector.data(), sector.size());
        const auto length = std::min<std::size_t>(sector.size(), output.size() - copied);
        std::copy_n(sector.data(), length, output.data() + copied);
        copied += length;
    }
    return output;
}

void NativeMedia::index_iso_directory(std::uint32_t extent, std::uint32_t size,
                                      const std::string& prefix, unsigned depth,
                                      std::vector<std::uint32_t>& visited) {
    if (depth > 8) throw std::runtime_error("original ISO directory nesting too deep");
    if (std::find(visited.begin(), visited.end(), extent) != visited.end()) return;
    if (visited.size() >= 4096) throw std::runtime_error("original ISO has too many directories");
    visited.push_back(extent);
    if (size > 16U * 1024U * 1024U)
        throw std::runtime_error("original ISO directory too large");
    const auto bytes = iso_read(extent, size);
    std::size_t pos = 0;
    while (pos < bytes.size()) {
        const auto length = bytes[pos];
        if (length == 0) {
            pos = (pos / 2048 + 1) * 2048;
            continue;
        }
        if (length < 34 || length > bytes.size() - pos)
            throw std::runtime_error("invalid original ISO directory record length");
        const auto* rec = bytes.data() + pos;
        const auto name_len = rec[32];
        if (std::size_t(33) + name_len > length)
            throw std::runtime_error("invalid original ISO filename length");
        const bool self = name_len == 1 && (rec[33] == 0 || rec[33] == 1);
        if (!self) {
            std::string raw(reinterpret_cast<const char*>(rec + 33), name_len);
            if (const auto semi = raw.find(';'); semi != std::string::npos)
                raw.resize(semi);
            while (!raw.empty() && raw.back() == '.') raw.pop_back();
            const bool directory = (rec[25] & 2) != 0;
            std::string key;
            try { key = native_asset_key(prefix + raw); }
            catch (const std::invalid_argument&) {
                throw std::runtime_error("unsupported original ISO asset filename");
            }
            const auto child_extent = le32(rec + 2), child_size = le32(rec + 10);
            if (directory) {
                index_iso_directory(child_extent, child_size, key + "/", depth + 1, visited);
            } else {
                if (files_.find(key) != files_.end())
                    throw std::runtime_error("ambiguous original ISO filename collision");
                // Bounded metadata indexing. Payload size is validated
                // and read only on request; no bulk ISO extraction.
                files_.emplace(key, File{{}, child_size, child_extent, true});
                if (files_.size() > 32768)
                    throw std::runtime_error("original ISO file index too large");
            }
        }
        pos += length;
    }
}

const NativeMedia::File* NativeMedia::find_record(std::string_view name) const {
    const auto key = native_asset_key(name);
    if (const auto it = files_.find(key); it != files_.end()) return &it->second;
    if (key.find('/') != std::string::npos) return nullptr;
    const File* match = nullptr;
    for (const auto& pair : files_) {
        const auto slash = pair.first.find_last_of('/');
        if (slash != std::string::npos && pair.first.substr(slash + 1) == key) {
            if (match != nullptr) return nullptr; // ambiguous basename
            match = &pair.second;
        }
    }
    return match;
}

bool NativeMedia::contains(std::string_view name) const {
    return find_record(name) != nullptr;
}

std::vector<std::uint8_t> NativeMedia::read(std::string_view name) const {
    const auto* selected = find_record(name);
    if (!selected) throw std::runtime_error("original asset missing or ambiguous");
    const auto& file = *selected;
    if (file.size > kMaxAssetBytes)
        throw std::runtime_error("requested original file exceeds 256 MiB limit");
    if (file.iso) return iso_read(file.extent, file.size);
    if (fs::is_symlink(file.loose_path) || !fs::is_regular_file(file.loose_path))
        throw std::runtime_error("indexed original asset path is no longer a regular file");
    std::vector<std::uint8_t> bytes(static_cast<std::size_t>(file.size));
    std::ifstream stream(file.loose_path, std::ios::binary);
    if (!stream) throw std::runtime_error("cannot open indexed original asset");
    if (!bytes.empty())
        read_exact(stream, 0, bytes.data(), bytes.size());
    return bytes;
}

std::vector<NativeAssetInfo> NativeMedia::inventory() const {
    std::vector<NativeAssetInfo> result;
    result.reserve(files_.size());
    for (const auto& entry : files_)
        result.push_back({entry.first, entry.second.size});
    return result;
}

} // namespace rloft_native
