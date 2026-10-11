#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

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
    static std::optional<GameSource> auto_locate();

    [[nodiscard]] GameSourceKind kind() const noexcept { return located_.kind; }
    [[nodiscard]] const LocatedGame& location() const noexcept { return located_; }

    [[nodiscard]] bool contains(std::string_view filename) const;
    [[nodiscard]] std::vector<std::uint8_t> read(std::string_view filename) const;
    // Optional, explicit independent writable profile for the standalone
    // native engine. When set, read_runtime and runtime_output_path NEVER
    // fall back to installed CHARS/SAVE.DAT/GAME.OPT or sibling files.
    // DemoV1 never opts into this mode and retains its existing behavior.
    void set_runtime_root(const std::filesystem::path& profile_directory);
    [[nodiscard]] std::optional<std::filesystem::path> runtime_file(
        std::string_view filename) const;
    [[nodiscard]] std::optional<std::vector<std::uint8_t>> read_runtime(
        std::string_view filename) const;
    [[nodiscard]] std::filesystem::path runtime_output_path(
        std::string_view filename) const;

private:
    LocatedGame located_{};
    std::unique_ptr<Iso9660Image> iso_{};
    std::optional<std::filesystem::path> runtime_root_{};
};

[[nodiscard]] std::string_view to_string(GameSourceKind kind) noexcept;

}  // namespace rloft
