#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/level_continuity.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>

namespace {
rloft::LevelContinuityEntry entry(
    int width, int height, std::uint8_t mark) {
    rloft::LevelContinuityEntry out{};
    out.level.width = width;
    out.level.height = height;
    out.level.cell_words.assign(
        static_cast<std::size_t>(width * height), mark);
    out.level.automap_cache.assign(
        static_cast<std::size_t>(width * height), mark);
    rloft::LiveNpcRecord npc{};
    npc.current_hp = static_cast<std::int16_t>(mark);
    out.world.npcs.push_back(npc);
    rloft::LiveObjectRecord item{};
    item.object_type_id = mark;
    out.world.loose_items.push_back(item);
    return out;
}
} // namespace

int main() {
    rloft::ExplorationSessionState session{};
    session.level_id = 22;
    session.coordinate_a = 0x00048000U;
    session.coordinate_b = 0x00048000U;
    session.facing = 270;
    session.time_of_day_16_16_hours = 0xE0000U;
    session.persistent_event_flags[7] = 0x44U;
    session.party.bytes[123] = 0xABU;
    auto original = entry(8, 8, 22);
    session.level = original.level;
    auto world = original.world;
    rloft::LevelContinuityStore store{};
    const rloft::LevelTransitionLocation to23{
        23, 0x00028000U, 0x00038000U, 90U};

    // A bad event target must be rejected before changing the live
    // world, source page, party, clock or the persistent-level store.
    bool rejected = false;
    try {
        store.enter(session, world,
                    {23, 0x00088000U, 0, 90U}, entry(8, 8, 23));
    } catch (const std::out_of_range&) { rejected = true; }
    assert(rejected);
    rejected = false;
    try {
        store.enter(session, world, to23, entry(8, 8, 23));
        // The regular entry below should only happen once. This branch
        // intentionally exercises the valid first transition.
    } catch (...) { assert(false); }
    assert(session.level_id == 23U);
    assert(session.facing == 90U);
    assert(session.coordinate_a == 0x00028000U);
    assert(session.coordinate_b == 0x00038000U);
    assert(session.party.bytes[123] == 0xABU);
    assert(session.persistent_event_flags[7] == 0x44U);
    assert(session.time_of_day_16_16_hours == 0xE0000U);
    assert(world.npcs.at(0).current_hp == 23);
    assert(store.stored_count() == 1U);

    // Changes to the destination level are carried back on re-entry.
    world.npcs.at(0).current_hp = 5;
    world.loose_items.clear();
    session.level.cell_words[2] = 0xBEEFU;
    session.level.automap_cache[3] = 0x71U;
    auto copy22 = store.recall(22);
    assert(copy22.has_value());
    copy22->world.npcs[0].current_hp = -99;
    assert(store.recall(22)->world.npcs[0].current_hp == 22);

    store.enter(session, world,
                {22, 0x00048000U, 0x00048000U, 270U},
                *store.recall(22));
    assert(session.level_id == 22U);
    assert(world.npcs.at(0).current_hp == 22);
    assert(world.loose_items.size() == 1U);
    assert(session.level.automap_cache.at(3) == 22U);
    assert(store.stored_count() == 1U);

    store.enter(session, world, to23, *store.recall(23));
    assert(world.npcs.at(0).current_hp == 5);
    assert(world.loose_items.empty());
    assert(session.level.cell_words[2] == 0xBEEFU);
    assert(session.level.automap_cache[3] == 0x71U);
    assert(session.party.bytes[123] == 0xABU);
    assert(session.persistent_event_flags[7] == 0x44U);

    const auto snapshot = store.stored_count();
    rejected = false;
    try {
        store.enter(session, world, to23, entry(8, 8, 23));
    } catch (const std::logic_error&) { rejected = true; }
    assert(rejected && store.stored_count() == snapshot);
    rejected = false;
    try {
        store.enter(session, world,
                    {22, 0, 0, 360U}, entry(8, 8, 22));
    } catch (const std::out_of_range&) { rejected = true; }
    assert(rejected && session.level_id == 23U);
    rejected = false;
    try {
        auto bad = entry(8, 8, 22);
        bad.level.cell_words.pop_back();
        store.enter(session, world,
                    {22, 0, 0, 90U}, std::move(bad));
    } catch (const std::runtime_error&) { rejected = true; }
    assert(rejected && world.npcs.at(0).current_hp == 5);
    assert(store.stored_count() == snapshot);
}
