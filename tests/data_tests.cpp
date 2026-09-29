#include "games/ravenloft/data/LevelDatabase.h"
#include <iostream>
#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/InitDatabase.h"
#include "games/ravenloft/data/TextDatabase.h"
#include "games/ravenloft/data/PaletteDatabase.h"
#include "games/ravenloft/data/SaveGame.h"
#include "games/ravenloft/data/ResourceStore.h"
#include "games/ravenloft/data/Iso9660Image.h"
#include "games/ravenloft/data/RavenloftDataSource.h"
#include "games/ravenloft/game/OpeningDemoSession.h"
#include "games/ravenloft/game/CinematicSession.h"
#include "games/ravenloft/game/AutomapState.h"
#include "games/ravenloft/game/AutomapGlyphSelector.h"
#include "games/ravenloft/game/AutomapView.h"
#include "games/ravenloft/game/OpeningDemoFlow.h"
#include "games/ravenloft/game/WorldScene.h"
#include "games/ravenloft/game/CollisionSystem.h"
#include "games/ravenloft/game/SpriteViewSelector.h"
#include "games/ravenloft/game/ProjectionSystem.h"
#include "games/ravenloft/game/SceneProjector.h"
#include "games/ravenloft/game/SpriteRasterGeometry.h"
#include "games/ravenloft/game/OriginalRenderQueue.h"
#include "games/ravenloft/game/SpriteRasterizer.h"
#include "games/ravenloft/game/SkyRenderer.h"
#include "games/ravenloft/game/FloorPlaneRenderer.h"
#include "games/ravenloft/game/LightingSystem.h"
#include "games/ravenloft/game/WorldRenderer.h"
#include "games/ravenloft/game/PlayerController.h"
#include "games/ravenloft/game/OriginalKeyboardMovement.h"
#include "games/ravenloft/game/AdventureHudModel.h"
#include "games/ravenloft/game/AdventureInterfaceTheme.h"
#include "games/ravenloft/game/AdventureScreenRenderer.h"
#include "games/ravenloft/game/CellInteractionSystem.h"
#include "games/ravenloft/game/CellInteractionRuntimeIndex.h"
#include "games/ravenloft/game/CellInteractionOccupancySystem.h"
#include "games/ravenloft/game/CellInteractionScheduler.h"
#include "games/ravenloft/game/CellInteractionSoundSystem.h"
#include "games/ravenloft/game/NpcCellInteractionSystem.h"
#include "games/ravenloft/game/NpcCandidateCellScanSystem.h"
#include "games/ravenloft/game/NpcCandidateCollisionSystem.h"
#include "games/ravenloft/game/SceneryCollisionSystem.h"
#include "games/ravenloft/game/NpcOverlapSystem.h"
#include "games/ravenloft/game/FilteredNpcOverlapSystem.h"
#include "games/ravenloft/game/NpcTypeFilter2ACDC.h"
#include "games/ravenloft/game/PartyOverlapSystem.h"
#include "games/ravenloft/game/CandidateCollisionTailSystem.h"
#include "games/ravenloft/game/CandidateCollisionSystem2D5D0.h"
#include "games/ravenloft/game/PortalCollisionTransition2D3D8.h"
#include "games/ravenloft/game/PlayerCollisionTrigger2D004.h"
#include "games/ravenloft/game/MovementVector2DF2C.h"
#include "games/ravenloft/game/NpcMovementTransaction1BBD8.h"
#include "games/ravenloft/game/OriginalRng30CA2.h"
#include "games/ravenloft/game/OriginalGameplayClock19C80.h"
#include "games/ravenloft/game/OriginalWorldClock1E75C.h"
#include "games/ravenloft/game/NpcDirectionSelector1AF90.h"
#include "games/ravenloft/game/PartyNpcPredicate1B6EC.h"
#include "games/ravenloft/game/NpcDecision1B760.h"
#include "games/ravenloft/game/NpcTargetExclusionD4E8.h"
#include "games/ravenloft/game/NpcHitTestC9A0.h"
#include "games/ravenloft/game/CharacterDefenseHelpers.h"
#include "games/ravenloft/game/ContainerWeightF5F8.h"
#include "games/ravenloft/game/CharacterDefenseEFA0.h"
#include "games/ravenloft/game/HandItemPredicates.h"
#include "games/ravenloft/game/HandItemClassifier240A8.h"
#include "games/ravenloft/game/HandItemUsability24478.h"
#include "games/ravenloft/game/CharacterDamageCD44.h"
#include "games/ravenloft/game/CharacterNameLookup14A60.h"
#include "games/ravenloft/game/OriginalDice1C328.h"
#include "games/ravenloft/game/NpcAttackDispatcherD550.h"
#include "games/ravenloft/game/NpcScheduler1BDAC.h"
#include "games/ravenloft/game/ObjectSpawn2195C.h"
#include "games/ravenloft/game/NpcDamageE284.h"
#include "games/ravenloft/game/PlayerHitTestC80C.h"
#include "games/ravenloft/game/PlayerDamageModifierC910.h"
#include "games/ravenloft/game/PlayerAttackStateE578.h"
#include "games/ravenloft/game/PlayerAttackCooldown24B48.h"
#include "games/ravenloft/game/PlayerAttackTimers1E4A0.h"
#include "games/ravenloft/game/PlayerNpcAttackE5F0.h"
#include "games/ravenloft/game/PlayerHandAttack26018.h"
#include "games/ravenloft/game/PlayerTargetSelectorDB94.h"
#include "games/ravenloft/game/PlayerHandClick214B8.h"
#include "games/ravenloft/game/ObjectOwnershipF390F4B0.h"
#include "games/ravenloft/game/ObjectRemovalF40CF540.h"
#include "games/ravenloft/game/InventorySlotExchange117A0.h"
#include "games/ravenloft/game/InventoryGridClick11FB0.h"
#include "games/ravenloft/game/GroundObjectPickup21704.h"
#include "games/ravenloft/game/OpeningGroundPickup223F4.h"
#include "games/ravenloft/game/OpeningGroundObjectHit223F4.h"
#include "games/ravenloft/data/TrigTable.h"
#include "engine/render/IndexedFrameBuffer.h"
#include "engine/core/FileSystem.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
using namespace ravenloft::data;
using namespace ravenloft::game;

int main(){

    {
        // CODE.2 0x30700/0x30708 + 0x19C80..0x19D15 exact clock cluster.
        OriginalGameplayClock19C80 clock(100u,200u,300u,1u);
        if(OriginalGameplayClock19C80::kOriginalTimerPeriodMicroseconds!=54925u)return 455;
        if(clock.timer30700()!=100u||clock.currentTick19CC4()!=200u||clock.biosTick311E0()!=300u)return 456;
        clock.irq30708();
        if(clock.timer30700()!=101u||clock.currentTick19CC4()!=201u||clock.biosTick311E0()!=301u)return 457;
        clock.push19CCC(0u);
        clock.irq30708();
        if(clock.timer30700()!=102u||clock.currentTick19CC4()!=201u||clock.biosTick311E0()!=302u)return 458;
        clock.pop19CF4();
        if(clock.enabledA7E8()!=1u||clock.stackDepthA7EC()!=0)return 459;

        // 19CCC stores only the low byte of the previous A7E8 value.
        OriginalGameplayClock19C80 lowByte(0u,0u,0u,0x12345678u);
        lowByte.push19CCC(0xABCDEF01u);
        lowByte.pop19CF4();
        if(lowByte.enabledA7E8()!=0x78u)return 460;

        // The original stack has 14 BYTE slots; a fifteenth push is ignored.
        OriginalGameplayClock19C80 stack;
        for(int i=0;i<14;++i)stack.push19CCC(static_cast<std::uint32_t>(i+2));
        if(stack.stackDepthA7EC()!=14u||stack.enabledA7E8()!=15u)return 461;
        stack.push19CCC(99u);
        if(stack.stackDepthA7EC()!=14u||stack.enabledA7E8()!=15u)return 462;
    }
    {
        // CODE.2 0x1E75C + 0x31F94 opening-relevant world clock.
        OriginalWorldClock1E75C world(0x000E1234u,1000u);
        if(world.valueCf4c()!=0x000E1234u||world.hourHighWord()!=14u)return 523;
        if(world.service1E75C(2079u)!=0u||world.valueCf4c()!=0x000E1234u)return 524;
        if(world.service1E75C(2080u)!=1u||world.valueCf4c()!=0x000F1234u||
           world.baselineBf50()!=2080u)return 525;
        if(world.service1E75C(4240u)!=2u||world.valueCf4c()!=0x00111234u)return 526;

        // 0x31F94 preserves the low word, maps high-word FFFF to 23, and
        // otherwise normalizes the hour modulo 24.
        if(OriginalWorldClock1E75C::normalize31F94(0xFFFF5678u)!=0x00175678u)return 527;
        OriginalWorldClock1E75C wrap(0x00175678u,0u);
        wrap.incrementHourAndNormalize31F94();
        if(wrap.valueCf4c()!=0x00005678u)return 528;

        // Quick Start hour 14 requires exactly twelve 0x222CD increments to
        // reach hour 2; the following status check succeeds without mutation.
        OriginalWorldClock1E75C mistClock(0x000E0000u,0u);
        for(int i=0;i<12;++i)if(mistClock.mistWaitStep222C0())return 529;
        if(mistClock.hourHighWord()!=2u||!mistClock.mistWaitStep222C0())return 530;
    }
    {
        // CODE.2 0x1C328 exact dice semantics: modifier is per die, final
        // modifier is once after the loop, and the result is clamped to 1.
        OriginalRng30CA2 rng(1);
        if(OriginalDice1C328::roll(3,6,2,5,rng)!=19)return 338; // seed 1 => 4,2,2 + 6 + 5
        const auto saved=rng.state();
        if(OriginalDice1C328::roll(0,6,99,-5,rng)!=1||rng.state()!=saved)return 339;
        OriginalRng30CA2 d20(1);
        if(OriginalDice1C328::roll(1,20,0,0,d20)!=11)return 340;
    }
    {
        // CODE.2 0xD550 ordinary hit: exact target-table choice, D20/damage RNG
        // consumption, CD44 damage boundary, dirty flag, and attack sound.
        std::array<CharacterRecord,4> party{};
        for(auto& c:party){c.raw.fill(0);c.raw[0x1C]=20;c.currentHitPoints=20;}
        RuntimePlacement22 actor{};actor.type=1;actor.field14=8; // direction 1 -> DS:687E target 0
        NpcDefinition npc{};npc.raw.fill(0);npc.raw[0x22]=0;npc.damageDice=1;npc.damageDie=6;npc.attackSound=7;
        std::array<std::uint8_t,4> dirty{};std::uint32_t sound=0;int sounds=0;
        NpcAttackContextD550 c;c.party=&party;c.timer30700=[](){return 1u;};
        c.characterArmorClassEFA0=[](std::size_t){return 0;};c.query14A60=[](int){return -1;};
        c.predicateCEE4=[](std::size_t){return 0;};c.currentTick19CC4=[](){return 100u;};
        c.soundBase258C0=1000;c.soundBase258BC=2000;c.queueSound28110=[&](std::uint32_t id,int,int,int){sound=id;++sounds;};
        c.damageContext.currentTick19CC4=[](){return 100u;};c.damageContext.cf5aCharacterFlags=&dirty;
        OriginalRng30CA2 rng(999);const auto r=NpcAttackDispatcherD550::execute(actor,npc,3,rng,c);
        if(r.path!=NpcAttackResultD550::Path::OrdinaryHit||!r.hit||r.target!=0||r.damage!=2)return 341;
        if(party[0].currentHitPoints!=18||dirty[0]!=1||sounds!=1||sound!=1007)return 342;
        OriginalRng30CA2 expected(1);OriginalDice1C328::roll(1,20,0,0,expected);OriginalDice1C328::roll(1,6,0,0,expected);
        if(rng.state()!=expected.state())return 343;
    }
    {
        // D550 ordinary miss and reach gate: miss presentation/sound occur only
        // after targeting/hit-test; out-of-range actors consume no attack RNG.
        std::array<CharacterRecord,4> party{};for(auto& c:party){c.raw.fill(0);c.raw[0x1C]=20;c.currentHitPoints=20;}
        RuntimePlacement22 actor{};actor.type=6;actor.field14=8;NpcDefinition npc{};npc.raw.fill(0);npc.raw[0x22]=100;
        int misses=0,sounds=0,timerCalls=0;std::uint32_t lastSound=0;
        NpcAttackContextD550 c;c.party=&party;c.timer30700=[&](){++timerCalls;return 1u;};c.characterArmorClassEFA0=[](std::size_t){return 0;};
        c.missFeedback=[&](std::uint16_t type){if(type==6)++misses;};c.soundBase258BC=3000;
        c.queueSound28110=[&](std::uint32_t id,int,int,int){lastSound=id;++sounds;};
        OriginalRng30CA2 rng(77);auto r=NpcAttackDispatcherD550::execute(actor,npc,0,rng,c);
        if(r.path!=NpcAttackResultD550::Path::OrdinaryMiss||r.hit||misses!=1||lastSound!=3009||timerCalls!=1)return 344;
        const auto stateAfterMiss=rng.state();actor.xFixed=0x20000;misses=sounds=timerCalls=0;
        r=NpcAttackDispatcherD550::execute(actor,npc,0,rng,c);
        if(r.path!=NpcAttackResultD550::Path::None||timerCalls!=0||sounds!=0||misses!=0||rng.state()!=stateAfterMiss)return 345;
    }
    {
        // D550 type-8 0x8000 phase bypasses ordinary attack processing and calls
        // 0x8A80 with the original angle and (y,x) fixed-point ordering.
        std::array<CharacterRecord,4> party{};RuntimePlacement22 actor{};actor.type=8;actor.field14=static_cast<std::uint16_t>(0x8000u|(2u<<3));actor.yFixed=123;actor.xFixed=456;
        NpcDefinition npc{};int calls=0,a0=9,angle=-1,a2=9;std::int32_t y=0,x=0;
        NpcAttackContextD550 c;c.party=&party;c.type8Special8A80=[&](int p0,int p1,int p2,std::int32_t py,std::int32_t px){++calls;a0=p0;angle=p1;a2=p2;y=py;x=px;};
        OriginalRng30CA2 rng(55);const auto before=rng.state();const auto r=NpcAttackDispatcherD550::execute(actor,npc,4,rng,c);
        if(r.path!=NpcAttackResultD550::Path::Type8Special||calls!=1||a0!=0||angle!=270||a2!=-1||y!=123||x!=456||rng.state()!=before)return 346;
    }
    {
        // D550 type-15 branch damages every eligible member by HP plus both
        // absorption pools, reproducing its direct "kill through pools" path.
        std::array<CharacterRecord,4> party{};for(auto& c:party)c.raw.fill(0);
        party[0].raw[0x1C]=5;party[0].currentHitPoints=5;
        auto put32=[](CharacterRecord& c,std::size_t o,std::uint32_t v){c.raw[o]=static_cast<std::uint8_t>(v);c.raw[o+1]=static_cast<std::uint8_t>(v>>8);c.raw[o+2]=static_cast<std::uint8_t>(v>>16);c.raw[o+3]=static_cast<std::uint8_t>(v>>24);};
        put32(party[0],0x1EA,2);put32(party[0],0x113,3);for(int i=1;i<4;++i)party[i].raw[0x11]=0x40;
        RuntimePlacement22 actor{};actor.type=0x0F;NpcDefinition npc{};npc.attackSound=12;
        std::array<std::uint8_t,4> dirty{};int deaths=0,defeat=0;std::uint32_t sound=0;
        NpcAttackContextD550 c;c.party=&party;c.soundBase258C0=500;c.queueSound28110=[&](std::uint32_t id,int,int,int){sound=id;};
        c.damageContext.cf5aCharacterFlags=&dirty;c.damageContext.onDeathAD24=[&](std::size_t i){if(i==0)++deaths;};c.damageContext.allPartyDownCBC8=[&](){++defeat;};
        OriginalRng30CA2 rng(1);const auto before=rng.state();const auto r=NpcAttackDispatcherD550::execute(actor,npc,0,rng,c);
        if(r.path!=NpcAttackResultD550::Path::Type15||party[0].currentHitPoints!=0||deaths!=1||defeat!=1||sound!=512||rng.state()!=before)return 347;
    }
    {
        // D550 targetMode != FF cycles target = (target+3)%4 through exclusions.
        std::array<CharacterRecord,4> party{};for(auto& c:party){c.raw.fill(0);c.raw[0x1C]=20;c.currentHitPoints=20;}
        party[0].raw[0x132]=0x20;RuntimePlacement22 actor{};actor.type=1;actor.field14=8;
        NpcDefinition npc{};npc.raw.fill(0);npc.raw[0x22]=0;npc.raw[0x1D]=5;npc.damageDice=1;npc.damageDie=1;
        NpcAttackContextD550 c;c.party=&party;c.targetMode258A8=0;c.timer30700=[](){return 1u;};c.characterArmorClassEFA0=[](std::size_t){return 0;};
        OriginalRng30CA2 rng(1);const auto r=NpcAttackDispatcherD550::execute(actor,npc,0,rng,c);
        if(r.path!=NpcAttackResultD550::Path::OrdinaryHit||r.target!=3||party[3].currentHitPoints!=19)return 348;
    }
    {
        // D550 Strength-drain branch: exact 2d4 duration and the corrected
        // assembly multiplier of 10,800 ticks.
        std::array<CharacterRecord,4> party{};for(auto& c:party){c.raw.fill(0);c.raw[0x1C]=20;c.currentHitPoints=20;}
        RuntimePlacement22 actor{};actor.type=1;actor.field14=8;NpcDefinition npc{};npc.raw.fill(0);npc.raw[0x22]=0;npc.damageDice=1;npc.damageDie=1;npc.traits3A=0x04;npc.raw[0x3A]=0x04;
        int adjusts=0,which=-1,delta=0;NpcAttackContextD550 c;c.party=&party;c.timer30700=[](){return 1u;};c.characterArmorClassEFA0=[](std::size_t){return 0;};
        c.predicateCEE4=[](std::size_t){return 0;};c.adjustAbility1202C=[&](std::size_t i,int d){++adjusts;which=static_cast<int>(i);delta=d;};c.currentTick19CC4=[](){return 500u;};
        OriginalRng30CA2 expected(1);OriginalDice1C328::roll(1,20,0,0,expected);OriginalDice1C328::roll(1,1,0,0,expected);const int duration=OriginalDice1C328::roll(2,4,0,0,expected);
        OriginalRng30CA2 rng(88);const auto r=NpcAttackDispatcherD550::execute(actor,npc,0,rng,c);
        auto get32=[](const CharacterRecord& x,std::size_t o){return static_cast<std::uint32_t>(x.raw[o])|(static_cast<std::uint32_t>(x.raw[o+1])<<8)|(static_cast<std::uint32_t>(x.raw[o+2])<<16)|(static_cast<std::uint32_t>(x.raw[o+3])<<24);};
        if(r.path!=NpcAttackResultD550::Path::OrdinaryHit||adjusts!=1||which!=0||delta!=-1)return 349;
        if(get32(party[0],0x1EE)!=500u+static_cast<std::uint32_t>(duration*10800)||rng.state()!=expected.state())return 350;
    }
    {
        // 0x3A bit 0x20 is evaluated BEFORE query 0x584. Even when the 10-HP
        // threshold suppresses ordinary damage, the protection is consumed and
        // E284 receives the original 2d6 roll.
        std::array<CharacterRecord,4> party{};for(auto& c:party){c.raw.fill(0);c.raw[0x1C]=20;c.currentHitPoints=20;}
        party[0].raw[0x1C]=12;party[0].currentHitPoints=12;party[0].raw[0x133]=0x40;
        party[0].raw[0x16E]=0x78;party[0].raw[0x16F]=0x56;party[0].raw[0x170]=0x34;party[0].raw[0x171]=0x12;
        RuntimePlacement22 actor{};actor.type=1;actor.field14=8;NpcDefinition npc{};npc.raw.fill(0);npc.raw[0x22]=0;npc.damageDice=1;npc.damageDie=6;npc.traits3A=0x20;npc.raw[0x3A]=0x20;
        int effectCalls=0,effectRoll=0,lastNpc=-1,lastZero=-1;NpcAttackContextD550 c;c.party=&party;c.timer30700=[](){return 1u;};c.characterArmorClassEFA0=[](std::size_t){return 0;};
        c.query14A60=[](int id){return id==0x584?0:-1;};c.protectedEffectE284=[&](RuntimePlacement22&,const NpcDefinition&,int ni,int roll,int zero){++effectCalls;lastNpc=ni;effectRoll=roll;lastZero=zero;};
        OriginalRng30CA2 expected(1);OriginalDice1C328::roll(1,20,0,0,expected);const int damage=OriginalDice1C328::roll(1,6,0,0,expected);const int protectionRoll=OriginalDice1C328::roll(2,6,0,0,expected);
        if(damage!=2)return 351;
        OriginalRng30CA2 rng(9);const auto r=NpcAttackDispatcherD550::execute(actor,npc,17,rng,c);
        auto get32=[](const CharacterRecord& x,std::size_t o){return static_cast<std::uint32_t>(x.raw[o])|(static_cast<std::uint32_t>(x.raw[o+1])<<8)|(static_cast<std::uint32_t>(x.raw[o+2])<<16)|(static_cast<std::uint32_t>(x.raw[o+3])<<24);};
        if(r.path!=NpcAttackResultD550::Path::OrdinaryHit||r.damage!=2||party[0].currentHitPoints!=12||party[0].raw[0x1C]!=12)return 352;
        if(!r.protectedEffectConsumed||effectCalls!=1||effectRoll!=protectionRoll||lastNpc!=17||lastZero!=0)return 353;
        if((party[0].raw[0x133]&0x40u)!=0||get32(party[0],0x16E)!=0||rng.state()!=expected.state())return 354;
    }
    {
        // D550 remaining status traits and paired-character dirty-state branch.
        std::array<CharacterRecord,4> party{};for(auto& c:party){c.raw.fill(0);c.raw[0x1C]=20;c.currentHitPoints=20;}
        RuntimePlacement22 actor{};actor.type=1;actor.field14=8;NpcDefinition npc{};npc.raw.fill(0);npc.raw[0x22]=0;npc.damageDice=1;npc.damageDie=1;npc.traits39=0x21;npc.traits3A=0x10;
        std::array<std::uint8_t,4> dirty{};std::array<std::uint8_t,2> pairActive{{1,0}};std::array<std::int32_t,2> pairChar{{0,-1}};
        NpcAttackContextD550 c;c.party=&party;c.timer30700=[](){return 1u;};c.characterArmorClassEFA0=[](std::size_t){return 0;};c.predicateCEE4=[](std::size_t){return 0;};c.queryEffectF2D4=[](std::size_t,int){return 0;};
        c.damageContext.cf5aCharacterFlags=&dirty;c.damageContext.d681PairActive=&pairActive;c.damageContext.d8a8PairCharacter=&pairChar;
        OriginalRng30CA2 rng(1);const auto r=NpcAttackDispatcherD550::execute(actor,npc,0,rng,c);
        if(r.path!=NpcAttackResultD550::Path::OrdinaryHit||(party[0].raw[0x11]&0x1Cu)!=0x1Cu)return 355;
        if(pairActive[0]!=0||dirty[0]!=1||dirty[1]!=1)return 356;
    }

    {
        // D550 DS:687E entries 4..7 use the original two-way RNG jump table.
        // Direction 0 selects value 5, whose seed-1 draw resolves to target 1.
        std::array<CharacterRecord,4> party{};for(auto& c:party){c.raw.fill(0);c.raw[0x1C]=20;c.currentHitPoints=20;}
        RuntimePlacement22 actor{};actor.type=1;actor.field14=0;NpcDefinition npc{};npc.raw.fill(0);npc.raw[0x22]=0;npc.damageDice=1;npc.damageDie=1;
        NpcAttackContextD550 c;c.party=&party;c.timer30700=[](){return 1u;};c.characterArmorClassEFA0=[](std::size_t){return 0;};
        OriginalRng30CA2 rng(999);const auto r=NpcAttackDispatcherD550::execute(actor,npc,0,rng,c);
        if(r.path!=NpcAttackResultD550::Path::OrdinaryHit||r.target!=1||party[1].currentHitPoints!=19)return 357;
    }
    {
        // In targetMode != FF, exhausting all four excluded candidates goes
        // directly to the fallback sound without the ordinary miss-feedback call.
        std::array<CharacterRecord,4> party{};for(auto& c:party){c.raw.fill(0);c.raw[0x1C]=20;c.currentHitPoints=20;c.raw[0x132]=0x20;}
        RuntimePlacement22 actor{};actor.type=1;actor.field14=8;NpcDefinition npc{};npc.raw.fill(0);npc.raw[0x1D]=5;
        int feedback=0,sounds=0;std::uint32_t sound=0;NpcAttackContextD550 c;c.party=&party;c.targetMode258A8=0;c.timer30700=[](){return 1u;};c.soundBase258BC=700;
        c.missFeedback=[&](std::uint16_t){++feedback;};c.queueSound28110=[&](std::uint32_t id,int,int,int){++sounds;sound=id;};
        OriginalRng30CA2 rng(5);const auto r=NpcAttackDispatcherD550::execute(actor,npc,0,rng,c);
        if(r.path!=NpcAttackResultD550::Path::OrdinaryMiss||r.target!=-1||feedback!=0||sounds!=1||sound!=709)return 358;
    }
    {
        // NPCS +0x3A bit 0x02 level-drain call boundary is reached only when
        // protection is absent and CEE4 does not suppress the effect.
        std::array<CharacterRecord,4> party{};for(auto& c:party){c.raw.fill(0);c.raw[0x1C]=20;c.currentHitPoints=20;}
        RuntimePlacement22 actor{};actor.type=1;actor.field14=8;NpcDefinition npc{};npc.raw.fill(0);npc.raw[0x22]=0;npc.damageDice=1;npc.damageDie=1;npc.traits3A=0x02;npc.raw[0x3A]=0x02;
        int drains=0,who=-1;NpcAttackContextD550 c;c.party=&party;c.timer30700=[](){return 1u;};c.characterArmorClassEFA0=[](std::size_t){return 0;};c.predicateCEE4=[](std::size_t){return 0;};
        c.levelDrainD1E0=[&](std::size_t i){++drains;who=static_cast<int>(i);};OriginalRng30CA2 rng(1);
        const auto r=NpcAttackDispatcherD550::execute(actor,npc,0,rng,c);
        if(r.path!=NpcAttackResultD550::Path::OrdinaryHit||drains!=1||who!=0)return 359;
    }

    {
        // CODE.2 0xCD44 full character-damage mutation order.
        std::array<CharacterRecord,4> party{};
        for(auto& c:party){c.raw.fill(0);c.status=0;c.currentHitPoints=20;c.raw[0x1C]=20;c.raw[0x1D]=0;}
        auto put32=[](CharacterRecord& c,std::size_t o,std::uint32_t v){
            c.raw[o]=static_cast<std::uint8_t>(v);c.raw[o+1]=static_cast<std::uint8_t>(v>>8);
            c.raw[o+2]=static_cast<std::uint8_t>(v>>16);c.raw[o+3]=static_cast<std::uint8_t>(v>>24);
        };
        auto get32=[](const CharacterRecord& c,std::size_t o){
            return static_cast<std::uint32_t>(c.raw[o])|(static_cast<std::uint32_t>(c.raw[o+1])<<8)|
                   (static_cast<std::uint32_t>(c.raw[o+2])<<16)|(static_cast<std::uint32_t>(c.raw[o+3])<<24);
        };
        std::array<std::uint8_t,4> dirty{};std::array<std::uint8_t,2> pairActive{{0,0}};
        std::array<std::int32_t,2> pairCharacter{{-1,-1}};std::int32_t bf44=0;int deaths=0,partyDown=0;
        CharacterDamageContextCD44 dc;
        dc.currentTick19CC4=[](){return 100u;};dc.cf5aCharacterFlags=&dirty;
        dc.d681PairActive=&pairActive;dc.d8a8PairCharacter=&pairCharacter;dc.bf44=&bf44;
        dc.onDeathAD24=[&](std::size_t){++deaths;};dc.query14A60=[](int id){return id==0x578?2:-1;};
        dc.query1C1DC=[](int id){return id==0x46?0:1;};dc.allPartyDownCBC8=[&](){++partyDown;};

        // +0x117 depletes but does not absorb; +0x1EA absorbs before +0x113.
        put32(party[0],0x117,5);party[0].raw[0x132]=1;put32(party[0],0x1EA,4);put32(party[0],0x113,3);
        const auto r0=CharacterDamageCD44::apply(0,6,party,dc);
        if(r0.remainingDamage!=0||get32(party[0],0x117)!=0||get32(party[0],0x1EA)!=0||get32(party[0],0x113)!=1)return 332;
        if((party[0].raw[0x132]&1)!=0||party[0].currentHitPoints!=20||get32(party[0],0x1B2)!=6||get32(party[0],0x1A6)!=154)return 333;
        if(dirty[0]!=1||r0.died)return 334;

        // Death: HP zeroed, 0x4000 set, 0x1C00 cleared, pair state and BF44 branch preserved.
        party[2].raw.fill(0);party[2].raw[0x1C]=5;party[2].currentHitPoints=5;party[2].raw[0x11]=0x1C;
        pairActive[1]=1;pairCharacter[1]=2;dirty.fill(0);
        const auto r1=CharacterDamageCD44::apply(2,7,party,dc);
        const std::uint16_t st2=static_cast<std::uint16_t>(party[2].raw[0x10])|(static_cast<std::uint16_t>(party[2].raw[0x11])<<8);
        if(!r1.died||party[2].currentHitPoints!=0||st2!=0x4000||deaths!=1)return 335;
        if(pairActive[1]!=0||dirty[2]!=1||dirty[3]!=1||bf44!=0x6A)return 336;

        // All four status words matching 0xD040 invokes the party-defeat boundary.
        for(auto& c:party){c.raw[0x10]=0;c.raw[0x11]=0x40;c.status=0x4000;}
        party[0].raw[0x1C]=1;party[0].currentHitPoints=1;
        const auto r2=CharacterDamageCD44::apply(0,1,party,dc);
        if(!r2.allPartyDown||partyDown!=1)return 337;
    }
    {
        // CODE.2 0xD4E8 target exclusion and 0xC9A0 NPC hit test.
        CharacterRecord c{}; NpcDefinition npc{}; RuntimePlacement22 actor{};
        c.raw[0x10]=0;c.raw[0x11]=0;npc.raw[0x1D]=4;npc.raw[0x22]=10;
        if(NpcTargetExclusionD4E8::evaluate(c,npc)!=0)return 281;
        c.raw[0x11]=0x40;if(NpcTargetExclusionD4E8::evaluate(c,npc)!=1)return 282;c.raw[0x11]=0;
        c.raw[0x132]=0x20;if(NpcTargetExclusionD4E8::evaluate(c,npc)!=1)return 283;c.raw[0x132]=0;
        c.raw[0x133]=0x02;npc.raw[0x3A]=0x20;if(NpcTargetExclusionD4E8::evaluate(c,npc)!=1)return 284;
        npc.raw[0x1D]=5;if(NpcTargetExclusionD4E8::evaluate(c,npc)!=0)return 285;
        c.raw[0x133]=0;npc.raw[0x3A]=0;npc.raw[0x22]=10;

        int d20=6,effectCalls=0;
        NpcHitTestContextC9A0 hc;
        hc.rollD20=[&](){return d20;};hc.characterArmorClassEFA0=[](std::size_t){return 5;};
        hc.effect871C=[&](std::size_t i,int id){if(i==2&&id==6)++effectCalls;};
        if(NpcHitTestC9A0::evaluate(2,c,npc,actor,hc)!=1)return 286; // target 5, roll 6
        d20=4;if(NpcHitTestC9A0::evaluate(2,c,npc,actor,hc)!=0)return 287;
        d20=20;if(NpcHitTestC9A0::evaluate(2,c,npc,actor,hc)!=1)return 288;
        d20=1;if(NpcHitTestC9A0::evaluate(2,c,npc,actor,hc)!=0)return 289;
        d20=6;c.raw[0x132]=0x20;if(NpcHitTestC9A0::evaluate(2,c,npc,actor,hc)!=0)return 290;
        c.raw[0x132]=0x40;if(NpcHitTestC9A0::evaluate(2,c,npc,actor,hc)!=0||effectCalls!=1)return 291;
        c.raw[0x132]=0;actor.raw[0x08]=0x80;d20=8;if(NpcHitTestC9A0::evaluate(2,c,npc,actor,hc)!=0)return 292;
        d20=9;if(NpcHitTestC9A0::evaluate(2,c,npc,actor,hc)!=1)return 293;
    }
    {
        // CODE.2 0xEEE0 / 0xEF34 / 0xF2D4 / 0xFD84 / 0xFE44 helper parity.
        const char* helperRoot=std::getenv("RAVENLOFT_DATA_DIR");
        if(helperRoot){
            const std::filesystem::path hr(helperRoot); ObjectDatabase odb(hr/"OBJECTS.DAT"); SaveGame sg(hr/"SAVE01"/"SAVE.DAT");
            const auto& eh=sg.quickStart().party[0]; const auto& an=sg.quickStart().party[1];
            if(CharacterDefenseHelpers::bodyIsArmorEEE0(eh,odb)!=0)return 294;
            if(CharacterDefenseHelpers::bodyIsArmorEEE0(an,odb)!=1)return 295;
            if(CharacterDefenseHelpers::ringSlotContainsF2D4(eh,70)!=0)return 296; // ring is carried, not equipped
            if(CharacterDefenseHelpers::itemAcMagicAdjustmentEF34(-1,odb)!=0)return 297;
            if(CharacterDefenseHelpers::encumbranceTierFE44(eh,odb,[](int){return 0;})!=0)return 298;
            if(CharacterDefenseHelpers::encumbranceTierFE44(an,odb,[](int){return 0;})!=0)return 299;
        }
    }
    {
        // CODE.2 0xF5F8 recursive container weight against the shipped serialized descriptors.
        const char* cwRoot=std::getenv("RAVENLOFT_DATA_DIR");
        if(cwRoot){
            const std::filesystem::path rr(cwRoot); ObjectDatabase odb(rr/"OBJECTS.DAT"); SaveGame sg(rr/"SAVE01"/"SAVE.DAT");
            if(sg.quickStart().dynamicEntityCount!=0)return 326;
            if(sg.quickStart().containerItems[14].size()!=8||sg.quickStart().containerItems[14][2]!=274)return 327;
            if(ContainerWeightF5F8::evaluate(14,odb,sg.quickStart())!=5)return 328; // includes nested descriptor 15
            if(ContainerWeightF5F8::evaluate(12,odb,sg.quickStart())!=88)return 329;
            if(ContainerWeightF5F8::evaluate(20,odb,sg.quickStart())!=odb.at(80).raw[0x30])return 330;
            if(ContainerWeightF5F8::evaluate(62,odb,sg.quickStart())!=0)return 331; // original object-329 descriptor defect
        }
    }
    {
        // CODE.2 0x14A60 character-name lookup used by D550/CD44.
        const char* nameRoot=std::getenv("RAVENLOFT_DATA_DIR");
        if(nameRoot){
            const std::filesystem::path nr(nameRoot); SaveGame sg(nr/"SAVE01"/"SAVE.DAT");
            auto party=sg.quickStart().party;
            if(CharacterNameLookup14A60::find(party,"VLADISLAV")!=-1)return 364;
            if(CharacterNameLookup14A60::find(party,"IRMGARDE")!=-1)return 365;
            party[2]={}; const char name[]="VLADISLAV";
            for(std::size_t i=0;i<sizeof(name);++i)party[2].raw[i]=static_cast<std::uint8_t>(name[i]);
            party[2].raw[0x10]=0x80;
            if(CharacterNameLookup14A60::find(party,"VLADISLAV")!=2)return 366;
            party[2].raw[0x10]=0;
            if(CharacterNameLookup14A60::find(party,"VLADISLAV")!=-1)return 367;
        }
    }
    {
        // CODE.2 0xEFA0 Quick Start parity, preserving 0x24478/0xF5F8 as explicit call boundaries.
        const char* defenseRoot=std::getenv("RAVENLOFT_DATA_DIR");
        if(defenseRoot){
            const std::filesystem::path dr(defenseRoot); ObjectDatabase odb(dr/"OBJECTS.DAT"); SaveGame sg(dr/"SAVE01"/"SAVE.DAT");
            CharacterDefenseContextEFA0 dc;dc.handUsable24478=[](std::size_t,int){return 1;};dc.containerWeightF5F8=[](int){return 0;};
            if(CharacterDefenseEFA0::evaluate(0,sg.quickStart().party[0],odb,dc)!=9)return 300;
            if(CharacterDefenseEFA0::evaluate(1,sg.quickStart().party[1],odb,dc)!=5)return 301;
        }
    }
    {
        // CODE.2 0x23E58/23EAC/23EE4/23F34/23F50/23FC4/24060 predicates.
        const char* predRoot=std::getenv("RAVENLOFT_DATA_DIR");
        if(predRoot){
            const std::filesystem::path pr(predRoot); ObjectDatabase odb(pr/"OBJECTS.DAT"); SaveGame sg(pr/"SAVE01"/"SAVE.DAT");
            if(!HandItemPredicates::spellHandRange23F34(0x7E)||!HandItemPredicates::spellHandRange23F34(0x86)||HandItemPredicates::spellHandRange23F34(0x87))return 302;
            if(!HandItemPredicates::predicate23EE4(0x2B)||HandItemPredicates::predicate23EE4(0x2C))return 303;
            if(!HandItemPredicates::predicate23FC4(0xF8,odb)||HandItemPredicates::predicate23FC4(0x25,odb))return 304;
            if(!HandItemPredicates::predicate24060(0x31)||HandItemPredicates::predicate24060(0x30))return 305;
            // EHLIANNA is a mage in one of the three original class bytes; ANVIL DAWN is not.
            if(!HandItemPredicates::characterHasClass23EAC(sg.quickStart().party[0],4))return 306;
            if(HandItemPredicates::characterHasClass23EAC(sg.quickStart().party[1],4))return 307;
        }
    }
    {
        // CODE.2 0x240A8 classifier: direct Quick Start and synthetic class/range branches.
        const char* classRoot=std::getenv("RAVENLOFT_DATA_DIR");
        if(classRoot){
            const std::filesystem::path cr(classRoot); ObjectDatabase odb(cr/"OBJECTS.DAT"); SaveGame sg(cr/"SAVE01"/"SAVE.DAT");
            auto party=sg.quickStart().party; HandItemClassifierContext240A8 cc;cc.party=&party;cc.global258A8=0xFF;int out=0;
            const int ehLeft=static_cast<std::int16_t>(static_cast<std::uint16_t>(party[0].raw[0x5C])|(static_cast<std::uint16_t>(party[0].raw[0x5D])<<8));
            const int anLeft=static_cast<std::int16_t>(static_cast<std::uint16_t>(party[1].raw[0x5C])|(static_cast<std::uint16_t>(party[1].raw[0x5D])<<8));
            const int er=HandItemClassifier240A8::evaluate(0,ehLeft,out,odb,cc);if(er!=0&&er!=1)return 308;
            const int ar=HandItemClassifier240A8::evaluate(1,anLeft,out,odb,cc);if(ar!=0&&ar!=1)return 309;
            CharacterRecord synthetic{};for(auto& b:synthetic.raw)b=0xFF;synthetic.raw[0x15]=4;synthetic.raw[0x16]=synthetic.raw[0x17]=0xFF;synthetic.raw[0x10]=synthetic.raw[0x11]=0;
            party[2]=synthetic;cc.party=&party;
            if(HandItemClassifier240A8::evaluate(2,0xA7,out,odb,cc)!=1)return 310;
            party[2].raw[0x15]=0;party[2].raw[0x16]=party[2].raw[0x17]=0xFF;
            if(HandItemClassifier240A8::evaluate(2,0xA7,out,odb,cc)!=0)return 311;
            party[2].raw[0x15]=5;party[2].raw[0x20]=10;party[2].raw[0x21]=0;
            if(HandItemClassifier240A8::evaluate(2,0xA7,out,odb,cc)!=1)return 312;
            // 0x2420D rear-party gate: spell-hand object 0x7E is rejected unless
            // both front-rank status words are exactly 0x8000.
            party[2].raw[0x15]=0;party[2].raw[0x16]=party[2].raw[0x17]=0xFF;
            party[0].raw[0x10]=party[0].raw[0x11]=0;party[1].raw[0x10]=party[1].raw[0x11]=0;
            if(HandItemClassifier240A8::evaluate(2,0x7E,out,odb,cc)!=0)return 317;
            party[0].raw[0x10]=0;party[0].raw[0x11]=0x80;party[1].raw[0x10]=0;party[1].raw[0x11]=0x80;
            if(HandItemClassifier240A8::evaluate(2,0x7E,out,odb,cc)!=1)return 318;
        }
    }

    {
        // CODE.2 0x24478 two-hand usability/arbitration, including recovery deadlines.
        const char* handRoot=std::getenv("RAVENLOFT_DATA_DIR");
        if(handRoot){
            const std::filesystem::path hr(handRoot); ObjectDatabase odb(hr/"OBJECTS.DAT"); SaveGame sg(hr/"SAVE01"/"SAVE.DAT");
            auto party=sg.quickStart().party; HandItemUsabilityContext24478 hc;hc.party=&party;hc.global258A8=0xFF;
            const int ehL=HandItemUsability24478::evaluate(0,0,odb,hc);
            const int ehR=HandItemUsability24478::evaluate(0,1,odb,hc);
            const int anL=HandItemUsability24478::evaluate(1,0,odb,hc);
            const int anR=HandItemUsability24478::evaluate(1,1,odb,hc);
            if((ehL!=0&&ehL!=1)||(ehR!=0&&ehR!=1)||(anL!=0&&anL!=1)||(anR!=0&&anR!=1))return 313;
            // Original +0x10B/+0x10F DWORDs independently suppress their hand.
            party[0].raw[0x10B]=1;party[0].raw[0x10C]=party[0].raw[0x10D]=party[0].raw[0x10E]=0;
            if(HandItemUsability24478::evaluate(0,0,odb,hc)!=0)return 314;
            party[0]=sg.quickStart().party[0];
            party[0].raw[0x10F]=1;party[0].raw[0x110]=party[0].raw[0x111]=party[0].raw[0x112]=0;
            if(HandItemUsability24478::evaluate(0,1,odb,hc)!=0)return 315;
            // The character status mask 0x7240 disables both non-spell hands.
            party[0]=sg.quickStart().party[0];party[0].raw[0x10]|=0x40;
            if(HandItemUsability24478::evaluate(0,0,odb,hc)!=0||HandItemUsability24478::evaluate(0,1,odb,hc)!=0)return 316;
            CharacterRecord dual{};for(auto& b:dual.raw)b=0;dual.raw[0x15]=0;dual.raw[0x16]=dual.raw[0x17]=0xFF;
            // Martial, left kind > right kind, and a lighter right item leaves both active.
            dual.raw[0x5C]=0x2E;dual.raw[0x5D]=0;dual.raw[0x5E]=0x2F;dual.raw[0x5F]=0;party[0]=dual;
            if(HandItemUsability24478::evaluate(0,0,odb,hc)!=1||HandItemUsability24478::evaluate(0,1,odb,hc)!=1)return 319;
            // Same-kind, equal-weight classified items keep left and suppress right.
            dual.raw[0x5C]=0xF4;dual.raw[0x5D]=0;dual.raw[0x5E]=0xF5;dual.raw[0x5F]=0;party[0]=dual;
            if(HandItemUsability24478::evaluate(0,0,odb,hc)!=1||HandItemUsability24478::evaluate(0,1,odb,hc)!=0)return 323;
            // Two kind-2 classified items suppress both hands.
            dual.raw[0x5C]=0x26;dual.raw[0x5D]=0;dual.raw[0x5E]=0x30;dual.raw[0x5F]=0;party[0]=dual;
            if(HandItemUsability24478::evaluate(0,0,odb,hc)!=0||HandItemUsability24478::evaluate(0,1,odb,hc)!=0)return 324;
            // Non-martial differing kinds keep only the lighter item.
            dual.raw[0x15]=3;dual.raw[0x5C]=0x25;dual.raw[0x5D]=0;dual.raw[0x5E]=0x4A;dual.raw[0x5F]=0;party[0]=dual;
            if(HandItemUsability24478::evaluate(0,0,odb,hc)!=0||HandItemUsability24478::evaluate(0,1,odb,hc)!=1)return 325;
            dual.raw[0x15]=0;
            // Two 0x23EE4-family items bypass the weight arbitration for the martial class group.
            dual.raw[0x5C]=0x59;dual.raw[0x5E]=0x59;party[0]=dual;
            if(HandItemUsability24478::evaluate(0,0,odb,hc)!=1||HandItemUsability24478::evaluate(0,1,odb,hc)!=1)return 320;
            // Two spell-created hand objects take their early branch before the 0x7240 status mask.
            dual.raw[0x10]=0x40;dual.raw[0x11]=0;dual.raw[0x5C]=0x7E;dual.raw[0x5E]=0x7F;party[0]=dual;
            if(HandItemUsability24478::evaluate(0,0,odb,hc)!=1||HandItemUsability24478::evaluate(0,1,odb,hc)!=1)return 321;
            // Rear character with two empty hands is disabled when the front-rank gate is false.
            CharacterRecord rear{};for(auto& b:rear.raw)b=0;rear.raw[0x15]=0;rear.raw[0x16]=rear.raw[0x17]=0xFF;
            rear.raw[0x5C]=rear.raw[0x5D]=rear.raw[0x5E]=rear.raw[0x5F]=0xFF;party[2]=rear;
            party[0].raw[0x10]=party[0].raw[0x11]=0;party[1].raw[0x10]=party[1].raw[0x11]=0;
            if(HandItemUsability24478::evaluate(2,0,odb,hc)!=0||HandItemUsability24478::evaluate(2,1,odb,hc)!=0)return 322;
        }
    }
    {
        // CODE.2 0x30CA2 / 0x30CC4 / 0x290D0 exact RNG and seed behavior.
        OriginalRng30CA2 rng(0x1234u);
        const std::uint16_t expected[5]={19915,19390,6099,9382,16769};
        for(auto v:expected)if(rng.next15()!=v)return 257;
        int rereads=0;
        if(rng.seedFromBiosLowWord(0x12345678u)!=0x5678u||rng.state()!=0x5678u)return 258;
        if(rng.seedFromBiosLowWord(0xABCD0000u,[&](){++rereads;return 0x76543210u;})!=0x3210u||rereads!=1||rng.state()!=0x3210u)return 259;

        // CODE.2 0x1AF90 direction priority and retry behavior.
        rng.seed(0x1234u);
        if(NpcDirectionSelector1AF90::select(0x00020000,0x00020000,7,0,rng)!=2)return 260;
        if(NpcDirectionSelector1AF90::select(0x00020000,0x00020000,7,1,rng)!=0)return 261;
        if(NpcDirectionSelector1AF90::select(0x00020000,0,7,0,rng)!=7)return 262; // no priority-4 entry
        if(NpcDirectionSelector1AF90::select(0x00020000,0,7,1,rng)!=2)return 263;
        rng.seed(0x1234u);
        if(NpcDirectionSelector1AF90::select(0,0,5,4,rng)!=static_cast<int>((19915u*8u)>>15u))return 264;

        // CODE.2 0x1B6EC party/NPC predicate, including its all-dead vacuous-true quirk.
        std::array<CharacterRecord,4> party{};NpcDefinition nd{};
        for(auto& c:party){c.raw[0x10]=0;c.raw[0x11]=0;}
        if(PartyNpcPredicate1B6EC::evaluate(party,nd))return 265;
        party[2].raw[0x132]=0x20;
        if(!PartyNpcPredicate1B6EC::evaluate(party,nd))return 266;
        party[2].raw[0x132]=0;
        nd.raw[0x3A]=0x20;nd.raw[0x1D]=4;
        for(auto& c:party)c.raw[0x133]=0x02;
        if(!PartyNpcPredicate1B6EC::evaluate(party,nd))return 267;
        party[1].raw[0x133]=0;
        if(PartyNpcPredicate1B6EC::evaluate(party,nd))return 268;
        for(auto& c:party){c.raw[0x10]=0;c.raw[0x11]=0x40;c.raw[0x133]=0;}
        if(!PartyNpcPredicate1B6EC::evaluate(party,nd))return 269;
    }
    {
        // CODE.2 0x1B760 complete decision routine branch/reseed parity.
        std::array<CharacterRecord,4> party{};
        for(auto& c:party){c.raw[0x10]=0;c.raw[0x11]=0;c.raw[0x132]=0;c.raw[0x133]=0;}
        NpcDefinition def{};def.viewCount=4;def.ambientSound=7;
        std::array<std::uint16_t,4> cells{};
        OriginalRng30CA2 rng(0x1234u);
        int biosReads=0,sounds=0;
        NpcDecisionContext1B760 ctx{};
        ctx.party=&party;ctx.levelCells=cells;ctx.partyXFixed=0x00050000;ctx.partyYFixed=0x00050000;
        ctx.levelWidth=30;ctx.levelHeight=15;ctx.soundBase258C0=100;
        ctx.biosTick311E0=[&](){++biosReads;return std::uint32_t{0x1234};};
        ctx.queueSound28110=[&](std::uint32_t id,int v,int ch,int pri){if(id==107&&v==0xFF&&ch==0&&pri==1)++sounds;};
        ctx.type8Relocate.collision2D5D0=[](std::int32_t,std::int32_t,int){return 1;};

        // Type 15 without cell bit 0x40 is an immediate no-op and does not reseed.
        RuntimePlacement22 a{};a.type=0x0F;a.field0C=0;a.field14=0x0123;auto before=a.field14;
        NpcDecision1B760::execute(a,def,0,0,rng,ctx);
        if(a.field14!=before||biosReads!=0)return 270;
        // With bit 0x40 and a low random roll, its phase advances modulo viewCount.
        cells[0]=0x0040;ctx.levelCells=cells;rng.seed(0);a.field14=0;
        NpcDecision1B760::execute(a,def,0,0,rng,ctx);
        if(((a.field14>>6)&7)!=1||(a.field14&0x0006u)!=0||biosReads!=0)return 271;
        // Seed 72 yields a 998/1000 roll, selecting the type-15 attack/state-2 path.
        rng.seed(72);a.field14=0x01C5;
        NpcDecision1B760::execute(a,def,0,0,rng,ctx);
        if((a.field14&0x0006u)!=0x0002u||biosReads!=0)return 272;

        // State +0B bit 0x40 uses two RNG draws, sets direction/move and a 0..24 countdown.
        a={};a.type=0;a.state=0x40000000u;rng.seed(0x1234u);
        NpcDecision1B760::execute(a,def,0,0,rng,ctx);
        if(((a.field14>>3)&7)!=4||(a.field14&1u)==0||a.hitPoints!=14||biosReads!=0)return 273;

        // Normal close-range attack: entry reseed occurs, but the attack transition returns
        // before the routine's normal second reseed.
        a={};a.type=0;a.xFixed=0x00058000;a.yFixed=0x00058000;a.field14=static_cast<std::uint16_t>(3u<<3u);
        ctx.partyXFixed=0x00050000;ctx.partyYFixed=0x00050000;biosReads=0;sounds=0;
        ctx.biosTick311E0=[&](){++biosReads;return std::uint32_t{0x1234};};
        NpcDecision1B760::execute(a,def,0,0,rng,ctx);
        if(biosReads!=1||(a.field14&0x0006u)!=0x0002u||((a.state>>24)&0x30u)!=0x10u)return 274;

        // High phase returns before the final reseed and decrements modulo four.
        a={};a.type=0;a.xFixed=0;a.yFixed=0;a.field14=static_cast<std::uint16_t>(0x4000u|(2u<<3u));
        ctx.partyXFixed=0x00140000;ctx.partyYFixed=0x00140000;biosReads=0;
        NpcDecision1B760::execute(a,def,0,0,rng,ctx);
        if(biosReads!=1||(a.field14&0xC000u)!=0||(a.field14&1u)==0)return 275;

        // Ordinary far completion executes both BIOS reseeds.
        a={};a.type=0;a.field14=static_cast<std::uint16_t>(5u<<3u);ctx.partyXFixed=0x00140000;ctx.partyYFixed=0;biosReads=0;
        int readNo=0;ctx.biosTick311E0=[&](){++biosReads;return ++readNo==1?std::uint32_t{0x1234}:std::uint32_t{0x5678};};
        NpcDecision1B760::execute(a,def,0,0,rng,ctx);
        if(biosReads!=2||rng.state()!=0x5678u)return 276;

        // Type 9 at its scripted endpoint snaps to the selected direction directly.
        a={};a.type=9;a.xFixed=0x00188000;a.yFixed=0x00078000;a.field14=0;
        ctx.partyXFixed=0x001E0000;ctx.partyYFixed=0x000A0000;biosReads=0;readNo=0;
        NpcDecision1B760::execute(a,def,0,0,rng,ctx);
        if(((a.field14>>3)&7)!=2||biosReads!=2)return 277;

        // Type 0x21 forces the current direction and clears movement.
        a={};a.type=0x21;a.field14=static_cast<std::uint16_t>((5u<<3u)|1u);ctx.partyXFixed=0x00140000;ctx.partyYFixed=0x00140000;biosReads=0;readNo=0;
        NpcDecision1B760::execute(a,def,0,0,rng,ctx);
        if(((a.field14>>3)&7)!=5||(a.field14&1u)!=0||biosReads!=2)return 278;

        // BIOS seed 72 makes the rare 991..999/1000 ambient-sound branch deterministic.
        a={};a.type=0;a.xFixed=0x00050000;a.yFixed=0x00050000;a.field14=static_cast<std::uint16_t>(2u<<3u);
        ctx.partyXFixed=0x00070000;ctx.partyYFixed=0x00070000;biosReads=0;sounds=0;
        ctx.biosTick311E0=[&](){++biosReads;return std::uint32_t{72};};
        NpcDecision1B760::execute(a,def,0,0,rng,ctx);
        if(sounds!=1||biosReads!=2)return 279;

        // Type 8 delegates to 0x1B27C and does not touch the BIOS reseed path.
        a={};a.type=8;a.field14=0xC000;ctx.partyXFixed=0x00070000;ctx.partyYFixed=0x00070000;biosReads=0;rng.seed(0x1234u);
        NpcDecision1B760::execute(a,def,0,0,rng,ctx);
        if(biosReads!=0)return 280;
    }
    {
        // CODE.2 0x2D004 player-only collision trigger.
        PlayerCollisionState2D004 st{};st.partyXFixed=0x11110000;st.partyYFixed=0x22220000;
        st.savedXFixed=0x00048000;st.savedYFixed=0x00058000;st.facingDegrees=270;st.currentLevel=8;
        PlayerCollisionTrigger2D004Callbacks cb;int seq=0,loads=0,flagQueries=0;
        cb.mappedCellObjectId=[](std::uint8_t c){return c==5?std::int16_t{0x242}:std::int16_t{0};};
        cb.queryFlag1C1DC=[&](std::uint16_t f){++flagQueries;return f==0x3E;};
        cb.sequence2CEB8=[&](){++seq;};
        cb.loadLevel181B4=[&](std::uint16_t){++loads;};
        if(PlayerCollisionTrigger2D004::evaluate(5,st,cb)!=1||seq!=1||loads!=0||flagQueries!=1)return 237;
        if(PlayerCollisionTrigger2D004::evaluate(4,st,cb)!=0)return 238;

        st.currentLevel=0x10;seq=0;loads=0;
        if(PlayerCollisionTrigger2D004::evaluate(5,st,cb)!=1||loads!=1||st.currentLevel!=0x1F||st.partyXFixed!=0x00218000||st.partyYFixed!=0x00028000||st.facingDegrees!=0)return 239;

        // Flag-clear trap path: only living (<0x4000) party records take 1d2;
        // it uses saved collision coordinates, renders twice, turns 180, BF44=0x2E.
        PlayerCollisionState2D004 trap{};trap.savedXFixed=0x12345678;trap.savedYFixed=0x23456789;trap.facingDegrees=270;trap.currentLevel=3;
        int f1=0,fea04=0,fea78=0,draws=0,rolls=0,damageCalls=0,damageSum=0;
        PlayerCollisionTrigger2D004Callbacks tcb;
        tcb.mappedCellObjectId=[](std::uint8_t){return std::int16_t{0x247};};
        tcb.queryFlag1C1DC=[](std::uint16_t){return false;};
        tcb.partyStatusWord=[](std::size_t i){const std::uint16_t v[4]={0,0x4000,1,0x8000};return v[i];};
        tcb.roll1d2=[&](){++rolls;return rolls;};
        tcb.damageCharacterCD44=[&](std::size_t,int d){++damageCalls;damageSum+=d;};
        tcb.refresh1F1C8=[&](){++f1;};tcb.refresh1EA04=[&](){++fea04;};tcb.refresh1EA78=[&](){++fea78;};
        tcb.resourceBase258EC=[](){return 100;};
        tcb.draw2E510=[&](int a,int b,std::int32_t c,int d){if(a==4&&b==0x20&&c==100&&d==9)++draws;};
        if(PlayerCollisionTrigger2D004::evaluate(5,trap,tcb)!=1||trap.partyXFixed!=trap.savedXFixed||trap.partyYFixed!=trap.savedYFixed||trap.facingDegrees!=90||trap.stateBF44!=0x2E)return 240;
        if(rolls!=2||damageCalls!=2||damageSum!=3||f1!=3||fea04!=2||fea78!=3||draws!=2)return 241;
    }
    {
        // CODE.2 0x2D3D8 portal state/call ordering core.
        PortalCollisionState2D3D8 st{};st.facingDegrees=315;st.currentLevel=5;
        PortalCollisionTransition2D3D8Callbacks cb;int faceCalls=0,loads=0,left5=0,entered11=0,sounds=0;
        cb.sourceHasTypeAD2CA5C=[](int x,int y){return x==7&&y==8;};
        cb.setFacing1C0C0=[&](std::int32_t f){if(f==45)++faceCalls;};
        cb.loadLevel181B4=[&](std::uint16_t l){if(l==0x0B)++loads;};
        cb.onLeaveLevel5_2D234=[&](){++left5;};cb.onEnterLevel11_2D2F4=[&](){++entered11;};
        cb.queueSound28110=[&](int id,int volume,int channel,int priority){if(id==7&&volume==0xFF&&channel==1&&priority==1)++sounds;};
        PortalCollisionTransition2D3D8::execute(7,8,3,4,2,0x0B,st,cb);
        if(st.partyXFixed!=0x00038000||st.partyYFixed!=0x00048000||st.facingDegrees!=45||st.currentLevel!=0x0B)return 242;
        if(faceCalls!=1||loads!=1||left5!=1||entered11!=1||sounds!=1)return 243;
    }

    // CODE.2 0x20C0C cell-interaction transition regression. This part is
    // independent of installed game data so it always executes.
    {
        CellInteractionRecord rec{};
        rec.type=1; rec.flags=0x05; rec.frame=0; rec.frameDelta=0;
        int refreshes=0; int occupancyCalls=0;
        CellInteractionCallbacks cb;
        std::uint8_t activeTransitions=0;
        cb.activeTransitionCount=&activeTransitions;
        cb.npcOccupied=[&](std::int32_t x,std::int32_t y){++occupancyCalls;return x!=(3*65536+0x8000)||y!=(4*65536+0x8000);};
        cb.filteredNpcOccupied2C8E4=[&](std::int32_t,std::int32_t){++occupancyCalls;return false;};
        cb.partyOccupied=[&](std::int32_t,std::int32_t){++occupancyCalls;return false;};
        cb.transitionSideEffect20A24=[&](CellInteractionRecord&){++refreshes;};
        if(CellInteractionSystem::activate(3,4,rec,cb)!=1||rec.frame!=0||rec.frameDelta!=1||refreshes!=1||occupancyCalls!=3||activeTransitions!=1)return 151;

        // Re-activation while already animating is rejected before occupancy.
        occupancyCalls=0;
        if(CellInteractionSystem::activate(3,4,rec,cb)!=0||occupancyCalls!=0)return 152;

        // Flags low bits == 1 with a nonzero frame forces frame 8 and closes.
        rec.frameDelta=0;rec.frame=3;refreshes=0;activeTransitions=0;
        if(CellInteractionSystem::activate(3,4,rec,cb)!=1||rec.frame!=8||rec.frameDelta!=-1||refreshes!=1)return 153;

        // Mode 2 always initiates the negative transition without forcing frame 8.
        rec.flags=0x06;rec.frameDelta=0;rec.frame=5;refreshes=0;activeTransitions=0;
        if(CellInteractionSystem::activate(3,4,rec,cb)!=1||rec.frame!=5||rec.frameDelta!=-1||refreshes!=1)return 154;

        // Occupancy short-circuits in original NPC -> object -> party order.
        rec.flags=0x05;rec.frameDelta=0;rec.frame=0;refreshes=0;occupancyCalls=0;
        cb.npcOccupied=[&](std::int32_t,std::int32_t){++occupancyCalls;return true;};
        if(CellInteractionSystem::activate(3,4,rec,cb)!=0||occupancyCalls!=1||refreshes!=0)return 155;

        // Enabled/lockout gates occur before all occupancy checks.
        cb.npcOccupied=[&](std::int32_t,std::int32_t){++occupancyCalls;return false;};
        occupancyCalls=0;rec.flags=0x01;
        if(CellInteractionSystem::activate(3,4,rec,cb)!=0||occupancyCalls!=0)return 156;
        rec.flags=0x45;
        if(CellInteractionSystem::activate(3,4,rec,cb)!=0||occupancyCalls!=0)return 157;
    }
    {
        // CODE.2 0x20A24 pure sound-selection/spatialization core.
        CellInteractionRecord rec{};rec.cellX=10;rec.cellY=12;rec.frame=0;
        const auto atSource=CellInteractionSoundSystem::select(rec,10,12,32,0x00010000u);
        if(!atSource||atSource->resourceHandle!=0x0001000Fu||atSource->volume!=255||atSource->channel!=0||atSource->priorityDistance!=0)return 162;
        const auto distOne=CellInteractionSoundSystem::select(rec,9,12,185,0x00010000u);
        if(!distOne||distOne->resourceHandle!=0x00010015u||distOne->volume!=223||distOne->priorityDistance!=1)return 163;
        const auto unknown=CellInteractionSoundSystem::select(rec,10,19,999,0x00020000u);
        if(!unknown||unknown->resourceHandle!=0x0002000Eu||unknown->priorityDistance!=7)return 164;
        if(CellInteractionSoundSystem::select(rec,10,20,32,0x00010000u).has_value())return 165; // distance 8 is silent
        rec.frame=4;
        if(CellInteractionSoundSystem::select(rec,10,12,32,0x00010000u).has_value())return 166; // 0x28164 stub path
        rec.frame=8;
        if(!CellInteractionSoundSystem::select(rec,10,12,32,0x00010000u).has_value())return 167;
    }
    {
        // Ordinary-NPC branch of CODE.2 0x2D5D0 cell interaction handling.
        CellInteractionRecord rec{};rec.type=1;rec.flags=0x05;rec.cellX=3;rec.cellY=4;rec.frame=0;rec.frameDelta=0;
        std::uint8_t active=0;int sideEffects=0;
        CellInteractionCallbacks cb;cb.activeTransitionCount=&active;
        cb.npcOccupied=[](std::int32_t,std::int32_t){return false;};
        cb.filteredNpcOccupied2C8E4=[](std::int32_t,std::int32_t){return false;};
        cb.partyOccupied=[](std::int32_t,std::int32_t){return false;};
        cb.transitionSideEffect20A24=[&](CellInteractionRecord&){++sideEffects;};
        auto q=NpcCellInteractionSystem::evaluate(0x8000,&rec,0x40,cb);
        if(q.blocked||!q.transitionTriggered||rec.frameDelta!=1||active!=1||sideEffects!=1)return 168;
        q=NpcCellInteractionSystem::evaluate(0x8000,&rec,0x40,cb);
        if(!q.blocked||q.transitionTriggered)return 169; // animation already active
        rec.frameDelta=0;rec.flags=0x01;
        q=NpcCellInteractionSystem::evaluate(0x8000,&rec,0x40,cb);
        if(!q.blocked)return 170; // record not enabled
        rec.flags=0x45;
        q=NpcCellInteractionSystem::evaluate(0x8000,&rec,0x40,cb);
        if(!q.blocked)return 171; // lockout bit
        rec.flags=0x05;
        q=NpcCellInteractionSystem::evaluate(0x8000,&rec,0x00,cb);
        if(!q.blocked)return 172; // NPC lacks door/interact trait 0x40
        q=NpcCellInteractionSystem::evaluate(0x1234,&rec,0x00,cb);
        if(!q.blocked)return 173;
        q=NpcCellInteractionSystem::evaluate(0x1234,&rec,0x80,cb);
        if(q.blocked)return 174; // alternate interaction pass-through trait
        rec.type=8;
        q=NpcCellInteractionSystem::evaluate(0x8000,&rec,0x00,cb);
        if(q.blocked||q.transitionTriggered)return 175;
    }
    {
        // CODE.2 0x20B40 animation scheduler. Two simultaneous transitions
        // complete on different ticks; normalization occurs only when the last
        // active transition reaches frame 0/8.
        std::vector<std::vector<CellInteractionRecord>> cols(2);
        CellInteractionRecord opening{}; opening.type=1; opening.frame=7; opening.frameDelta=1;
        CellInteractionRecord closing{}; closing.type=1; closing.frame=2; closing.frameDelta=-1;
        CellInteractionRecord idle{}; idle.type=1; idle.frame=5; idle.frameDelta=0;
        cols[0]={opening,idle}; cols[1]={closing};
        std::uint8_t active=2;
        CellInteractionScheduler::tick(cols,active);
        if(active!=1||cols[0][0].frame!=8||cols[0][0].frameDelta!=0||cols[1][0].frame!=1||cols[1][0].frameDelta!=-1||cols[0][1].frame!=5)return 158;
        CellInteractionScheduler::tick(cols,active);
        if(active!=0||cols[1][0].frame!=0||cols[1][0].frameDelta!=0)return 159;
        // Final normalization preserves frame 8 and collapses every other idle frame to zero.
        if(cols[0][0].frame!=8||cols[0][1].frame!=0)return 160;
        const auto frozen=cols;
        CellInteractionScheduler::tick(cols,active);
        if(cols[0][0].frame!=frozen[0][0].frame||cols[0][1].frame!=frozen[0][1].frame)return 161;
    }
    {
        // CODE.2 0x2ACDC exact NPC-type filter used by 0x2C8E4.
        if(!NpcTypeFilter2ACDC::includes(0x00)||!NpcTypeFilter2ACDC::includes(0x21))return 193;
        if(NpcTypeFilter2ACDC::includes(0x22)||NpcTypeFilter2ACDC::includes(0x4F))return 194;
        if(!NpcTypeFilter2ACDC::includes(0x50)||NpcTypeFilter2ACDC::includes(0x51))return 195;
    }
    {
        // Ordinary-NPC cell-scan subset of CODE.2 0x2D5D0.
        LevelRecord scanLevel{};scanLevel.width=3;scanLevel.height=3;
        Interaction ir{};ir.inputColumn=1;ir.raw={0,0x05,0,0,1,1,0,0,0,0};
        scanLevel.interactions.push_back(ir);
        WorkLevelSnapshot scanWork{};scanWork.width=3;scanWork.height=3;scanWork.cells.assign(9,0);
        CellInteractionRuntimeIndex scanIndex(scanLevel);
        // Occupancy callbacks are intentionally clear and the party is placed a
        // full cell away from the interaction center.
        CellInteractionCallbacks icb;std::uint8_t active=0;int sideEffects=0;
        icb.npcOccupied=[](std::int32_t,std::int32_t){return false;};
        icb.filteredNpcOccupied2C8E4=[](std::int32_t,std::int32_t){return false;};
        icb.partyOccupied=[](std::int32_t,std::int32_t){return false;};
        icb.activeTransitionCount=&active;
        icb.transitionSideEffect20A24=[&](CellInteractionRecord&){++sideEffects;};
        NpcCandidateCellScanSystem scan(scanWork,scanIndex);
        constexpr std::int32_t center=0x00018000;
        if(scan.evaluate(center,center,0,0,false,icb).blocked)return 215;
        scanWork.cells[4]=0x0001u;
        if(!scan.evaluate(center,center,0,0,false,icb).blocked)return 216;
        scanWork.cells[4]=0x0801u;
        if(!scan.evaluate(center,center,0,0,false,icb).blocked)return 217;
        // 0x1000 with no interaction pointer is passable for an ordinary NPC.
        LevelRecord emptyLevel{};emptyLevel.width=3;emptyLevel.height=3;
        CellInteractionRuntimeIndex emptyIndex(emptyLevel);
        NpcCandidateCellScanSystem emptyScan(scanWork,emptyIndex);
        scanWork.cells[4]=0x1001u;
        if(emptyScan.evaluate(center,center,0,0,false,icb).blocked)return 218;
        // With a non-type1/non-type8 interaction record the same geometry blocks.
        if(!scan.evaluate(center,center,0,0,false,icb).blocked)return 219;
        // Bit 0x0080 drives the 0x8000 interaction code. Trait 0x40 permits
        // this synthetic enabled record to start its original transition.
        scanWork.cells[4]=0x0080u;active=0;sideEffects=0;
        const auto transition=scan.evaluate(center,center,0,0x40,false,icb);
        if(transition.blocked||!transition.transitionTriggered||active!=1||sideEffects!=1)return 220;
        // DS:F048 bypasses the entire per-cell body.
        scanWork.cells[4]=0x0001u;
        if(scan.evaluate(center,center,0,0,true,icb).blocked)return 221;
    }
    {
        // CODE.2 0x223FB..0x224E6 opening ground-object cursor scan: previous
        // frame draw order is searched from nearest/topmost (count-1) down,
        // rectangle comparisons are inclusive, and click bit 0 is mandatory.
        std::array<OpeningGroundObjectHitRecord223F4,2> recs{};
        recs[0].drawOrder=0;recs[0].sourceIndex=3;recs[0].left=10;recs[0].right=20;recs[0].top=10;recs[0].bottom=20;
        recs[1].drawOrder=1;recs[1].sourceIndex=7;recs[1].left=15;recs[1].right=25;recs[1].top=15;recs[1].bottom=25;
        if(OpeningGroundObjectHit223F4::select(16,16,1,recs)!=std::optional<std::size_t>(7))return 483;
        if(OpeningGroundObjectHit223F4::select(10,10,1,recs)!=std::optional<std::size_t>(3))return 484;
        if(OpeningGroundObjectHit223F4::select(20,20,0,recs).has_value())return 485;
        if(OpeningGroundObjectHit223F4::select(25,25,1,recs)!=std::optional<std::size_t>(7))return 486;
    }
    const char* env=std::getenv("RAVENLOFT_DATA_DIR");
    if(!env){std::cout<<"RAVENLOFT_DATA_DIR not set; structural integration test skipped.\n";return 0;}
    std::filesystem::path r=env;
    std::filesystem::path gogExtractRoot;
    if(const char* gogEnv=std::getenv("RAVENLOFT_GOG_IMAGE")){
        const std::filesystem::path gog=gogEnv;
        if(!looksLikeGogImage(gog))return 143;
        Iso9660Image iso(gog);
        const auto cd=iso.list("/RLOFT");
        if(cd.size()!=68)return 144;
        if(iso.find("/RLOFT/LEVELS.DAT").size!=499878)return 145;
        if(iso.find("/RLOFT/RES10.").size!=46060354)return 146;
        if(iso.find("/RLOFT/CINE00.ANM").size!=13781936)return 147;
        const auto tmp=std::filesystem::temp_directory_path()/"ravenloft_alpha_gog_test";
        std::error_code ec;std::filesystem::remove_all(tmp,ec);
        const auto resolved=resolveRavenloftData(gog,tmp);
        if(!resolved.fromGogImage||!looksLikeRloftDirectory(resolved.root))return 148;
        gogExtractRoot=resolved.root;
        LevelDatabase cdLevels(resolved.root/"LEVELS.DAT");
        ResourceStore cdResources(resolved.root);
        if(cdLevels.count()!=32||cdResources.count(9)!=555||cdResources.count(10)!=219)return 149;
        if(!std::filesystem::is_regular_file(resolved.root/"CINE12.ANM"))return 150;
    }
    LevelDatabase levels(r/"LEVELS.DAT");NpcDatabase npcs(r/"NPCS.DAT");ObjectDatabase objects(r/"OBJECTS.DAT");InitDatabase init(r/"INIT");TextDatabase text(r/"T");PaletteDatabase colors(r/"COLORS");
    if(levels.count()!=32||npcs.count()!=83||objects.count()!=336||init.scenery().size()!=175||text.count()!=1073||colors.count()!=15)return 1;
    std::size_t npc=0,sc=0,obj=0;
    for(std::size_t i=0;i<levels.count();++i){auto L=levels.load(i);npc+=L.npcs.size();sc+=L.scenery.size();obj+=L.objects.size();for(auto&p:L.npcs)if(p.type>=npcs.count())return 2;for(auto&p:L.scenery)if(p.type>=init.scenery().size())return 3;for(auto&p:L.objects)if(p.type>=objects.count())return 4;}
    if(npc!=1258||sc!=4324||obj!=622)return 5;

    // CODE.2 uses archive selector 4 for world texture materials and selector 3
    // for masked scenery/sprites. Keep those semantic roles explicit so numeric
    // resource IDs are never interpreted against the wrong RES archive.
    const char* cineEnv=std::getenv("RAVENLOFT_CINEMATIC_DIR");
    std::unique_ptr<ResourceStore> resourceStore;
    if(cineEnv) resourceStore=std::make_unique<ResourceStore>(r,std::filesystem::path(cineEnv));
    else resourceStore=std::make_unique<ResourceStore>(r);
    auto& resources=*resourceStore;
    const std::size_t expectedInstalledCounts[9]={141,428,38,4234,667,98,266,62,73};
    for(std::size_t i=0;i<9;++i)if(resources.count(static_cast<std::uint8_t>(i))!=expectedInstalledCounts[i])return 44;
    if(cineEnv && (resources.count(9)!=555||resources.count(10)!=219))return 48;
    const auto floor655=resources.texture(655);
    if(floor655.width!=64||floor655.height!=64||floor655.pixels.size()!=4096)return 45;
    const auto wall147=resources.texture(147);
    if(wall147.width!=64||wall147.height!=128||wall147.pixels.size()!=8192)return 46;
    const auto tree2513=resources.sprite(2513);
    if(tree2513.width<=0||tree2513.height<=0||tree2513.pixels.empty())return 47;
    if(init.interfaceResourceBase()!=15)return 139;
    const auto interfaceResources=AdventureInterfaceTheme::resourcesForLevel(22,init);
    if(interfaceResources.theme!=4||interfaceResources.workspacePrimary!=28||interfaceResources.workspaceSecondary!=29||interfaceResources.widgets!=30)return 140;
    const auto ui28=resources.uiBitmap(interfaceResources.workspacePrimary);
    const auto ui29=resources.uiBitmap(interfaceResources.workspaceSecondary);
    if(ui28.width!=320||ui28.height!=200||ui28.pixels.size()!=64000)return 141;
    if(ui29.width!=320||ui29.height!=200||ui29.pixels.size()!=64000)return 142;

    {
        // Level 22's recovered adventure-screen layout uses RES0:28 for the
        // upper viewport surround and RES0:29 for the lower workspace. The
        // world aperture is exactly x=32..287, y=4..131.
        ssi::render::IndexedFrameBuffer staging(256,256);
        for(int y=0;y<256;++y)for(int x=0;x<256;++x)
            staging.set(x,y,static_cast<std::uint8_t>((x+y)&0xFF));
        ssi::render::IndexedFrameBuffer adventure(320,200);
        AdventureScreenRenderer adventureRenderer(init,resources);
        const auto ars=adventureRenderer.renderOpening(adventure,staging,22);
        if(ars.primaryResource!=28||ars.secondaryResource!=29||ars.worldPixels!=256u*128u)return 540;
        if(adventure.pixels()[0]!=ui28.pixels[0])return 541;
        if(adventure.pixels()[static_cast<std::size_t>(150)*320+10]!=ui29.pixels[static_cast<std::size_t>(150)*320+10])return 542;
        if(adventure.pixels()[static_cast<std::size_t>(4)*320+32]!=staging.pixels()[0])return 543;
        if(adventure.pixels()[static_cast<std::size_t>(131)*320+287]!=staging.pixels()[static_cast<std::size_t>(254)*256+255])return 544;
    }

    auto L=levels.load(22);if(L.npcs.size()!=1||L.npcs[0].type!=9||npcs.at(9).label!="ASSASSIN")return 6;if(L.width!=30||L.height!=15)return 7;
    {
        // Loader 0x184AF..0x18650 + lookup routines 0x1E8C8/0x1E930.
        CellInteractionRuntimeIndex idx22(L);
        const auto* staticRec=idx22.find(5,6);
        if(!staticRec||staticRec->type!=0||staticRec->flags!=0x10||staticRec->cellX!=5||staticRec->cellY!=6)return 207;
        if(idx22.staticColumn(5).size()!=1||!idx22.dynamicColumn(5).empty())return 208;
        if(idx22.dynamicInteractionCode(6*30+5)!=0x8000u)return 209;
        if(idx22.find(-1,6)!=nullptr||idx22.find(5,15)!=nullptr)return 210;

        const auto L0=levels.load(0);
        CellInteractionRuntimeIndex idx0(L0);
        const auto* dynamic8=idx0.find(1,13);
        if(!dynamic8||dynamic8->type!=1||dynamic8->cellX!=1||dynamic8->cellY!=13||dynamic8->frame!=8)return 211;
        if(idx0.dynamicColumn(1).empty()||idx0.dynamicInteractionCode(13*L0.width+1)!=0x8000u)return 212;

        const auto L10=levels.load(10);
        CellInteractionRuntimeIndex idx10(L10);
        const auto* dynamic0=idx10.find(26,93);
        if(!dynamic0||dynamic0->type!=1||dynamic0->frame!=0)return 213;
        if(idx10.dynamicInteractionCode(93*L10.width+26)!=0u)return 214;
    }
    WorldScene pristineScene(L,init,npcs);
    if(!pristineScene.floorResource().has_value()||*pristineScene.floorResource()!=655||pristineScene.ceilingResource().has_value())return 49;
    if(pristineScene.scenery().size()!=110||pristineScene.npcs().size()!=1)return 50;
    if(pristineScene.scenery().front().type!=62||pristineScene.scenery().front().baseResource!=2513||!pristineScene.scenery().front().hasPairedUpper||pristineScene.scenery().front().pairedUpperResource!=2521||pristineScene.scenery().front().pairedVerticalOffsetRows!=0x58)return 51;
    if(pristineScene.npcs().front().baseResource!=315||pristineScene.npcs().front().facingStride!=8||pristineScene.npcs().front().viewCount!=4)return 52;
    // Level 22's boundary slot resolves to resource 147. CODE.2 explicitly returns without drawing it.
    if(!pristineScene.wallMaterials().front().noDrawXLower||!pristineScene.wallMaterials().front().noDrawYLower)return 53;
    if(pristineScene.paletteIndex()!=0||pristineScene.skySelector()!=1||pristineScene.ceilingFallbackColor()!=58||pristineScene.floorFallbackColor()!=155||pristineScene.upperSourceRows()!=96)return 60;
    if(init.skyGroups().size()!=2||init.skyGroups()[0].resourceBase!=8||init.skyGroups()[0].elements.size()!=38||init.skyGroups()[1].resourceBase!=45||init.skyGroups()[1].elements.size()!=27)return 81;
    {
        const auto& sky=init.skyGroups()[1];
        std::size_t decoded=0;
        for(const auto& e:sky.elements){
            if(e.resource==0) continue;
            const auto index=static_cast<std::uint16_t>(e.resource<8?e.resource:e.resource+sky.resourceBase);
            const auto image=resources.skyBitmap(index);
            if(!image.width||!image.height||image.pixels.size()!=std::size_t(image.width)*image.height)return 82;
            ++decoded;
        }
        if(decoded==0)return 83;
    }
    {
        SkyRenderer skyRenderer(init,resources);
        const auto commands=skyRenderer.buildDrawList(1,270,0);
        if(commands.size()<32)return 92;
        for(std::size_t i=0;i<32;++i){
            if(!commands[i].backgroundStrip||commands[i].screenX!=32+static_cast<int>(i)*8||commands[i].screenY!=4)return 93;
            if(commands[i].resource<45||commands[i].resource>51)return 94;
            const auto strip=resources.skyBitmap(commands[i].resource);
            if(strip.width!=8||strip.height!=128)return 95;
        }
        const auto overlayCount=std::count_if(commands.begin(),commands.end(),[](const SkyDrawCommand& c){return !c.backgroundStrip;});
        if(overlayCount==0)return 96;
        ssi::render::IndexedFrameBuffer skyFb(256,256);
        skyFb.clear(0);
        const auto stats=skyRenderer.render(skyFb,1,270,0);
        if(stats.backgroundStrips!=32||stats.panoramaElements!=overlayCount||stats.rasterizedDraws==0)return 97;
        // The sky pass is clipped to the upper 128 rows and must never touch the floor half.
        for(int y=128;y<256;++y)for(int x=0;x<256;++x)if(skyFb.pixels()[std::size_t(y)*256+x]!=0)return 98;
    }
    TrigTable trig(r/"SINTAB");
    if(trig.sin14(0)!=0||trig.sin14(90)!=16384||trig.sin14(270)!=-16384||trig.cos14(0)!=16384)return 99;
    if(trig.planeDistanceTable()[0]!=309||trig.planeDistanceTable()[127]!=6831||trig.planeStartTable()[0]!=293||trig.planeStartTable()[127]!=6489)return 100;
    if(trig.lightingPivot()!=127||trig.lightingTableExtent()!=508||trig.lightingAux()!=128)return 104;
    {
        // CODE.2 0x2DF2C movement-vector helper.
        const auto north=MovementVector2DF2C::compute(trig,0x00002000,0,false);
        if(north.deltaX!=-trig.sinMul16_16(0x00002000,0)||north.deltaY!=trig.cosMul16_16(0x00002000,0))return 247;
        const auto west=MovementVector2DF2C::compute(trig,0x00002000,90,false);
        if(west.deltaX!=-trig.sinMul16_16(0x00002000,90)||west.deltaY!=trig.cosMul16_16(0x00002000,90))return 248;
        const auto clamped=MovementVector2DF2C::compute(trig,0x0000B000,0,false);
        if(clamped.deltaX!=-trig.sinMul16_16(0x0000AFFF,0)||clamped.deltaY!=trig.cosMul16_16(0x0000AFFF,0))return 249;
        const auto snap0=MovementVector2DF2C::compute(trig,0,0,true);
        const auto snap3=MovementVector2DF2C::compute(trig,0,135,true);
        if(snap0.deltaX!=0||snap0.deltaY!=0x00010000)return 250;
        if(snap3.deltaX!=-0x00010000||snap3.deltaY!=-0x00010000)return 251;
    }
    {
        ssi::render::IndexedFrameBuffer floorFb(256,256);
        floorFb.clear(17);
        FloorPlaneRenderer floorRenderer(trig);
        const auto floorStats=floorRenderer.render(floorFb,floor655,4*65536+32768,4*65536+32768,90);
        if(floorStats.rows!=128||floorStats.pixels!=128u*256u)return 101;
        for(int y=0;y<128;++y)for(int x=0;x<256;++x)if(floorFb.pixels()[std::size_t(y)*256+x]!=17)return 102;
        bool changed=false;
        for(int y=128;y<256&&!changed;++y)for(int x=0;x<256;++x)if(floorFb.pixels()[std::size_t(y)*256+x]!=17){changed=true;break;}
        if(!changed)return 103;
    }
    const auto& rc=trig.rendererConstants();
    if(rc.tableWidthA!=256||rc.tableWidthB!=256||rc.screenCenterX!=127||rc.viewportConstantA!=328||rc.viewportConstantB!=328||rc.cameraOffset!=22272||rc.perspectiveScale!=175||rc.depthScaleNumerator!=179200)return 71;
    ProjectionSystem projection(trig);
    // At heading 270 the original transform's forward vector is -X.  A point
    // exactly one world unit ahead must land on the SINTAB center with the
    // executable's 0x2000 near-plane bias and 8.8 scale calculation.
    const auto straight=projection.project(3*65536+32768,4*65536+32768,4*65536+32768,4*65536+32768,270);
    if(!straight.visible||straight.screenX!=127||straight.depthFixed!=0xDFFF||straight.scale8_8!=803)return 72;
    {
        const auto probeSprite=resources.sprite(2513);
        const auto probeRect=SpriteRasterGeometry::compute(straight.screenX,straight.scale8_8,probeSprite);
        // 0x21BF0 first reduces 803 >> 3 = 100 before its size math.
        const auto expectedFactor=(100-(100/8))*4;
        if(probeRect.scaleFactor!=expectedFactor)return 89;
    }
    const auto behind=projection.project(5*65536+32768,4*65536+32768,4*65536+32768,4*65536+32768,270);
    if(behind.visible)return 73;
    {
        // CODE.2 0xDACC/0xDB94 previous-render-buffer NPC auto-targeting.
        std::array<RuntimePlacement22,3> actors{};
        for(auto& a:actors)a.state=0x10000000u; // +0x0B bit 4 targetable, bit 1 clear
        std::array<PlayerTargetRecordDACC,3> records{{
            {100,static_cast<std::uint16_t>(0x7D00+0x0200),0,0,0},
            {150,static_cast<std::uint16_t>(0x7D00+0x0300),0,1,2},
            {120,static_cast<std::uint16_t>(0x7D00+0x0280),1,2,1}
        }};
        if(PlayerTargetSelectorDACC::select(0,records,actors)!=0)return 440;
        if(PlayerTargetSelectorDACC::select(1,records,actors)!=1)return 441;
        if(PlayerAutoTargetDB94::select(0,records,actors)!=0)return 442;
        if(PlayerAutoTargetDB94::select(1,records,actors)!=1)return 443;

        // Threshold is exactly ((biasedScale-0x7D00)>>3) >= 0x3B.
        records[0].biasedScale=static_cast<std::uint16_t>(0x7D00+0x01D7);
        if(PlayerTargetSelectorDACC::select(0,records,actors)!=-1)return 444;
        records[0].biasedScale=static_cast<std::uint16_t>(0x7D00+0x01D8);
        if(PlayerTargetSelectorDACC::select(0,records,actors)!=0)return 445;

        // Suppressed/dead (+0x0B bit 1) and non-targetable (+0x0B bit 4 clear) actors are rejected.
        actors[0].state=0x12000000u;
        if(PlayerTargetSelectorDACC::select(0,records,actors)!=-1)return 446;
        actors[0].state=0;
        if(PlayerTargetSelectorDACC::select(0,records,actors)!=-1)return 447;
    }
    {
        ssi::render::IndexedFrameBuffer fb(4,2);fb.clear(155);fb.set(1,0,58);
        auto rgb=fb.toRgb(colors.at(pristineScene.paletteIndex()));
        if(rgb.size()!=24)return 61;
        const auto c58=colors.at(0)[58],c155=colors.at(0)[155];
        if(rgb[3]!=c58.r||rgb[4]!=c58.g||rgb[5]!=c58.b)return 62;
        if(rgb[0]!=c155.r||rgb[1]!=c155.g||rgb[2]!=c155.b)return 63;
    }

    const auto quickStartSavePath = std::filesystem::exists(r/"SAVE01"/"SAVE.DAT") ? (r/"SAVE01"/"SAVE.DAT") : (r/"SAVE.DAT");
    const auto quickStartWorkPath = std::filesystem::exists(r/"SAVE01"/"LEV022.DAT") ? (r/"SAVE01"/"LEV022.DAT") : (r/"LEV022.DAT");
    if(std::filesystem::exists(quickStartSavePath) && std::filesystem::exists(quickStartWorkPath)){
        SaveGame save(quickStartSavePath);auto const&q=save.quickStart();
        if(q.description!="QUICK START PARTY; ELVEN F/M - DWARVEN F/C"||q.level!=22||q.partyX()!=4.5||q.partyY()!=4.5||q.facingDegrees!=270)return 8;
        if(q.timerF58C!=1899u||q.gameplayClockA7E4!=1735u)return 463;
        if(q.worldClock!=0x000E0000u||q.savedBaseDarkness!=0||q.ambientLightModifier!=0)return 105;
        {
            // CODE.2 0x67D4..0x6C55 exact SAVE.DAT writer ordering. An
            // untouched shipped Quick Start decode must serialize byte-for-byte
            // identically, including all address-named opaque tail blocks.
            const auto originalBytes=ssi::core::readFile(quickStartSavePath);
            const auto encoded=SaveGame::encode(q);
            if(encoded!=originalBytes)return 501;
            if(q.heldObjectId!=-1||q.heldObjectState!=0||
               q.dynamicEntityCount!=q.dynamicEntities.size())return 502;

            // The writer is variable-sized by exactly 0x16 bytes per DS:C038
            // dynamic record. Mutate fields spread across each proven writer
            // region, write them, and require the original loader mapping to
            // recover them without normalizing raw character/container state.
            auto mutated=q;
            mutated.partyXFixed+=0x1234;
            mutated.partyYFixed-=0x2345;
            mutated.facingDegrees=90;
            mutated.timerF58C+=7;
            mutated.gameplayClockA7E4+=9;
            mutated.heldObjectId=136;
            mutated.heldObjectState=1;
            mutated.party[0].raw[0x1C]=0x21;
            mutated.party[0].raw[0x1D]=0;
            mutated.containerItems[0][0]=233;
            mutated.containerState[0]^=1u;
            mutated.savedBaseDarkness=4;
            mutated.ambientLightModifier=-2;
            SaveDynamicRecord dyn{};
            for(std::size_t i=0;i<dyn.size();++i)dyn[i]=static_cast<std::uint8_t>(i*7u+3u);
            mutated.dynamicEntities.push_back(dyn);
            mutated.dynamicEntityCount=1;
            const auto dynamicBytes=SaveGame::encode(mutated);
            if(dynamicBytes.size()!=originalBytes.size()+kSaveDynamicRecordSize)return 503;

            const auto temp=std::filesystem::temp_directory_path()/"ravenloft_save_writer_0226.dat";
            SaveGame::write(temp,mutated);
            SaveGame reread(temp);
            std::filesystem::remove(temp);
            const auto& m=reread.quickStart();
            if(m.partyXFixed!=mutated.partyXFixed||m.partyYFixed!=mutated.partyYFixed||
               m.facingDegrees!=90||m.timerF58C!=mutated.timerF58C||
               m.gameplayClockA7E4!=mutated.gameplayClockA7E4)return 504;
            if(m.heldObjectId!=136||m.heldObjectState!=1||m.party[0].currentHitPoints!=33)return 505;
            if(m.containerItems[0][0]!=233||m.containerState[0]!=mutated.containerState[0]||
               m.savedBaseDarkness!=4||m.ambientLightModifier!=-2)return 506;
            if(m.dynamicEntityCount!=1||m.dynamicEntities.size()!=1||m.dynamicEntities[0]!=dyn)return 507;

            // Live demo SAVE.DAT snapshots use current party/cursor/container and
            // reconstructed F58C/A7E4 state rather than the immutable boot copy.
            OpeningDemoSession liveSave(r,0x1234u);
            liveSave.pulseOriginalTimerIrq();
            (void)liveSave.moveOriginalAxes(20,0,0);
            liveSave.resolveAssassinDeath();
            const auto drop=std::find_if(liveSave.runtimeGroundObjects().begin(),liveSave.runtimeGroundObjects().end(),
                [](const RuntimePlacement22& o){return o.type==136;});
            if(drop==liveSave.runtimeGroundObjects().end())return 508;
            const auto dropIndex=static_cast<std::size_t>(std::distance(liveSave.runtimeGroundObjects().begin(),drop));
            if(!liveSave.pickupGroundObject(dropIndex,1).picked)return 509;
            const auto liveState=liveSave.snapshotSaveDatState();
            if(liveState.timerF58C!=1900u||liveState.gameplayClockA7E4!=1736u||
               liveState.worldClock!=q.worldClock||
               liveState.partyXFixed!=liveSave.party().position.x||liveState.partyYFixed!=liveSave.party().position.y||
               liveState.facingDegrees!=liveSave.party().headingDegrees||
               liveState.heldObjectId!=liveSave.heldObject().objectId||
               liveState.heldObjectState!=liveSave.heldObject().state)return 510;
            if(liveSave.encodeSaveDat()!=SaveGame::encode(liveState))return 511;
        }
        LightingSystem lightingSystem(trig);
        const auto openingLight=lightingSystem.fromQuickStart(q);
        if(openingLight.hour!=14||openingLight.baseDarkness!=0||openingLight.ambientModifier!=0||openingLight.partyLightOverride!=0||openingLight.skyShade!=0)return 106;
        if(openingLight.distanceRemap.size()!=509)return 107;
        if(!std::all_of(openingLight.floorShade.begin(),openingLight.floorShade.end(),[](std::uint8_t v){return v==1;}))return 108;
        if(LightingSystem::hourlyDarkness(0)!=10||LightingSystem::hourlyDarkness(7)!=9||LightingSystem::hourlyDarkness(14)!=0||LightingSystem::hourlyDarkness(23)!=10)return 109;
        {
            WorldScene renderScene(L,init,npcs);
            // Apply the mutable Quick Start assassin position just as the demo session does.
            WorldScene liveRenderScene(L,init,npcs,OpeningDemoSession(r).assassin());
            WorldRenderer worldRenderer(init,resources,trig);
            ssi::render::IndexedFrameBuffer worldFb(256,256);
            const auto ws=worldRenderer.render(worldFb,liveRenderScene,q.partyXFixed,q.partyYFixed,90,openingLight);
            if(ws.sky.backgroundStrips!=32||ws.sky.rasterizedDraws==0||ws.floor.rows!=128||ws.floor.pixels!=32768||ws.sprites.rasterizedDraws==0)return 110;
            // Both halves must contain rendered original data, not the clear fallback alone.
            bool upper=false,lower=false;
            for(int y=0;y<128&&!upper;++y)for(int x=0;x<256;++x)if(worldFb.pixels()[std::size_t(y)*256+x]!=liveRenderScene.floorFallbackColor()){upper=true;break;}
            for(int y=128;y<256&&!lower;++y)for(int x=0;x<256;++x)if(worldFb.pixels()[std::size_t(y)*256+x]!=liveRenderScene.floorFallbackColor()){lower=true;break;}
            if(!upper||!lower)return 111;

            // Bind the production renderer to live PartyState: one original turn
            // step must alter the composed first-person frame without changing
            // any world placements or renderer-side camera state.
            OpeningDemoSession liveCamera(r);
            ssi::render::IndexedFrameBuffer beforeFb(256,256),afterFb(256,256);
            worldRenderer.render(beforeFb,liveRenderScene,liveCamera.party(),openingLight);
            liveCamera.moveOriginalAxes(20,0,0);
            worldRenderer.render(afterFb,liveRenderScene,liveCamera.party(),openingLight);
            if(std::equal(beforeFb.pixels().begin(),beforeFb.pixels().end(),afterFb.pixels().begin()))return 117;
        }
        if(!q.party[0].active()||q.party[0].name!="EHLIANNA"||q.party[0].race!=5||q.party[0].classes[0]!=0||q.party[0].classes[1]!=4)return 9;
        if(!q.party[1].active()||q.party[1].name!="ANVIL DAWN"||q.party[1].race!=4||q.party[1].classes[0]!=0||q.party[1].classes[1]!=3)return 10;
        if(q.party[0].carriedItems[0]!=166||q.party[1].carriedItems[0]!=60)return 11;
        if(q.party[0].currentHitPoints!=38||q.party[0].maximumHitPoints!=38)return 134;
        if(q.party[1].currentHitPoints!=55||q.party[1].maximumHitPoints!=55)return 135;
        {
            // CODE.2 0xFD84/0xFE44: shipped Quick Start carried/equipped
            // object weights and exact Strength threshold tiers.
            const auto cw=[&](int index){return ContainerWeightF5F8::evaluate(index,objects,q);};
            if(CharacterDefenseHelpers::carriedWeightFD84(q.party[0],objects,cw)!=15)return 385;
            if(CharacterDefenseHelpers::carriedWeightFD84(q.party[1],objects,cw)!=57)return 386;
            if(CharacterDefenseHelpers::encumbranceTierFE44(q.party[0],objects,cw)!=0||
               CharacterDefenseHelpers::encumbranceTierFE44(q.party[1],objects,cw)!=0)return 387;

            // C7C4 and E5F0 Strength helpers use different original tables.
            if(PlayerHitTestC80C::strengthHitAdjustmentC7C4(q.party[0])!=1||
               PlayerHitTestC80C::strengthHitAdjustmentC7C4(q.party[1])!=3)return 388;
            if(PlayerNpcAttackE5F0::strengthDamageModifier(q.party[0])!=1||
               PlayerNpcAttackE5F0::strengthDamageModifier(q.party[1])!=6)return 389;
            if(PlayerNpcAttackE5F0::recoveryDuration(252,q.party[0],objects)!=45||
               PlayerNpcAttackE5F0::recoveryDuration(256,q.party[1],objects)!=36)return 390;

            // CODE.2 0xC80C against the actual type-9 assassin. Seed 1 gives
            // an 11: Ehliana needs 10 and hits. Seed 2 gives exactly 1 and
            // takes the original forced-miss branch.
            RuntimePlacement22 target{};target.type=9;
            PlayerHitTestContextC80C hc{};hc.containerWeightF5F8=cw;
            OriginalRng30CA2 hitRng(1);
            const auto hit=PlayerHitTestC80C::evaluate(0,q.party[0],target,npcs.at(9),252,true,objects,hitRng,hc);
            if(!hit.hit||hit.roll!=11||hit.required!=10||hit.encumbranceTier!=0||hit.strengthAdjustment!=1)return 391;
            OriginalRng30CA2 missRng(2);
            const auto miss=PlayerHitTestC80C::evaluate(0,q.party[0],target,npcs.at(9),252,true,objects,missRng,hc);
            if(miss.hit||miss.roll!=1||miss.required!=10)return 392;
            if(PlayerDamageModifierC910::adjust(7,npcs.at(9),252,objects)!=7||
               PlayerDamageModifierC910::adjust(7,npcs.at(9),256,objects)!=7)return 393;
        }
        {
            // CODE.2 0x214B8 normal click-1 attack gate: inactive/status/click
            // gating, 0x24478 usability, then literal target -1 into 0x26018.
            auto party=q.party;
            HandItemUsabilityContext24478 uc{};uc.party=&party;uc.global258A8=0xFF;
            int calls=0,lastTarget=99;
            PlayerHandClickContext214B8 cc{};
            cc.usability24478=uc;
            cc.attack26018=[&](int target){++calls;lastTarget=target;return PlayerHandAttackResult26018{};};
            auto r0=PlayerHandClick214B8::execute(party,0,0,0,objects,cc);
            if(r0.path!=PlayerHandClickResult214B8::Path::IgnoredClick||calls!=0)return 448;
            auto r8=PlayerHandClick214B8::execute(party,0,0,8,objects,cc);
            if(r8.path!=PlayerHandClickResult214B8::Path::ItemExchangeBranch||calls!=0)return 449;
            party[0].raw[0x11]|=0x80u;
            auto inactive=PlayerHandClick214B8::execute(party,0,0,1,objects,cc);
            if(inactive.path!=PlayerHandClickResult214B8::Path::InactiveCharacter||calls!=0)return 450;
            party=q.party;cc.usability24478.party=&party;
            party[0].raw[0x11]|=0x02u; // status word 0x0200 intersects 0xF240
            auto blocked=PlayerHandClick214B8::execute(party,0,0,1,objects,cc);
            if(blocked.path!=PlayerHandClickResult214B8::Path::CharacterStateBlocked||calls!=0)return 451;
            party=q.party;cc.usability24478.party=&party;
            auto fired=PlayerHandClick214B8::execute(party,0,0,1,objects,cc);
            if(fired.path!=PlayerHandClickResult214B8::Path::AttackDispatched||calls!=1||lastTarget!=-1)return 452;
        }
        {
            // E5F0 must resolve literal target -1 through DB94 before attack
            // state clear/hit testing. Exercise that exact integration with a
            // synthetic target using the shipped Quick Start attacker.
            auto party=q.party;
            std::array<RuntimePlacement22,1> actors{};
            actors[0].type=9;actors[0].hitPoints=100;actors[0].state=0x10000000u;
            OriginalRng30CA2 rng(1);
            std::uint32_t tick=100;
            PlayerHandAttackContext26018 ac{};
            ac.melee.currentTick19CC4=[&](){return tick;};
            ac.melee.global258A8=0xFF;
            ac.melee.hitTest.containerWeightF5F8=[&](int index){return ContainerWeightF5F8::evaluate(index,objects,q);};
            int targetCalls=0;
            ac.melee.autoTargetDB94=[&](std::size_t ci){++targetCalls;return ci==0?0:-1;};
            const auto ar=PlayerHandAttack26018::execute(party,0,0,actors,npcs,-1,objects,rng,ac);
            if(ar.path!=PlayerHandAttackResult26018::Path::Melee||targetCalls!=1||ar.melee.targetNpcIndex!=0||
               ar.melee.path!=PlayerNpcAttackResultE5F0::Path::Hit||actors[0].hitPoints>=100)return 453;
        }
        {
            // Live opening click path at the shipped initial camera state. The
            // assassin begins behind/outside DB94 range, so normal click-1 is
            // dispatched but correctly resolves to NoTarget rather than using
            // the old hard-coded actor index.
            OpeningDemoSession autoDemo(r);
            std::uint32_t tick=100;auto clock=[&](){return tick;};
            const auto click=autoDemo.attackOpeningHand(0,0,1,clock);
            if(click.path!=PlayerHandClickResult214B8::Path::AttackDispatched||
               click.attack.path!=PlayerHandAttackResult26018::Path::Melee||
               click.attack.melee.path!=PlayerNpcAttackResultE5F0::Path::NoTarget||
               click.attack.melee.targetNpcIndex!=-1||autoDemo.assassin().hitPoints!=15)return 454;
        }
        {
            // Live production click route can kill the shipped assassin without
            // the development resolveAssassinDeath() helper. Approach him through
            // the reconstructed movement controller, face him, then use only
            // 0x214B8 -> 0x24478 -> 0x26018 -> 0xE5F0 -> 0xE284.
            OpeningDemoSession liveCombat(r);
            for(int i=0;i<8;++i)liveCombat.moveOriginalAxes(-200,0,0); // 270 -> 350
            for(int i=0;i<18;++i)liveCombat.moveOriginalAxes(0,-400,0);
            for(int i=0;i<11;++i)liveCombat.moveOriginalAxes(-200,0,0); // 350 -> 100 view heading
            if(liveCombat.assassin().hitPoints!=15)return 469;
            const int expectedHp[5]={12,12,3,3,0};
            const int expectedDamage[5]={3,0,9,0,3};
            const PlayerNpcAttackResultE5F0::Path expectedPath[5]={
                PlayerNpcAttackResultE5F0::Path::Hit,
                PlayerNpcAttackResultE5F0::Path::Miss,
                PlayerNpcAttackResultE5F0::Path::Hit,
                PlayerNpcAttackResultE5F0::Path::Miss,
                PlayerNpcAttackResultE5F0::Path::Hit};
            for(int i=0;i<5;++i){
                const auto click=liveCombat.attackOpeningHand(0,0,1);
                if(click.path!=PlayerHandClickResult214B8::Path::AttackDispatched||
                   click.attack.path!=PlayerHandAttackResult26018::Path::Melee||
                   click.attack.melee.targetNpcIndex!=0||
                   click.attack.melee.path!=expectedPath[i]||
                   click.attack.melee.damage!=expectedDamage[i]||
                   liveCombat.assassin().hitPoints!=expectedHp[i])return 470+i;
                for(int tick=0;tick<60;++tick)liveCombat.pulseOriginalTimerIrq();
                liveCombat.tickPlayerAttackTimers(liveCombat.gameplayClock().currentTick19CC4());
            }
            if(liveCombat.runtimeGroundObjects().size()!=4||liveCombat.eventStateBF44()!=1||
               ((liveCombat.assassin().state>>24u)&0x02u)==0u)return 475;
            const std::uint16_t expectedLiveDrops[4]={136,135,233,116};
            for(std::size_t i=0;i<4;++i)
                if(liveCombat.runtimeGroundObjects()[i].type!=expectedLiveDrops[i])return 476;
        }
        {
            // Opening ground-object hit-selection now starts from actual screen
            // coordinates, not an object id. The records are reconstructed from
            // the real four assassin drops through 0x35798/0x2B2F4/0x21BF0
            // geometry, then 0x223F4 selects the topmost hit and feeds its
            // runtime source index into the already-converted 0x21704 pickup.
            OpeningDemoSession clickPickup(r);
            for(int i=0;i<8;++i)clickPickup.moveOriginalAxes(-200,0,0);
            for(int i=0;i<18;++i)clickPickup.moveOriginalAxes(0,-400,0);
            for(int i=0;i<11;++i)clickPickup.moveOriginalAxes(-200,0,0);
            clickPickup.resolveAssassinDeath();
            if(clickPickup.runtimeGroundObjects().size()!=4)return 487;
            std::array<bool,336> seen{};
            TrigTable pickupTrig(r/"SINTAB");
            for(int remaining=4;remaining>0;--remaining){
                std::optional<std::pair<int,int>> clickPoint;
                std::optional<std::size_t> selected;
                int faceGuard=0;
                while(!clickPoint && faceGuard++<36){
                    const auto hit=OpeningGroundObjectHit223F4::build(
                        clickPickup.runtimeGroundObjects(),objects,resources,pickupTrig,clickPickup.party());
                    for(auto it=hit.rbegin();it!=hit.rend();++it){
                        const int l=std::max<int>(0,it->left), rgt=std::min<int>(255,it->right);
                        const int t=std::max<int>(0,it->top), b=std::min<int>(255,it->bottom);
                        if(l>rgt||t>b)continue;
                        const int sx=(l+rgt)/2, sy=(t+b)/2;
                        auto candidate=OpeningGroundObjectHit223F4::select(sx,sy,1,hit);
                        if(candidate){clickPoint=std::pair<int,int>{sx,sy};selected=candidate;break;}
                    }
                    if(!clickPoint)clickPickup.moveOriginalAxes(-200,0,0);
                }
                if(!clickPoint||!selected)return 488;
                const auto expectedId=clickPickup.runtimeGroundObjects().at(*selected).type;
                if(expectedId>=seen.size()||seen[expectedId])return 490;
                const auto picked=clickPickup.pickupGroundObjectAtScreen(clickPoint->first,clickPoint->second,1,resources);
                if(!picked.picked||picked.objectId!=expectedId)return 491;
                seen[expectedId]=true;
                if(remaining>1){
                    if(picked.startMistTransition)return 492;
                    // Stow through the original mouse-selected inventory-grid path,
                    // not the first-free-slot development helper. Find an empty
                    // slot only to choose a legal click coordinate for this test.
                    std::optional<std::pair<std::size_t,std::size_t>> emptySlot;
                    const auto& partyRecords=clickPickup.partyRecords();
                    for(std::size_t ci=0;ci<partyRecords.size()&&!emptySlot;++ci){
                        const auto status=static_cast<std::uint16_t>(partyRecords[ci].raw[0x10])|
                                          static_cast<std::uint16_t>(partyRecords[ci].raw[0x11]<<8u);
                        if((status&0x8000u)!=0)continue;
                        for(std::size_t slot=0;slot<12;++slot){
                            const auto off=0x40+slot*2;
                            const auto id=static_cast<std::uint16_t>(partyRecords[ci].raw[off])|
                                          static_cast<std::uint16_t>(partyRecords[ci].raw[off+1]<<8u);
                            if(id==0xFFFFu){emptySlot=std::pair{ci,slot};break;}
                        }
                    }
                    if(!emptySlot)return 493;
                    const auto [ci,slot]=*emptySlot;
                    const int ix=InventoryGridClick11FB0::kLeft+static_cast<int>(slot/4)*InventoryGridClick11FB0::kColumnPitch;
                    const int iy=static_cast<int>(ci)*InventoryGridClick11FB0::kPanelHeight+
                                 InventoryGridClick11FB0::kTop+static_cast<int>(slot%4)*InventoryGridClick11FB0::kRowPitch;
                    const auto invClick=clickPickup.clickInventoryAtOriginalScreen(ix,iy,1);
                    if(!invClick.handled||!invClick.exchangeHandled||clickPickup.heldObject().objectId!=-1)return 523;
                } else if(!picked.startMistTransition||!clickPickup.mistTransitionActive())return 494;
            }
            if(!seen[116]||!seen[135]||!seen[136]||!seen[233]||!clickPickup.runtimeGroundObjects().empty())return 495;
        }
        {
            // Full production opening attack chain: 0x26018 -> 0xE5F0 ->
            // C80C/1C328/C910 -> E284. Cooldown expiry is driven through the
            // converted 0x1E4A0 timer service, never by editing the hand field.
            OpeningDemoSession combatDemo(r);
            std::uint32_t tick=100;
            auto clock=[&](){return tick;};
            auto read32=[](const CharacterRecord& c,std::size_t o){
                return static_cast<std::uint32_t>(c.raw[o])|
                       (static_cast<std::uint32_t>(c.raw[o+1])<<8u)|
                       (static_cast<std::uint32_t>(c.raw[o+2])<<16u)|
                       (static_cast<std::uint32_t>(c.raw[o+3])<<24u);
            };
            const auto a1=combatDemo.attackOpeningAssassin(0,0,clock);
            if(a1.path!=PlayerHandAttackResult26018::Path::Melee||
               a1.itemId!=252||a1.itemType!=10||
               a1.melee.path!=PlayerNpcAttackResultE5F0::Path::Hit||
               a1.melee.hitRoll!=11||a1.melee.damage!=3||combatDemo.assassin().hitPoints!=12)return 394;
            if(read32(combatDemo.partyRecords()[0],0x10B)!=145u||
               read32(combatDemo.partyRecords()[0],0x19E)!=122u)return 395;
            const auto blocked=combatDemo.attackOpeningAssassin(0,0,clock);
            if(blocked.path!=PlayerHandAttackResult26018::Path::CoolingDown||combatDemo.assassin().hitPoints!=12)return 396;

            tick=145;combatDemo.tickPlayerAttackTimers(tick);
            if(read32(combatDemo.partyRecords()[0],0x10B)!=0u||read32(combatDemo.partyRecords()[0],0x19E)!=0u)return 397;
            const auto a2=combatDemo.attackOpeningAssassin(0,0,clock);
            if(a2.melee.path!=PlayerNpcAttackResultE5F0::Path::Miss||a2.melee.hitRoll!=7||combatDemo.assassin().hitPoints!=12)return 398;

            tick=190;combatDemo.tickPlayerAttackTimers(tick);
            const auto a3=combatDemo.attackOpeningAssassin(0,0,clock);
            if(a3.melee.path!=PlayerNpcAttackResultE5F0::Path::Hit||a3.melee.hitRoll!=11||a3.melee.damage!=9||combatDemo.assassin().hitPoints!=3)return 399;

            tick=235;combatDemo.tickPlayerAttackTimers(tick);
            const auto a4=combatDemo.attackOpeningAssassin(0,0,clock);
            if(a4.melee.path!=PlayerNpcAttackResultE5F0::Path::Miss||a4.melee.hitRoll!=4||combatDemo.assassin().hitPoints!=3)return 400;

            tick=280;combatDemo.tickPlayerAttackTimers(tick);
            const auto a5=combatDemo.attackOpeningAssassin(0,0,clock);
            if(a5.melee.path!=PlayerNpcAttackResultE5F0::Path::Hit||a5.melee.hitRoll!=15||a5.melee.damage!=3||!a5.melee.died)return 401;
            if(combatDemo.assassin().hitPoints!=0||combatDemo.groundObjects().size()!=4||
               combatDemo.runtimeGroundObjects().size()!=4||combatDemo.eventStateBF44()!=1)return 402;
            const std::uint16_t attackDropIds[4]={136,135,233,116};
            for(std::size_t i=0;i<4;++i)if(combatDemo.groundObjects()[i].objectId!=attackDropIds[i])return 403;
            if(((combatDemo.assassin().state>>24u)&0x02u)==0)return 404;
        }
        {
            // Production session owns the saved gameplay clocks and preserves
            // frame order: 0x1E4A0 timer service before 0x1F1C8/0x1BDAC.
            OpeningDemoSession timedDemo(r,0x1234u);
            if(timedDemo.gameplayClock().timer30700()!=1899u||
               timedDemo.gameplayClock().currentTick19CC4()!=1735u||
               timedDemo.gameplayClock().biosTick311E0()!=0x1234u)return 464;
            timedDemo.pulseOriginalTimerIrq();
            if(timedDemo.gameplayClock().timer30700()!=1900u||
               timedDemo.gameplayClock().currentTick19CC4()!=1736u||
               timedDemo.gameplayClock().biosTick311E0()!=0x1235u)return 465;

            // Live hand attack now consumes the session-owned 0x19CC4 value;
            // the cooldown deadline must be based on 1736, not a test tick.
            const auto attack=timedDemo.attackOpeningAssassin(0,0);
            if(attack.path!=PlayerHandAttackResult26018::Path::Melee)return 466;
            auto read32=[](const CharacterRecord& c,std::size_t o){
                return static_cast<std::uint32_t>(c.raw[o])|
                       (static_cast<std::uint32_t>(c.raw[o+1])<<8u)|
                       (static_cast<std::uint32_t>(c.raw[o+2])<<16u)|
                       (static_cast<std::uint32_t>(c.raw[o+3])<<24u);
            };
            if(read32(timedDemo.partyRecords()[0],0x10B)!=1781u)return 467;

            // Advance the IRQ clock to the exact deadline, then one gameplay
            // frame must run 1E4A0 first and clear the expired hand timer.
            while(timedDemo.gameplayClock().currentTick19CC4()<1781u)timedDemo.pulseOriginalTimerIrq();
            timedDemo.tickOriginalGameplayFrame();
            if(read32(timedDemo.partyRecords()[0],0x10B)!=0u)return 468;
        }
        {
            // Live main-loop integration of 0x1E75C plus SAVE.DAT CF4C.
            OpeningDemoSession worldDemo(r,0u);
            if(worldDemo.worldClock().valueCf4c()!=0x000E0000u)return 531;
            for(std::uint32_t i=0;i<OriginalWorldClock1E75C::kGameplayTicksPerHour;++i)
                worldDemo.pulseOriginalTimerIrq();
            worldDemo.tickOriginalGameplayFrame();
            if(worldDemo.worldClock().valueCf4c()!=0x000F0000u)return 532;
            if(worldDemo.snapshotSaveDatState().worldClock!=0x000F0000u)return 533;
        }
        {
            AdventureHudModel hud(q.party);
            const auto& hc=hud.characters();
            if(!hc[0].active||hc[0].name!="EHLIANNA"||hc[0].currentHitPoints!=38||hc[0].maximumHitPoints!=38||hc[0].carriedItemCount!=8)return 136;
            if(!hc[1].active||hc[1].name!="ANVIL DAWN"||hc[1].currentHitPoints!=55||hc[1].maximumHitPoints!=55||hc[1].carriedItemCount!=2)return 137;
            if(hc[2].active||hc[3].active)return 138;
        }

        OpeningDemoSession demo(r);
        {
            // CODE.2 0x1BBD8 type-9 branch: scripted eastward whole-cell step,
            // preserving the fractional X word and bypassing 0x2D5D0 entirely.
            auto a=demo.assassin();
            const auto oldFrac=static_cast<std::uint16_t>(a.xFixed);
            int decisions=0,collisions=0;
            NpcMovementCallbacks1BBD8 cb;
            cb.decide1B760=[&](RuntimePlacement22& x,int mover,int retry){
                if(mover==0&&retry==0)++decisions;
                x.field14=static_cast<std::uint16_t>(x.field14|1u);
            };
            cb.collision2D5D0=[&](std::int32_t,std::int32_t,int){++collisions;return 0;};
            NpcMovementTransaction1BBD8::execute(a,npcs.at(a.type),0,demo.workLevel().width,trig,false,cb);
            if(decisions!=1||collisions!=0)return 252;
            if(static_cast<std::int16_t>(static_cast<std::uint32_t>(a.xFixed)>>16u)!=19||static_cast<std::uint16_t>(a.xFixed)!=oldFrac)return 253;
            if((a.field14&1u)!=0||a.field0C!=static_cast<std::uint16_t>(7*demo.workLevel().width+19))return 254;

            // Generic branch: four rejected candidates cause retries 0..3,
            // then clear runtime state bit 0x10 and the movement bit.
            RuntimePlacement22 g{};g.type=0;g.xFixed=0x000A8000;g.yFixed=0x000A8000;g.state=0x10;g.field14=0;
            int retryCalls=0,collisionCalls=0;
            cb.decide1B760=[&](RuntimePlacement22& x,int,int retry){
                if(retry!=retryCalls)return;
                ++retryCalls;
                x.field14=static_cast<std::uint16_t>((x.field14&~0x39u)|1u); // direction 0, phase 0, move
            };
            cb.collision2D5D0=[&](std::int32_t,std::int32_t,int){++collisionCalls;return 0;};
            NpcMovementTransaction1BBD8::execute(g,npcs.at(0),0,demo.workLevel().width,trig,false,cb);
            if(retryCalls!=4||collisionCalls!=4||(g.state&0x10u)!=0||(g.field14&1u)!=0)return 255;

            // Any nonzero collision return commits, including -1.
            RuntimePlacement22 c{};c.type=0;c.xFixed=0x000A8000;c.yFixed=0x000A8000;c.state=0x10;c.field14=0;
            const auto beforeX=c.xFixed,beforeY=c.yFixed;int acceptedQueries=0;
            cb.decide1B760=[&](RuntimePlacement22& x,int,int){x.field14=static_cast<std::uint16_t>((x.field14&~0x39u)|1u);};
            cb.collision2D5D0=[&](std::int32_t,std::int32_t,int){++acceptedQueries;return -1;};
            NpcMovementTransaction1BBD8::execute(c,npcs.at(0),0,demo.workLevel().width,trig,false,cb);
            if(acceptedQueries!=1||(c.xFixed==beforeX&&c.yFixed==beforeY)||(c.field14&1u)!=0)return 256;
        }
        {
            // CODE.2 0x1BDAC primary NPC scheduler: countdown, action-state
            // dispatch, D550 promotion, recovery delay, and live 1BBD8 caller.
            auto base=demo.assassin();
            std::array<RuntimePlacement22,1> actors{{base}};
            NpcSchedulerContext1BDAC sc{};
            OriginalRng30CA2 srng(1);

            // Nonzero +0x10 is decremented and bypasses all action work.
            actors[0].field10=2;
            NpcScheduler1BDAC::execute(actors,npcs,srng,sc);
            if(actors[0].field10!=1||actors[0].xFixed!=base.xFixed)return 360;

            // Action 2 is the fixed 7-tick post-attack recovery state and
            // clears bits 1..2 back to action 0.
            actors[0]=base;actors[0].field10=0;
            actors[0].field14=static_cast<std::uint16_t>((actors[0].field14&0xFFF9u)|0x0004u);
            NpcScheduler1BDAC::execute(actors,npcs,srng,sc);
            if(actors[0].field10!=7||((actors[0].field14>>1u)&3u)!=0)return 361;

            // Action 1 promotes to state 2 before D550. Put the assassin in
            // reach and guarantee a non-natural-1 hit through the converted
            // armor boundary; the real NPCS.DAT damage definition is used.
            actors[0]=base;actors[0].field10=0;actors[0].xFixed=q.partyXFixed;actors[0].yFixed=q.partyYFixed;
            actors[0].field14=static_cast<std::uint16_t>((actors[0].field14&0xFFC1u)|(1u<<3u)|0x0002u);
            auto liveParty=q.party;const auto hpBefore=liveParty[0].currentHitPoints+liveParty[1].currentHitPoints;
            sc.attack.party=&liveParty;sc.attack.partyXFixed=q.partyXFixed;sc.attack.partyYFixed=q.partyYFixed;
            sc.attack.timer30700=[](){return 1u;};sc.attack.characterArmorClassEFA0=[](std::size_t){return 100;};
            sc.attack.query14A60=[](int){return -1;};sc.attack.predicateCEE4=[](std::size_t){return 0;};
            sc.attack.damageContext.currentTick19CC4=[](){return 10u;};
            NpcScheduler1BDAC::execute(actors,npcs,srng,sc);
            const auto hpAfter=liveParty[0].currentHitPoints+liveParty[1].currentHitPoints;
            if(((actors[0].field14>>1u)&3u)!=2||actors[0].field10!=0||hpAfter>=hpBefore)return 362;

            // The shipped Quick Start starts the assassin more than 13 cells
            // from the party. 0x1B760 therefore changes its proximity state and
            // schedules action 1 without moving; 0x1BBD8 never reaches 2D5D0.
            actors[0]=base;actors[0].field10=0;liveParty=q.party;
            sc.trig=&trig;sc.levelWidth=demo.workLevel().width;sc.decision.party=&liveParty;
            sc.decision.levelCells=demo.workLevel().cells;sc.decision.partyXFixed=q.partyXFixed;sc.decision.partyYFixed=q.partyYFixed;
            sc.decision.levelWidth=static_cast<std::int16_t>(demo.workLevel().width);sc.decision.levelHeight=static_cast<std::int16_t>(demo.workLevel().height);
            std::uint32_t bios=100;sc.decision.biosTick311E0=[&](){return bios++;};
            int schedulerCollisions=0;sc.collision2D5D0=[&](std::int32_t,std::int32_t,int){++schedulerCollisions;return 1;};
            const auto oldX=actors[0].xFixed,oldY=actors[0].yFixed;
            NpcScheduler1BDAC::execute(actors,npcs,srng,sc);
            if(actors[0].xFixed!=oldX||actors[0].yFixed!=oldY||schedulerCollisions!=0||
               ((actors[0].field14>>1u)&3u)!=1)return 363;
        }
        {
            // Production OpeningDemoSession integration: the shipped +0x10
            // countdown reaches zero, the far-range decision schedules D550,
            // D550 performs its exact reach rejection, then state 2 installs
            // the seven-tick recovery delay. No party HP changes out of reach.
            OpeningDemoSession liveNpc(r);
            std::uint32_t tick=100;
            OpeningNpcTickClocks clocks{};
            clocks.timer30700=[&](){return tick;};
            clocks.biosTick311E0=[&](){return tick;};
            clocks.currentTick19CC4=[&](){return tick;};
            const auto x0=liveNpc.assassin().xFixed,y0=liveNpc.assassin().yFixed;
            for(int i=0;i<4;++i){liveNpc.tickNpcScheduler(clocks);++tick;}
            if(liveNpc.assassin().field10!=0||liveNpc.assassin().xFixed!=x0||liveNpc.assassin().yFixed!=y0)return 368;
            liveNpc.tickNpcScheduler(clocks);++tick;
            if(((liveNpc.assassin().field14>>1u)&3u)!=1||liveNpc.assassin().xFixed!=x0)return 369;
            const auto hp0=liveNpc.partyRecords()[0].currentHitPoints+liveNpc.partyRecords()[1].currentHitPoints;
            liveNpc.tickNpcScheduler(clocks);++tick;
            if(((liveNpc.assassin().field14>>1u)&3u)!=2)return 370;
            const auto hp1=liveNpc.partyRecords()[0].currentHitPoints+liveNpc.partyRecords()[1].currentHitPoints;
            if(hp1!=hp0)return 371;
            liveNpc.tickNpcScheduler(clocks);++tick;
            if(liveNpc.assassin().field10!=7||((liveNpc.assassin().field14>>1u)&3u)!=0)return 372;
        }
        {
            PortalCollisionState2D3D8 pst{};pst.currentLevel=22;pst.facingDegrees=0;
            PortalCollisionTransition2D3D8Callbacks pcb;
            WorkLevelSnapshot pw=demo.workLevel();
            // Put the shipped assassin directly at the destination point so the
            // 2*definition-radius cleanup scan reaches the DS:258AA type path.
            pw.npcs[0].xFixed=0x00048000;pw.npcs[0].yFixed=0x00058000;pw.npcs[0].state&=~0x02000000u;
            pcb.currentWorkLevel=[&]() -> const WorkLevelSnapshot& {return pw;};
            pcb.npcDatabase=[&]() -> const NpcDatabase& {return npcs;};
            pcb.specialNpcTypes258AA={9,-1,-1};int helperCalls=0;
            pcb.npcHelperE0F8=[&](std::size_t i){if(i==0)++helperCalls;return 1;};
            pcb.loadLevel181B4=[](std::uint16_t){};
            PortalCollisionTransition2D3D8::execute(1,1,4,5,0,22,pst,pcb);
            if(helperCalls!=1)return 244;
        }
        {
            NpcOverlapSystem ncoll(demo.workLevel(),npcs);
            if(!ncoll.overlaps(demo.assassin().yFixed,demo.assassin().xFixed,-1))return 182;
            if(ncoll.overlaps(demo.assassin().yFixed,demo.assassin().xFixed,0))return 183;
            auto excludedWork=demo.workLevel();excludedWork.npcs[0].state|=0x02000000u;
            NpcOverlapSystem excludedNcoll(excludedWork,npcs);
            if(excludedNcoll.overlaps(demo.assassin().yFixed,demo.assassin().xFixed,-1))return 184;

            CellInteractionOccupancySystem exactInteractionOccupancy(demo.workLevel(),npcs);
            std::uint8_t exactActive=0;
            int exactSideEffects=0;
            auto exactCallbacks=exactInteractionOccupancy.callbacks(q.partyXFixed,q.partyYFixed,&exactActive,[&](CellInteractionRecord&){++exactSideEffects;});
            if(!exactCallbacks.npcOccupied(demo.assassin().xFixed,demo.assassin().yFixed))return 203;
            if(!exactCallbacks.filteredNpcOccupied2C8E4(demo.assassin().xFixed,demo.assassin().yFixed))return 204;
            CellInteractionRecord occupiedDoor{};occupiedDoor.type=1;occupiedDoor.flags=0x05;
            if(CellInteractionSystem::activate(18,7,occupiedDoor,exactCallbacks)!=0||exactActive!=0||exactSideEffects!=0)return 205;
            CellInteractionRecord clearDoor{};clearDoor.type=1;clearDoor.flags=0x05;
            if(CellInteractionSystem::activate(29,14,clearDoor,exactCallbacks)!=1||exactActive!=1||exactSideEffects!=1)return 206;

            CellInteractionRuntimeIndex openingInteractions(L);
            NpcCandidateCellScanSystem openingCellScan(demo.workLevel(),openingInteractions);
            const auto assassinCellScan=openingCellScan.evaluate(
                demo.assassin().yFixed,demo.assassin().xFixed,0,npcs.at(9).traits39,
                false,exactCallbacks);
            if(assassinCellScan.blocked||assassinCellScan.transitionTriggered)return 222;

            NpcCandidateCollisionSystem fullNpcCandidate(demo.workLevel(),npcs,init,openingInteractions);
            if(fullNpcCandidate.evaluate(demo.assassin().yFixed,demo.assassin().xFixed,0,q.partyXFixed,q.partyYFixed,false,exactCallbacks)!=1)return 223;
            const auto& candidateTree=demo.workLevel().scenery.front();
            CandidateCollisionSystem2D5D0 completeCandidate(demo.workLevel(),npcs,init,openingInteractions);
            std::uint32_t f048=0;
            if(completeCandidate.evaluate(demo.assassin().yFixed,demo.assassin().xFixed,0,q.partyXFixed,q.partyYFixed,22,f048,exactCallbacks)!=1)return 226;
            if(completeCandidate.evaluate(candidateTree.yFixed,candidateTree.xFixed,0,q.partyXFixed,q.partyYFixed,22,f048,exactCallbacks)!=0)return 227;
            if(completeCandidate.evaluate(q.partyYFixed,q.partyXFixed,0,q.partyXFixed,q.partyYFixed,22,f048,exactCallbacks)!=0)return 228;
            if(completeCandidate.evaluate(demo.assassin().yFixed,demo.assassin().xFixed,-1,q.partyXFixed,q.partyYFixed,22,f048,exactCallbacks)!=0)return 229;
            // Mover -2 bypasses the complete late overlap/scenery/party tail.
            if(completeCandidate.evaluate(demo.assassin().yFixed,demo.assassin().xFixed,-2,q.partyXFixed,q.partyYFixed,22,f048,exactCallbacks)!=1)return 230;
            if(fullNpcCandidate.evaluate(candidateTree.yFixed,candidateTree.xFixed,0,q.partyXFixed,q.partyYFixed,false,exactCallbacks)!=0)return 224;
            if(fullNpcCandidate.evaluate(q.partyYFixed,q.partyXFixed,0,q.partyXFixed,q.partyYFixed,false,exactCallbacks)!=0)return 225;

            FilteredNpcOverlapSystem filteredNcoll(demo.workLevel(),npcs);
            if(!filteredNcoll.overlaps(demo.assassin().yFixed,demo.assassin().xFixed,-1))return 196;
            if(filteredNcoll.overlaps(demo.assassin().yFixed,demo.assassin().xFixed,0))return 197;
            // 0x2C8E4 uses the raw definition half-extent. 0x2C800 adds 0x3E8.
            const auto assassinRadius=static_cast<std::int32_t>(npcs.at(9).collisionHalfExtent);
            if(filteredNcoll.overlaps(demo.assassin().yFixed,demo.assassin().xFixed+assassinRadius+1,-1))return 198;
            if(!ncoll.overlaps(demo.assassin().yFixed,demo.assassin().xFixed+assassinRadius+1,-1))return 199;
            auto filteredWork=demo.workLevel();
            filteredWork.npcs[0].type=0x22; // first type excluded by 0x2ACDC
            FilteredNpcOverlapSystem excludedType(filteredWork,npcs);
            if(excludedType.overlaps(filteredWork.npcs[0].yFixed,filteredWork.npcs[0].xFixed,-1))return 200;
            filteredWork.npcs[0].type=0x50; // explicit inclusion exception
            FilteredNpcOverlapSystem type50(filteredWork,npcs);
            if(!type50.overlaps(filteredWork.npcs[0].yFixed,filteredWork.npcs[0].xFixed,-1))return 201;
            filteredWork.npcs[0].state|=0x02000000u;
            FilteredNpcOverlapSystem filteredState(filteredWork,npcs);
            if(filteredState.overlaps(filteredWork.npcs[0].yFixed,filteredWork.npcs[0].xFixed,-1))return 202;

            constexpr std::int32_t partyR=0x8FA0;
            if(!PartyOverlapSystem::overlaps(q.partyYFixed+partyR,q.partyXFixed+partyR,q.partyXFixed,q.partyYFixed))return 185;
            if(PartyOverlapSystem::overlaps(q.partyYFixed,q.partyXFixed+partyR+1,q.partyXFixed,q.partyYFixed))return 186;

            SceneryCollisionSystem scoll(demo.workLevel(),init);
            CandidateCollisionTailSystem tail(ncoll,scoll);
            if(tail.evaluate(demo.assassin().yFixed,demo.assassin().xFixed,0,npcs.at(9).traits39,22,q.partyXFixed,q.partyYFixed,false)!=1)return 187;
            if(tail.evaluate(demo.assassin().yFixed,demo.assassin().xFixed,-1,0,22,q.partyXFixed,q.partyYFixed,false)!=0)return 188;
            const auto& tailTree=demo.workLevel().scenery.front();
            if(tail.evaluate(tailTree.yFixed,tailTree.xFixed,0,0,22,q.partyXFixed,q.partyYFixed,false)!=0)return 189;
            if(tail.evaluate(tailTree.yFixed,tailTree.xFixed,0,0x80,22,q.partyXFixed,q.partyYFixed,false)!=1)return 190;
            if(tail.evaluate(demo.assassin().yFixed,demo.assassin().xFixed,0,npcs.at(9).traits39,22,q.partyXFixed,q.partyYFixed,true)!=0)return 191;

            const auto& firstTree=demo.workLevel().scenery.front();
            if(scoll.query(firstTree.yFixed,firstTree.xFixed,0,npcs.at(9).traits39,22)!=SceneryCollisionResult::Blocked)return 176;
            if(scoll.query(firstTree.yFixed,firstTree.xFixed,0,0x80,22)!=SceneryCollisionResult::Clear)return 177;
            if(scoll.query(firstTree.yFixed,firstTree.xFixed,-1,0,22)!=SceneryCollisionResult::Blocked)return 178;

            // Player-only special scenery behavior from 0x2CD61..0x2CE59.
            WorkLevelSnapshot synthetic{};
            RuntimePlacement22 sp{};sp.type=0x54;sp.xFixed=0x00100000;sp.yFixed=0x00200000;sp.field14=0x0040;
            synthetic.scenery.push_back(sp);
            SceneryCollisionSystem special54(synthetic,init);NpcOverlapSystem emptyNpc(synthetic,npcs);CandidateCollisionTailSystem specialTail(emptyNpc,special54);int specialCount=0;
            SceneryCollisionPlayerState sps{};sps.currentLevel=0x17;int sFace=0,sLoad=0;
            SceneryCollisionCallbacks scb;scb.playerState=&sps;
            scb.setFacing1C0C0=[&](std::int32_t f){if(f==180)++sFace;};
            scb.loadLevel181B4=[&](std::uint16_t l){if(l==0x13)++sLoad;};
            scb.onType54Special=[&](std::uint16_t level){if(level==0x17)++specialCount;};
            if(special54.query(sp.yFixed,sp.xFixed,-1,0,0x17,scb)!=SceneryCollisionResult::Special||specialCount!=1)return 179;
            if(sps.partyXFixed!=0x00038000||sps.partyYFixed!=0x000A8000||sps.facingDegrees!=180||sps.currentLevel!=0x13||sFace!=1||sLoad!=1)return 245;
            specialCount=0;if(specialTail.evaluate(sp.yFixed,sp.xFixed,-1,0,0x17,q.partyXFixed,q.partyYFixed,false,scb)!=-1||specialCount!=1)return 192;
            if(special54.query(sp.yFixed,sp.xFixed,-1,0,22,scb)!=SceneryCollisionResult::Clear)return 180;
            SceneryCollisionPlayerState sps10{};sps10.currentLevel=0x0A;int load6=0;SceneryCollisionCallbacks scb10;scb10.playerState=&sps10;
            scb10.loadLevel181B4=[&](std::uint16_t l){if(l==6)++load6;};
            if(special54.query(sp.yFixed,sp.xFixed,-1,0,0x0A,scb10)!=SceneryCollisionResult::Special||sps10.partyXFixed!=0x00068000||sps10.partyYFixed!=0x003E8000||sps10.facingDegrees!=180||sps10.currentLevel!=6||load6!=1)return 246;
            synthetic.scenery[0].type=0x7A;SceneryCollisionSystem special7a(synthetic,init);specialCount=0;
            scb.onType7ASpecial=[&](){++specialCount;};
            if(special7a.query(sp.yFixed,sp.xFixed,-1,0,22,scb)!=SceneryCollisionResult::Special||specialCount!=1)return 181;
            // Branch-complete CODE.2 0x2D5D0 tests for player -1, query -2, and special mover 0x190.
            LevelRecord branchLevel{};branchLevel.width=1;branchLevel.height=1;branchLevel.cells={0x0005};
            WorkLevelSnapshot branchWork{};branchWork.width=1;branchWork.height=1;branchWork.cells={0x0005};
            CellInteractionRuntimeIndex branchIndex(branchLevel);
            CandidateCollisionSystem2D5D0 branchCollision(branchWork,npcs,init,branchIndex);
            CellInteractionCallbacks noOcc;
            noOcc.npcOccupied=[](std::int32_t,std::int32_t){return false;};
            noOcc.filteredNpcOccupied2C8E4=[](std::int32_t,std::int32_t){return false;};
            noOcc.partyOccupied=[](std::int32_t,std::int32_t){return false;};
            std::uint32_t branchF048=0;int triggerCalls=0;
            CandidateCollisionPlayerCallbacks pcb;
            pcb.trigger2D004=[&](std::uint8_t code){++triggerCalls;return code==5?1:0;};
            constexpr std::int32_t half=0x00008000;
            if(branchCollision.evaluate(half,half,-1,0x100000,0x100000,22,branchF048,noOcc,pcb)!=1||triggerCalls!=1)return 231;
            pcb.trigger2D004=[&](std::uint8_t){++triggerCalls;return 0;};
            if(branchCollision.evaluate(half,half,-1,0x100000,0x100000,22,branchF048,noOcc,pcb)!=0)return 232;
            if(branchCollision.evaluate(half,half,-2,0x100000,0x100000,22,branchF048,noOcc,pcb)!=0)return 233;
            branchWork.cells[0]=0;
            CandidateCollisionSystem2D5D0 branchClear(branchWork,npcs,init,branchIndex);
            if(branchClear.evaluate(half,half,-2,half,half,22,branchF048,noOcc,pcb)!=1)return 234;

            // Type-8 portal: player -1 invokes 0x2D3D8 and sets F048; mover 0x190 blocks.
            LevelRecord portalLevel{};portalLevel.width=1;portalLevel.height=1;portalLevel.cells={0x1001};
            Interaction pi{};pi.type=8;pi.inputColumn=0;pi.raw[0]=8;pi.raw[4]=0;pi.raw[5]=0;pi.raw[6]=22;pi.raw[7]=3;pi.raw[8]=4;pi.raw[9]=2;
            portalLevel.interactions.push_back(pi);
            WorkLevelSnapshot portalWork{};portalWork.width=1;portalWork.height=1;portalWork.cells={0x1001};
            CellInteractionRuntimeIndex portalIndex(portalLevel);
            CandidateCollisionSystem2D5D0 portalCollision(portalWork,npcs,init,portalIndex);
            int portalCalls=0;branchF048=0;
            pcb.portal2D3D8=[&](int sx,int sy,std::uint8_t dx,std::uint8_t dy,std::uint8_t fd,std::uint8_t dl){
                if(sx==0&&sy==0&&dx==3&&dy==4&&fd==2&&dl==22)++portalCalls;
            };
            if(portalCollision.evaluate(half,half,-1,0x100000,0x100000,22,branchF048,noOcc,pcb)!=1||portalCalls!=1||branchF048!=1)return 235;
            branchF048=0;
            if(portalCollision.evaluate(half,half,0x190,0x100000,0x100000,22,branchF048,noOcc,pcb)!=0||branchF048!=0)return 236;
        }
        AutomapState amap(demo.workLevel().automap);
        if(amap.width()!=30||amap.height()!=15||amap.discoveredCount()!=320||!amap.notes().empty())return 41;
        {
            // CODE.2 0x09B0 exact automap glyph selection. The Quick Start map
            // contains 313 already-resolved floor glyphs (0x2E), seven wall
            // topology cells (1), and 130 undiscovered cells (0xFF).
            const auto glyphs=AutomapGlyphSelector::buildGrid(amap);
            if(glyphs.size()!=450)return 118;
            if(AutomapGlyphSelector::select(amap,6,0)!=0x0D)return 119;
            if(AutomapGlyphSelector::select(amap,7,0)!=0x0F)return 120;
            if(AutomapGlyphSelector::select(amap,29,0)!=0x06)return 121;
            if(AutomapGlyphSelector::select(amap,8,0)!=0x2E)return 122;
            if(AutomapGlyphSelector::select(amap,0,0)!=0xFF)return 123;
            AutomapView mapView(amap,demo.party());
            if(mapView.partyCellX()!=4||mapView.partyCellY()!=4)return 124;
            if(!mapView.discoveredBounds().valid||mapView.discoveredBounds().minX!=4||mapView.discoveredBounds().minY!=0||mapView.discoveredBounds().maxX!=29||mapView.discoveredBounds().maxY!=14)return 125;
        }
        if(demo.assassin().type!=9 || demo.assassin().hitPoints!=15)return 12;
        if(std::abs(demo.assassin().x()-18.484375)>1e-9 || std::abs(demo.assassin().y()-7.46875)>1e-9)return 13;
        WorldScene runtimeScene(L,init,npcs,demo.assassin());
        if(std::abs(runtimeScene.npcs().front().x-18.484375)>1e-9||std::abs(runtimeScene.npcs().front().y-7.46875)>1e-9)return 54;
        if(runtimeScene.npcs().front().placementField14!=48)return 67;
        // At projected center the recovered 4/8-view selector has deterministic results.
        // CODE.2 0x2BC1C..0x2BC53 seeds the resource group from +0x14 bits 6..8
        // (animation phase), while 0x2B3EF..0x2B40E uses bits 3..5 only for facing.
        // The first tree has direction 6 but phase 0, so it stays in the RES3 2513 foliage group.
        const auto treeView=SpriteViewSelector::selectNormal(runtimeScene.scenery().front().baseResource,8,runtimeScene.scenery().front().placementField14,88,88,270);
        if(treeView.resource!=2513||treeView.direction!=6||treeView.animationPhase!=0||treeView.cameraSector!=0||treeView.viewOffset!=0)return 68;
        // Assassin likewise starts at phase 0; direction 6 changes the selected facing, not the sprite-group base.
        const auto assassinView=SpriteViewSelector::selectNormal(runtimeScene.npcs().front().baseResource,4,runtimeScene.npcs().front().placementField14,88,88,270);
        // CODE.2 0x2B6D0 uses render category 0 (DS:0x27100 == 0) for NPCs.
        // Assassin NPCS.DAT +0x14 is therefore RES3:315, not INIT global[5]+315.
        if(assassinView.resource!=315||assassinView.direction!=6||assassinView.animationPhase!=0||assassinView.cameraSector!=0||assassinView.viewOffset!=0)return 69;
        const auto treeQuarter=SpriteViewSelector::selectNormal(runtimeScene.scenery().front().baseResource,8,runtimeScene.scenery().front().placementField14,88,88,0);
        if(treeQuarter.cameraSector!=2||treeQuarter.viewOffset!=2||treeQuarter.resource!=2515)return 70;
        // Project the actual saved assassin through CODE.2 0x35798 math.  The
        // Quick Start party faces away from him, so he is correctly culled at boot.
        const auto assassinProjected=projection.project(demo.assassin().xFixed,demo.assassin().yFixed,q.partyXFixed,q.partyYFixed,q.facingDegrees);
        if(assassinProjected.visible)return 74;
        SceneProjector sceneProjector(projection);
        const auto projectedBoot=sceneProjector.project(runtimeScene,q.partyXFixed,q.partyYFixed,q.facingDegrees);
        if(projectedBoot.scenery.empty())return 75;
        if(!projectedBoot.npcs.empty())return 76; // assassin is behind the Quick Start camera at boot
        const auto renderQueue=OriginalRenderQueue::build(projectedBoot,runtimeScene);
        if(renderQueue.size()!=projectedBoot.scenery.size()+projectedBoot.npcs.size())return 83;
        for(std::size_t qi=1;qi<renderQueue.size();++qi)if(renderQueue[qi-1].scale8_8>renderQueue[qi].scale8_8)return 84;
        for(const auto& rr:renderQueue){
            if(rr.category!=OriginalRenderCategory::Scenery)return 85; // assassin is behind camera at boot
            if(rr.draws.empty()||rr.draws.back().part!=OriginalSpritePart::Main)return 86;
            const auto& src=runtimeScene.scenery().at(rr.sourceIndex);
            if(src.hasPairedUpper){
                if(rr.draws.size()!=2||rr.draws.front().part!=OriginalSpritePart::PairedUpper||rr.draws.front().verticalSourceOffsetRows!=0x58)return 87;
            } else if(rr.draws.size()!=1)return 88;
        }
        {
            // Geometry-only raster smoke test in the original 256x128 world viewport.
            // Background colors are the level's shipped ceiling/floor fallbacks;
            // floor/sky perspective drawing is validated separately.
            ssi::render::IndexedFrameBuffer view(256,128);
            auto pix=view.pixels();
            for(int y=0;y<128;++y)for(int x=0;x<256;++x)pix[std::size_t(y)*256+x]=(y<64?runtimeScene.ceilingFallbackColor():runtimeScene.floorFallbackColor());
            // The shipped Quick Start heading 270 faces a deliberately sparse
            // direction; use a deterministic 90-degree inspection heading so
            // this test exercises visible forest rasterization.
            const auto rasterProjected=sceneProjector.project(runtimeScene,q.partyXFixed,q.partyYFixed,90);
            const auto rasterQueue=OriginalRenderQueue::build(rasterProjected,runtimeScene);
            std::size_t draws=0;
            for(const auto& rr:rasterQueue)for(const auto& d:rr.draws){
                const auto sprite=resources.sprite(d.resource);
                if(SpriteRasterizer::draw(view,d.screenX,d.scale8_8,sprite,d.verticalSourceOffsetRows))++draws;
            }
            if(draws==0)return 90;
            const auto changed=std::count_if(view.pixels().begin(),view.pixels().end(),[&](std::uint8_t c){return c!=runtimeScene.ceilingFallbackColor()&&c!=runtimeScene.floorFallbackColor();});
            if(changed==0)return 91;
        }
        for(const auto& ps:projectedBoot.scenery){
            if(ps.lowerView.resource>=resources.count(3))return 77;
            const auto decoded=resources.sprite(ps.lowerView.resource);
            if(decoded.pixels.empty())return 78;
            const auto rect=SpriteRasterGeometry::compute(ps.projection.screenX,ps.projection.scale8_8,decoded);
            if((ps.projection.scale8_8>>3)>=40 && !rect.drawable)return 81;
            if(rect.drawable && rect.y1>260)return 82;
            if(ps.hasPairedUpper){
                if(ps.upperView.resource>=resources.count(3))return 79;
                if(resources.sprite(ps.upperView.resource).pixels.empty())return 80;
            }
        }
        CollisionSystem collision(demo.workLevel(),init);
        if(collision.blocksByScenery(q.partyXFixed,q.partyYFixed))return 64;
        const auto& firstTree=demo.workLevel().scenery.front();
        if(!collision.blocksByScenery(firstTree.xFixed,firstTree.yFixed))return 65;
        const auto ext=static_cast<std::int32_t>(init.scenery().at(firstTree.type).radius);
        if(!collision.blocksByScenery(firstTree.xFixed+ext,firstTree.yFixed))return 66;
        {
            // CODE.2 0x2E120 turning: 20 accumulated input units consume exactly one degree.
            PartyState mover{22,{q.partyXFixed,q.partyYFixed},270,{}};
            PlayerController pc(trig,collision);
            const auto turned=pc.tickOriginalAxes(mover,20,0,0);
            if(!turned.headingChanged||mover.headingDegrees!=269||pc.turnAccumulator()!=0)return 112;
            const auto old=mover.position;
            const auto stepped=pc.tickOriginalAxes(mover,0,-20,0);
            const auto expectedDx=trig.cosMul16_16(3000,269);
            const auto expectedDy=-trig.sinMul16_16(3000,269);
            if(!stepped.positionChanged||stepped.collided||mover.position.x!=old.x+expectedDx||mover.position.y!=old.y+expectedDy)return 113;

            // CODE.2 0x2DBAC collision/slide path: place the party immediately
            // outside a shipped tree AABB and attempt to walk into it.
            PartyState blocked{22,{firstTree.xFixed-ext-1000,firstTree.yFixed},0,{}};
            PlayerController pcBlocked(trig,collision);
            const auto hit=pcBlocked.tickOriginalAxes(blocked,0,-20,0);
            if(hit.positionChanged||!hit.collided)return 114;
        }
        {
            // The production demo session exposes the same controller so live party
            // state, later rendering, and collision all share one source of truth.
            const auto start=demo.party().position;
            const auto turn=demo.moveOriginalAxes(20,0,0);
            if(!turn.headingChanged||demo.party().headingDegrees!=269)return 115;
            const auto walk=demo.moveOriginalAxes(0,-20,0);
            if(!walk.positionChanged||demo.party().position.x==start.x&&demo.party().position.y==start.y)return 116;
        }
        {
            // CODE.2 0x1FC8C..0x20199 keyboard dispatch/integrator.
            OriginalKeyboardMovement keys;
            if(!keys.keyEvent(0x0148))return 126; // Up
            auto a=keys.axes();
            if(a.forwardBack!=-25||a.turn!=0||a.strafe!=0)return 127;
            keys.idleTick();
            if(keys.axes().forwardBack!=0)return 128;
            OriginalKeyboardMovement turnLeft;
            if(!turnLeft.keyEvent(0x0147)||turnLeft.axes().turn!=-38)return 129;
            OriginalKeyboardMovement strafeLeft;
            if(!strafeLeft.keyEvent(0x014B)||strafeLeft.axes().strafe!=-10)return 130;
            OriginalKeyboardMovement combo;
            const auto heldHome=[](std::uint8_t scan){return scan==0x47;};
            if(!combo.keyEvent(0x0148,heldHome))return 131;
            a=combo.axes();
            if(a.forwardBack!=-25||a.turn!=-25)return 132;

            OpeningDemoSession keyDemo(r);
            const auto before=keyDemo.party().position;
            const auto keyWalk=keyDemo.moveOriginalKeyboardKey(0x0148);
            if(!keyWalk.positionChanged||keyDemo.party().position.x==before.x&&keyDemo.party().position.y==before.y)return 133;
        }
        if(demo.party().level!=22)return 14;

        {
            // CODE.2 0x2195C exact opening-use placement: the scattered point
            // is accepted on any nonzero 0x2D5D0 return, while zero falls
            // back to the source coordinates and cell index.
            std::vector<RuntimePlacement22> spawned;
            ObjectSpawnContext2195C osc{};osc.trig=&trig;osc.levelWidth25619=demo.workLevel().width;osc.facingDegreesC658=q.facingDegrees;
            int collisionCalls=0;osc.collision2D5D0=[&](std::int32_t,std::int32_t,int mover){++collisionCalls;return mover==0x190?1:0;};
            const auto accepted=ObjectSpawn2195C::execute(0x6e,136,0,demo.assassin().xFixed,demo.assassin().yFixed,1,0,spawned,osc);
            if(collisionCalls!=1||accepted.index!=0||accepted.collisionFallback||spawned.size()!=1)return 373;
            if(spawned[0].type!=136||spawned[0].field0C!=static_cast<std::uint16_t>((spawned[0].yFixed>>16)*demo.workLevel().width+(spawned[0].xFixed>>16)))return 374;
            if(spawned[0].xFixed==demo.assassin().xFixed&&spawned[0].yFixed==demo.assassin().yFixed)return 375;
            spawned.clear();collisionCalls=0;osc.collision2D5D0=[&](std::int32_t,std::int32_t,int){++collisionCalls;return 0;};
            const auto rejected=ObjectSpawn2195C::execute(0x6e,136,1,demo.assassin().xFixed,demo.assassin().yFixed,1,0,spawned,osc);
            if(collisionCalls!=1||!rejected.collisionFallback||spawned[0].xFixed!=demo.assassin().xFixed||spawned[0].yFixed!=demo.assassin().yFixed)return 376;
            if((spawned[0].field14&0x0400u)==0)return 377;
        }
        {
            // CODE.2 0xE284 type-9 common + death path: nonlethal damage is
            // stateful; lethal damage issues the four literal 0x2195C calls,
            // writes BF44=1, and suppresses the dead runtime actor.
            auto a=demo.assassin();const auto hp0=a.hitPoints;
            std::vector<std::uint16_t> ids;std::vector<int> params;int bf44=0;int xpCalls=0;
            NpcDamageContextE284 nd{};
            nd.spawn2195C=[&](int p,std::uint16_t id,int f,std::int32_t x,std::int32_t y,int scatter,int last){
                if(f!=0||x!=a.xFixed||y!=a.yFixed||scatter!=1||last!=0)return;params.push_back(p);ids.push_back(id);
            };
            nd.setEventStateBF44=[&](int v){bf44=v;};nd.specialType258AA=[](std::uint16_t){return false;};nd.boundaryE0F8=[](int){return 0;};
            nd.boundaryDDA4=[&](std::uint16_t){++xpCalls;};
            auto nonlethal=NpcDamageE284::apply(a,npcs.at(a.type),0,1,1,nd);
            if(nonlethal.died||a.hitPoints!=hp0-1||a.field10!=1)return 378;
            if(((a.state>>24u)&0x30u)!=0x10u)return 379;
            const auto lethal=NpcDamageE284::apply(a,npcs.at(a.type),0,a.hitPoints,1,nd);
            if(!lethal.died||!lethal.openingAssassinDropsSpawned||a.hitPoints!=0)return 380;
            const std::uint16_t expectedIdsE284[4]={136,135,233,116};const int expectedParamsE284[4]={0x6e,0x78,0x82,0x8c};
            if(ids.size()!=4||params.size()!=4)return 381;
            for(int i=0;i<4;++i)if(ids[i]!=expectedIdsE284[i]||params[i]!=expectedParamsE284[i])return 382;
            if(bf44!=1||xpCalls!=1||(((a.state>>24u)&0x02u)==0))return 383;
        }

        demo.resolveAssassinDeath();
        if(demo.groundObjects().size()!=4)return 15;
        if(demo.runtimeGroundObjects().size()!=4||demo.eventStateBF44()!=1||demo.assassin().hitPoints!=0||(((demo.assassin().state>>24u)&0x02u)==0))return 384;
        const std::uint16_t expectedIds[4]={136,135,233,116};
        for(std::size_t i=0;i<4;++i){
            const auto& drop=demo.groundObjects()[i];
            if(drop.objectId!=expectedIds[i])return 16;
            const double dx=drop.position.xd()-demo.assassin().x();
            const double dy=drop.position.yd()-demo.assassin().y();
            const double dist=std::sqrt(dx*dx+dy*dy);
            if(std::abs(dist-1.5)>0.002)return 17;
        }

        {
            // CODE.2 0xF390/0xF4B0 ownership is broader than the 14 carried
            // slots exposed by the early decoder: it searches the held object,
            // all 21 WORD party slots, and recursively descends containers.
            HeldObjectState1A344 none{};
            if(!PartyOwnsObjectF4B0::evaluate(166,none,q.party,objects,q.containerItems))return 405; // direct carried slot
            if(!PartyOwnsObjectF4B0::evaluate(252,none,q.party,objects,q.containerItems))return 406; // equipment slot 14
            if(!PartyOwnsObjectF4B0::evaluate(57,none,q.party,objects,q.containerItems))return 407;  // arrow inside object 321 quiver
            if(PartyOwnsObjectF4B0::evaluate(233,none,q.party,objects,q.containerItems))return 408;  // quest parchment not owned initially
            HeldObjectState1A344 held233{233,0};
            if(!PartyOwnsObjectF4B0::evaluate(233,held233,q.party,objects,q.containerItems))return 409;
        }
        {
            // CODE.2 0x117A0 opening-relevant ordinary exchange: the held
            // object is written to an empty party slot, F238 stores the held
            // state bit, then F790/1A5B0 receive the previous slot object/state.
            auto partyCopy=q.party;
            std::size_t emptySlot=14;
            for(std::size_t slot=0;slot<14;++slot){
                const auto off=0x40+slot*2;
                const auto id=static_cast<std::uint16_t>(partyCopy[0].raw[off])|
                              static_cast<std::uint16_t>(partyCopy[0].raw[off+1]<<8u);
                if(id==0xFFFFu){emptySlot=slot;break;}
            }
            if(emptySlot>=14)return 434;
            const auto readStateWord=[&](const CharacterRecord& ch){
                return static_cast<std::uint32_t>(ch.raw[0x12B])|
                       (static_cast<std::uint32_t>(ch.raw[0x12C])<<8u)|
                       (static_cast<std::uint32_t>(ch.raw[0x12D])<<16u)|
                       (static_cast<std::uint32_t>(ch.raw[0x12E])<<24u);
            };
            const auto oldBits=readStateWord(partyCopy[0]);
            const auto oldState=static_cast<std::uint8_t>((oldBits>>emptySlot)&1u);
            HeldObjectState1A344 held136{136,1};
            int order=0,f238Order=0,f790Order=0,heldOrder=0;
            int f790Object=999,heldObject=999;std::uint8_t f790State=9,heldState=9;
            InventorySlotExchangeContext117A0 slotContext{};
            slotContext.slotBoundaryF238=[&](std::size_t ci,std::size_t slot,int value){
                if(ci!=0||slot!=emptySlot||value!=1)return;f238Order=++order;
            };
            slotContext.presentationF790=[&](int id,std::uint8_t state){f790Object=id;f790State=state;f790Order=++order;};
            slotContext.heldBoundary1A5B0=[&](int id,std::uint8_t state){heldObject=id;heldState=state;heldOrder=++order;};
            const auto exchange=InventorySlotExchange117A0::executeOpeningEmptySlot(0,emptySlot,1,held136,partyCopy,slotContext);
            const auto off=0x40+emptySlot*2;
            const auto stored=static_cast<std::uint16_t>(partyCopy[0].raw[off])|
                              static_cast<std::uint16_t>(partyCopy[0].raw[off+1]<<8u);
            if(!exchange.handled||exchange.oldSlotObject!=-1||exchange.oldSlotState!=oldState||stored!=136||partyCopy[0].carriedItems[emptySlot]!=136)return 435;
            if(((readStateWord(partyCopy[0])>>emptySlot)&1u)!=1u)return 436;
            if(held136.objectId!=-1||held136.state!=oldState)return 437;
            if(f238Order!=1||f790Order!=2||heldOrder!=3||f790Object!=-1||heldObject!=-1||f790State!=oldState||heldState!=oldState)return 438;

            // F238 does not normalize its value before the shift. For the
            // opening state values 0/1 this means state 0 clears the slot bit.
            partyCopy=q.party;
            partyCopy[0].raw[0x12B+emptySlot/8] = partyCopy[0].raw[0x12B+emptySlot/8]; // retain raw layout; bit set below via dword
            auto bits=readStateWord(partyCopy[0]) | (std::uint32_t(1u)<<emptySlot);
            partyCopy[0].raw[0x12B]=static_cast<std::uint8_t>(bits);
            partyCopy[0].raw[0x12C]=static_cast<std::uint8_t>(bits>>8u);
            partyCopy[0].raw[0x12D]=static_cast<std::uint8_t>(bits>>16u);
            partyCopy[0].raw[0x12E]=static_cast<std::uint8_t>(bits>>24u);
            HeldObjectState1A344 held135{135,0};
            const auto exchange0=InventorySlotExchange117A0::executeOpeningEmptySlot(0,emptySlot,1,held135,partyCopy);
            if(!exchange0.handled||exchange0.oldSlotState!=1||((readStateWord(partyCopy[0])>>emptySlot)&1u)!=0u||held135.objectId!=-1||held135.state!=1)return 439;
        }
        {
            // CODE.2 0xF40C / 0xF540 removal semantics: first recursive
            // container occurrence only, held object priority, then 21 party slots.
            auto nestedItems=q.containerItems;
            auto nestedState=q.containerState;
            const auto beforeState=nestedState[53];
            if(!ContainerRemoveF40C::execute(321,57,objects,nestedItems,nestedState))return 421;
            if(nestedItems[53].empty()||nestedItems[53][0]!=-1)return 422;
            if(nestedItems[53].size()>1&&nestedItems[53][1]!=57)return 423; // routine returns after first match
            if(nestedState[53]!=static_cast<std::uint8_t>(beforeState&~1u))return 424;

            auto partyCopy=q.party;
            auto itemsCopy=q.containerItems;
            auto stateCopy=q.containerState;
            HeldObjectState1A344 heldDirect{233,1};
            if(!ObjectRemoveF540::execute(233,heldDirect,partyCopy,objects,itemsCopy,stateCopy)||heldDirect.objectId!=-1||heldDirect.state!=0)return 425;

            partyCopy=q.party;itemsCopy=q.containerItems;stateCopy=q.containerState;
            HeldObjectState1A344 noHeld{};
            std::size_t callbackChar=99,callbackSlot=99;int callbackValue=99;
            ObjectRemoveContextF540 removeContext{};
            removeContext.slotBoundaryF238=[&](std::size_t ci,std::size_t slot,int value){callbackChar=ci;callbackSlot=slot;callbackValue=value;};
            if(!ObjectRemoveF540::execute(252,noHeld,partyCopy,objects,itemsCopy,stateCopy,removeContext))return 426;
            const auto slot14=static_cast<std::uint16_t>(partyCopy[0].raw[0x40+28])|static_cast<std::uint16_t>(partyCopy[0].raw[0x40+29]<<8u);
            if(slot14!=0xFFFFu||callbackChar!=0||callbackSlot!=14||callbackValue!=0)return 427;

            partyCopy=q.party;itemsCopy=q.containerItems;stateCopy=q.containerState;
            if(!ObjectRemoveF540::execute(57,noHeld,partyCopy,objects,itemsCopy,stateCopy))return 428;
            if(itemsCopy[53].empty()||itemsCopy[53][0]!=-1)return 429;
        }
        {
            // CODE.2 0x12E70 / 0x12F9B -> 0x11FB0 inventory-grid selector.
            // Global Y chooses one of four 80-pixel character panels; the
            // original X 297..396 / local-Y 5..74 rectangle maps to a 3x4
            // inventory grid before 0x117A0 performs the actual exchange.
            if(InventoryGridClick11FB0::mapSlot(297,5)!=std::optional<std::size_t>{0})return 512;
            if(InventoryGridClick11FB0::mapSlot(330,22)!=std::optional<std::size_t>{0})return 513;
            if(InventoryGridClick11FB0::mapSlot(331,23)!=std::optional<std::size_t>{5})return 514;
            if(InventoryGridClick11FB0::mapSlot(396,74)!=std::optional<std::size_t>{11})return 515;
            if(InventoryGridClick11FB0::mapSlot(296,5)||InventoryGridClick11FB0::mapSlot(397,74) ||
               InventoryGridClick11FB0::mapSlot(297,4)||InventoryGridClick11FB0::mapSlot(297,75))return 516;

            auto gridParty=q.party;
            HeldObjectState1A344 gridHeld{136,1};
            std::optional<std::pair<std::size_t,std::size_t>> emptyGrid;
            for(std::size_t ci=0;ci<gridParty.size()&&!emptyGrid;++ci){
                if((static_cast<std::uint16_t>(gridParty[ci].raw[0x10])|
                    static_cast<std::uint16_t>(gridParty[ci].raw[0x11]<<8u))&0x8000u)continue;
                for(std::size_t slot=0;slot<12;++slot){
                    const auto off=0x40+slot*2;
                    const auto id=static_cast<std::uint16_t>(gridParty[ci].raw[off])|
                                  static_cast<std::uint16_t>(gridParty[ci].raw[off+1]<<8u);
                    if(id==0xFFFFu){emptyGrid=std::pair{ci,slot};break;}
                }
            }
            if(!emptyGrid)return 517;
            const auto [gridCi,gridSlot]=*emptyGrid;
            const int gridX=InventoryGridClick11FB0::kLeft+static_cast<int>(gridSlot/4)*InventoryGridClick11FB0::kColumnPitch;
            const int gridLocalY=InventoryGridClick11FB0::kTop+static_cast<int>(gridSlot%4)*InventoryGridClick11FB0::kRowPitch;
            const int gridY=static_cast<int>(gridCi)*InventoryGridClick11FB0::kPanelHeight+gridLocalY;

            auto rejectedParty=gridParty;
            auto rejectedHeld=gridHeld;
            const auto noLeft=InventoryGridClick11FB0::execute(gridX,gridY,0,rejectedHeld,rejectedParty);
            if(noLeft.handled||rejectedHeld.objectId!=136)return 518;
            InventoryGridClickContext11FB0 modeContext{};
            modeContext.panelMode6F90[gridCi]=1;
            const auto modeBlocked=InventoryGridClick11FB0::execute(gridX,gridY,1,rejectedHeld,rejectedParty,modeContext);
            if(modeBlocked.handled||rejectedHeld.objectId!=136)return 519;
            const auto bit1Blocked=InventoryGridClick11FB0::execute(gridX,gridY,3,rejectedHeld,rejectedParty);
            if(bit1Blocked.handled||rejectedHeld.objectId!=136)return 520;

            const auto clicked=InventoryGridClick11FB0::execute(gridX,gridY,1,gridHeld,gridParty);
            if(!clicked.handled||!clicked.exchangeHandled||clicked.characterIndex!=gridCi||
               clicked.slotIndex!=gridSlot||gridHeld.objectId!=-1)return 521;
            const auto storedOff=0x40+gridSlot*2;
            const auto stored=static_cast<std::uint16_t>(gridParty[gridCi].raw[storedOff])|
                              static_cast<std::uint16_t>(gridParty[gridCi].raw[storedOff+1]<<8u);
            if(stored!=136)return 522;
        }
        {
            // CODE.2 0x224E6..0x22713 opening pickup branch. A missing click
            // bit must leave the runtime object array and cursor state intact.
            OpeningDemoSession noClick(r);
            noClick.resolveAssassinDeath();
            const auto beforeCount=noClick.runtimeGroundObjects().size();
            const auto ignored=noClick.pickupGroundObject(0,0);
            if(ignored.handled||ignored.picked||ignored.startMistTransition)return 410;
            if(noClick.runtimeGroundObjects().size()!=beforeCount||noClick.heldObject().objectId!=-1)return 411;
        }
        {
            // Production opening acquisition chain: 0x223F4 -> 0x21704 ->
            // 0x1A344 held state + 0xF390/0xF4B0 ownership -> 0x21F70.
            // The HUD is still selecting the first empty slot automatically, but
            // the actual transfer now runs through the original 0x117A0 exchange.
            OpeningDemoSession pickupDemo(r);
            pickupDemo.resolveAssassinDeath();
            if(pickupDemo.runtimeGroundObjects().size()!=4)return 412;
            const auto pickById=[&](std::uint16_t id){
                const auto& ro=pickupDemo.runtimeGroundObjects();
                const auto it=std::find_if(ro.begin(),ro.end(),[&](const RuntimePlacement22& o){return o.type==id;});
                if(it==ro.end())return OpeningGroundPickupResult223F4{};
                return pickupDemo.pickupGroundObject(static_cast<std::size_t>(std::distance(ro.begin(),it)),1);
            };
            auto p136=pickById(136);
            if(!p136.handled||!p136.picked||p136.startMistTransition||pickupDemo.heldObject().objectId!=136||pickupDemo.runtimeGroundObjects().size()!=3)return 413;
            if(!pickupDemo.stowHeldObjectInFirstFreeCarriedSlot()||pickupDemo.heldObject().objectId!=-1)return 414;
            auto p135=pickById(135);
            if(!p135.picked||p135.startMistTransition||pickupDemo.heldObject().objectId!=135||pickupDemo.runtimeGroundObjects().size()!=2)return 415;
            if(!pickupDemo.stowHeldObjectInFirstFreeCarriedSlot())return 416;
            auto p116=pickById(116);
            if(!p116.picked||p116.startMistTransition||pickupDemo.heldObject().objectId!=116||pickupDemo.runtimeGroundObjects().size()!=1)return 417;
            if(!pickupDemo.stowHeldObjectInFirstFreeCarriedSlot())return 418;
            auto p233=pickById(233);
            if(!p233.picked||!p233.startMistTransition||pickupDemo.heldObject().objectId!=233||!pickupDemo.runtimeGroundObjects().empty())return 419;
            if(!pickupDemo.mistTransitionActive()||pickupDemo.party().level!=22)return 420;
            if(pickupDemo.gameplayClock().enabledA7E8()!=0u||
               pickupDemo.gameplayClock().stackDepthA7EC()!=1u)return 534;

            // Reach 0x222C0 with Quick Start CF4C still at hour 14.
            int pickupGuard=0;
            while(pickupDemo.mistTransition()->phase()!=MistTransition::Phase::WaitMistEffect && ++pickupGuard<160)
                (void)pickupDemo.tickMistTransition();
            if(pickupGuard>=160||pickupDemo.worldClock().hourHighWord()!=14u)return 535;
            for(int i=0;i<12;++i)if(pickupDemo.tickMistTransition())return 536;
            if(pickupDemo.worldClock().hourHighWord()!=2u)return 537;

            while(!pickupDemo.tickMistTransition()&&++pickupGuard<240){}
            if(pickupGuard>=240||pickupDemo.party().level!=23)return 430;
            if(pickupDemo.gameplayClock().enabledA7E8()!=1u||
               pickupDemo.gameplayClock().stackDepthA7EC()!=0u)return 538;
            if(pickupDemo.snapshotSaveDatState().worldClock!=0x00020000u)return 539;
            // 0x223B5..0x223D6 invokes F540 for 135,136,116 only.
            // Parchment 233 remains on the cursor in this test ordering.
            if(pickupDemo.heldObject().objectId!=233)return 431;
            const auto rawOwns=[&](std::uint16_t id){
                for(const auto& ch:pickupDemo.partyRecords()){
                    const auto status=static_cast<std::uint16_t>(ch.raw[0x10])|static_cast<std::uint16_t>(ch.raw[0x11]<<8u);
                    if((status&0x8000u)!=0)continue;
                    for(std::size_t slot=0;slot<21;++slot){
                        const auto off=0x40+slot*2;
                        const auto v=static_cast<std::uint16_t>(ch.raw[off])|static_cast<std::uint16_t>(ch.raw[off+1]<<8u);
                        if(v==id)return true;
                    }
                }
                return false;
            };
            if(rawOwns(135)||rawOwns(136)||rawOwns(116))return 432;
            if(!OpeningSequence::hasObject(pickupDemo.party(),233))return 433;
        }

        if(demo.acquireGroundObject(136).triggered)return 18;
        if(demo.acquireGroundObject(135).triggered)return 19;
        if(demo.acquireGroundObject(233).triggered)return 20;
        const auto transition=demo.acquireGroundObject(116);
        if(!transition.triggered||transition.destinationLevel!=23||std::abs(transition.destination.xd()-9.5)>1e-9||std::abs(transition.destination.yd()-58.5)>1e-9||transition.destinationHeading!=180)return 21;
        if(!demo.mistTransitionActive()||demo.party().level!=22)return 22; // no instant teleport
        int guard=0;
        while(!demo.tickMistTransition() && ++guard<200){}
        if(guard>=200)return 23;
        if(demo.party().level!=23||demo.party().headingDegrees!=180)return 24;
        if(std::abs(demo.party().position.xd()-9.5)>1e-9||std::abs(demo.party().position.yd()-58.5)>1e-9)return 35;
        if(OpeningSequence::hasObject(demo.party(),135)||OpeningSequence::hasObject(demo.party(),136)||OpeningSequence::hasObject(demo.party(),116))return 36;
        if(!OpeningSequence::hasObject(demo.party(),233))return 37;
    }

    if(cineEnv){
        const std::filesystem::path cineRoot=cineEnv;
        // CINE00 is the executable-selected opening movie. Its first subtitle control
        // is index 8; CODE.2 adds 0x324, resolving to T[812].
        CinematicSession intro(cineRoot/"CINE00.ANM",text);
        if(intro.movie().frameCount()!=405 || intro.movie().fileSize()!=13781936u)return 25;
        intro.step(); // frame 0: clear/palette/full image
        const auto f1=intro.step();
        if(f1.frameIndex!=1 || !intro.subtitle().has_value())return 26;
        if(*intro.subtitle()!=text.at(812))return 27;
        if(intro.subtitle()->find("Lord Dhelt")==std::string::npos)return 38;
        while(!intro.finished()) intro.step();
        if(intro.nextFrame()!=405)return 39;

        // Decode every shipped frame through the same production decoder.
        for(int i=0;i<=12;++i){
            char name[16];std::snprintf(name,sizeof(name),"CINE%02d.ANM",i);
            CinematicSession movie(cineRoot/name,text);
            while(!movie.finished()) movie.step();
            if(movie.movie().parsedBytes()!=movie.movie().fileSize())return 40;
        }
        OpeningDemoFlow flow(r,cineRoot);
        if(flow.phase()!=OpeningDemoFlow::Phase::OpeningCinematic)return 42;
        while(flow.phase()==OpeningDemoFlow::Phase::OpeningCinematic)flow.advanceCinematicFrame();
        if(flow.phase()!=OpeningDemoFlow::Phase::ForgottenRealmsForest)return 43;
        flow.game().resolveAssassinDeath();
        if(flow.acquireGroundObject(136).triggered)return 55;
        if(flow.acquireGroundObject(135).triggered)return 56;
        if(flow.acquireGroundObject(233).triggered)return 57;
        if(!flow.acquireGroundObject(116).triggered||flow.phase()!=OpeningDemoFlow::Phase::MistTransfer)return 58;
        for(int i=0;i<200&&flow.phase()==OpeningDemoFlow::Phase::MistTransfer;++i)flow.tickMist();
        if(flow.phase()!=OpeningDemoFlow::Phase::DemoComplete)return 59;
    }

    std::cout<<"PASS: core data, Quick Start opening flow, and shipped cinematic decode validated.\n";
    return 0;
}
