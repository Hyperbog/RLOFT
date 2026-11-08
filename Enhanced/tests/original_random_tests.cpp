#include "rloft/original_random.hpp"

#include <cassert>

int main() {
    rloft::OriginalRandom rng;

    // Exact 30CA2 sequence from shipped DGROUP initial state 1.
    assert(rng.next15() == 16838U);
    assert(rng.next15() == 5758U);
    assert(rng.next15() == 10113U);
    assert(rng.next15() == 17515U);

    rng.seed(
        rloft::OriginalRandom::kShippedInitialState);

    // LEVELS.DAT Level 22 ASSASSIN: 4d8 via 1C328. This reproduces the
    // supplied LEV022.DAT live HP word exactly.
    assert(rng.dice(4U, 8U) == 15U);

    rng.seed(1U);
    assert(rng.dice(0U, 8U) == 1U);
    // Original CODE.2 1C33B..1C350 invokes 30CA2 for EACH die,
    // adds the signed per-die argument at 1C34A inside that loop,
    // and applies the final bonus only once at 1C352.
    // Original immutable CODE.2 object1 SHA-256:
    // 014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3
    // Shipped initial RNG next15 sequence: 16838,5758,10113,17515.
    // For 2d6 the source rolls are 4 and 2, so
    // 4+3 + 2+3 -4(final) = 8, NOT 5 (bonus applied once).
    rng.seed(1U);
    assert(rng.dice(2U,6U,3,-4)==8U);
    assert(rng.next15()==10113U); // precisely two RNG draws
    // For 3d4 original rolls are 3,1,2: signed -2 applies
    // THREE times, final +5 only once -> 5.
    rng.seed(1U);
    assert(rng.dice(3U,4U,-2,5)==5U);
    assert(rng.next15()==17515U); // precisely three draws
    // Original 1C356 clamps AFTER all per-die and final adjustments.
    rng.seed(1U);
    assert(rng.dice(1U,8U,5,-12)==1U);
    assert(rng.next15()==5758U);
    // No-dice path consumes no RNG but includes the final bonus.
    rng.seed(1U);
    assert(rng.dice(0U,8U,99,7)==7U);
    assert(rng.next15()==16838U);

    return 0;
}
