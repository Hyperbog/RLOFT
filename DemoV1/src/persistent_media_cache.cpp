#include "rloft/persistent_media_cache.hpp"
#include "rloft/game_source.hpp"
#include "rloft/resource_assets.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstdint>
#include <array>
#include <fstream>
#include <iomanip>
#include <limits>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace rloft {
namespace {
namespace fs = std::filesystem;
constexpr const char* kVersion = "rloft-native-media-v3";
constexpr std::size_t kPerFileLimit = 512U * 1024U * 1024U;
constexpr std::size_t kMediaLimit = 3ULL * 1024ULL * 1024ULL * 1024ULL;

bool safe_filename(std::string_view n) {
    if (n.empty() || n.size() > 120 || n[0] == '.') return false;
    for (unsigned char ch : n)
        if (!(std::isalnum(ch) || ch == '_' || ch == '-' || ch == '.')) return false;
    return n != "." && n != "..";
}
std::string uppercase(std::string s) {
    for (char& ch : s) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    return s;
}
bool media_file(const std::string& name) {
    auto n = uppercase(name);
    if (!safe_filename(n) || n == "CHARS" || n == "SAVE.DAT" ||
        (n.rfind("LEV", 0) == 0 && n != "LEVELS.DAT") ||
        n.rfind("SAV", 0) == 0 ||
        n == "GAME.GOG") return false;
    if (n == "T" || n == "INIT" || n == "COLORS" || n == "SINTAB" ||
        n == "CODE.1" || n == "CODE.2") return true;
    if (n.size() > 3 && n.rfind("RES", 0) == 0 &&
        std::all_of(n.begin() + 3, n.end(), [](unsigned char c){return std::isdigit(c) != 0;})) return true;
    const auto pos = n.rfind('.');
    if (pos == std::string::npos) return false;
    auto ext = n.substr(pos);
    return ext == ".DAT" || ext == ".ANM" || ext == ".VOC" ||
           ext == ".MID" || ext == ".XMI" || ext == ".MUS" ||
           ext == ".PAL" || ext == ".CPS" || ext == ".SHP";
}
std::uint64_t checksum(const std::vector<std::uint8_t>& bytes) {
    std::uint64_t h = 14695981039346656037ULL;
    for (auto b : bytes) { h ^= b; h *= 1099511628211ULL; }
    return h;
}
void write_file(const fs::path& p, const std::vector<std::uint8_t>& data) {
    fs::create_directories(p.parent_path());
    std::ofstream out(p, std::ios::binary | std::ios::trunc);
    if (!out) throw std::runtime_error("Cannot open cache asset: " + p.string());
    if (!data.empty()) out.write(reinterpret_cast<const char*>(data.data()),
                                 static_cast<std::streamsize>(data.size()));
    out.close();
    if (!out) throw std::runtime_error("Cannot create cache asset: " + p.string());
}
void append_u32(std::vector<std::uint8_t>& out, std::uint32_t n) {
    for (unsigned i = 0; i < 4; ++i) out.push_back(static_cast<std::uint8_t>(n >> (i*8)));
}
void export_indexed(const fs::path& p, const Type1Raster& image) {
    if (image.width <= 0 || image.height <= 0 || image.count <= 0 ||
        image.pixels.size() != static_cast<std::size_t>(image.width) *
                               static_cast<std::size_t>(image.height) *
                               static_cast<std::size_t>(image.count))
        throw std::runtime_error("Malformed decoded indexed image");
    std::vector<std::uint8_t> data = {'R','8','I','1'};
    append_u32(data, static_cast<std::uint32_t>(image.width));
    append_u32(data, static_cast<std::uint32_t>(image.height));
    append_u32(data, static_cast<std::uint32_t>(image.count));
    data.push_back(image.transparent_index);
    data.insert(data.end(), image.pixels.begin(), image.pixels.end());
    write_file(p, data);
}
void export_type7(const fs::path& p, const Type7NativeRaster& image) {
    const auto length = static_cast<std::size_t>(image.width) * image.height;
    if (length == 0 || length != image.pixels.size() || length != image.mask.size())
        throw std::runtime_error("Malformed decoded type-7 mask");
    std::vector<std::uint8_t> data = {'R','8','M','1'};
    append_u32(data, image.width);
    append_u32(data, image.height);
    data.insert(data.end(), image.pixels.begin(), image.pixels.end());
    data.insert(data.end(), image.mask.begin(), image.mask.end());
    write_file(p, data);
}
void decode_res(const fs::path& decoded, const std::string& name,
                const std::vector<std::uint8_t>& bytes, std::ostream& log) {
    ResArchive archive(bytes); // Existing, authenticated game's RES decoder.
    if (archive.entry_count() > 50000) throw std::runtime_error("Oversized RES table");
    unsigned extracted = 0, unsupported = 0;
    for (int i = 0; i < archive.entry_count(); ++i) {
        const auto raw = archive.entry(i);
        std::ostringstream base;
        base << std::setw(5) << std::setfill('0') << i;
        const auto dir = decoded / name;
        write_file(dir / (base.str() + ".eh"), raw); // Editable original entry.
        if (raw.size() < 5 || raw[0] != 'E' || raw[1] != 'H') {
            ++unsupported; continue;
        }
        try {
            switch (raw[2]) {
                case 1: export_indexed(dir/(base.str()+".r8i"), archive.decode_type1(i)); break;
                case 2: {
                    auto font = archive.decode_type2(i);
                    export_indexed(dir/(base.str()+".r8i"), font.raster);
                    write_file(dir/(base.str()+".widths"),
                        std::vector<std::uint8_t>(font.widths.begin(),font.widths.end()));
                    break;
                }
                case 3: write_file(dir/(base.str()+".xmi"), archive.decode_type3(i)); break;
                case 4: write_file(dir/(base.str()+".voc"), archive.decode_type4(i)); break;
                case 6: write_file(dir/(base.str()+".bin"), archive.decode_type6_block(i)); break;
                case 7: export_type7(dir/(base.str()+".r8m"), archive.decode_type7_native_raster(i)); break;
                default: ++unsupported; continue;
            }
            ++extracted;
        } catch (const std::exception& e) {
            log << "DECODE_FAILURE\t" << name << '\t' << i << '\t' << e.what() << '\n';
        }
    }
    log << "ARCHIVE\t" << name << '\t' << archive.entry_count() << '\t'
        << extracted << '\t' << unsupported << '\n';
}
void split_levels(const std::vector<std::uint8_t>& data, const fs::path& output) {
    auto get32 = [&](std::size_t off) -> std::uint32_t {
        if (off > data.size() || data.size() - off < 4) throw std::runtime_error("Bad LEVELS archive");
        return std::uint32_t(data[off]) | std::uint32_t(data[off+1]) << 8U |
               std::uint32_t(data[off+2]) << 16U | std::uint32_t(data[off+3]) << 24U;
    };
    if (get32(0) != 32 || get32(4) != 132) throw std::runtime_error("Unexpected LEVELS directory");
    for (unsigned i=0; i<32; ++i) {
        auto start = get32(4U + i*4U);
        auto end = i==31 ? data.size() : get32(8U + i*4U);
        if (start >= end || end > data.size() || end-start < 29 ||
            get32(start+1) != 29) throw std::runtime_error("Corrupt LEVELS record");
        std::ostringstream name;
        name << "level_" << std::setw(2) << std::setfill('0') << i << ".bin";
        write_file(output/name.str(), std::vector<std::uint8_t>(data.begin()+start,data.begin()+end));
    }
}
std::optional<fs::path> cache_override() {
    if (const char* p = std::getenv("RLOFT_CACHE_DIR"); p && *p) return fs::path(p);
    return std::nullopt;
}
} // namespace

fs::path persistent_media_cache_directory() {
    if (auto p = cache_override()) return *p;
#ifdef _WIN32
    if (const char* home = std::getenv("LOCALAPPDATA"); home && *home)
        return fs::path(home) / "RavenloftEnhanced" / "AssetCache";
#endif
    if (const char* home = std::getenv("XDG_CACHE_HOME"); home && *home)
        return fs::path(home) / "rloft-enhanced";
    if (const char* home = std::getenv("HOME"); home && *home)
        return fs::path(home) / ".cache" / "rloft-enhanced";
    return fs::temp_directory_path() / "rloft-enhanced";
}

std::optional<GameSource> open_persistent_media_cache() {
    if (const char* force = std::getenv("RLOFT_REIMPORT_ASSETS"); force && *force=='1')
        return std::nullopt;
    auto root = persistent_media_cache_directory() / "v3";
    auto marker = root / "complete.tsv";
    std::ifstream in(marker);
    std::string line;
    if (!in || !std::getline(in,line) || line != kVersion) return std::nullopt;
    std::set<std::string> seen;
    while (std::getline(in,line)) {
        std::istringstream row(line);
        std::string name, trailing, digest;
        std::uintmax_t size=0;
        if (!(row >> name >> size >> digest) || (row >> trailing) ||
            !safe_filename(name) || !media_file(name) ||
            uppercase(name) != name || !seen.insert(name).second ||
            digest.size() != 16 ||
            !std::all_of(digest.begin(), digest.end(), [](unsigned char c) {
                return std::isxdigit(c) != 0;
            })) return std::nullopt;
        const auto path = root / "media" / name;
        std::error_code ec;
        if (fs::is_symlink(path,ec) || !fs::is_regular_file(path,ec) ||
            fs::file_size(path,ec) != size || ec) return std::nullopt;
        std::ifstream source(path, std::ios::binary);
        if (!source) return std::nullopt;
        std::array<char, 65536> chunk{};
        std::uint64_t actual = 14695981039346656037ULL;
        while (source) {
            source.read(chunk.data(), static_cast<std::streamsize>(chunk.size()));
            const auto got = source.gcount();
            for (std::streamsize i=0; i<got; ++i) {
                actual ^= static_cast<unsigned char>(chunk[static_cast<std::size_t>(i)]);
                actual *= 1099511628211ULL;
            }
        }
        if (!source.eof()) return std::nullopt;
        std::ostringstream expected;
        expected << std::hex << std::setw(16) << std::setfill('0') << actual;
        if (digest != expected.str()) return std::nullopt;
    }
    if (seen.size() < 18 || !in.eof()) return std::nullopt;
    auto result = GameSource::open(root / "media");
    if (!result.has_full_game_media()) return std::nullopt;
    result.set_runtime_overlay(root.parent_path() / "runtime");
    return result;
}

GameSource import_persistent_media_cache(GameSource original) {
    auto root = persistent_media_cache_directory();
    auto final = root / "v3";
    auto staging = root / "v3.partial";
    auto backup = root / "v3.previous";
    std::set<std::string> names;
    for (auto name : original.file_names()) {
        name = uppercase(name);
        if (media_file(name) && !names.insert(name).second)
            throw std::runtime_error("Ambiguous original-media filename: " + name);
    }
    if (names.size() < 18) throw std::runtime_error("Incomplete original media set");
    std::error_code ec;
    fs::create_directories(root);
    fs::remove_all(staging,ec);
    try {
        fs::create_directories(staging / "media");
        std::ostringstream manifest, decoded_log;
        manifest << kVersion << '\n';
        std::size_t total=0;
        for (const auto& name : names) {
            auto bytes = original.read(name);
            if (bytes.size() > kPerFileLimit || total > kMediaLimit - bytes.size())
                throw std::runtime_error("Original game files exceed cache safety limit");
            total += bytes.size();
            write_file(staging/"media"/name,bytes);
            manifest << name << '\t' << bytes.size() << '\t' << std::hex
                     << std::setw(16) << std::setfill('0') << checksum(bytes) << std::dec << '\n';
            if (name == "LEVELS.DAT") split_levels(bytes, staging/"levels");
            if (name.size()>3 && name.rfind("RES",0)==0 &&
                std::all_of(name.begin()+3,name.end(),[](unsigned char c){return std::isdigit(c)!=0;})) {
                try { decode_res(staging/"decoded",name,bytes,decoded_log); }
                catch (const std::exception& e) {decoded_log << "ARCHIVE_FAILURE\t" << name << '\t' << e.what() << '\n';}
            }
        }
        const auto manifest_text = manifest.str(), report_text=decoded_log.str();
        write_file(staging/"complete.tsv",std::vector<std::uint8_t>(manifest_text.begin(), manifest_text.end()));
        write_file(staging/"decode_report.tsv",std::vector<std::uint8_t>(report_text.begin(), report_text.end()));
        // Validate before swapping in the new installation. A failed reimport
        // must not destroy an older working cache or the independent save overlay.
        auto checked = GameSource::open(staging/"media");
        if (!checked.has_full_game_media())
            throw std::runtime_error("Cached installation incomplete");
        fs::remove_all(backup,ec);
        const bool had_final = fs::exists(final);
        if (had_final) fs::rename(final,backup);
        try {
            fs::rename(staging,final);
        } catch (...) {
            if (had_final) fs::rename(backup,final);
            throw;
        }
        fs::remove_all(backup,ec);
        auto result = GameSource::open(final/"media");
        result.set_runtime_overlay(root/"runtime");
        return result;
    } catch (...) { fs::remove_all(staging,ec); throw; }
}
}
