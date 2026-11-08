#include "rloft/boot_flow.hpp"

#include <stdexcept>
#include <string>

namespace rloft {

void BootFlow::boot() {
    if (state_.stage != BootStage::SupervisorBoot) {
        invalid_transition("boot", state_.stage);
    }
    state_.module = LegacyModule::Code2;
    state_.stage = BootStage::Code2Startup;
}

void BootFlow::finish_code2_startup(bool chars_present,
                                    bool presentation_latch_set) {
    if (state_.stage != BootStage::Code2Startup) {
        invalid_transition("finish_code2_startup", state_.stage);
    }

    state_.chars_present = chars_present;
    state_.presentation_latch_set = presentation_latch_set;

    // CODE.2 0x204B3: CHARS present takes the existing-party import path.
    // The gameplay runtime begins after that import and the first level load.
    if (chars_present) {
        state_.stage = BootStage::Exploration;
        return;
    }

    // CODE.2 0x19888: byte [F050+0x72] bypasses the staged cold-start
    // presentation and lands at the common startup-menu tail.
    state_.stage = presentation_latch_set ? BootStage::StartupMenu
                                          : BootStage::ColdStartPresentation;
}

void BootFlow::finish_cold_start_presentation() {
    if (state_.stage != BootStage::ColdStartPresentation) {
        invalid_transition("finish_cold_start_presentation", state_.stage);
    }
    state_.stage = BootStage::StartupMenu;
}

void BootFlow::skip_cold_start_presentation() {
    if (state_.stage != BootStage::ColdStartPresentation) {
        invalid_transition("skip_cold_start_presentation", state_.stage);
    }

    // CODE.2 0x19A64/0x19AE8/0x19C23: ESC (0x1B) promotes the local skip
    // state to 1 and bypasses the remaining staged presentation work.
    state_.presentation_skipped = true;
    state_.stage = BootStage::StartupMenu;
}

void BootFlow::select_startup_menu(int index) {
    if (state_.stage != BootStage::StartupMenu) {
        invalid_transition("select_startup_menu", state_.stage);
    }

    // CODE.2 jump table 0x19454 = {194B5,19529,19644,19688}.
    // Entry 2 reaches 0x19684: push 0x0A; jump to 0x205B4.
    if (index == kCharacterWorkflowMenuIndex) {
        state_.pending_result = LegacyResult::ContinueModuleCycle;
        state_.stage = BootStage::Code2ExitToSupervisor;
        return;
    }

    // Entry 3 reaches the same exit helper with status 0. Entries 0 and 1
    // remain live CODE.2 menu paths and are deliberately not relabeled here.
    if (index == 3) {
        state_.pending_result = LegacyResult::OrdinaryTermination;
        state_.stage = BootStage::Code2ExitToSupervisor;
        return;
    }

    if (index < 0 || index > 3) {
        throw std::out_of_range("CODE.2 startup menu index must be 0..3");
    }

    // Menu entries 0/1 return to the menu loop after their own CODE.2 work.
    // Their full visible behavior belongs to the next implementation phase.
}

void BootFlow::finish_character_creation(
    LegacyResult result) {
    if (state_.stage != BootStage::CharacterCreation ||
        state_.module != LegacyModule::Code1) {
        invalid_transition("finish_character_creation", state_.stage);
    }

    if (result != LegacyResult::ContinueModuleCycle &&
        result != LegacyResult::OrdinaryTermination) {
        throw std::logic_error(
            "CODE.1 character controller only returns status 0 or 10");
    }

    state_.pending_result = result;
    state_.stage = BootStage::Code1ExitToSupervisor;
}

void BootFlow::supervisor_dispatch() {
    const auto exit_stage = state_.stage;
    if ((exit_stage != BootStage::Code2ExitToSupervisor &&
         exit_stage != BootStage::Code1ExitToSupervisor) ||
        !state_.pending_result.has_value()) {
        invalid_transition("supervisor_dispatch", state_.stage);
    }

    const auto result = *state_.pending_result;
    state_.pending_result.reset();

    if (result == LegacyResult::ContinueModuleCycle) {
        if (exit_stage == BootStage::Code2ExitToSupervisor) {
            // R.EXE: CODE.2 status 10 -> launch CODE.1.
            state_.module = LegacyModule::Code1;
            state_.stage = BootStage::CharacterCreation;
        } else {
            // R.EXE: CODE.1 status 10 -> launch CODE.2. The normal completed
            // character path has serialized CHARS immediately before exit.
            state_.module = LegacyModule::Code2;
            state_.stage = BootStage::Code2Startup;
            state_.chars_present = true;
        }
        return;
    }

    if (result == LegacyResult::OrdinaryTermination) {
        state_.module = LegacyModule::Supervisor;
        state_.stage = BootStage::Terminated;
        return;
    }

    throw std::logic_error("unexpected supervisor result in demo boot slice");
}

void BootFlow::invalid_transition(std::string_view operation, BootStage stage) {
    throw std::logic_error(std::string(operation) + " is invalid from stage " +
                           std::string(to_string(stage)));
}

std::string_view to_string(LegacyModule module) noexcept {
    switch (module) {
        case LegacyModule::Supervisor: return "R.EXE supervisor";
        case LegacyModule::Code2: return "CODE.2";
        case LegacyModule::Code1: return "CODE.1";
    }
    return "unknown module";
}

std::string_view to_string(BootStage stage) noexcept {
    switch (stage) {
        case BootStage::SupervisorBoot: return "supervisor boot";
        case BootStage::Code2Startup: return "CODE.2 startup";
        case BootStage::ColdStartPresentation: return "cold-start presentation";
        case BootStage::StartupMenu: return "startup menu";
        case BootStage::Code2ExitToSupervisor: return "CODE.2 exit to supervisor";
        case BootStage::CharacterCreation: return "character creation";
        case BootStage::Code1ExitToSupervisor: return "CODE.1 exit to supervisor";
        case BootStage::Exploration: return "exploration runtime";
        case BootStage::Terminated: return "terminated";
    }
    return "unknown stage";
}

}  // namespace rloft
