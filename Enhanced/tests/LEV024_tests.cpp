#include "rloft/LEV024.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <vector>

namespace {
using Bank = rloft::Lev023ResourceBank;
using Group = rloft::Lev019ControlGroup;

void need(bool condition, const char* why) {
    if (!condition) throw std::runtime_error(why);
}
template<class F> void rejects(F f) {
    bool failed = false;
    try { f(); } catch (const std::exception&) { failed = true; }
    need(failed, "LEV024 malformed patch was accepted");
}
void put16(std::vector<std::uint8_t>& b, std::size_t p, unsigned n) {
    b.at(p) = static_cast<std::uint8_t>(n);
    b.at(p + 1U) = static_cast<std::uint8_t>(n >> 8U);
}
void put32(std::vector<std::uint8_t>& b, std::size_t p, unsigned n) {
    for (unsigned i = 0U; i < 4U; ++i)
        b.at(p + i) = static_cast<std::uint8_t>(n >> (8U*i));
}
std::vector<std::uint8_t> source_notes() {
    return {0x7FU,0x01U,0xADU,0x00U,0x0DU,0x00U,
            'R','Y','S','Z','A','R','D',0x27U,'S',0x0DU,'H','U','T',
            0x22U,0x00U,0x35U,0x00U,0x04U,0x00U,'C','A','V','E'};
}
std::vector<std::uint8_t> synthetic() {
    constexpr std::size_t h = 0x1DU, g = h + 0x2A4U;
    constexpr unsigned w = 4U, ht = 3U;
    constexpr std::size_t b1 = g + 2U * w * ht;
    constexpr std::size_t b2 = b1 + (2U*w + 10U) + 1U + 2U*w;
    constexpr std::size_t b3 = b2 + 22U;
    constexpr std::size_t b4 = b3 + 2U + 22U;
    constexpr std::size_t b5 = b4 + 22U, b6 = b5 + 0x514U;
    constexpr std::array<std::size_t,7> bounds{{h,b1,b2,b3,b4,b5,b6}};
    const auto annotations = source_notes();
    std::vector<std::uint8_t> b(b6 + 4U + w*ht + 4U, 0U);
    b[0] = 0xD7U; // Writer-unowned first byte must remain intact.
    for (std::size_t i = 0U; i < bounds.size(); ++i)
        put32(b, 1U + 4U*i, static_cast<unsigned>(bounds[i]));
    put16(b, h + 1U, w); put16(b, h + 3U, ht);
    put16(b, h + 0x28AU, 1U); // NPC
    put16(b, h + 0x28CU, 1U); // Loose
    put16(b, h + 0x28EU, 1U); // Placed
    b[h + 0x29CU] = 36U;
    put16(b, h + 0x00AU + 2U*16U, 0x00ABU);
    put16(b, h + 0x00AU + 2U*19U, 0x00AAU);
    put16(b, h + 0x10AU + 2U*17U, 0x00ABU);
    put16(b, h + 0x10AU + 2U*18U, 0x00AAU);
    std::size_t p = b1;
    put16(b, p, 0U); p += 2U;
    put16(b, p, 1U); p += 2U;
    b[p] = 8U; b[p + 4U] = 1U; b[p + 5U] = 1U; p += 10U;
    for (unsigned i = 2U; i < w; ++i) { put16(b,p,0U); p += 2U; }
    b[p++] = 0U; // Active-animation byte.
    for (unsigned i = 0U; i < w; ++i) { put16(b,p,0U); p += 2U; }
    need(p == b2, "synthetic LEV024 controls invalid");
    put32(b, b2, 1U << 16U); put32(b, b2 + 4U, 1U << 16U);
    put16(b, b2 + 12U, w + 1U);
    put16(b, b2 + 14U, 21U); put16(b, b2 + 18U, 31U);
    put16(b, b3, 1U);
    put32(b, b3 + 2U, 1U << 16U); put32(b, b3 + 6U, 2U << 16U);
    put16(b, b3 + 14U, w + 2U); put16(b, b3 + 16U, 60U);
    put32(b, b4, 1U << 16U); put32(b, b4 + 4U, 3U << 16U);
    put16(b, b4 + 12U, w + 3U); put16(b, b4 + 14U, 164U);
    put16(b, b6, w); put16(b, b6 + 2U, ht);
    put32(b, b6 + 4U + w*ht, annotations.size());
    b.insert(b.end(), annotations.begin(), annotations.end());
    return b;
}
void exercise(const std::vector<std::uint8_t>& original, bool retail) {
    rloft::Lev024File file(original);
    const auto before = file.summary();
    const auto original_state = file.decode();
    need(file.encode() == original, "original LEV024 bytes changed during loading");
    if (retail) {
        need(original.size() == 19071U && before.width == 64 &&
             before.height == 64 && before.group1_controls == 17U &&
             before.group2_controls == 0U &&
             before.world_counts == std::array<std::size_t,3>{{54U,134U,8U}} &&
             before.active_event_slots == 0U &&
             before.active_animation_count == 0U &&
             before.annotation_bytes == 29U && before.music_sequence == 36U,
             "uploaded Level 24 identity differs");
        need(std::count_if(original_state.cell_words.begin(),
             original_state.cell_words.end(), [](std::uint16_t word) {
                 return (word & 0x0400U) != 0U; }) == 3626 &&
             std::count_if(original_state.cell_words.begin(),
             original_state.cell_words.end(), [](std::uint16_t word) {
                 return (word & 0x0040U) != 0U; }) == 7,
             "original map exploration state differs");
        need(file.npc(0U).npc_type_id() == 21U &&
             file.npc(0U).current_hp() == 31 &&
             file.placed_object(0U).object_type_id() == 60U &&
             file.loose_item(0U).object_type_id() == 164U,
             "original NPC, placed or loose source changed");
    }
    const auto prior_notes = file.notes();
    need(prior_notes.size() == 2U &&
         prior_notes[0U].x == 383 && prior_notes[0U].y == 173 &&
         prior_notes[0U].label == std::vector<std::uint8_t>({
             'R','Y','S','Z','A','R','D','\x27','S','\r','H','U','T'}) &&
         prior_notes[1U].x == 34 && prior_notes[1U].y == 53 &&
         prior_notes[1U].label == std::vector<std::uint8_t>({'C','A','V','E'}),
         "original CP437 annotation content/coordinates changed");
    need(file.resource_word(Bank::PrimaryA,16U) == 0x00ABU &&
         file.resource_word(Bank::PrimaryB,17U) == 0x00ABU,
         "original level resource mappings differ");

    const auto original_npc = file.npc(0U);
    auto updated_npc = original_npc;
    updated_npc.set_current_hp(static_cast<std::int16_t>(original_npc.current_hp() - 1));
    auto amended = prior_notes[0U]; amended.label.push_back('!');
    const rloft::Lev011MapNote appended{12, 34, {'X', '\r', 'Y'}};
    const auto control = file.controls(Group::Primary, retail ? 4 : 1);
    need(control.size() == 1U && control[0U].kind() == 8U,
         "expected original kind-8 control not found");
    auto changed_control = control;
    changed_control[0U].raw[7U] ^= 1U;

    rloft::Lev024Edits changes{};
    changes.resources = {{Bank::PrimaryA,16U,0x00ABU,0x00AAU},
                         {Bank::PrimaryB,17U,0x00ABU,0x00AAU}};
    changes.npcs = {{0U,original_npc,updated_npc}};
    changes.controls = {{Group::Primary,retail?4:1,control,changed_control}};
    changes.notes = {{0U,prior_notes[0U],amended},
                     {2U,std::nullopt,appended}};
    file.apply(changes);
    const auto after = file.decode();
    need(file.encode().size() == original.size() + 10U &&
         file.summary().boundaries == before.boundaries &&
         file.summary().annotation_bytes == before.annotation_bytes + 10U &&
         file.notes().size() == 3U &&
         file.notes()[0U].label == amended.label &&
         file.notes()[2U].label == appended.label &&
         file.resource_word(Bank::PrimaryA,16U) == 0x00AAU &&
         file.resource_word(Bank::PrimaryB,17U) == 0x00AAU &&
         file.npc(0U).raw == updated_npc.raw &&
         file.controls(Group::Primary,retail?4:1)[0U].raw == changed_control[0U].raw &&
         after.cell_words == original_state.cell_words &&
         after.automap_cache == original_state.automap_cache &&
         after.event_scripts == original_state.event_scripts &&
         file.bytes()[0U] == original[0U],
         "multi-domain LEV024 edit modified unrelated source or failed");

    const auto committed = file.encode();
    rejects([&] { file.apply(changes); }); // stale note, NPC and resource
    auto invalid = rloft::Lev024Edits{};
    invalid.notes = {{0U,amended,prior_notes[0U]},
                     {0U,amended,prior_notes[0U]}};
    rejects([&] { file.apply(invalid); });
    invalid.notes = {{0U,std::nullopt,std::nullopt}};
    rejects([&] { file.apply(invalid); });
    invalid.notes = {{999U,std::nullopt,appended}};
    rejects([&] { file.apply(invalid); });
    invalid.notes = {{0U,prior_notes[0U],amended}};
    invalid.npcs = {{0U,updated_npc,original_npc}};
    rejects([&] { file.apply(invalid); }); // valid NPC, stale note -> atomic rollback
    invalid.notes = {{0U,amended,prior_notes[0U]}};
    invalid.notes[0].replacement->label.assign(2047U,'X');
    rejects([&] { file.apply(invalid); }); // CODE.2 note-byte capacity
    need(file.encode() == committed, "rejected patch partially changed state");

    rloft::Lev024Edits undo{};
    undo.resources = {{Bank::PrimaryA,16U,0x00AAU,0x00ABU},
                      {Bank::PrimaryB,17U,0x00AAU,0x00ABU}};
    undo.npcs = {{0U,updated_npc,original_npc}};
    undo.controls = {{Group::Primary,retail?4:1,changed_control,control}};
    undo.notes = {{0U,amended,prior_notes[0U]},
                  {2U,appended,std::nullopt}};
    file.apply(undo);
    need(file.encode() == original, "LEV024 inverse operation is not byte-exact");
    file.apply({});
    need(file.encode() == original, "empty LEV024 patch mutated source");
    auto truncated = original;
    truncated.pop_back();
    rejects([&] { (void)rloft::Lev024File(truncated); });
}
} // namespace

int main(int argc, char** argv) {
    exercise(synthetic(), false);
    if (argc > 2)
        throw std::invalid_argument("usage: LEV024_tests [LEV024.DAT]");
    if (argc == 2) {
        std::ifstream stream(argv[1], std::ios::binary);
        if (!stream) throw std::runtime_error("cannot open original LEV024.DAT");
        const std::vector<std::uint8_t> raw{
            std::istreambuf_iterator<char>(stream),
            std::istreambuf_iterator<char>()};
        exercise(raw, true);
    }
}
