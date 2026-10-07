#include "rloft/character_stats.hpp"

#include <cassert>
#include <iostream>

namespace {

rloft::CharacterStatTables tables() {
    rloft::CharacterStatTables t;

    // One level for every class: threshold[0]=0, threshold[1]=1000.
    for (int cls = 0; cls < 6; ++cls) {
        t.progression[static_cast<std::size_t>(cls) * 20U] = 0;
        for (int level = 1; level < 20; ++level) {
            t.progression[
                static_cast<std::size_t>(cls) * 20U +
                static_cast<std::size_t>(level)] = 1000;
        }
    }

    // Race base/random triplets: base 10 + 1d1.
    for (int race = 0; race < 6; ++race) {
        const auto q = static_cast<std::size_t>(race) * 3U;
        t.race_roll[q] = 10;
        t.race_roll[q + 1U] = 1;
        t.race_roll[q + 2U] = 1;
    }

    // Every race permits 3..18 with no modifier.
    for (int race = 0; race < 6; ++race) {
        for (int stat = 0; stat < 6; ++stat) {
            const auto q =
                static_cast<std::size_t>(race) * 12U +
                static_cast<std::size_t>(stat) * 2U;
            t.race_bounds[q] = 3;
            t.race_bounds[q + 1U] = 18;
        }
    }

    // No class minima beyond the race floor.
    t.class_minima.fill(0);

    // All classes use d10 in this controlled test image.
    t.hit_die.fill(10);

    // No constitution/variant HP adjustment.
    t.hp_variant.fill(0);

    // Secondary derived value: index 11 gets -1 to exercise signed lookup.
    t.secondary_bonus.fill(0);
    t.secondary_bonus[11] = -1;

    // Canonical CODE.1 byte 04D4 is even: edits jump to min/max.
    t.step_mode = false;
    return t;
}

rloft::CharacterDraft draft() {
    rloft::CharacterDraft d;
    d.actor = 0;
    d.sex = 0;
    d.race = 0;
    d.classes = {{0, -1, -1}};
    d.option = 0;
    d.face = 0;
    return d;
}

}  // namespace

int main() {
    {
        rloft::CharacterStatsModel model(tables());
        model.seed(0);
        model.generate(draft());

        const auto& s = model.value();
        assert(s.base_6810 == 11);
        assert((s.abilities == std::array<int, 6>{{14, 11, 12, 13, 14, 17}}));
        assert(s.levels[0] == 1);
        assert(s.levels[1] == 0);
        assert(s.levels[2] == 0);
        assert(s.exceptional_strength == 0);
        assert(s.hit_points == 6);
        assert(model.secondary_display_value() == 9);

        assert(model.adjust(draft(), 1, true));
        assert(model.value().abilities[1] == 18);
        assert(model.adjust(draft(), 1, false));
        assert(model.value().abilities[1] == 3);

        assert(model.adjust(draft(), 10, true));
        assert(model.value().hit_points == 10);
        assert(model.adjust(draft(), 10, false));
        assert(model.value().hit_points == 1);
    }

    {
        rloft::CharacterStatsModel model(tables());
        model.seed(12);
        model.generate(draft());

        // Controlled seed reaches exactly 18 STR, then the original
        // function_334C one-percentile roll yields 44.
        assert(model.value().abilities[0] == 18);
        assert(model.value().exceptional_strength == 44);

        assert(model.adjust(draft(), 0, true));
        assert(model.value().abilities[0] == 18);
        assert(model.value().exceptional_strength == 100);

        assert(model.adjust(draft(), 0, false));
        assert(model.value().abilities[0] == 3);
        assert(model.value().exceptional_strength == 0);
    }

    {
        auto t = tables();

        // CODE.1 334C can transiently roll CON above 18 because it stores
        // function_0010(3,6,0,0)+3 before 3404 clamps race bounds. Seed 2
        // reaches raw CON 19. 37C0 must use that address exactly rather than
        // rejecting it as an out-of-range table index.
        t.hp_variant[19] = 7;
        rloft::CharacterStatsModel model(t);
        model.seed(2);
        model.generate(draft());
        assert(model.value().abilities[2] == 18);
        assert(model.value().hit_points >= 1);
    }

    std::cout << "character_stats_tests: PASS\n";
}
