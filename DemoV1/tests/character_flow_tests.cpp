#include "rloft/character_creation_flow.hpp"

#include <cassert>
#include <iostream>

using rloft::CharacterCreationFlow;
using rloft::CharacterCreationState;

int main() {
    // Slot 2/3 are the two state-0 actor cards. The first coordinate is the
    // 400-row axis, the second the 320-column axis.
    assert(CharacterCreationFlow::hit_test(
               CharacterCreationState::ActorChoice, 100, 110) == 3);
    assert(CharacterCreationFlow::hit_test(
               CharacterCreationState::ActorChoice, 100, 170) == 4);

    // 035C is independent of active resources: other geometric slot events
    // can be returned and are ignored by the state-0/state-1 controller loops.
    assert(CharacterCreationFlow::hit_test(
               CharacterCreationState::ActorChoice, 20, 10) == 1);

    // Fixed cancel rectangle is evaluated after normal slots.
    assert(CharacterCreationFlow::hit_test(
               CharacterCreationState::RaceChoice, 300, 280) == 0);

    CharacterCreationFlow flow;
    assert(flow.state() == CharacterCreationState::ActorChoice);
    assert(!flow.apply_event(1));
    assert(flow.apply_event(3));
    assert(flow.draft().actor == 0);
    assert(flow.state() == CharacterCreationState::SexChoice);

    assert(flow.apply_event(4));
    assert(flow.draft().sex == 1);
    assert(flow.state() == CharacterCreationState::RaceChoice);

    assert(flow.apply_event(6));
    assert(flow.draft().race == 5);
    assert(flow.race_committed());

    CharacterCreationFlow back;
    assert(back.apply_event(4));
    assert(back.state() == CharacterCreationState::SexChoice);
    assert(back.apply_event(0));
    assert(back.state() == CharacterCreationState::ActorChoice);
    assert(!back.aborted());
    assert(back.apply_event(0));
    assert(back.aborted());

    std::cout << "character_flow_tests: PASS\n";
}
