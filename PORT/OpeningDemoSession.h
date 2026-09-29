#pragma once
#include "games/ravenloft/data/LevelDatabase.h"
#include "games/ravenloft/data/InitDatabase.h"
#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/SaveGame.h"
#include "games/ravenloft/data/TrigTable.h"
#include "games/ravenloft/data/WorkLevel.h"
#include "games/ravenloft/game/OpeningSequence.h"
#include "games/ravenloft/game/MistTransition.h"
#include "games/ravenloft/game/CollisionSystem.h"
#include "games/ravenloft/game/PlayerController.h"
#include "games/ravenloft/game/OriginalKeyboardMovement.h"
#include "games/ravenloft/game/OriginalGameplayClock19C80.h"
#include "games/ravenloft/game/OriginalWorldClock1E75C.h"
#include "games/ravenloft/game/NpcScheduler1BDAC.h"
#include "games/ravenloft/game/ObjectSpawn2195C.h"
#include "games/ravenloft/game/NpcDamageE284.h"
#include "games/ravenloft/game/PlayerHandAttack26018.h"
#include "games/ravenloft/game/PlayerHandClick214B8.h"
#include "games/ravenloft/game/PlayerTargetSelectorDB94.h"
#include "games/ravenloft/game/PlayerAttackTimers1E4A0.h"
#include "games/ravenloft/game/OpeningGroundPickup223F4.h"
#include "games/ravenloft/game/OpeningGroundObjectHit223F4.h"
#include "games/ravenloft/game/ObjectRemovalF40CF540.h"
#include "games/ravenloft/game/InventorySlotExchange117A0.h"
#include "games/ravenloft/game/InventoryGridClick11FB0.h"
#include <filesystem>
#include <vector>
#include <memory>
#include <array>
#include <functional>

namespace ravenloft::game {

struct OpeningNpcTickClocks {
    // Exact external timing boundaries read by the original NPC path. The
    // platform loop supplies them; the reconstructed gameplay code does not
    // substitute presentation-frame timing.
    std::function<std::uint32_t()> timer30700;
    std::function<std::uint32_t()> biosTick311E0;
    std::function<std::uint32_t()> currentTick19CC4;
};

// First production vertical-slice state container. It deliberately boots from SSI's
// shipped Quick Start SAVE.DAT + SAVE01/LEV022.DAT pair rather than fabricating state.
class OpeningDemoSession {
public:
    explicit OpeningDemoSession(const std::filesystem::path& rloftRoot, std::uint32_t biosTickSeed311E0=0);

    const data::QuickStartSave& quickStart() const { return save_.quickStart(); }
    const data::WorkLevelSnapshot& workLevel() const { return workLevel_.snapshot(); }
    const data::RuntimePlacement22& assassin() const { return assassin_; }
    const PartyState& party() const { return party_; }
    const std::vector<WorldObject>& groundObjects() const { return groundObjects_; }
    const std::vector<data::RuntimePlacement22>& runtimeGroundObjects() const { return runtimeGroundObjects_; }
    int eventStateBF44() const { return eventStateBF44_; }
    PlayerController::StepResult moveOriginalAxes(std::int32_t turnAxis,std::int32_t forwardBackAxis,std::int32_t strafeAxis);
    PlayerController::StepResult moveOriginalKeyboardKey(std::uint16_t biosKey,const OriginalKeyboardMovement::HeldQuery& held={});
    PlayerController::StepResult originalKeyboardIdleTick();
    const OriginalKeyboardMovement& keyboardMovement() const { return keyboard_; }
    const std::array<data::CharacterRecord,4>& partyRecords() const { return partyRecords_; }
    bool partyDefeated() const { return partyDefeated_; }

    // Original CODE.2 timer/gameplay-clock cluster. The session owns these
    // counters from the shipped SAVE.DAT rather than accepting fabricated
    // combat ticks from the platform layer. 0x30708 is the IRQ0 pulse; the
    // gameplay-frame service preserves 0x1E4A0 before 0x1F1C8/0x1BDAC order.
    const OriginalGameplayClock19C80& gameplayClock() const { return gameplayClock_; }
    const OriginalWorldClock1E75C& worldClock() const { return worldClock_; }
    void pulseOriginalTimerIrq();
    void tickOriginalGameplayFrame();

    // Reconstructs the SAVE.DAT state owned by CODE.2 0x67D4..0x6C55 from
    // the current live opening session while preserving writer blocks that the
    // demo has not mutated. Level-work-file state is intentionally separate.
    data::QuickStartSave snapshotSaveDatState() const;
    std::vector<std::uint8_t> encodeSaveDat() const;
    void writeSaveDat(const std::filesystem::path& path) const;

    // Original player-hand -> NPC path: CODE.2 0x26018 -> 0xE5F0 ->
    // 0xC80C/0x1C328/0xC910 -> 0xE284. The caller supplies the same 0x19CC4
    // gameplay clock that the original attack/cooldown path reads.
    PlayerHandAttackResult26018 attackOpeningAssassin(std::size_t characterIndex,int handIndex,
                                                       const std::function<std::uint32_t()>& currentTick19CC4);
    PlayerHandAttackResult26018 attackOpeningAssassin(std::size_t characterIndex,int handIndex);
    // Production click-1 hand path: CODE.2 0x214B8 -> 0x24478 -> 0x26018.
    // 0x26018 passes target -1 and E5F0 resolves the previous-frame NPC via
    // the converted 0xDB94/0xDACC selector.
    PlayerHandClickResult214B8 attackOpeningHand(std::size_t characterIndex,int handIndex,
                                                  std::uint32_t clickFlags,
                                                  const std::function<std::uint32_t()>& currentTick19CC4);
    PlayerHandClickResult214B8 attackOpeningHand(std::size_t characterIndex,int handIndex,
                                                  std::uint32_t clickFlags);
    void tickPlayerAttackTimers(std::uint32_t currentTick19CC4);

    // Drives the opening level's primary NPC array through the original
    // CODE.2 0x1BDAC scheduler, including 0x1BBD8/0x1B760 and 0xD550.
    void tickNpcScheduler(const OpeningNpcTickClocks& clocks);

    // Development trigger retained for the alpha UI, but it now reaches death through
    // the converted CODE.2 0xE284 -> 0x2195C path instead of fabricating drops.
    void resolveAssassinDeath();

    // Production world-object pickup branch after the 0x223F4 screen-selection
    // boundary. The viewport/UI eventually supplies the selected runtime index.
    OpeningGroundPickupResult223F4 pickupGroundObject(std::size_t objectIndex,std::uint32_t clickFlags=1);
    OpeningGroundPickupResult223F4 pickupGroundObjectAtScreen(std::int32_t screenX,std::int32_t screenY,
                                                               std::uint32_t clickFlags,
                                                               const data::ResourceStore& resources);
    const HeldObjectState1A344& heldObject() const { return heldObject_; }

    // Production opening inventory-grid selection path: CODE.2 0x12E70 /
    // 0x12F9B -> 0x11FB0 -> 0x117A0. Coordinates are original-interface
    // framebuffer coordinates (X 297..396, four stacked 80-pixel character
    // panels). The final HUD renderer can feed these coordinates directly.
    InventoryGridClickResult11FB0 clickInventoryAtOriginalScreen(
        std::int32_t screenX,std::int32_t screenY,std::uint32_t clickFlags=1);

    // Development/test helper retained outside the live input path. It selects
    // the first free carried slot but still transfers through 0x117A0.
    bool stowHeldObjectInFirstFreeCarriedSlot();

    MistTransitionResult acquireGroundObject(std::uint16_t objectId);
    bool tickMistTransition();
    bool mistTransitionActive() const { return static_cast<bool>(mistTransition_); }
    const MistTransition* mistTransition() const { return mistTransition_.get(); }

private:
    data::SaveGame save_;
    data::WorkLevel workLevel_;
    data::LevelDatabase levels_;
    data::TrigTable trig_;
    data::InitDatabase init_;
    data::NpcDatabase npcs_;
    data::ObjectDatabase objects_;
    CollisionSystem collision_;
    PlayerController playerController_;
    OriginalKeyboardMovement keyboard_;
    OriginalGameplayClock19C80 gameplayClock_;
    OriginalWorldClock1E75C worldClock_;
    OpeningSequence opening_;
    data::RuntimePlacement22 assassin_{};
    std::array<data::CharacterRecord,4> partyRecords_{};
    std::array<std::uint8_t,4> characterDirty_{};
    bool partyDefeated_ = false;
    OriginalRng30CA2 gameplayRng_{1};
    PartyState party_{};
    std::vector<WorldObject> groundObjects_;
    std::vector<data::RuntimePlacement22> runtimeGroundObjects_;
    int eventStateBF44_ = 0;
    std::array<std::uint32_t,4> inventoryPanelMode6F90_{};
    HeldObjectState1A344 heldObject_{};
    std::array<std::vector<std::int16_t>,data::kContainerDescriptorCount> containerItems_{};
    std::array<std::uint8_t,data::kContainerDescriptorCount> containerState_{};
    bool mistOwnershipCleanupApplied_ = false;
    bool mistGameplayClockPushed_ = false;
    bool mistGameplayClockRestored_ = false;
    std::unique_ptr<MistTransition> mistTransition_;

    NpcDamageContextE284 makeAssassinDamageContext();

    void spawnGroundObject2195C(int scatterParameter,std::uint16_t objectId,int placementFlagSource,
                                std::int32_t sourceXFixed,std::int32_t sourceYFixed,
                                int scatterFromFacing,int invokeBoundary1A344);
};

} // namespace ravenloft::game
