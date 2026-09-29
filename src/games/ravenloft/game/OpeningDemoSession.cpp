#include "games/ravenloft/game/OpeningDemoSession.h"
#include "games/ravenloft/game/CharacterDefenseEFA0.h"
#include "games/ravenloft/game/HandItemUsability24478.h"
#include "games/ravenloft/game/ContainerWeightF5F8.h"
#include "games/ravenloft/game/CharacterNameLookup14A60.h"
#include "games/ravenloft/game/CellInteractionRuntimeIndex.h"
#include "games/ravenloft/game/CandidateCollisionSystem2D5D0.h"
#include "games/ravenloft/game/ProjectionSystem.h"
#include "games/ravenloft/game/PlayerTargetSelectorDB94.h"
#include "games/ravenloft/game/PlayerHandClick214B8.h"
#include <algorithm>
#include <stdexcept>

namespace ravenloft::game {
namespace {
std::filesystem::path findQuickStartSave(const std::filesystem::path& root) {
    const auto save01 = root / "SAVE01" / "SAVE.DAT";
    if (std::filesystem::exists(save01)) return save01;
    const auto flat = root / "SAVE.DAT";
    if (std::filesystem::exists(flat)) return flat;
    throw std::runtime_error("Quick Start SAVE.DAT not found");
}
std::filesystem::path findWorkLevel22(const std::filesystem::path& root) {
    const auto save01 = root / "SAVE01" / "LEV022.DAT";
    if (std::filesystem::exists(save01)) return save01;
    const auto flat = root / "LEV022.DAT";
    if (std::filesystem::exists(flat)) return flat;
    throw std::runtime_error("Quick Start LEV022.DAT not found");
}
std::uint16_t rawU16(const data::CharacterRecord& c,std::size_t p){
    return static_cast<std::uint16_t>(c.raw[p]) |
           static_cast<std::uint16_t>(static_cast<std::uint16_t>(c.raw[p+1])<<8u);
}
void rawSetU16(data::CharacterRecord& c,std::size_t p,std::uint16_t v){
    c.raw[p]=static_cast<std::uint8_t>(v&0xFFu);
    c.raw[p+1]=static_cast<std::uint8_t>((v>>8u)&0xFFu);
}
}

OpeningDemoSession::OpeningDemoSession(const std::filesystem::path& rloftRoot, std::uint32_t biosTickSeed311E0)
    : save_(findQuickStartSave(rloftRoot)),
      workLevel_(findWorkLevel22(rloftRoot)),
      levels_(rloftRoot / "LEVELS.DAT"),
      trig_(rloftRoot / "SINTAB"),
      init_(rloftRoot / "INIT"),
      npcs_(rloftRoot / "NPCS.DAT"),
      objects_(rloftRoot / "OBJECTS.DAT"),
      collision_(workLevel_.snapshot(),init_),
      playerController_(trig_,collision_),
      keyboard_(),
      gameplayClock_(save_.quickStart().timerF58C,
                     save_.quickStart().gameplayClockA7E4,
                     biosTickSeed311E0, 1u),
      worldClock_(save_.quickStart().worldClock,
                  save_.quickStart().gameplayClockA7E4),
      opening_(trig_) {
    const auto& q = save_.quickStart();
    partyRecords_ = q.party;
    containerItems_ = q.containerItems;
    containerState_ = q.containerState;
    if (q.level != 22) throw std::runtime_error("Shipped Quick Start does not begin on level 22");

    const auto pristine = levels_.load(22);
    const auto& work = workLevel_.snapshot();
    if (work.width != pristine.width || work.height != pristine.height)
        throw std::runtime_error("Quick Start work level dimensions do not match LEVELS.DAT level 22");
    if (work.npcs.size() != 1 || work.npcs.front().type != 9)
        throw std::runtime_error("Quick Start level 22 does not contain the expected assassin");
    assassin_ = work.npcs.front();

    party_.level = q.level;
    party_.position = {q.partyXFixed, q.partyYFixed};
    party_.headingDegrees = q.facingDegrees;
    for (const auto& c : q.party) {
        if (!c.active()) continue;
        for (const auto id : c.carriedItems) {
            if (id != 0xFFFFu) party_.ownedObjects.push_back(id);
        }
    }
}


PlayerController::StepResult OpeningDemoSession::moveOriginalAxes(std::int32_t turnAxis,std::int32_t forwardBackAxis,std::int32_t strafeAxis){
    if(mistTransition_) return {}; // scripted transfer owns party motion once triggered
    return playerController_.tickOriginalAxes(party_,turnAxis,forwardBackAxis,strafeAxis);
}

void OpeningDemoSession::pulseOriginalTimerIrq(){
    gameplayClock_.irq30708();
}

void OpeningDemoSession::tickOriginalGameplayFrame(){
    if(mistTransition_ || partyDefeated_)return;

    // Main gameplay frame ordering recovered at CODE.2 0x201AC and 0x201C5:
    // 0x1E4A0 services attack deadlines before 0x1F1C8 reaches 0x1BDAC.
    PlayerAttackTimers1E4A0::tick(partyRecords_,gameplayClock_.currentTick19CC4(),&characterDirty_);

    OpeningNpcTickClocks clocks{};
    clocks.timer30700=[this](){return gameplayClock_.timer30700();};
    clocks.biosTick311E0=[this](){return gameplayClock_.biosTick311E0();};
    clocks.currentTick19CC4=[this](){return gameplayClock_.currentTick19CC4();};
    tickNpcScheduler(clocks);

    // Main-loop CODE.2 0x1E75C world-clock service. The opening scope only
    // needs the CF4C/BF50 hour path; unrelated hunger/party-time side effects
    // remain outside this demo slice.
    (void)worldClock_.service1E75C(gameplayClock_.currentTick19CC4());
}

data::QuickStartSave OpeningDemoSession::snapshotSaveDatState() const {
    auto snapshot=save_.quickStart();
    snapshot.level=party_.level;
    snapshot.partyXFixed=party_.position.x;
    snapshot.partyYFixed=party_.position.y;
    snapshot.facingDegrees=party_.headingDegrees;
    snapshot.timerF58C=gameplayClock_.timer30700();
    snapshot.gameplayClockA7E4=gameplayClock_.currentTick19CC4();
    snapshot.worldClock=worldClock_.valueCf4c();
    snapshot.heldObjectId=static_cast<std::int16_t>(heldObject_.objectId);
    snapshot.heldObjectState=heldObject_.state;
    snapshot.party=partyRecords_;
    snapshot.containerItems=containerItems_;
    snapshot.containerState=containerState_;
    return snapshot;
}

std::vector<std::uint8_t> OpeningDemoSession::encodeSaveDat() const {
    return data::SaveGame::encode(snapshotSaveDatState());
}

void OpeningDemoSession::writeSaveDat(const std::filesystem::path& path) const {
    data::SaveGame::write(path,snapshotSaveDatState());
}

PlayerController::StepResult OpeningDemoSession::moveOriginalKeyboardKey(std::uint16_t biosKey,const OriginalKeyboardMovement::HeldQuery& held){
    if(mistTransition_) return {};
    if(!keyboard_.keyEvent(biosKey,held)) return {};
    const auto a=keyboard_.axes();
    return playerController_.tickOriginalAxes(party_,a.turn,a.forwardBack,a.strafe);
}

PlayerController::StepResult OpeningDemoSession::originalKeyboardIdleTick(){
    if(mistTransition_) return {};
    keyboard_.idleTick();
    const auto a=keyboard_.axes();
    return playerController_.tickOriginalAxes(party_,a.turn,a.forwardBack,a.strafe);
}

void OpeningDemoSession::tickNpcScheduler(const OpeningNpcTickClocks& clocks) {
    if (mistTransition_ || partyDefeated_) return;
    if (!clocks.timer30700 || !clocks.biosTick311E0 || !clocks.currentTick19CC4)
        throw std::invalid_argument("Opening NPC scheduler requires original timer/BIOS/combat clock boundaries");

    NpcSchedulerContext1BDAC c{};
    c.enabledBF38 = true;
    c.levelWidth = workLevel_.snapshot().width;
    c.trig = &trig_;

    c.decision.party = &partyRecords_;
    c.decision.levelCells = workLevel_.snapshot().cells;
    c.decision.partyXFixed = party_.position.x;
    c.decision.partyYFixed = party_.position.y;
    c.decision.levelWidth = static_cast<std::int16_t>(workLevel_.snapshot().width);
    c.decision.levelHeight = static_cast<std::int16_t>(workLevel_.snapshot().height);
    c.decision.biosTick311E0 = clocks.biosTick311E0;

    c.attack.party = &partyRecords_;
    c.attack.partyXFixed = party_.position.x;
    c.attack.partyYFixed = party_.position.y;
    c.attack.facingDegreesC658 = party_.headingDegrees;
    c.attack.timer30700 = clocks.timer30700;
    c.attack.currentTick19CC4 = clocks.currentTick19CC4;

    c.attack.characterArmorClassEFA0 = [this](std::size_t characterIndex) {
        CharacterDefenseContextEFA0 defense{};
        defense.handUsable24478 = [this](std::size_t ci, int hand) {
            HandItemUsabilityContext24478 hc{};
            hc.party = &partyRecords_;
            hc.global258A8 = 0xFF;
            return HandItemUsability24478::evaluate(ci, hand, objects_, hc);
        };
        defense.containerWeightF5F8 = [this](int containerIndex) {
            return ContainerWeightF5F8::evaluate(containerIndex, objects_, save_.quickStart());
        };
        return CharacterDefenseEFA0::evaluate(characterIndex, partyRecords_.at(characterIndex), objects_, defense);
    };

    // The two currently proven 0x14A60 call sites pass literal addresses in
    // CODE.2 object 2: 0x584="VLADISLAV", 0x578="IRMGARDE".
    const auto nameLookup = [this](int literalOffset) {
        switch (literalOffset) {
        case 0x584: return CharacterNameLookup14A60::find(partyRecords_, "VLADISLAV");
        case 0x578: return CharacterNameLookup14A60::find(partyRecords_, "IRMGARDE");
        default: return -1;
        }
    };
    c.attack.query14A60 = nameLookup;

    c.attack.damageContext.currentTick19CC4 = clocks.currentTick19CC4;
    c.attack.damageContext.cf5aCharacterFlags = &characterDirty_;
    c.attack.damageContext.query14A60 = nameLookup;
    c.attack.damageContext.allPartyDownCBC8 = [this]() { partyDefeated_ = true; };

    // The assassin's original type-9 branch never reaches generic 0x2D5D0:
    // 0x1BBD8 performs its scripted whole-X-cell move first. Keep this as a
    // fail-fast boundary rather than silently substituting collision behavior.
    c.collision2D5D0 = [](std::int32_t, std::int32_t, int) -> int {
        throw std::runtime_error("opening type-9 assassin unexpectedly reached generic 0x2D5D0");
    };

    std::span<data::RuntimePlacement22> actors(&assassin_, 1);
    NpcScheduler1BDAC::execute(actors, npcs_, gameplayRng_, c);
}

PlayerHandAttackResult26018 OpeningDemoSession::attackOpeningAssassin(std::size_t characterIndex,int handIndex,
                                                                         const std::function<std::uint32_t()>& currentTick19CC4){
    if(mistTransition_ || partyDefeated_) return {};
    if(!currentTick19CC4)throw std::invalid_argument("opening player attack requires 0x19CC4 clock boundary");

    PlayerHandAttackContext26018 c{};
    c.melee.characterDirty=&characterDirty_;
    c.melee.currentTick19CC4=currentTick19CC4;
    c.melee.global258A8=0xFF;
    c.melee.hitTest.containerWeightF5F8=[this](int containerIndex){
        return ContainerWeightF5F8::evaluate(containerIndex,objects_,save_.quickStart());
    };
    c.melee.damageE284=makeAssassinDamageContext();
    std::span<data::RuntimePlacement22> actors(&assassin_,1);
    return PlayerHandAttack26018::execute(partyRecords_,characterIndex,handIndex,
                                           actors,npcs_,0,objects_,gameplayRng_,c);
}

PlayerHandAttackResult26018 OpeningDemoSession::attackOpeningAssassin(std::size_t characterIndex,int handIndex){
    return attackOpeningAssassin(characterIndex,handIndex,[this](){return gameplayClock_.currentTick19CC4();});
}

PlayerHandClickResult214B8 OpeningDemoSession::attackOpeningHand(
    std::size_t characterIndex,int handIndex,std::uint32_t clickFlags,
    const std::function<std::uint32_t()>& currentTick19CC4){
    if(mistTransition_ || partyDefeated_)return {};
    if(!currentTick19CC4)throw std::invalid_argument("opening player attack requires 0x19CC4 clock boundary");

    PlayerHandClickContext214B8 hc{};
    hc.usability24478.party=&partyRecords_;
    hc.usability24478.global258A8=0xFF;
    hc.attack26018=[this,characterIndex,handIndex,currentTick19CC4](int requestedTargetNpcIndex){
        PlayerHandAttackContext26018 ac{};
        ac.melee.characterDirty=&characterDirty_;
        ac.melee.currentTick19CC4=currentTick19CC4;
        ac.melee.global258A8=0xFF;
        ac.melee.hitTest.containerWeightF5F8=[this](int containerIndex){
            return ContainerWeightF5F8::evaluate(containerIndex,objects_,save_.quickStart());
        };
        ac.melee.damageE284=makeAssassinDamageContext();

        // CODE.2 0xDB94 reads the previous render buffer. For level 22 there
        // is one NPC; recreate the exact DACC-consumed record fields from the
        // live actor and exact 0x35798 projection. 0x2C7BD leaves the previous
        // buffer's scale word biased by +0x7D00.
        ac.melee.autoTargetDB94=[this](std::size_t ci){
            ProjectionSystem projection(trig_);
            const auto p=projection.project(assassin_.xFixed,assassin_.yFixed,
                                            party_.position.x,party_.position.y,
                                            party_.headingDegrees);
            if(!p.visible)return -1;
            PlayerTargetRecordDACC record{};
            record.screenX=static_cast<std::uint16_t>(p.screenX);
            record.biasedScale=static_cast<std::uint16_t>(p.scale8_8+0x7D00u);
            record.category=0;
            record.actorIndex=0;
            record.sortRank=0;
            const std::array<PlayerTargetRecordDACC,1> records{{record}};
            const std::span<const data::RuntimePlacement22> actors(&assassin_,1);
            return PlayerAutoTargetDB94::select(ci,records,actors);
        };

        std::span<data::RuntimePlacement22> actors(&assassin_,1);
        return PlayerHandAttack26018::execute(partyRecords_,characterIndex,handIndex,
                                               actors,npcs_,requestedTargetNpcIndex,
                                               objects_,gameplayRng_,ac);
    };
    return PlayerHandClick214B8::execute(partyRecords_,characterIndex,handIndex,clickFlags,objects_,hc);
}

PlayerHandClickResult214B8 OpeningDemoSession::attackOpeningHand(
    std::size_t characterIndex,int handIndex,std::uint32_t clickFlags){
    return attackOpeningHand(characterIndex,handIndex,clickFlags,
                             [this](){return gameplayClock_.currentTick19CC4();});
}

void OpeningDemoSession::tickPlayerAttackTimers(std::uint32_t currentTick19CC4){
    PlayerAttackTimers1E4A0::tick(partyRecords_,currentTick19CC4,&characterDirty_);
}

NpcDamageContextE284 OpeningDemoSession::makeAssassinDamageContext(){
    NpcDamageContextE284 dc{};
    dc.spawn2195C=[this](int p,std::uint16_t id,int f,std::int32_t x,std::int32_t y,int scatter,int boundary){
        spawnGroundObject2195C(p,id,f,x,y,scatter,boundary);
    };
    dc.setEventStateBF44=[this](int v){eventStateBF44_=v;};
    dc.specialType258AA=[](std::uint16_t){return false;};
    dc.boundaryE0F8=[](int){return 0;};
    return dc;
}

void OpeningDemoSession::spawnGroundObject2195C(int scatterParameter,std::uint16_t objectId,int placementFlagSource,
                                                  std::int32_t sourceXFixed,std::int32_t sourceYFixed,
                                                  int scatterFromFacing,int invokeBoundary1A344){
    // 0x2195C calls the full 0x2D5D0 classifier with mover id 0x190. Rebuild
    // the loader-derived interaction index from pristine level 22; this path
    // does not mutate interactions for mover 0x190.
    const auto pristine=levels_.load(22);
    CellInteractionRuntimeIndex interactions(pristine);
    CandidateCollisionSystem2D5D0 collision(workLevel_.snapshot(),npcs_,init_,interactions);
    std::uint32_t scanSuppressedF048=0;

    ObjectSpawnContext2195C sc{};
    sc.trig=&trig_;
    sc.levelWidth25619=workLevel_.snapshot().width;
    sc.facingDegreesC658=party_.headingDegrees;
    sc.collision2D5D0=[&](std::int32_t y,std::int32_t x,int mover){
        return collision.evaluate(y,x,mover,party_.position.x,party_.position.y,
                                  static_cast<std::uint16_t>(party_.level),scanSuppressedF048,{});
    };

    const auto r=ObjectSpawn2195C::execute(scatterParameter,objectId,placementFlagSource,
                                            sourceXFixed,sourceYFixed,scatterFromFacing,invokeBoundary1A344,
                                            runtimeGroundObjects_,sc);
    const auto& rec=runtimeGroundObjects_.at(r.index);
    WorldObject view{objectId,{rec.xFixed,rec.yFixed},static_cast<std::uint16_t>(scatterParameter)};
    if(r.index==groundObjects_.size())groundObjects_.push_back(view);
    else if(r.index<groundObjects_.size())groundObjects_[r.index]=view;
}

void OpeningDemoSession::resolveAssassinDeath() {
    if (!groundObjects_.empty()) return;
    auto dc=makeAssassinDamageContext();
    const int lethal=assassin_.hitPoints>0?assassin_.hitPoints:0;
    (void)NpcDamageE284::apply(assassin_,npcs_.at(assassin_.type),0,lethal,1,dc);
}

OpeningGroundPickupResult223F4 OpeningDemoSession::pickupGroundObject(std::size_t objectIndex,std::uint32_t clickFlags) {
    if(mistTransition_ || partyDefeated_ || heldObject_.objectId!=-1)return {};
    if(objectIndex>=runtimeGroundObjects_.size())return {};

    GroundObjectPickupContext21704 pc{};
    pc.ownsObjectF4B0=[this](int id){
        return PartyOwnsObjectF4B0::evaluate(id,heldObject_,partyRecords_,objects_,containerItems_);
    };
    // Quest flag 0x51 is not set by the shipped Quick Start opening state; keep
    // the original query boundary explicit rather than inventing later quest state.
    pc.query1C1DC=[](int){return 0;};
    pc.setEventStateBF44=[this](int v){eventStateBF44_=v;};

    auto r=OpeningGroundPickup223F4::execute(objectIndex,clickFlags,runtimeGroundObjects_,heldObject_,
                                               partyRecords_,objects_,containerItems_,pc);
    if(!r.picked)return r;
    if(objectIndex<groundObjects_.size())groundObjects_.erase(groundObjects_.begin()+static_cast<std::ptrdiff_t>(objectIndex));
    if(!OpeningSequence::hasObject(party_,r.objectId))party_.ownedObjects.push_back(r.objectId);
    if(r.startMistTransition && !mistTransition_){
        // CODE.2 0x21F78..0x21F7F disables the gameplay A7E4 clock through
        // 0x19CCC(0) before the scripted transfer begins.
        gameplayClock_.push19CCC(0u);
        mistGameplayClockPushed_=true;
        mistTransition_=std::make_unique<MistTransition>(party_,worldClock_);
    }
    return r;
}

OpeningGroundPickupResult223F4 OpeningDemoSession::pickupGroundObjectAtScreen(
    std::int32_t screenX,std::int32_t screenY,std::uint32_t clickFlags,
    const data::ResourceStore& resources){
    if(mistTransition_ || partyDefeated_ || heldObject_.objectId!=-1)return {};
    const auto records=OpeningGroundObjectHit223F4::build(runtimeGroundObjects_,objects_,resources,trig_,party_);
    const auto selected=OpeningGroundObjectHit223F4::select(screenX,screenY,clickFlags,records);
    if(!selected)return {};
    return pickupGroundObject(*selected,clickFlags);
}

InventoryGridClickResult11FB0 OpeningDemoSession::clickInventoryAtOriginalScreen(
    std::int32_t screenX,std::int32_t screenY,std::uint32_t clickFlags){
    InventoryGridClickContext11FB0 context{};
    context.panelMode6F90=inventoryPanelMode6F90_;
    return InventoryGridClick11FB0::execute(
        screenX,screenY,clickFlags,heldObject_,partyRecords_,context);
}

bool OpeningDemoSession::stowHeldObjectInFirstFreeCarriedSlot(){
    if(heldObject_.objectId<0)return false;
    for(std::size_t ci=0;ci<partyRecords_.size();++ci){
        const auto& c=partyRecords_[ci];
        if((rawU16(c,0x10)&0x8000u)!=0)continue;
        for(std::size_t slot=0;slot<14;++slot){
            const auto off=0x40+slot*2;
            if(rawU16(c,off)!=0xFFFFu)continue;
            // The UI bridge chooses the slot only. The actual transfer now runs
            // through the original opening-relevant 0x117A0 exchange semantics.
            const auto r=InventorySlotExchange117A0::executeOpeningEmptySlot(
                ci,slot,1,heldObject_,partyRecords_);
            return r.handled;
        }
    }
    return false;
}

MistTransitionResult OpeningDemoSession::acquireGroundObject(std::uint16_t objectId) {
    // Temporary alpha control bridge: unlike the old implementation it enters
    // through the converted 0x223F4 -> 0x21704 production pickup branch. Prior
    // held objects are stowed only because the inventory HUD is not interactive yet.
    if(heldObject_.objectId!=-1 && !stowHeldObjectInFirstFreeCarriedSlot())return {};
    const auto it=std::find_if(runtimeGroundObjects_.begin(),runtimeGroundObjects_.end(),
        [objectId](const data::RuntimePlacement22& o){return o.type==objectId;});
    if(it==runtimeGroundObjects_.end())return {};
    const auto index=static_cast<std::size_t>(std::distance(runtimeGroundObjects_.begin(),it));
    const auto pickup=pickupGroundObject(index,1);
    if(!pickup.picked)return {};
    if(pickup.startMistTransition){
        return {true,23,{static_cast<std::int32_t>(9*65536+0x8000),static_cast<std::int32_t>(58*65536+0x8000)},180};
    }
    (void)stowHeldObjectInFirstFreeCarriedSlot();
    return {};
}

bool OpeningDemoSession::tickMistTransition(){
    if(!mistTransition_) return false;
    const bool done=mistTransition_->tick();
    if(done && !mistOwnershipCleanupApplied_){
        // CODE.2 0x223B5..0x223D6 calls F540 for 135, 136, and 116 in
        // that exact order. Parchment 233 is intentionally retained.
        ObjectRemoveContextF540 rc{};
        for(const int id:{135,136,116})
            (void)ObjectRemoveF540::execute(id,heldObject_,partyRecords_,objects_,containerItems_,containerState_,rc);
        mistOwnershipCleanupApplied_=true;
    }
    if(done && mistGameplayClockPushed_ && !mistGameplayClockRestored_){
        // CODE.2 0x223D9 restores the pre-transition 0x19CCC state only after
        // the three F540 ownership removals have completed.
        gameplayClock_.pop19CF4();
        mistGameplayClockRestored_=true;
    }
    return done;
}

} // namespace ravenloft::game
