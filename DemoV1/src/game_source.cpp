#include "rloft/game_source.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <map>
#include <set>
#include <stdexcept>
#include <system_error>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

namespace fs = std::filesystem;

namespace rloft {
namespace {

constexpr std::uint32_t kLogicalSector = 2048;

struct SectorLayout {
    std::uint32_t physical_sector;
    std::uint32_t user_offset;
};

std::string upper_ascii(std::string_view value) {
    std::string out(value);
    std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) {
        return static_cast<char>(std::toupper(c));
    });
    return out;
}

std::string iso_name(std::string_view raw) {
    auto name = upper_ascii(raw);
    if (const auto semi = name.find(';'); semi != std::string::npos) {
        name.resize(semi);
    }
    while (!name.empty() && name.back() == '.') {
        name.pop_back();
    }
    return name;
}

std::uint32_t le32(const std::uint8_t* p) {
    return static_cast<std::uint32_t>(p[0]) |
           (static_cast<std::uint32_t>(p[1]) << 8U) |
           (static_cast<std::uint32_t>(p[2]) << 16U) |
           (static_cast<std::uint32_t>(p[3]) << 24U);
}

std::optional<fs::path> child_case_insensitive(const fs::path& dir,
                                                std::string_view name) {
    std::error_code ec;
    if (!fs::is_directory(dir, ec)) return std::nullopt;
    const auto wanted = upper_ascii(name);
    for (const auto& entry : fs::directory_iterator(dir, ec)) {
        if (ec) break;
        if (upper_ascii(entry.path().filename().string()) == wanted) {
            return entry.path();
        }
    }
    return std::nullopt;
}

bool looks_like_loose_media(const fs::path& dir) {
    return child_case_insensitive(dir, "RES0").has_value() &&
           child_case_insensitive(dir, "INIT").has_value() &&
           child_case_insensitive(dir, "T").has_value();
}

std::optional<fs::path> find_game_gog(const fs::path& root, int depth = 2) {
    if (auto direct = child_case_insensitive(root, "game.gog")) return direct;
    if (depth <= 0) return std::nullopt;

    std::error_code ec;
    for (const auto& entry : fs::directory_iterator(root, ec)) {
        if (ec) break;
        if (!entry.is_directory(ec)) continue;
        if (auto nested = find_game_gog(entry.path(), depth - 1)) return nested;
    }
    return std::nullopt;
}

void add_env_candidate(std::vector<fs::path>& out, const char* name) {
    if (const char* value = std::getenv(name); value && *value) out.emplace_back(value);
}

#ifdef _WIN32
std::optional<fs::path> gog_registry_path(HKEY hive, REGSAM view) {
    const wchar_t* key_path = L"SOFTWARE\\GOG.com\\Games\\1432905053";
    HKEY key = nullptr;
    if (RegOpenKeyExW(hive, key_path, 0, KEY_READ | view, &key) != ERROR_SUCCESS) {
        return std::nullopt;
    }

    std::array<wchar_t, 32768> buffer{};
    DWORD bytes = static_cast<DWORD>(buffer.size() * sizeof(wchar_t));
    DWORD type = 0;
    const auto status = RegQueryValueExW(
        key, L"path", nullptr, &type,
        reinterpret_cast<LPBYTE>(buffer.data()), &bytes);
    RegCloseKey(key);

    if (status != ERROR_SUCCESS || (type != REG_SZ && type != REG_EXPAND_SZ)) {
        return std::nullopt;
    }
    return fs::path(buffer.data());
}
#endif

std::vector<fs::path> discovery_candidates() {
    std::vector<fs::path> out;
    add_env_candidate(out, "RLOFT_GAME_DIR");
    out.push_back(fs::current_path());

#ifdef _WIN32
    for (const auto hive : {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE}) {
        for (const auto view : {KEY_WOW64_32KEY, KEY_WOW64_64KEY}) {
            if (auto p = gog_registry_path(hive, view)) out.push_back(*p);
        }
    }
    if (const char* home = std::getenv("USERPROFILE")) {
        out.emplace_back(fs::path(home) / "GOG Games");
    }
    out.emplace_back("C:/GOG Games");
    add_env_candidate(out, "GOG_GAMES");

    if (const char* pf = std::getenv("ProgramFiles")) {
        out.emplace_back(fs::path(pf) / "GOG Galaxy" / "Games");
        out.emplace_back(fs::path(pf) / "Steam" / "steamapps" / "common");
    }
    if (const char* pf86 = std::getenv("ProgramFiles(x86)")) {
        out.emplace_back(fs::path(pf86) / "GOG Galaxy" / "Games");
        out.emplace_back(fs::path(pf86) / "Steam" / "steamapps" / "common");
    }
#else
    if (const char* home = std::getenv("HOME")) {
        out.emplace_back(fs::path(home) / "GOG Games");
        out.emplace_back(fs::path(home) / ".local" / "share" / "Steam" /
                         "steamapps" / "common");
    }
#endif
    return out;
}

}  // namespace

struct Iso9660Image::Impl {
    struct Entry {
        std::uint32_t extent{};
        std::uint32_t size{};
        bool directory{};
    };

    fs::path path;
    mutable std::ifstream stream;
    SectorLayout layout{};
    std::map<std::string, Entry> by_name;
    std::set<std::uint32_t> visited_directories;

    explicit Impl(fs::path input)
        : path(std::move(input)), stream(path, std::ios::binary) {
        if (!stream) {
            throw std::runtime_error("Unable to open CD image: " + path.string());
        }
        layout = detect_layout();
        index_volume();
    }

    std::array<std::uint8_t, kLogicalSector> sector(std::uint32_t lba) const {
        std::array<std::uint8_t, kLogicalSector> out{};
        const auto pos = static_cast<std::uint64_t>(lba) * layout.physical_sector +
                         layout.user_offset;

        stream.clear();
        stream.seekg(static_cast<std::streamoff>(pos));
        stream.read(reinterpret_cast<char*>(out.data()), out.size());
        if (stream.gcount() != static_cast<std::streamsize>(out.size())) {
            throw std::runtime_error("Short read in CD image");
        }
        return out;
    }

    SectorLayout detect_layout() const {
        constexpr std::array<SectorLayout, 3> layouts{{
            {2048, 0},
            {2352, 16},
            {2352, 24},
        }};

        for (const auto candidate : layouts) {
            const auto pos = static_cast<std::uint64_t>(16) *
                                 candidate.physical_sector +
                             candidate.user_offset;
            std::array<std::uint8_t, 7> sig{};
            stream.clear();
            stream.seekg(static_cast<std::streamoff>(pos));
            stream.read(reinterpret_cast<char*>(sig.data()), sig.size());
            if (stream.gcount() == static_cast<std::streamsize>(sig.size()) &&
                sig[0] == 1 &&
                std::equal(sig.begin() + 1, sig.begin() + 6, "CD001") &&
                sig[6] == 1) {
                return candidate;
            }
        }

        throw std::runtime_error(
            "game.gog is not a supported ISO-9660 data image");
    }

    std::vector<std::uint8_t> read_extent(std::uint32_t extent,
                                          std::uint32_t size) const {
        std::vector<std::uint8_t> out(size);
        std::uint32_t copied = 0;
        for (std::uint32_t i = 0; copied < size; ++i) {
            const auto data = sector(extent + i);
            const auto count =
                std::min<std::uint32_t>(kLogicalSector, size - copied);
            std::copy_n(data.begin(), count, out.begin() + copied);
            copied += count;
        }
        return out;
    }

    void index_volume() {
        const auto pvd = sector(16);
        if (pvd[0] != 1 ||
            std::string_view(reinterpret_cast<const char*>(pvd.data() + 1), 5) !=
                "CD001") {
            throw std::runtime_error("ISO primary volume descriptor missing");
        }

        const auto* root = pvd.data() + 156;
        if (root[0] < 34) {
            throw std::runtime_error("ISO root directory record invalid");
        }
        index_directory(le32(root + 2), le32(root + 10), 0);
    }

    void index_directory(std::uint32_t extent, std::uint32_t size, int depth) {
        if (depth > 8 || !visited_directories.insert(extent).second) return;

        const auto bytes = read_extent(extent, size);
        std::size_t pos = 0;
        while (pos < bytes.size()) {
            const auto len = bytes[pos];
            if (len == 0) {
                pos = ((pos / kLogicalSector) + 1) * kLogicalSector;
                continue;
            }
            if (pos + len > bytes.size() || len < 34) break;

            const auto* rec = bytes.data() + pos;
            const auto name_len = rec[32];
            if (33U + name_len > len) break;

            const auto raw = std::string_view(
                reinterpret_cast<const char*>(rec + 33), name_len);
            const auto child_extent = le32(rec + 2);
            const auto child_size = le32(rec + 10);
            const bool directory = (rec[25] & 0x02U) != 0;

            if (!(name_len == 1 && (rec[33] == 0 || rec[33] == 1))) {
                const auto name = iso_name(raw);
                if (!name.empty()) {
                    by_name.emplace(
                        name, Entry{child_extent, child_size, directory});
                }
                if (directory) {
                    index_directory(child_extent, child_size, depth + 1);
                }
            }
            pos += len;
        }
    }
};

Iso9660Image::Iso9660Image(fs::path image_path)
    : impl_(std::make_unique<Impl>(std::move(image_path))) {}
Iso9660Image::~Iso9660Image() = default;
Iso9660Image::Iso9660Image(Iso9660Image&&) noexcept = default;
Iso9660Image& Iso9660Image::operator=(Iso9660Image&&) noexcept = default;

bool Iso9660Image::contains(std::string_view filename) const {
    const auto it = impl_->by_name.find(iso_name(filename));
    return it != impl_->by_name.end() && !it->second.directory;
}

std::vector<std::uint8_t> Iso9660Image::read(std::string_view filename) const {
    const auto it = impl_->by_name.find(iso_name(filename));
    if (it == impl_->by_name.end() || it->second.directory) {
        throw std::runtime_error(
            "File not found in CD image: " + std::string(filename));
    }
    return impl_->read_extent(it->second.extent, it->second.size);
}

const fs::path& Iso9660Image::path() const noexcept {
    return impl_->path;
}

GameSource GameSource::open(const fs::path& input) {
    std::error_code ec;
    const auto absolute = fs::absolute(input, ec);
    const auto path = ec ? input : absolute;

    GameSource source;
    if (fs::is_regular_file(path, ec)) {
        source.located_ = {
            path.parent_path(), GameSourceKind::GogCdImage, path};
        source.iso_ = std::make_unique<Iso9660Image>(path);
        return source;
    }

    if (!fs::is_directory(path, ec)) {
        throw std::runtime_error(
            "Game path does not exist: " + path.string());
    }

    if (looks_like_loose_media(path)) {
        source.located_ = {
            path, GameSourceKind::LooseDirectory, path};
        return source;
    }

    if (auto image = find_game_gog(path, 2)) {
        source.located_ = {
            image->parent_path(), GameSourceKind::GogCdImage, *image};
        source.iso_ = std::make_unique<Iso9660Image>(*image);
        return source;
    }

    throw std::runtime_error(
        "No Ravenloft media corpus or game.gog found under: " +
        path.string());
}

std::optional<GameSource> GameSource::auto_locate() {
    std::set<fs::path> seen;
    for (const auto& candidate : discovery_candidates()) {
        std::error_code ec;
        if (!fs::exists(candidate, ec)) continue;

        auto normalized = fs::weakly_canonical(candidate, ec);
        if (ec) normalized = candidate;
        if (!seen.insert(normalized).second) continue;

        try {
            auto source = open(normalized);
            if (source.contains("RES0") &&
                source.contains("INIT") &&
                source.contains("T")) {
                return source;
            }
        } catch (...) {
            // Discovery is best-effort. Explicit open() reports failures.
        }
    }
    return std::nullopt;
}

bool GameSource::contains(std::string_view filename) const {
    if (located_.kind == GameSourceKind::GogCdImage) {
        return iso_->contains(filename);
    }
    return child_case_insensitive(
               located_.media_path, filename).has_value();
}

std::vector<std::uint8_t> GameSource::read(
    std::string_view filename) const {
    if (located_.kind == GameSourceKind::GogCdImage) {
        return iso_->read(filename);
    }

    const auto path =
        child_case_insensitive(located_.media_path, filename);
    if (!path || !fs::is_regular_file(*path)) {
        throw std::runtime_error(
            "File not found in Ravenloft source: " +
            std::string(filename));
    }

    std::ifstream in(*path, std::ios::binary);
    if (!in) {
        throw std::runtime_error("Unable to open: " + path->string());
    }

    in.seekg(0, std::ios::end);
    const auto size = in.tellg();
    in.seekg(0);

    std::vector<std::uint8_t> bytes(static_cast<std::size_t>(size));
    in.read(reinterpret_cast<char*>(bytes.data()), size);
    if (!in) {
        throw std::runtime_error("Unable to read: " + path->string());
    }
    return bytes;
}

std::optional<fs::path> GameSource::runtime_file(
    std::string_view filename) const {
    for (const auto& root :
         {located_.install_root, located_.install_root / "RLOFT"}) {
        if (auto p = child_case_insensitive(root, filename);
            p && fs::is_regular_file(*p)) {
            return p;
        }
    }
    return std::nullopt;
}

fs::path GameSource::runtime_output_path(
    std::string_view filename) const {
    if (const auto existing = runtime_file(filename)) {
        return *existing;
    }

    // R.EXE/CODE.1 use a relative "CHARS" pathname in the writable runtime
    // directory. GOG installations commonly keep that runtime directory as
    // an RLOFT child next to game.gog; loose-media installs are already rooted
    // at the runtime directory.
    if (const auto rloft =
            child_case_insensitive(located_.install_root, "RLOFT")) {
        std::error_code ec;
        if (fs::is_directory(*rloft, ec)) {
            return *rloft / std::string(filename);
        }
    }
    return located_.install_root / std::string(filename);
}

std::string_view to_string(GameSourceKind kind) noexcept {
    switch (kind) {
        case GameSourceKind::LooseDirectory:
            return "loose Ravenloft media directory";
        case GameSourceKind::GogCdImage:
            return "installed GOG game.gog CD image";
    }
    return "unknown source";
}

}  // namespace rloft
