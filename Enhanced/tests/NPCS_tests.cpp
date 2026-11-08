#include "rloft/NPCS.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void require(bool ok, const char* message) {
    if (!ok) throw std::runtime_error(message);
}

template <typename Action>
void rejects(Action action) {
    bool rejected = false;
    try { action(); } catch (const std::exception&) { rejected = true; }
    require(rejected, "expected invalid NPCS.DAT input to be rejected");
}

void supplied_file(const std::vector<std::uint8_t>& bytes) {
    const auto original = rloft::decode_npcs_dat(bytes);
    require(bytes.size() == 5731U && original.size() == 83U,
            "unexpected supplied NPCS.DAT geometry");
    require(original[0].name == "ZOMBIE", "original first NPC");
    require(original[1].name == "DOOM GUARD", "original second NPC");
    require(original[8].name == "STRAHD", "original Strahd NPC index");
    require(original[0].hit_dice_count == 4U, "original zombie hit dice");
    require(original[0].armor_class == 8, "original zombie AC");
    require(original[0].experience_award == 270U, "original zombie XP");
    require(rloft::encode_npcs_dat(original) == bytes,
            "semantic decoder/encoder changed the shipped file");
    const auto catalog = rloft::NpcCatalog::parse(bytes);
    require(catalog.size() == 83U && catalog.at(9U).name == "ASSASSIN",
            "gameplay catalog diverges from NPCS codec");
    require(catalog.encode() == bytes, "gameplay catalog round trip differs");
    auto edited_catalog = rloft::NpcCatalog::parse(bytes);
    edited_catalog.edit(9U).experience_award += 1U;
    const auto catalog_bytes = edited_catalog.encode();
    require(catalog_bytes.size() == bytes.size() && catalog_bytes != bytes,
            "catalog semantic edit not applied");
    require(std::equal(catalog_bytes.begin(), catalog_bytes.begin() + 4U + 9U * 69U,
                       bytes.begin()), "catalog edit affected earlier records");
    require(std::equal(catalog_bytes.begin() + 4U + 10U * 69U, catalog_bytes.end(),
                       bytes.begin() + 4U + 10U * 69U),
            "catalog edit affected later records");
    std::size_t nonzero_name_tails = 0U;
    std::size_t combat_definitions = 0U;
    for (const auto& n : original) {
        const auto end = std::find(n.original_name_bytes.begin(),
                                   n.original_name_bytes.end(), 0U);
        require(end != n.original_name_bytes.end(), "unterminated NPC name");
        if (std::any_of(end + 1, n.original_name_bytes.end(),
                        [](std::uint8_t b) { return b != 0; })) {
            ++nonzero_name_tails;
        }
        if (n.hit_dice_count != 0U) ++combat_definitions;
    }
    require(nonzero_name_tails == 20U, "post-NUL source name residues lost");
    require(combat_definitions == 38U, "combat definition count differs");

    // Semantic edits change only the precisely specified bytes. All other
    // records, unknown flags and post-NUL name residue remain byte-identical.
    auto edited = original;
    edited[0].experience_award = 777U;
    edited[0].behavior_flags ^= 0x80000000U;
    edited[0].unresolved_3b = 0xA5U;
    edited[0].name = "ZOM";
    const auto output = rloft::encode_npcs_dat(edited);
    const auto check = rloft::decode_npcs_dat(output);
    require(check[0].name == "ZOM" && check[0].experience_award == 777U,
            "typed edits not reserialized");
    require(check[0].unresolved_3b == 0xA5U,
            "unresolved byte not preserved through edit");
    require(check[0].behavior_flags == edited[0].behavior_flags,
            "complete 32-bit behavior flags not serialized");
    require(std::equal(output.begin() + 4U + 0x45U, output.end(),
                       bytes.begin() + 4U + 0x45U),
            "editing NPC 0 changed unrelated definitions");
    // The short name overwrites only its live prefix and NUL; residue remains.
    require(output[4U + 7U] == bytes[4U + 7U],
            "post-NUL name residue was normalized");
}

void malformed_file_tests() {
    std::vector<std::uint8_t> simple(4U + 0x45U, 0U);
    simple[0] = 1;
    simple[4] = 'A';
    simple[4U + 0x21U] = 0xFDU;
    simple[4U + 0x3BU] = 0x9AU;
    simple[4U + 0x3CU] = 0xFEU;
    simple[4U + 0x16U] = 0xFFU;
    simple[4U + 0x17U] = 0xFFU;
    const auto fields = rloft::decode_npcs_dat(simple);
    require(fields.size() == 1U && fields[0].name == "A", "single record");
    require(fields[0].armor_class == -3, "signed armor class");
    require(fields[0].collision_half_extent == 0xFFFFU,
            "fixed point collision encoding");
    require(fields[0].unresolved_3b == 0x9AU &&
            fields[0].unresolved_3c == 0xFEU, "opaque byte preservation");
    require(rloft::encode_npcs_dat(fields) == simple,
            "synthetic semantic round trip");
    auto bad = simple;
    bad[0] = 2;
    rejects([&] { (void)rloft::decode_npcs_dat(bad); });
    bad = simple; bad.pop_back();
    rejects([&] { (void)rloft::decode_npcs_dat(bad); });
    bad = simple;
    std::fill(bad.begin() + 4U, bad.begin() + 24U, 'X');
    rejects([&] { (void)rloft::decode_npcs_dat(bad); });
    rejects([&] { (void)rloft::decode_npcs_dat({}); });
    auto invalid = fields;
    invalid[0].name = std::string(20U, 'X');
    rejects([&] { (void)rloft::encode_npcs_dat(invalid); });
    invalid[0].name = std::string("A\0B", 3U);
    rejects([&] { (void)rloft::encode_npcs_dat(invalid); });
}
} // namespace

int main(int argc, char** argv) {
    malformed_file_tests();
    if (argc > 2) throw std::runtime_error("usage: NPCS_tests [NPCS.DAT]");
    if (argc == 2) {
        std::ifstream stream(argv[1], std::ios::binary);
        if (!stream) throw std::runtime_error("unable to open original NPCS.DAT");
        const std::vector<std::uint8_t> bytes(
            (std::istreambuf_iterator<char>(stream)),
            std::istreambuf_iterator<char>());
        supplied_file(bytes);
    }
    return 0;
}
