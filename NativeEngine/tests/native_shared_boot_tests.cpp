#include "rloft/boot_flow.hpp"
#include <cassert>
#include <stdexcept>
using namespace rloft;

int main() {
    // The real legacy supervisor flow is shared with DemoV1, NOT copied.
    // Normal no-CHARS cold start -> animation/menu -> CODE.1 -> CODE.2.
    BootFlow cold;
    cold.boot();
    assert(cold.snapshot().stage==BootStage::Code2Startup);
    cold.finish_code2_startup(false,false);
    assert(cold.snapshot().stage==BootStage::ColdStartPresentation);
    cold.finish_cold_start_presentation();
    assert(cold.snapshot().stage==BootStage::StartupMenu);
    cold.select_startup_menu(BootFlow::kCharacterWorkflowMenuIndex);
    assert(cold.snapshot().pending_result==LegacyResult::ContinueModuleCycle);
    cold.supervisor_dispatch();
    assert(cold.snapshot().module==LegacyModule::Code1);
    assert(cold.snapshot().stage==BootStage::CharacterCreation);
    cold.finish_character_creation(LegacyResult::ContinueModuleCycle);
    cold.supervisor_dispatch();
    assert(cold.snapshot().module==LegacyModule::Code2);
    assert(cold.snapshot().chars_present);
    cold.finish_code2_startup(true,false);
    assert(cold.snapshot().stage==BootStage::Exploration);
    // Existing-party path must not force character creation or skip
    // into world without an original CHARS file.
    BootFlow existing;
    existing.boot();
    existing.finish_code2_startup(true,false);
    assert(existing.snapshot().stage==BootStage::Exploration);
    assert(existing.snapshot().module==LegacyModule::Code2);
    BootFlow no_chars;
    no_chars.boot();
    no_chars.finish_code2_startup(false,true);
    assert(no_chars.snapshot().stage==BootStage::StartupMenu);
    no_chars.select_startup_menu(3);
    no_chars.supervisor_dispatch();
    assert(no_chars.snapshot().stage==BootStage::Terminated);
    // A phantom supervisor continuation must not mutate the boot state.
    bool refused=false;
    try{cold.supervisor_dispatch();}
    catch(const std::logic_error&){refused=true;}
    assert(refused);
    assert(cold.snapshot().stage==BootStage::Exploration);
}
