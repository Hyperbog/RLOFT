#include "rloft/RES9.hpp"
#include "rloft/resource_assets.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
void require(bool value, const char* message) {
    if (!value) throw std::runtime_error(message);
}
template <class Callable> void rejects(Callable call) {
    bool rejected = false;
    try { call(); } catch (const std::exception&) { rejected = true; }
    require(rejected, "RES9 incorrectly accepted invalid or stale edit");
}
void append16(std::vector<std::uint8_t>& b, unsigned n) {
    b.push_back(static_cast<std::uint8_t>(n));
    b.push_back(static_cast<std::uint8_t>(n >> 8U));
}
void put32(std::vector<std::uint8_t>& b, std::size_t at, unsigned value) {
    for (unsigned i = 0; i < 4U; ++i)
        b.at(at + i) = static_cast<std::uint8_t>(value >> (8U*i));
}
std::vector<std::uint8_t> raster(bool compressed) {
    std::vector<std::uint8_t> raw{'E','H',1U,8U,0U,1U,0U,1U,0U,1U,0xFEU,
                                   static_cast<std::uint8_t>(compressed ? 3U : 0U),0U};
    if (compressed) raw.push_back(0xFFU); // Eight literal LZSS bytes.
    for (unsigned i = 0; i < 8U; ++i)
        raw.push_back(static_cast<std::uint8_t>(i + 10U));
    return raw;
}
std::vector<std::uint8_t> program(const std::vector<std::uint16_t>& words) {
    std::vector<std::uint8_t> raw;
    append16(raw,static_cast<unsigned>(words.size()));
    for (const auto word : words) append16(raw,word);
    for (std::size_t i = 0; i < words.size(); ++i) append16(raw,0U);
    return raw;
}
std::vector<std::uint8_t> synthetic() {
    std::vector<std::vector<std::uint8_t>> entries{
        raster(true),raster(false),
        program({0x17U,7U,0x19U,239U,0x1BU}),
        program({0x01U,1U,4U,0x1BU,0x1BU})};
    const auto count = entries.size();
    std::vector<std::uint8_t> bytes(4U + count*4U,0U);
    put32(bytes,0U,static_cast<unsigned>(count));
    for (std::size_t i = 0U; i < count; ++i) {
        put32(bytes,4U + i*4U,static_cast<unsigned>(bytes.size()));
        bytes.insert(bytes.end(),entries[i].begin(),entries[i].end());
    }
    return bytes;
}

void exercise(const std::vector<std::uint8_t>& original, bool retail) {
    rloft::Res9Archive archive(original);
    const auto stats = archive.stats();
    require(archive.encode() == original && stats.encoded_bytes == original.size(),
            "original RES9 archive changed on load/encode");
    if (retail) {
        require(original.size() == 8061481U && archive.entry_count() == 555 &&
                archive.image_count() == 517 && archive.program_count() == 38 &&
                stats.raw_images == 71U && stats.lzss_images == 446U &&
                stats.decoded_pixel_bytes == 13212400U &&
                stats.program_words == 11396U && stats.instructions == 4237U &&
                stats.nonzero_companion_words == 0U,
                "uploaded RES9 original archive inventory differs");
        require(archive.program(0).words.size() == 451U &&
                archive.program(24).words.size() == 29U &&
                archive.program(37).words.size() == 313U &&
                archive.instructions(0).front().opcode == 0x27U,
                "uploaded RES9 wordcode program structure differs");
    } else {
        require(archive.entry_count() == 4 && archive.image_count() == 2 &&
                archive.program_count() == 2 && stats.raw_images == 1U &&
                stats.lzss_images == 1U && stats.decoded_pixel_bytes == 16U &&
                stats.program_words == 10U && stats.instructions == 6U &&
                stats.nonzero_companion_words == 0U,
                "synthetic RES9 entry splitting or decoding failed");
    }
    const auto image_count = archive.image_count();
    std::uint64_t total_pixels = 0U;
    for (int i = 0; i < image_count; ++i) {
        const auto desc = archive.raster_info(i);
        const auto raster_pixels = archive.decode_raster(i);
        require(desc.decoded_bytes == raster_pixels.pixels.size() &&
                desc.count == raster_pixels.count &&
                desc.width == raster_pixels.width &&
                desc.height == raster_pixels.height,
                "RES9 raster geometry disagrees with decoded pixels");
        total_pixels += desc.decoded_bytes;
    }
    require(total_pixels == stats.decoded_pixel_bytes,
            "RES9 inventory failed full pixel decode");
    const auto before_first = archive.entry(0);
    const auto before_second = archive.entry(1);
    const auto before_last_image = archive.entry(image_count - 1);
    const auto before_program0 = archive.program(0);
    const auto before_program1 = archive.program(1);
    const auto first_raster = archive.decode_raster(0);
    auto modified_pixels = first_raster.pixels;
    modified_pixels[0U] ^= 1U;
    auto modified_words = before_program0.words;
    require(!archive.instructions(0).front().operands.empty(),
            "RES9 first opcode has no editable operand");
    modified_words[1U] ^= 1U; // Resource/sound base, not an opcode or target.
    archive.apply({{{0,before_first,modified_pixels,{}}},
                   {{0,before_program0.words,modified_words}}});
    const auto committed = archive.encode();
    require(archive.decode_raster(0).pixels == modified_pixels &&
            archive.raster_info(0).compression == 0U &&
            archive.program(0).words == modified_words &&
            archive.program(0).companion_words == before_program0.companion_words &&
            archive.program(1).words == before_program1.words &&
            archive.entry(1) == before_second &&
            archive.entry(image_count - 1) == before_last_image &&
            archive.stats().instructions == stats.instructions,
            "RES9 atomic raster/program edit lost a field or changed other entry");
    rejects([&]{ archive.apply({{{0,before_first,modified_pixels,{}}},{}}); });
    rejects([&]{ archive.apply({{},{{0,before_program0.words,modified_words}}}); });
    rejects([&]{ archive.apply({{{0,archive.entry(0),{1,2,3},{}}},{}}); });
    rejects([&]{ archive.apply({{{0,archive.entry(0),modified_pixels,{}},
                                {0,archive.entry(0),modified_pixels,{}}},{}}); });
    rejects([&]{ archive.apply({{},{{0,modified_words,{0xFFFFU,1U,2U,3U,4U}}}}); });
    rejects([&]{ archive.apply({{},{{0,modified_words,{0x18U,3U,0x19U,239U,0x1BU}}}}); });
    rejects([&]{ archive.apply({{},{{0,modified_words,{0x1BU}}}}); });
    rejects([&]{ archive.apply({{{image_count,{},modified_pixels,{}}},{}}); });
    rejects([&]{ archive.apply({{},{{archive.program_count(),{},modified_words}}}); });
    require(archive.encode() == committed, "rejected edit changed live RES9 bytes");

    // Explicit source-encoded restoration reinstates the exact original
    // compressed entry, not merely equivalent pixels serialized raw.
    archive.apply({{{0,archive.entry(0),{},before_first}},
                   {{0,modified_words,before_program0.words}}});
    require(archive.encode() == original &&
            archive.stats().raw_images == stats.raw_images &&
            archive.stats().lzss_images == stats.lzss_images,
            "RES9 inverse did not restore every source byte");
    archive.apply({});
    require(archive.encode() == original, "empty RES9 edit changed source");
    rejects([&]{(void)archive.program(archive.program_count());});
    rejects([&]{(void)archive.raster_info(-1);});
    auto corrupt = original;
    corrupt.pop_back();
    rejects([&]{(void)rloft::Res9Archive(corrupt);});
    // VM operand and absolute-target validation cannot infer targets from
    // arbitrary words: only complete decoded instruction starts are valid.
    rejects([&]{(void)rloft::disassemble_res9_wordcode({0x02U,0U,1U,2U});});
    rejects([&]{(void)rloft::disassemble_res9_wordcode({0x18U,1U,0x1BU});});
}
} // namespace

int main(int argc, char** argv) {
    exercise(synthetic(),false);
    if (argc > 2) throw std::invalid_argument("usage: RES9_tests [RES9]");
    if (argc == 2) {
        std::ifstream file(argv[1],std::ios::binary);
        if (!file) throw std::runtime_error("cannot open original RES9 file");
        const std::vector<std::uint8_t> bytes{
            std::istreambuf_iterator<char>(file),std::istreambuf_iterator<char>()};
        exercise(bytes,true);
    }
}
