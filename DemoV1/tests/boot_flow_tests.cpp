#include "rloft/boot_flow.hpp"

#include <cassert>
#include <iostream>

using rloft::BootFlow;
using rloft::BootStage;
using rloft::LegacyModule;
using rloft::LegacyResult;

static void cold_boot_reaches_character_creation() {
    BootFlow flow;
    flow.boot();
    assert(flow.snapshot().module == LegacyModule::Code2);
    assert(flow.snapshot().stage == BootStage::Code2Startup);

    flow.finish_code2_startup(false, false);
    assert(flow.snapshot().stage == BootStage::ColdStartPresentation);

    flow.finish_cold_start_presentation();
    assert(flow.snapshot().stage == BootStage::StartupMenu);

    flow.select_startup_menu(BootFlow::kCharacterWorkflowMenuIndex);
    assert(flow.snapshot().stage == BootStage::Code2ExitToSupervisor);
    assert(flow.snapshot().pending_result == LegacyResult::ContinueModuleCycle);

    flow.supervisor_dispatch();
    assert(flow.snapshot().module == LegacyModule::Code1);
    assert(flow.snapshot().stage == BootStage::CharacterCreation);
}

static void escape_skips_cold_start_presentation() {
    BootFlow flow;
    flow.boot();
    flow.finish_code2_startup(false, false);
    flow.skip_cold_start_presentation();

    assert(flow.snapshot().presentation_skipped);
    assert(flow.snapshot().stage == BootStage::StartupMenu);
}

static void startup_latch_bypasses_presentation() {
    BootFlow flow;
    flow.boot();
    flow.finish_code2_startup(false, true);

    assert(flow.snapshot().presentation_latch_set);
    assert(flow.snapshot().stage == BootStage::StartupMenu);
}


static void character_creation_status10_returns_to_code2() {
    BootFlow flow;
    flow.boot();
    flow.finish_code2_startup(false, true);
    flow.select_startup_menu(BootFlow::kCharacterWorkflowMenuIndex);
    flow.supervisor_dispatch();

    flow.finish_character_creation(LegacyResult::ContinueModuleCycle);
    assert(flow.snapshot().stage == BootStage::Code1ExitToSupervisor);
    assert(flow.snapshot().pending_result == LegacyResult::ContinueModuleCycle);

    flow.supervisor_dispatch();
    assert(flow.snapshot().module == LegacyModule::Code2);
    assert(flow.snapshot().stage == BootStage::Code2Startup);
    assert(flow.snapshot().chars_present);

    flow.finish_code2_startup(true, false);
    assert(flow.snapshot().module == LegacyModule::Code2);
    assert(flow.snapshot().stage == BootStage::Exploration);
}

static void existing_chars_enters_exploration() {
    BootFlow flow;
    flow.boot();
    flow.finish_code2_startup(true, false);

    assert(flow.snapshot().module == LegacyModule::Code2);
    assert(flow.snapshot().chars_present);
    assert(flow.snapshot().stage == BootStage::Exploration);
}

static void menu_exit_uses_ordinary_status() {
    BootFlow flow;
    flow.boot();
    flow.finish_code2_startup(false, true);
    flow.select_startup_menu(3);

    assert(flow.snapshot().pending_result == LegacyResult::OrdinaryTermination);
    flow.supervisor_dispatch();
    assert(flow.snapshot().stage == BootStage::Terminated);
}

int main() {
    cold_boot_reaches_character_creation();
    escape_skips_cold_start_presentation();
    startup_latch_bypasses_presentation();
    character_creation_status10_returns_to_code2();
    existing_chars_enters_exploration();
    menu_exit_uses_ordinary_status();
    std::cout << "boot_flow_tests: PASS\n";
}
