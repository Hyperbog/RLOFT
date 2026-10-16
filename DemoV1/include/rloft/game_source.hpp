#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <utility>

namespace rloft {

enum class GameSourceKind : std::uint8_t {
    LooseDirectory,
    GogCdImage,
};

struct LocatedGame {
    std::filesystem::path install_root;
    GameSourceKind kind{GameSourceKind::LooseDirectory};
    std::filesystem::path media_path;
};

class Iso9660Image final {
public:
    explicit Iso9660Image(std::filesystem::path image_path);
    ~Iso9660Image();

    Iso9660Image(Iso9660Image&&) noexcept;
    Iso9660Image& operator=(Iso9660Image&&) noexcept;
    Iso9660Image(const Iso9660Image&) = delete;
    Iso9660Image& operator=(const Iso9660Image&) = delete;

    [[nodiscard]] bool contains(std::string_view filename) const;
    [[nodiscard]] std::vector<std::uint8_t> read(std::string_view filename) const;
    [[nodiscard]] const std::filesystem::path& path() const noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

class GameSource final {
public:
    static GameSource open(const std::filesystem::path& path);
    // 'full_game' skips partial DOS installations missing movies,
    // original game logic, scenery or world archive. Leave the default
    // metadata-only discovery available for standalone research tools.
    static std::optional<GameSource> auto_locate(bool full_game=false);
    [[nodiscard]] bool has_full_game_media() const;

    [[nodiscard]] GameSourceKind kind() const noexcept { return located_.kind; }
    [[nodiscard]] const LocatedGame& location() const noexcept { return located_; }

    // Native EXE writes CODE.1 -> CODE.2 CHARS outside original DOS media.
    void set_runtime_overlay(std::filesystem::path root) {
        runtime_overlay_=std::move(root);
    }
    [[nodiscard]] bool contains(std::string_view filename) const;
    [[nodiscard]] std::vector<std::uint8_t> read(std::string_view filename) const;
    [[nodiscard]] std::optional<std::filesystem::path> runtime_file(
        std::string_view filename) const;
    [[nodiscard]] std::optional<std::vector<std::uint8_t>> read_runtime(
        std::string_view filename) const;
    [[nodiscard]] std::filesystem::path runtime_output_path(
        std::string_view filename) const;

private:
    LocatedGame located_{};
    std::unique_ptr<Iso9660Image> iso_{};
    std::optional<std::filesystem::path> runtime_overlay_;
};

[[nodiscard]] std::string_view to_string(GameSourceKind kind) noexcept;

}  // namespace rloft
