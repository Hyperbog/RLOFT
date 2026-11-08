#include "rloft/RES4.hpp"
#include "rloft/surface_texture.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
void require(bool value, const char* reason) {
    if (!value) throw std::runtime_error(reason);
}
template <class Fn> void rejects(Fn fn) {
    bool caught = false;
    try { fn(); } catch (const std::exception&) { caught = true; }
    require(caught, "invalid RES4 operation was accepted");
}
void put32(std::vector<std::uint8_t>& bytes, std::size_t pos,
           std::uint32_t number) {
    for (unsigned i = 0; i < 4U; ++i)
        bytes[pos + i] = static_cast<std::uint8_t>(number >> (8U * i));
}
std::vector<std::uint8_t> make_archive(unsigned count) {
    std::vector<std::uint8_t> result(4U + 4U * count, 0U);
    put32(result, 0U, count);
    for (unsigned i = 0; i < count; ++i) {
        put32(result, 4U + 4U * i, static_cast<std::uint32_t>(result.size()));
        const unsigned size = i == 0U ? 4096U : 8192U;
        result.insert(result.end(), {'E', 'H', 6U, 0U, 0U,
                                      static_cast<std::uint8_t>(size),
                                      static_cast<std::uint8_t>(size >> 8U)});
        for (unsigned at = 0; at < size; at += 8U) {
            result.push_back(0xFFU);
            for (unsigned k = 0; k < 8U; ++k)
                result.push_back(static_cast<std::uint8_t>(at + k + i));
        }
    }
    return result;
}
void synthetic() {
    const auto input = make_archive(2U);
    rloft::SurfaceTextureArchive res4(input);
    require(res4.encode() == input && res4.entry_count() == 2,
            "synthetic RES4 round trip");
    auto tex = res4.texture(0);
    require(tex.width == 32 && tex.texel(0,0) == 0U &&
            tex.texel(1,0) == 128U && tex.texel(31,127) == 255U,
            "synthetic column-major texture geometry");
    require(res4.texture(1).width == 64, "second original texture dimensions");
    tex.texels_column_major[130U] = 0x5AU;
    res4.replace_texture(0, tex);
    const auto modified = res4.encode();
    rloft::SurfaceTextureArchive reopened(modified);
    require(reopened.texture(0).texels_column_major == tex.texels_column_major,
            "literal LZSS source did not round-trip");
    const rloft::ResArchive original(input), updated(modified);
    require(original.entry(1) == updated.entry(1), "unmodified source changed");
    rejects([&] { res4.replace_texture(0, rloft::SurfaceTexture{64, {}}); });
    rejects([&] { (void)res4.texture(-1); });
    rejects([&] { (void)res4.texture(2); });
    rejects([&] { (void)res4.texture(0).texel(32, 0); });
    auto bad = input;
    bad[15U] = 1U; // wrong EH6 extended-header length in first record
    rejects([&] { (void)rloft::SurfaceTextureArchive(bad).texture(0); });
    bad = input;
    bad[12U] = 0U;
    rejects([&] { (void)rloft::SurfaceTextureArchive(bad).texture(0); });
    bad = input;
    bad[19U] = 0x00U; // invalid first compressed control
    rejects([&] { (void)rloft::SurfaceTextureArchive(bad).texture(0); });
}
void original(const std::vector<std::uint8_t>& bytes) {
    require(bytes.size() == 1975542U, "unexpected original RES4 file size");
    const rloft::ResArchive original(bytes);
    rloft::SurfaceTextureArchive archive(bytes);
    require(archive.entry_count() == 667 && archive.encode() == bytes,
            "original RES4 file not preserved");
    unsigned small = 0U, large = 0U;
    std::size_t texels = 0U;
    for (int i = 0; i < archive.entry_count(); ++i) {
        const auto info = rloft::inspect_res4_texture(original, i);
        const auto tex = archive.texture(i);
        require(tex.width == info.width && info.height == 128 &&
                tex.texels_column_major.size() == info.decoded_bytes,
                "RES4 decoded texture differs");
        if (info.width == 32) ++small; else if (info.width == 64) ++large;
        texels += tex.texels_column_major.size();
    }
    require(small == 67U && large == 600U && texels == 5189632U,
            "original RES4 texture inventory mismatch");
    for (const int selected : {600, 100}) {
        auto tex = archive.texture(selected);
        tex.texels_column_major[101U] ^= 0xFFU;
        archive.replace_texture(selected, tex);
        require(archive.texture(selected).texels_column_major ==
                tex.texels_column_major, "replacement not decodable");
    }
    const auto modified = archive.encode();
    rloft::SurfaceTextureArchive reopened(modified);
    const rloft::ResArchive edited(modified);
    require(modified != bytes && reopened.entry_count() == 667,
            "changed RES4 archive not encoded");
    for (int i = 0; i < 667; ++i) {
        if (i != 100 && i != 600)
            require(original.entry(i) == edited.entry(i),
                    "editing one texture changed a different compressed record");
        else
            require(archive.texture(i).texels_column_major ==
                    reopened.texture(i).texels_column_major,
                    "changed texture does not round-trip");
    }
}
}  // namespace
int main(int argc, char** argv) {
    synthetic();
    if (argc > 2) throw std::runtime_error("usage: RES4_tests [RES4]");
    if (argc == 2) {
        std::ifstream file(argv[1], std::ios::binary);
        if (!file) throw std::runtime_error("original RES4 unavailable");
        const std::vector<std::uint8_t> bytes(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());
        original(bytes);
    }
}
