#include "rloft_native/original_resource_preview.hpp"
#include "rloft_native/cpu_reference_presenter.hpp"
#include <cassert>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;
namespace {
void put32(std::vector<std::uint8_t>& bytes, std::size_t at, std::uint32_t n) {
    bytes[at] = static_cast<std::uint8_t>(n);
    bytes[at + 1] = static_cast<std::uint8_t>(n >> 8);
    bytes[at + 2] = static_cast<std::uint8_t>(n >> 16);
    bytes[at + 3] = static_cast<std::uint8_t>(n >> 24);
}

void write(const fs::path& path, const std::vector<std::uint8_t>& bytes) {
    std::ofstream output(path, std::ios::binary);
    if (!output) throw std::runtime_error("cannot create synthetic media");
    output.write(reinterpret_cast<const char*>(bytes.data()),
                 static_cast<std::streamsize>(bytes.size()));
    if (!output) throw std::runtime_error("cannot finish synthetic media");
}

void exercise() {
    // INIT[0] == 0, so canonical CODE.1/CODE.2 default cursor is RES0[47].
    constexpr int entries = 48;
    const auto table_size = 4U + 4U * entries;
    std::vector<std::uint8_t> archive(table_size, 0U);
    put32(archive, 0, entries);
    for (int i = 0; i < entries; ++i) put32(archive, 4U + 4U * i, table_size);
    // EH(type=1), 8-byte descriptor, count=1, height=2, width=8 pixels,
    // transparent=255, raw uncompressed, no transform, 16 data bytes.
    const std::vector<std::uint8_t> cursor{
        'E','H', 1, 8, 0, 1, 0, 2, 0, 1, 255, 0, 0,
        0, 1, 2, 3, 4, 5, 6, 7,
        7, 6, 5, 4, 3, 2, 1, 0,
    };
    archive.insert(archive.end(), cursor.begin(), cursor.end());
    std::vector<std::uint8_t> colors(1 + 15 * 768, 0);
    colors[0] = 15;
    // Palette12 index1 has red 63 and no green/blue.
    colors[1 + 12 * 768 + 3] = 63;
    const auto directory = fs::temp_directory_path() /
        ("rloft-source-asset-" + std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count()));
    fs::create_directory(directory);
    try {
        write(directory / "RES0", archive);
        write(directory / "INIT", {0, 0});
        write(directory / "T", {1});  // signature marker only
        write(directory / "COLORS", colors);
        const auto source = rloft::GameSource::open(directory);
        const auto preview = rloft_native::make_source_cursor_preview(source);
        assert(preview.original_res0_entry == 47);
        assert(preview.raster_width == 8 && preview.raster_height == 2);
        assert(preview.frame.pixels.size() == 320U * 400U);
        assert(preview.frame.dac6[3] == 63);
        const unsigned top = 199U * 320U + 156U;
        assert(preview.frame.pixels[top] == 0);
        assert(preview.frame.pixels[top + 1] == 1);
        assert(preview.frame.pixels[top + 7] == 7);
        assert(preview.frame.pixels[top + 320U] == 7);
        rloft_native::CpuReferencePresenter cpu;
        cpu.present(preview.frame);
        assert(cpu.rgba().size() == 320U * 400U * 4U);
        assert(cpu.rgba()[(top + 1) * 4U] == 255);
        const auto again = rloft_native::make_source_cursor_preview(source);
        assert(rloft_native::frame_identity(again.frame).pixels_fnv1a64 ==
               rloft_native::frame_identity(preview.frame).pixels_fnv1a64);
        // Wrong COLORS lengths must not silently draw a synthetic cursor.
        colors.pop_back();
        write(directory / "COLORS", colors);
        bool rejected = false;
        try { static_cast<void>(rloft_native::make_source_cursor_preview(source)); }
        catch (const std::exception&) { rejected = true; }
        assert(rejected);
    } catch (...) {
        fs::remove_all(directory);
        throw;
    }
    fs::remove_all(directory);
}
}
int main() { exercise(); }
