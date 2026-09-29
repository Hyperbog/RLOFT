#include "games/ravenloft/data/Iso9660Image.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <fstream>
#include <stdexcept>

namespace ravenloft::data {
namespace {

std::uint32_t le32(const std::uint8_t* p) {
    return static_cast<std::uint32_t>(p[0]) |
           (static_cast<std::uint32_t>(p[1]) << 8) |
           (static_cast<std::uint32_t>(p[2]) << 16) |
           (static_cast<std::uint32_t>(p[3]) << 24);
}

std::string upperAscii(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return static_cast<char>(std::toupper(c));
    });
    return s;
}

} // namespace

Iso9660Image::Iso9660Image(std::filesystem::path imagePath)
    : imagePath_(std::move(imagePath)) {
    std::ifstream f(imagePath_, std::ios::binary);
    if (!f) throw std::runtime_error("Could not open ISO image: " + imagePath_.string());

    std::array<std::uint8_t, kSectorSize> sector{};
    bool foundPrimary = false;
    for (std::uint32_t lba = 16; lba < 64; ++lba) {
        f.seekg(static_cast<std::streamoff>(lba) * kSectorSize);
        f.read(reinterpret_cast<char*>(sector.data()), sector.size());
        if (f.gcount() != static_cast<std::streamsize>(sector.size())) break;
        if (std::string(reinterpret_cast<const char*>(sector.data() + 1), 5) != "CD001") continue;
        const auto type = sector[0];
        if (type == 1) {
            const auto* r = sector.data() + 156;
            if (r[0] < 34) throw std::runtime_error("Malformed ISO root directory record");
            root_.name = "/";
            root_.extentLba = le32(r + 2);
            root_.size = le32(r + 10);
            root_.directory = true;
            foundPrimary = true;
            break;
        }
        if (type == 255) break;
    }
    if (!foundPrimary) throw std::runtime_error("game.gog is not a supported ISO-9660 image");
    valid_ = true;
}

std::vector<std::uint8_t> Iso9660Image::readExtent(std::uint32_t lba, std::uint32_t size) const {
    std::ifstream f(imagePath_, std::ios::binary);
    if (!f) throw std::runtime_error("Could not reopen ISO image: " + imagePath_.string());
    f.seekg(static_cast<std::streamoff>(lba) * kSectorSize);
    std::vector<std::uint8_t> out(size);
    if (size) {
        f.read(reinterpret_cast<char*>(out.data()), static_cast<std::streamsize>(size));
        if (f.gcount() != static_cast<std::streamsize>(size))
            throw std::runtime_error("Short read from ISO image");
    }
    return out;
}

std::string Iso9660Image::canonicalName(std::string name) {
    const auto semi = name.find(';');
    if (semi != std::string::npos) name.resize(semi);
    // ISO-9660 represents extensionless DOS filenames such as "RES0." with a
    // trailing period. Preserve a meaningful DOS trailing dot, but normalize
    // the special filename form where ISO contributes an empty extension.
    return upperAscii(name);
}

std::vector<std::string> Iso9660Image::splitPath(std::string path) {
    std::replace(path.begin(), path.end(), '\\', '/');
    std::vector<std::string> parts;
    std::size_t p = 0;
    while (p < path.size()) {
        while (p < path.size() && path[p] == '/') ++p;
        if (p >= path.size()) break;
        auto q = path.find('/', p);
        if (q == std::string::npos) q = path.size();
        auto part = path.substr(p, q - p);
        if (!part.empty() && part != ".") parts.push_back(canonicalName(part));
        p = q + 1;
    }
    return parts;
}

std::vector<Iso9660Image::Entry> Iso9660Image::readDirectory(const Entry& dir) const {
    if (!dir.directory) throw std::runtime_error("ISO entry is not a directory: " + dir.name);
    const auto bytes = readExtent(dir.extentLba, dir.size);
    std::vector<Entry> out;
    std::size_t pos = 0;
    while (pos < bytes.size()) {
        const auto len = bytes[pos];
        if (len == 0) {
            pos = ((pos / kSectorSize) + 1) * kSectorSize;
            continue;
        }
        if (pos + len > bytes.size() || len < 34)
            throw std::runtime_error("Malformed ISO directory record");
        const auto* r = bytes.data() + pos;
        const auto nameLen = r[32];
        if (33u + nameLen > len) throw std::runtime_error("Malformed ISO filename record");
        const auto rawFirst = r[33];
        if (!(nameLen == 1 && (rawFirst == 0 || rawFirst == 1))) {
            std::string raw(reinterpret_cast<const char*>(r + 33), nameLen);
            Entry e;
            e.name = canonicalName(raw);
            e.extentLba = le32(r + 2);
            e.size = le32(r + 10);
            e.directory = (r[25] & 0x02) != 0;
            out.push_back(std::move(e));
        }
        pos += len;
    }
    return out;
}

Iso9660Image::Entry Iso9660Image::find(std::string path) const {
    auto parts = splitPath(std::move(path));
    Entry current = root_;
    for (const auto& wanted : parts) {
        if (!current.directory) throw std::runtime_error("ISO path descends through a file");
        auto entries = readDirectory(current);
        auto it = std::find_if(entries.begin(), entries.end(), [&](const Entry& e) {
            return e.name == wanted ||
                   (!wanted.empty() && wanted.back() == '.' && e.name == wanted.substr(0, wanted.size()-1)) ||
                   (!e.name.empty() && e.name.back() == '.' && e.name.substr(0, e.name.size()-1) == wanted);
        });
        if (it == entries.end()) throw std::runtime_error("ISO path not found: " + wanted);
        current = *it;
    }
    return current;
}

std::vector<Iso9660Image::Entry> Iso9660Image::list(std::string path) const {
    return readDirectory(find(std::move(path)));
}

std::vector<std::uint8_t> Iso9660Image::readFile(std::string path) const {
    const auto e = find(std::move(path));
    if (e.directory) throw std::runtime_error("ISO path is a directory: " + e.name);
    return readExtent(e.extentLba, e.size);
}

void Iso9660Image::extractDirectory(std::string isoPath,
                                    const std::filesystem::path& destination) const {
    const auto dir = find(isoPath);
    if (!dir.directory) throw std::runtime_error("Requested ISO extraction source is not a directory");
    std::filesystem::create_directories(destination);
    for (const auto& e : readDirectory(dir)) {
        if (e.directory) continue; // Ravenloft's RLOFT directory is flat.
        auto filename = e.name;
        // Windows cannot create names ending in '.', while the installed DOS
        // tree conventionally stores RESn without it. Normalize only that case.
        if (!filename.empty() && filename.back() == '.') filename.pop_back();
        const auto outPath = destination / filename;
        std::error_code ec;
        if (std::filesystem::is_regular_file(outPath, ec) &&
            std::filesystem::file_size(outPath, ec) == e.size) {
            continue;
        }
        const auto bytes = readExtent(e.extentLba, e.size);
        std::ofstream out(outPath, std::ios::binary | std::ios::trunc);
        if (!out) throw std::runtime_error("Could not create extracted Ravenloft file: " + outPath.string());
        if (!bytes.empty()) out.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
        if (!out) throw std::runtime_error("Could not write extracted Ravenloft file: " + outPath.string());
    }
}

} // namespace ravenloft::data
