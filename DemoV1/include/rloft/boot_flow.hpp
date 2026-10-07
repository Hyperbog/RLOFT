#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

namespace rloft {

enum class LegacyModule : std::uint8_t {
    Supervisor,
    Code2,
    Code1,
};

enum class BootStage : std::uint8_t {
    SupervisorBoot,
    Code2Startup,
    ColdStartPresentation,
    StartupMenu,
    Code2ExitToSupervisor,
    CharacterCreation,
    Code1ExitToSupervisor,
    Terminated,
};

enum class LegacyResult : std::int32_t {
    OrdinaryTermination = 0,
    ContinueModuleCycle = 10,
    RestartWithoutTransferredParty = 12,
};

struct BootSnapshot {
    LegacyModule module{LegacyModule::Supervisor};
    BootStage stage{BootStage::SupervisorBoot};
    std::optional<LegacyResult> pending_result{};
    bool chars_present{false};
    bool presentation_latch_set{false};
    bool presentation_skipped{false};
};

class BootFlow final {
public:
    // Binary-proven CODE.2 startup menu index that exits with legacy status 10.
    static constexpr int kCharacterWorkflowMenuIndex = 2;

    [[nodiscard]] const BootSnapshot& snapshot() const noexcept { return state_; }

    void boot();
    void finish_code2_startup(bool chars_present, bool presentation_latch_set);
    void finish_cold_start_presentation();
    void skip_cold_start_presentation();
    void select_startup_menu(int index);
    void finish_character_creation(LegacyResult result);
    void supervisor_dispatch();

private:
    [[noreturn]] static void invalid_transition(std::string_view operation,
                                                BootStage stage);

    BootSnapshot state_{};
};

[[nodiscard]] std::string_view to_string(LegacyModule module) noexcept;
[[nodiscard]] std::string_view to_string(BootStage stage) noexcept;

}  // namespace rloft
