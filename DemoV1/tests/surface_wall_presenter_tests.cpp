#include "rloft/surface_wall_presenter.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace {

void put16(std::vector<std::uint8_t>& b,
           std::size_t o,
           std::uint16_t v) {
    b[o] = static_cast<std::uint8_t>(v);
    b[o + 1U] = static_cast<std::uint8_t>(v >> 8U);
}

void put32(std::vector<std::uint8_t>& b,
           std::size_t o,
           std::uint32_t v) {
    for (int i = 0; i < 4; ++i) {
        b[o + static_cast<std::size_t>(i)] =
            static_cast<std::uint8_t>(v >> (i * 8));
    }
}

std::vector<std::uint8_t> one_texture_archive() {
    constexpr std::size_t decoded_size = 4096U;
    std::vector<std::uint8_t> decoded(decoded_size);
    for (int x = 0; x < 32; ++x) {
        for (int y = 0; y < 128; ++y) {
            decoded[
                static_cast<std::size_t>(x) * 128U +
                static_cast<std::size_t>(y)] =
                static_cast<std::uint8_t>(x * 4 + y);
        }
    }

    std::vector<std::uint8_t> compressed;
    for (std::size_t i = 0; i < decoded.size(); i += 8U) {
        compressed.push_back(0xFF);
        for (std::size_t j = 0; j < 8U; ++j) {
            compressed.push_back(decoded[i + j]);
        }
    }

    std::vector<std::uint8_t> archive(
        8U + 7U + compressed.size(), 0);
    put32(archive, 0, 1);
    put32(archive, 4, 8);

    std::size_t p = 8;
    archive[p++] = 'E';
    archive[p++] = 'H';
    archive[p++] = 6;
    put16(archive, p, 0);
    p += 2U;
    put16(archive, p,
          static_cast<std::uint16_t>(decoded_size));
    p += 2U;
    for (const auto value : compressed) {
        archive[p++] = value;
    }
    return archive;
}

}  // namespace

int main() {
    const auto root =
        std::filesystem::temp_directory_path() /
        "rloft_surface_wall_presenter_test";
    std::filesystem::create_directories(root);
    {
        std::ofstream out(
            root / "RES4",
            std::ios::binary | std::ios::trunc);
        const auto bytes = one_texture_archive();
        out.write(
            reinterpret_cast<const char*>(bytes.data()),
            static_cast<std::streamsize>(bytes.size()));
    }
    // GameSource loose-media discovery requires these canonical marker files;
    // the presenter test intentionally does not read their contents.
    std::ofstream(root / "RES0", std::ios::binary).put('\0');
    std::ofstream(root / "INIT", std::ios::binary).put('\0');
    std::ofstream(root / "T", std::ios::binary).put('\0');

    auto source = rloft::GameSource::open(root);
    rloft::SurfaceWallPresenter presenter(source);
    rloft::IndexedFramebuffer framebuffer;
    framebuffer.clear(0xEE);

    rloft::SurfaceShadeTable shades{};
    shades.packed.fill(0);
    shades.fallback = 0;

    rloft::ViewSweepRun main{};
    main.pass = rloft::SweepRenderPass::Main;
    main.first_ray = 2;
    main.ray_count = 3;
    main.depth = 128; // extent 64
    main.resource_id = 0;
    main.texture_column_offset = 0x40;
    main.texture_column_index = 1;
    main.shade_decrement = 0;

    rloft::ViewSweepRun prepass = main;
    prepass.pass = rloft::SweepRenderPass::Prepass;
    prepass.first_ray = 20;

    presenter.draw_main_runs(
        framebuffer,
        {prepass, main},
        shades);

    const auto& px = framebuffer.pixels();
    auto at = [&](int y, int x) {
        return px[
            static_cast<std::size_t>(y) * 320U +
            static_cast<std::size_t>(x)];
    };

    // extent 64 -> scaler row 104; viewport base row is 4.
    assert(at(108, 34) == 4);
    assert(at(109, 34) == 6);
    assert(at(108, 35) == 4);
    assert(at(108, 36) == 4);

    assert(at(107, 34) == 0xEE);
    assert(at(108, 33) == 0xEE);
    assert(at(108, 37) == 0xEE);

    // Prepass is deliberately untouched by this presenter.
    assert(at(108, 52) == 0xEE);

    std::filesystem::remove_all(root);
}
