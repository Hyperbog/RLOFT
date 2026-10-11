#pragma once

// Independent original-asset media reader for a replacement game engine.
// No reference to DemoV1, the original DOS executable, or native save files.
// ISO indexing is eager; asset payload reads are lazy and never write media.
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace rloft_native {

enum class NativeMediaKind { LooseDirectory, GogIso2048, GogIso2352_16, GogIso2352_24 };

struct NativeAssetInfo {
    std::string path;
    std::uint64_t size{};
};

class NativeMedia final {
public:
    static NativeMedia open(const std::filesystem::path& input);
    [[nodiscard]] NativeMediaKind kind() const noexcept { return kind_; }
    [[nodiscard]] const std::filesystem::path& source_path() const noexcept { return source_; }
    [[nodiscard]] bool contains(std::string_view path) const;
    [[nodiscard]] std::vector<std::uint8_t> read(std::string_view path) const;
    [[nodiscard]] std::vector<NativeAssetInfo> inventory() const;
    [[nodiscard]] std::size_t size() const noexcept { return files_.size(); }

private:
    struct File {
        std::filesystem::path loose_path;
        std::uint64_t size{};
        std::uint32_t extent{};
        bool iso{};
    };
    std::map<std::string, File> files_;
    std::filesystem::path source_;
    NativeMediaKind kind_{NativeMediaKind::LooseDirectory};
    std::uint32_t sector_size_{2048};
    std::uint32_t user_offset_{};
    std::uint64_t physical_length_{};

    void index_loose(const std::filesystem::path& root);
    void index_iso(const std::filesystem::path& image);
    void index_iso_directory(std::uint32_t extent, std::uint32_t size,
                             const std::string& prefix, unsigned depth,
                             std::vector<std::uint32_t>& visited);
    [[nodiscard]] std::vector<std::uint8_t> iso_read(
        std::uint32_t extent, std::uint64_t byte_count) const;
};

// Strict ASCII virtual paths: uppercase, slash-separated, no traversal,
// absolute paths, reserved device punctuation, empty components or NUL.
[[nodiscard]] std::string native_asset_key(std::string_view path);

} // namespace rloft_native
