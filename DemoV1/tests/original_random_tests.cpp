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

    return 0;
}
