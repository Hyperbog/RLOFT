#include "rloft/character_creation_flow.hpp"

#include <cassert>
#include <iostream>

using rloft::CharacterCreationFlow;
using rloft::CharacterCreationState;

int main() {
    assert(CharacterCreationFlow::hit_test(
               CharacterCreationState::ActorChoice, 100, 110) == 3);
    assert(CharacterCreationFlow::hit_test(
               CharacterCreationState::ActorChoice, 100, 170) == 4);

    // 035C returns geometric slot hits independently of whether the current
    // controller accepts that slot.
    assert(CharacterCreationFlow::hit_test(
               CharacterCreationState::ActorChoice, 20, 10) == 1);

    // Fixed cancel rectangle.
    assert(CharacterCreationFlow::hit_test(
               CharacterCreationState::RaceChoice, 300, 280) == 0);

    // Modes 3 and 5 add the second fixed rectangle at 114h/D7h.
    assert(CharacterCreationFlow::hit_test(
               CharacterCreationState::ClassChoice, 300, 220) == 0xFF);
    assert(CharacterCreationFlow::hit_test(
               CharacterCreationState::PortraitChoice, 300, 220) == 0xFF);

    CharacterCreationFlow flow;
    assert(flow.state() == CharacterCreationState::ActorChoice);
    assert(!flow.apply_event(1));
    assert(flow.apply_event(3));  // actor 0
    assert(flow.draft().actor == 0);
    assert(flow.state() == CharacterCreationState::SexChoice);

    assert(flow.apply_event(4));  // sex 1
    assert(flow.draft().sex == 1);
    assert(flow.state() == CharacterCreationState::RaceChoice);

    assert(flow.apply_event(2));  // race 1
    assert(flow.draft().race == 1);
    assert(flow.state() == CharacterCreationState::ClassChoice);
    {
        const auto a = flow.available_choices();
        assert(a[0] && !a[1] && a[2] && a[3] && a[4] && a[5]);
    }

    assert(flow.apply_event(1));  // first class 0
    assert(flow.class_stage() == 1);
    {
        const auto a = flow.available_choices();
        assert(!a[0] && !a[1] && !a[2] && a[3] && a[4] && a[5]);
    }

    assert(flow.apply_event(4));  // second class 3
    assert(flow.class_stage() == 2);
    {
        const auto a = flow.available_choices();
        assert(!a[0] && !a[1] && !a[2] && !a[3] && a[4] && !a[5]);
    }

    assert(flow.apply_event(5));  // third class 4
    assert(flow.state() == CharacterCreationState::OptionChoice);

    // function_1F24 canonicalizes a full three-class non-class-5 combination.
    assert(flow.draft().classes[0] == 0);
    assert(flow.draft().classes[1] == 4);
    assert(flow.draft().classes[2] == 3);

    {
        const auto a = flow.available_choices();
        for (bool enabled : a) assert(enabled);
    }

    assert(flow.apply_event(6));  // +6812 option 5
    assert(flow.draft().option == 5);
    assert(flow.state() == CharacterCreationState::PortraitChoice);
    assert(flow.portrait_page() == 0);

    assert(flow.apply_event(0xFF));
    assert(flow.portrait_page() == 6);
    assert(flow.apply_event(0xFF));
    assert(flow.portrait_page() == 12);
    {
        const auto a = flow.available_choices();
        assert(a[0] && a[1] && a[2] && a[3] && a[4] && !a[5]);
    }

    assert(!flow.apply_event(6)); // face 17 is deliberately disabled
    assert(flow.apply_event(5));  // face 16
    assert(flow.draft().face == 16);
    assert(flow.state() == CharacterCreationState::StatsReview);

    CharacterCreationFlow reset_classes;
    assert(reset_classes.apply_event(3));
    assert(reset_classes.apply_event(3));
    assert(reset_classes.apply_event(1)); // race 0
    assert(reset_classes.apply_event(1)); // first class
    assert(reset_classes.class_stage() == 1);
    assert(reset_classes.apply_event(0));
    assert(reset_classes.state() == CharacterCreationState::ClassChoice);
    assert(reset_classes.class_stage() == 0);
    assert(reset_classes.draft().classes[0] == -1);

    CharacterCreationFlow back;
    assert(back.apply_event(4));
    assert(back.state() == CharacterCreationState::SexChoice);
    assert(back.apply_event(0));
    assert(back.state() == CharacterCreationState::ActorChoice);
    assert(!back.aborted());
    assert(back.apply_event(0));
    assert(back.aborted());

    // 3138 completion followed by 4978 mode-7 -> mode-0 wrap retains the
    // accepted actor while resetting the working draft.
    CharacterCreationFlow retained;
    assert(retained.apply_event(3));
    assert(retained.apply_event(3));
    assert(retained.apply_event(1));
    assert(retained.apply_event(1));
    assert(retained.state() == CharacterCreationState::OptionChoice);
    assert(retained.apply_event(1));
    assert(retained.apply_event(1));
    retained.finish_current_actor(true, "STRAHD");
    assert(retained.state() == CharacterCreationState::ActorChoice);
    assert(retained.actor_present(0));
    assert(!retained.actor_present(1));
    assert(retained.actor(0).name == "STRAHD");
    assert(retained.actor(0).face == 0);
    assert(retained.draft().actor == -1);

    // Existing actor slots do not restart generation through 1CD0; the host
    // owns the 1BD4 keep/delete submode.
    assert(!retained.apply_event(3));
    assert(retained.state() == CharacterCreationState::ActorChoice);

    retained.resolve_existing_actor(0, true);
    assert(retained.actor_present(0));

    // Start actor 1 with the same sex. At portrait mode, actor 0's face 0 is
    // excluded exactly as function_2A64 specifies.
    assert(retained.apply_event(4));
    assert(retained.apply_event(3));
    assert(retained.apply_event(1));
    assert(retained.apply_event(1));
    assert(retained.state() == CharacterCreationState::OptionChoice);
    assert(retained.apply_event(1));
    assert(retained.state() == CharacterCreationState::PortraitChoice);
    {
        const auto a = retained.available_choices();
        assert(!a[0]);
        assert(a[1]);
    }

    // Back out to actor choice before exercising existing-slot deletion.
    assert(retained.apply_event(0));
    assert(retained.apply_event(0));
    assert(retained.apply_event(0));
    assert(retained.state() == CharacterCreationState::RaceChoice);
    assert(retained.apply_event(0));
    assert(retained.state() == CharacterCreationState::SexChoice);
    assert(retained.apply_event(0));
    assert(retained.state() == CharacterCreationState::ActorChoice);

    retained.resolve_existing_actor(0, false);
    assert(!retained.actor_present(0));

    std::cout << "character_flow_tests: PASS\n";
}
