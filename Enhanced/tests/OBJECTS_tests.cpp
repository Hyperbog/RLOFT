#include "rloft/OBJECTS.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
void require(bool success, const char* why) {
    if (!success) throw std::runtime_error(why);
}
template <typename F> void rejects(F&& action) {
    bool failed = false;
    try { action(); } catch (const std::exception&) { failed = true; }
    require(failed, "malformed OBJECTS.DAT accepted");
}
void fixture(const std::vector<std::uint8_t>& bytes) {
    auto objects = rloft::decode_objects_dat(bytes);
    require(bytes.size() == 21844U && objects.size() == 336U, "record geometry");
    require(objects[0].name == "POUCH" && objects[1].name == "SACK", "original names");
    require(rloft::encode_objects_dat(objects) == bytes, "byte-exact round trip");
    auto catalog = rloft::ObjectCatalog::parse(bytes);
    require(catalog.size() == 336U && catalog.encode() == bytes, "catalog round trip");
    std::size_t blanks = 0U;
    for (const auto& o : objects) {
        if (o.name.empty()) ++blanks;
        require(o.item_category < 16U, "item category outside original range");
        require(o.ui_graphic_index < 168U, "RES1 item UI local selector");
        require(o.scene_shape_base_local + o.scene_shape_stride <= 842U,
                "RES3 class-2 bank shape index out of bounds");
    }
    require(blanks == 16U, "blank display-name definitions lost");
    auto modified = objects;
    modified[0].name = "MODDED POUCH";
    modified[0].item_category = 4;
    modified[0].intrinsic_magic_marker = 1;
    modified[0].armor_class_adjustment = -7;
    modified[0].unresolved_2c = 0xAB;
    modified[0].range_long = 77;
    const auto saved = rloft::encode_objects_dat(modified);
    const auto again = rloft::decode_objects_dat(saved);
    require(again[0].name == "MODDED POUCH" && again[0].range_long == 77,
            "edited field not serialized");
    require(again[0].armor_class_adjustment == -7 && again[0].unresolved_2c == 0xAB,
            "signed/opaque edit lost");
    require(std::equal(saved.begin() + 69U, saved.end(), bytes.begin() + 69U),
            "editing first definition damaged subsequent definitions");
    catalog.edit(1).name = "NEW SACK";
    require(rloft::ObjectCatalog::parse(catalog.encode()).at(1).name == "NEW SACK",
            "catalog mutation not serialized");
    rejects([&] { (void)catalog.edit(336); });
    rejects([&] { (void)catalog.at(336); });
}
void synthetic() {
    std::vector<std::uint8_t> data(69, 0);
    data[0] = 1; data[4] = 'X';
    data[4 + 0x31] = 0xFF; data[4 + 0x34] = 0xFD;
    data[4 + 0x35] = 0xDA; data[4 + 0x36] = 0xA5;
    data[4 + 0x2C] = 0xC7; data[4 + 0x40] = 7;
    auto records = rloft::decode_objects_dat(data);
    require(records.size() == 1 && records[0].name == "X", "synthetic parse");
    require(records[0].container_state_index == -1 &&
            records[0].armor_class_adjustment == -3, "signed data");
    require(records[0].raw_word_35 == 0xA5DA &&
            records[0].hand_pair_class() == 2U &&
            records[0].weapon_speed_factor() == 6U, "attack-profile bitfields");
    require(records[0].unresolved_2c == 0xC7 &&
            rloft::encode_objects_dat(records) == data, "opaque storage");
    auto bad = data; bad[0] = 2;
    rejects([&] { (void)rloft::decode_objects_dat(bad); });
    bad = data; bad.pop_back();
    rejects([&] { (void)rloft::decode_objects_dat(bad); });
    bad = data;
    std::fill(bad.begin() + 4, bad.begin() + 44, 'A');
    rejects([&] { (void)rloft::decode_objects_dat(bad); });
    rejects([&] { (void)rloft::decode_objects_dat({}); });
    records[0].name = std::string(40, 'A');
    rejects([&] { (void)rloft::encode_objects_dat(records); });
    records[0].name = "X";
    records[0].raw_byte_35_low2 = 1;
    rejects([&] { (void)rloft::encode_objects_dat(records); });
}
} // namespace
int main(int argc, char** argv) {
    synthetic();
    if (argc > 2) throw std::runtime_error("usage: OBJECTS_tests [OBJECTS.DAT]");
    if (argc == 2) {
        std::ifstream input(argv[1], std::ios::binary);
        if (!input) throw std::runtime_error("can't open OBJECTS.DAT fixture");
        const std::vector<std::uint8_t> data(
            (std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
        fixture(data);
    }
}
