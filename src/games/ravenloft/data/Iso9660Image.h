#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace ravenloft::data {

// Minimal ISO-9660 reader used for GOG's game.gog CD image.  It intentionally
// implements only the filesystem features the shipped Ravenloft disc requires:
// 2048-byte logical sectors, the primary volume descriptor, ordinary directory
// records, and versioned ISO filenames (";1").  The original image is never
// modified.
class Iso9660Image {
public:
    struct Entry {
        std::string name;
        std::uint32_t extentLba{};
        std::uint32_t size{};
        bool directory{};
    };

    explicit Iso9660Image(std::filesystem::path imagePath);

    const std::filesystem::path& imagePath() const noexcept { return imagePath_; }
    bool valid() const noexcept { return valid_; }

    Entry find(std::string path) const;
    std::vector<Entry> list(std::string path) const;
    std::vector<std::uint8_t> readFile(std::string path) const;

    // Extract one ISO directory without requiring the player to mount/extract
    // game.gog manually. Existing identical-sized files are left in place.
    void extractDirectory(std::string isoPath,
                          const std::filesystem::path& destination) const;

private:
    static constexpr std::uint32_t kSectorSize = 2048;

    std::filesystem::path imagePath_;
    Entry root_{};
    bool valid_{};

    std::vector<std::uint8_t> readExtent(std::uint32_t lba, std::uint32_t size) const;
    std::vector<Entry> readDirectory(const Entry& dir) const;
    static std::string canonicalName(std::string name);
    static std::vector<std::string> splitPath(std::string path);
};

} // namespace ravenloft::data
