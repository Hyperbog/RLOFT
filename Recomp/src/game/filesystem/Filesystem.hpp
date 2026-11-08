#pragma once
// Read-only, fail-closed host transport for DOS INT 21h AH=3Dh/3Fh/3Eh.
// This is a host service component, not an emulation of DOS/4GW or game boot.
// No writes, implicit host-current-directory lookup, or DOS device access.
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <map>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>

namespace rloft_recomp {
struct DosFileResult {
    bool carry;
    std::uint16_t ax;
};
// DOS INT 21h AH=42h: carry-clear returns DX:AX, carry-set AX=error.
struct DosSeekResult {
    bool carry;
    std::uint16_t ax;
    std::uint16_t dx;
    std::uint32_t offset() const noexcept {
        return (static_cast<std::uint32_t>(dx)<<16u)|ax;
    }
};

class ReadOnlyDosFiles {
public:
    explicit ReadOnlyDosFiles(std::filesystem::path installation_root);

    // An exact original CODE.1 CHARS payload may be transferred by the
    // R.EXE supervisor into the next CODE.2 process without changing the
    // original installation. This overlay is intentionally limited to the
    // 0x920-byte CHARS file, never an arbitrary DOS pathname or write API.
    static constexpr std::size_t kCharsBytes=0x920u;
    void install_virtual_chars(const std::uint8_t* bytes,std::size_t count);
    bool has_virtual_chars() const noexcept;

    // Read-only second original-media directory, generally the extracted CD
    // RLOFT. Installation files take priority exactly as DOS working-dir
    // lookups do. Never search an arbitrary parent/current directory.
    void mount_readonly_media(std::filesystem::path cd_root);
    bool has_readonly_media() const noexcept;

    // AL=00h (read-only, compatibility sharing) only. Other modes fail
    // without opening the host file; original callers retain DOS errors.
    DosFileResult open(const std::string& dos_name,std::uint8_t al);

    // Count is the DOS CX word, not an unbounded C++ allocation request.
    DosFileResult read(std::uint16_t handle,std::uint8_t* destination,
                       std::size_t count);

    // Signed CX:DX relative displacement. AL=0 start, 1 current, 2 end.
    // No writes occur; seeking beyond EOF is allowed, just as on DOS files.
    DosSeekResult seek(std::uint16_t handle,std::int32_t displacement,
                       std::uint8_t origin);

    DosFileResult close(std::uint16_t handle);

    std::size_t open_handles() const noexcept;

private:
    static std::string ascii_upper(const std::string& s);
    static bool valid_basename(const std::string& v);
    struct OpenFile {
        std::ifstream stream;
        std::uint32_t cursor;
        bool virtual_chars=false;
    };
    std::filesystem::path root_;
    std::filesystem::path media_root_;
    std::array<std::uint8_t,kCharsBytes> virtual_chars_{};
    bool virtual_chars_active_=false;
    std::map<std::uint16_t,OpenFile> files_;
};
} // namespace rloft_recomp
