#include "games/ravenloft/game/Code1CharacterInit3884.h"

#include "games/ravenloft/game/Code1CharacterHitRoll37C0.h"
#include "games/ravenloft/game/Code1CharacterNormalize3404.h"
#include "games/ravenloft/game/Code1CharacterProgress3208.h"
#include "games/ravenloft/game/Code1CharacterRolls334C.h"
#include "games/ravenloft/game/Code1CharacterVital32F4.h"

namespace ravenloft::game {

void Code1CharacterInit3884::initialize(
    Code1CharacterInitState3884& state,
    Code1RuntimeRng618A& rng) noexcept {
    // 0x3887: CODE.1 0x3208.
    Code1CharacterProgressState3208 progress{};
    progress.fields680D = state.fields680D;
    progress.words6818 = state.words6818;
    progress.dwords681E = state.dwords681E;
    Code1CharacterProgress3208::initialize(progress);
    state.words6818 = progress.words6818;
    state.dwords681E = progress.dwords681E;

    // 0x388C: CODE.1 0x32F4.
    Code1CharacterVitalState32F4 vital{};
    vital.field680C = state.field680C;
    vital.word6810 = state.word6810;
    Code1CharacterVital32F4::initialize(vital, rng);
    state.word6810 = vital.word6810;

    // 0x3891: CODE.1 0x334C.
    Code1CharacterRollState334C rolls{};
    rolls.field680C = state.field680C;
    rolls.fields680D = state.fields680D;
    rolls.bytes682A = state.bytes682A;
    Code1CharacterRolls334C::initialize(rolls, rng);
    state.bytes682A = rolls.bytes682A;

    // 0x3896: CODE.1 0x37C0.
    Code1CharacterHitRollState37C0 hit{};
    hit.fields680D = state.fields680D;
    hit.word6816 = state.word6816;
    hit.words6818 = state.words6818;
    hit.bytes682A = state.bytes682A;
    Code1CharacterHitRoll37C0::initialize(hit, rng);
    state.word6816 = hit.word6816;

    // 0x389B: CODE.1 0x3404.
    Code1CharacterNormalizeState3404 normalize{};
    normalize.field680C = state.field680C;
    normalize.fields680D = state.fields680D;
    normalize.field6813 = state.field6813;
    normalize.word6816 = state.word6816;
    normalize.words6818 = state.words6818;
    normalize.bytes682A = state.bytes682A;
    normalize.global7118 = state.global7118;
    normalize.global711C = state.global711C;
    Code1CharacterNormalize3404::apply(normalize);

    state.field6813 = normalize.field6813;
    state.word6816 = normalize.word6816;
    state.bytes682A = normalize.bytes682A;
    state.global7118 = normalize.global7118;
    state.global711C = normalize.global711C;
}

} // namespace ravenloft::game
