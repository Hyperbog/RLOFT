#pragma once

// Read-only inspection of the original DOS supervisor + DOS/4G LE images.
// This does not execute original machine code or manufacture source semantics.
#include "rloft/game_profile.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string_view>
#include <utility>
#include <vector>

namespace rloft {

enum class OriginalExecutableKind : std::uint8_t { DosMz, BoundLinearExecutable };

struct OriginalLeObject {
    std::uint32_t virtual_size{};
    std::uint32_t base{};
    std::uint32_t flags{};
    std::uint32_t first_page{};
    std::uint32_t page_count{};
};

struct OriginalExecutableLayout {
    OriginalExecutableKind kind{OriginalExecutableKind::DosMz};
    std::uint32_t mz_image_bytes{};
    std::uint32_t mz_entry_file_offset{};
    std::uint16_t mz_relocations{};
    std::uint32_t le_file_offset{};
    std::uint32_t le_entry_object{};
    std::uint32_t le_entry_offset{};
    std::uint32_t le_page_size{};
    std::uint32_t le_pages{};
    std::vector<OriginalLeObject> objects;
};

// Source identities independently checked against the original game ZIP
// inventory SHA-256 records. Hashes are records, not a substitute for a
// byte-by-byte or SHA-256 check at a consumer boundary.
struct OriginalExecutableSpec {
    GameId game;
    std::string_view path;
    std::size_t file_size;
    std::string_view sha256;
    OriginalExecutableKind kind;
    std::uint32_t le_file_offset;
    std::uint32_t code_object_size;
    std::uint32_t code_entry_offset;
};

inline constexpr std::array<OriginalExecutableSpec, 9> kOriginalExecutables{{
    {GameId::StrahdsPossession, "R.EXE", 12368,
     "a6d5874dc91fb068a3ed0a07290547c310ee7f4f50be4d8176c4788f251631b0",
     OriginalExecutableKind::DosMz, 0, 0, 0},
    {GameId::StrahdsPossession, "CODE.1", 277787,
     "fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed",
     OriginalExecutableKind::BoundLinearExecutable, 0x27bdc, 0x11302, 0xcc1c},
    {GameId::StrahdsPossession, "CODE.2", 558689,
     "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314",
     OriginalExecutableKind::BoundLinearExecutable, 0x27bdc, 0x3a852, 0x353a0},
    {GameId::Menzoberranzan, "M.EXE", 12448,
     "45ac5a511b5c645d92cd674459ad033a0c4d05565f85829a64bf2d9c78a9c4a9",
     OriginalExecutableKind::DosMz, 0, 0, 0},
    {GameId::Menzoberranzan, "CODE.1", 294203,
     "905590c145176f93cef1651eb6b34a2253758aea94e1a621205402f0b7e2bda9",
     OriginalExecutableKind::BoundLinearExecutable, 0x27bdc, 70285, 52408},
    {GameId::Menzoberranzan, "CODE.2", 650385,
     "4a4fa725dddea00cb62ff44f44c48999cf6ed4c899b16c21826e8141ec7fda96",
     OriginalExecutableKind::BoundLinearExecutable, 0x27bdc, 274430, 248556},
    {GameId::StoneProphet, "PROPHET/SP.EXE", 12736,
     "abd21528c111cf1bdbcf718d8ed8d0f28851f986f5b9a5f0faa6d8bbbe6db2c1",
     OriginalExecutableKind::DosMz, 0, 0, 0},
    {GameId::StoneProphet, "PROPHET/CODE.1", 313819,
     "004d042a70c620187ebbf3686d63bdbf7e8d2ffc35e1f20ffbac2f7d6126224a",
     OriginalExecutableKind::BoundLinearExecutable, 0x28774, 94328, 69012},
    {GameId::StoneProphet, "PROPHET/CODE.2", 820497,
     "00e225ec8f141a8fdb72ff0547fe93b2ea3aabdb4ec3b59d9261a5268de9203e",
     OriginalExecutableKind::BoundLinearExecutable, 0x28774, 345851, 305620},
}};

[[nodiscard]] inline OriginalExecutableLayout inspect_original_executable(
    const std::vector<std::uint8_t>& source) {
    const auto size = source.size();
    const auto u16 = [&](std::size_t at) -> std::uint16_t {
        if (at > size || size - at < 2) throw std::invalid_argument("truncated DOS word");
        return static_cast<std::uint16_t>(source[at] | (source[at + 1] << 8));
    };
    const auto u32 = [&](std::size_t at) -> std::uint32_t {
        if (at > size || size - at < 4) throw std::invalid_argument("truncated LE dword");
        return std::uint32_t(source[at]) | (std::uint32_t(source[at + 1]) << 8) |
               (std::uint32_t(source[at + 2]) << 16) | (std::uint32_t(source[at + 3]) << 24);
    };
    if (size < 0x40 || source[0] != 'M' || source[1] != 'Z')
        throw std::invalid_argument("not a DOS MZ executable");
    const auto pages = u16(4), final_page_bytes = u16(2);
    const auto mz_size = pages ? (std::uint64_t(pages - 1) * 512 +
                                  (final_page_bytes ? final_page_bytes : 512)) : 0;
    const auto header_size = std::uint64_t(u16(8)) * 16;
    if (mz_size < header_size || mz_size > size || final_page_bytes > 512)
        throw std::invalid_argument("invalid MZ image extent");
    const auto entry = header_size + std::uint64_t(u16(0x16)) * 16 + u16(0x14);
    if (entry >= mz_size) throw std::invalid_argument("MZ entry outside image");

    OriginalExecutableLayout out;
    out.mz_image_bytes = static_cast<std::uint32_t>(mz_size);
    out.mz_entry_file_offset = static_cast<std::uint32_t>(entry);
    out.mz_relocations = u16(6);
    // Locate the bound DOS/4G LE image. In all six original CODE programs,
    // the MZ e_lfanew field is not a valid ordinary PE/LE pointer.
    // Parse the actual LE grammar instead of assuming a fixed binder offset.
    for (std::size_t at = static_cast<std::size_t>(mz_size); at + 0xe0 <= size; ++at) {
        if (source[at] != 'L' || source[at + 1] != 'E' ||
            source[at + 2] != 0 || source[at + 3] != 0) continue;
        if (u32(at + 0x28) != 4096) continue;
        const auto count = u32(at + 0x44);
        const auto table = u32(at + 0x40);
        if (count == 0 || count > 16 || table < 0xc0 || table > 0x10000 ||
            table > size - at || count * 24ULL > size - at - table) continue;
        const auto entry_obj = u32(at + 0x18);
        if (entry_obj == 0 || entry_obj > count) continue;
        std::vector<OriginalLeObject> objects;
        objects.reserve(count);
        std::uint64_t next_page = 1;
        bool valid = true;
        for (std::uint32_t i = 0; i < count; ++i) {
            const auto off = at + table + i * 24;
            const OriginalLeObject object{
                u32(off), u32(off + 4), u32(off + 8),
                u32(off + 12), u32(off + 16)};
            if (object.first_page != next_page || object.page_count == 0 ||
                object.virtual_size == 0) {
                valid = false;
                break;
            }
            next_page += object.page_count;
            objects.push_back(object);
        }
        if (!valid || next_page - 1 != u32(at + 0x14) ||
            u32(at + 0x1c) >= objects[entry_obj - 1].virtual_size) continue;
        if (out.kind == OriginalExecutableKind::BoundLinearExecutable)
            throw std::invalid_argument("multiple plausible LE images");
        out.kind = OriginalExecutableKind::BoundLinearExecutable;
        out.le_file_offset = static_cast<std::uint32_t>(at);
        out.le_entry_object = entry_obj;
        out.le_entry_offset = u32(at + 0x1c);
        out.le_page_size = 4096;
        out.le_pages = u32(at + 0x14);
        out.objects = std::move(objects);
    }
    if (out.kind == OriginalExecutableKind::DosMz && size != mz_size)
        throw std::invalid_argument("unrecognized MZ overlay; not a certified LE image");
    return out;
}

// Structural check only. Callers must also compare SHA-256 before treating an
// original executable as authenticated. Does not imply playable game support.
[[nodiscard]] inline bool matches_original_executable_structure(
    const OriginalExecutableSpec& spec, std::size_t file_size,
    const OriginalExecutableLayout& parsed) noexcept {
    if (file_size != spec.file_size || parsed.kind != spec.kind) return false;
    if (spec.kind == OriginalExecutableKind::DosMz)
        return parsed.mz_image_bytes == file_size;
    return parsed.le_file_offset == spec.le_file_offset &&
           parsed.le_entry_object == 1 &&
           parsed.le_entry_offset == spec.code_entry_offset &&
           !parsed.objects.empty() &&
           parsed.objects[0].virtual_size == spec.code_object_size;
}

} // namespace rloft
