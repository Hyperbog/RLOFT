#include "rloft/exploration_runtime.hpp"

#include "rloft/exploration_world.hpp"
#include "rloft/original_pickup_ribbon_text.hpp"
#include "rloft/resource_assets.hpp"
#include "rloft/options_menu.hpp"
#include "rloft/enhanced_game_save.hpp"
#include "rloft/assassin_relic_transition.hpp"
#include "rloft/level22_quest_playback.hpp"

#include <exception>
#include <stdexcept>
#include <utility>

namespace rloft {

ExplorationRuntime::ExplorationRuntime(
    ExplorationSessionState session,
    RendererTables renderer_tables,
    std::vector<EnvironmentProfile> environment_profiles)
    : session_(std::move(session)),
      renderer_tables_(std::move(renderer_tables)),
      environment_profiles_(std::move(environment_profiles)) {
    motion_state_.integrator.heading = session_.facing;
}

ExplorationRuntime ExplorationRuntime::load_fresh(
    const GameSource& source,
    const std::vector<std::uint8_t>& chars) {
    return ExplorationRuntime(
        ExplorationSessionLoader::load_fresh_party(
            source, chars),
        RendererTables::load(source),
        decode_environment_profiles(
            source.read("INIT")));
}

EnhancedGameSnapshot ExplorationRuntime::capture_native_checkpoint() const {
    if(!gameplay_state_initialized_)
        throw std::logic_error("native save needs initialized gameplay");
    EnhancedGameSnapshot out{};
    out.session=session_;
    out.world=world_;
    out.inventory=inventory_;
    out.visited_levels=level_continuity_.snapshots();
    out.random_state=original_random_.state();
    out.original_story_selector=original_story_message_bf44_;
    return out;
}

void ExplorationRuntime::restore_native_checkpoint(
    const EnhancedGameSnapshot& saved,
    const GameSource& source) {
    if(!gameplay_state_initialized_)
        throw std::logic_error("native load needs initialized gameplay");
    if(saved.session.level_id>=32U)
        throw std::invalid_argument("saved level outside original game");
    if(saved.session.level.width<=0 || saved.session.level.height<=0 ||
       saved.session.level.cell_words.size()!=
       static_cast<std::size_t>(
           saved.session.level.width*saved.session.level.height))
        throw std::invalid_argument("saved world geometry does not match");

    // Stage all throwing validation BEFORE the active session changes.
    // Reacquire immutable CODE.2 mage scroll source from original media.
    auto staged_inventory=saved.inventory;
    auto source_inventory=InventoryRuntimeState::fresh(source);
    staged_inventory.mage_scroll_source=
        std::move(source_inventory.mage_scroll_source);
    staged_inventory.mage_scribe_feedback.reset();
    std::optional<OriginalGroundRes4Selection> staged_ground;
    std::string staged_ground_failure;
    if(source.contains("INIT") && source.contains("RES4")) {
        try {
            const ResArchive res4(source.read("RES4"));
            staged_ground=OriginalGroundRes4Audit::resolve(
                source.read("INIT"),saved.session.level,res4);
        }catch(const std::exception& e) {
            staged_ground_failure=e.what();
        }
    }
    auto visited=saved.visited_levels;
    auto session=saved.session;
    auto world=saved.world;
    // Save data was independently decoded and source media verified.
    // Runtime is authoritative after this one atomic replacement.
    session_=std::move(session);
    world_=std::move(world);
    inventory_=std::move(staged_inventory);
    level_continuity_.restore_snapshots(std::move(visited));
    original_random_.seed(saved.random_state);
    original_story_message_bf44_=saved.original_story_selector;
    original_pickup_ribbon_.reset();
    original_pickup_ribbon_prefix_.clear();
    scene_shape_cache_.clear();
    ground_source_=std::move(staged_ground);
    ground_source_failure_=std::move(staged_ground_failure);
    motion_state_=ExplorationRuntimeMotionState{};
    motion_state_.integrator.heading=session_.facing;
    inventory_save_backing_.reset();
    inventory_save_world_revision_=0U;
    ++world_scene_revision_;
}

void ExplorationRuntime::set_scene_video_page(
    std::uint8_t page) {
    if (page > 1U) {
        throw std::out_of_range(
            "scene VGA page must be 0 or 1");
    }
    scene_video_page_ = page;
}

void ExplorationRuntime::initialize_gameplay_state(
    const GameSource& source) {
    initialize_gameplay_state(
        source,
        [this](
            std::uint8_t count,
            std::uint8_t sides) {
            return original_random_.dice(
                count,
                sides);
        });
}

void ExplorationRuntime::initialize_gameplay_state(
    const GameSource& source,
    const LevelWorldStateCodec::DiceRoller& roll) {
    npc_catalog_ = NpcCatalog::load(source);
    object_catalog_ = ObjectCatalog::load(source);
    scene_source_banks_ = SceneSourceBanks::load(source);
    placed_scene_catalog_ = PlacedSceneCatalog::load(source);
    scene_shape_archive_.emplace(source.read("RES3"));
    scene_shape_cache_.clear();

    // CODE.2 16E90 establishes the RES4 base from INIT+0x0E;
    // 1F388 adds the signed selector from the CURRENT level's
    // header+7. Neither source routine hardcodes Level22.
    // Preserve one authenticated 64x64 type-1 resource identity
    // for every loaded level, including future Level23, without
    // guessing artwork or silently substituting a texture.
    ground_source_.reset();
    ground_source_failure_.clear();
    if (source.contains("INIT") && source.contains("RES4")) {
        try {
            const ResArchive res4(source.read("RES4"));
            ground_source_=OriginalGroundRes4Audit::resolve(
                source.read("INIT"),session_.level,res4);
        } catch(const std::exception& error) {
            ground_source_failure_=error.what();
        }
    }
    // A real DOS install supplies CODE.2. Tests with partial synthetic
    // resource sets can omit it, leaving the diagnostic VGA trace absent.
    scene_vga_masks_.reset();
    scene_video_page_ = 0U;
    initial_code2_ground_draw_page_c050_.reset();
    scene_individual_columns_mode_ = false;
    if (source.contains("CODE.2")) {
        scene_vga_masks_.emplace(
            Type7VgaPlaneMaskModel::load(source));
        scene_video_page_ =
            scene_vga_masks_->initial_video_page();
        initial_code2_ground_draw_page_c050_=
            scene_vga_masks_->initial_ground_draw_page_c050();
        scene_individual_columns_mode_ =
            scene_vga_masks_->initial_individual_columns_mode();
    }
    world_ = LevelWorldStateCodec::build(
        session_.level,
        npc_catalog_,
        roll);
    inventory_ = InventoryRuntimeState::fresh(source);
    // Original CODE.2 object-2 stored initial DWORD BF44=0.
    // Preserve original global value across the normal Level22
    // combat and pickup handlers, not a separate UI-only mock.
    original_story_message_bf44_ = 0U;
    original_pickup_ribbon_prefix_.clear();
    original_pickup_ribbon_.reset();
    // A fresh world invalidates prior SAVE backing and its origin
    // revision; never export an inventory over stale global state.
    inventory_save_backing_.reset();
    inventory_save_world_revision_=0U;
    inventory_drop_c02f_center_fraction_=false;
    combat_strength_source_.reset();
    combat_burden_source_.reset();
    combat_progression_source_.reset();
    if(source.contains("CODE.2")) {
        const auto code2=source.read("CODE.2");
        combat_strength_source_=
            CombatStrengthAttackModel::from_code2(code2);
        combat_burden_source_=
            InventoryEncumbranceModel::from_code2(code2);
        combat_progression_source_=
            CombatProgressionTables::from_code2(code2);
        if(const auto prefix=
            OriginalPickupRibbonTextModel::source_prefix_from_code2(code2))
            original_pickup_ribbon_prefix_=*prefix;
        constexpr auto initial_at=0x75F24U+0xC02FU;
        if(code2.size()<=initial_at)
            throw std::runtime_error(
                "CODE.2 truncated before original drop grid mode C02F");
        inventory_drop_c02f_center_fraction_=
            code2[initial_at]!=0U;
    }
    gameplay_state_initialized_ = true;
    ++world_scene_revision_;
}

void ExplorationRuntime::transition_to_level(
    const GameSource& source,
    const LevelTransitionLocation& target) {
    if (!gameplay_state_initialized_)
        throw std::logic_error(
            "level transition requires initialized gameplay state");
    // Do not bypass original hand-container close/cancel semantics.
    if (inventory_.open_container)
        throw std::logic_error(
            "level transition requires closed inventory container");

    // Restore a visited mutable world rather than reinitializing it.
    // Fresh levels use CODE.2 181B4 LEVnnn -> LEVELS source precedence.
    auto destination = level_continuity_.recall(target.level_id);
    auto staged_rng = original_random_;
    if (!destination) {
        LevelContinuityEntry loaded{};
        loaded.level = ExplorationSessionLoader::load_level(
            source, target.level_id, &loaded.from_direct_save);
        loaded.world = LevelWorldStateCodec::build(
            loaded.level, npc_catalog_,
            [&staged_rng](std::uint8_t count, std::uint8_t sides) {
                return staged_rng.dice(count, sides);
            });
        destination = std::move(loaded);
    }

    // Resolve original INIT/header-derived terrain for the destination.
    // A missing RES4 source is exposed, never silently synthesized.
    std::optional<OriginalGroundRes4Selection> staged_ground{};
    std::string staged_ground_failure{};
    if (source.contains("INIT") && source.contains("RES4")) {
        try {
            const ResArchive archive(source.read("RES4"));
            staged_ground = OriginalGroundRes4Audit::resolve(
                source.read("INIT"), destination->level, archive);
        } catch (const std::exception& error) {
            staged_ground_failure = error.what();
        }
    }

    // All validation and potentially throwing snapshot allocations
    // precede mutation of the current live level/party/world.
    level_continuity_.enter(
        session_, world_, target, std::move(*destination));
    original_random_ = staged_rng;
    ground_source_ = std::move(staged_ground);
    ground_source_failure_ = std::move(staged_ground_failure);
    scene_shape_cache_.clear();
    motion_state_.integrator.heading = session_.facing;
    // Inventory-only SAVE cannot safely carry a different world.
    inventory_save_backing_.reset();
    inventory_save_world_revision_ = 0U;
    ++world_scene_revision_;
}

void ExplorationRuntime::import_matching_save_inventory(
    const std::vector<std::uint8_t>& original_save) {
    if(!gameplay_state_initialized_)
        throw std::logic_error(
            "inventory-only LOAD requires initialized original world");

    // Decode and preflight the ENTIRE input before mutating the
    // live party/inventory. This cannot recreate moved monsters,
    // ambient effects or a different source world level.
    auto incoming=InventorySaveCodec::decode(
        original_save,inventory_);
    // An already active runtime hand-container menu has no
    // lossless source-backed SAVE transition at present.
    if(inventory_.open_container)
        throw std::logic_error(
            "inventory-only LOAD requires closed live containers");
    InventorySaveCodec::apply_to_matching_session(
        incoming,session_,inventory_,&object_catalog_);
    inventory_save_backing_=std::move(incoming);
    // Live hand/cursor and party model changed; reject any
    // stale pre-LOAD world hit-test snapshot.
    ++world_scene_revision_;
    inventory_save_world_revision_=world_scene_revision_;
}

std::vector<std::uint8_t>
ExplorationRuntime::export_matching_save_inventory() const {
    if(!gameplay_state_initialized_ || !inventory_save_backing_)
        throw std::logic_error(
            "inventory-only SAVE requires a compatible imported image");
    if(world_scene_revision_!=inventory_save_world_revision_)
        throw std::logic_error(
            "inventory-only SAVE cannot preserve changed world effects");
    const auto snapshot=InventorySaveCodec::capture_matching_session(
        *inventory_save_backing_,session_,inventory_,&object_catalog_);
    return InventorySaveCodec::encode(snapshot);
}

NpcDamageResult ExplorationRuntime::damage_level22_assassin(
    std::size_t npc_index,
    std::uint16_t amount,
    std::uint16_t reaction_mode,
    const Level22EncounterModel::PlacementProbe& can_place) {
    if (!gameplay_state_initialized_) {
        throw std::logic_error("gameplay state is not initialized");
    }
    if (session_.level_id != Level22Story::kLevelId) {
        throw std::logic_error("Assassin damage outside Level 22");
    }
    // The encounter stages NPC injury and all four unique quest drops
    // atomically. Do not invalidate the displayed source frame if an
    // unsupported original branch or placement probe rejects the hit.
    const auto result=Level22EncounterModel::damage_assassin(
        world_,
        session_.level,
        npc_index,
        amount,
        reaction_mode,
        session_.facing,
        npc_catalog_,
        renderer_tables_,
        can_place);
    // Original E436 writes literal 1 to BF44 after its FOUR
    // Level22 type9 death drops; nonlethal E284 leaves it alone.
    if(result.died) original_story_message_bf44_=1U;
    ++world_scene_revision_;
    return result;
}

bool ExplorationRuntime::complete_level22_relic_transition(
    const GameSource& source,const Level22MistPresenter& present) {
    if(!gameplay_state_initialized_ ||
       session_.level_id!=Level22Story::kLevelId)
        throw std::logic_error(
            "original Level22 mist event requires live Level22 gameplay");
    if(!present)
        throw std::invalid_argument("Level22 mist presentation is missing");
    // The fourth actual pickup places its item in the original cursor.
    // The other three must be in legitimate inventory/containers; never
    // transition merely because a test or save set a story selector.
    for(const auto relic:AssassinRelicTransition::kRelics) {
        if(!inventory_.has_object(session_.party,object_catalog_,
               static_cast<std::int16_t>(relic)))
            throw std::logic_error(
                "Level22 mist requires all four original quest relics");
    }

    // Original 21F9D begins the staging sequence from (23.5,7.5)
    // facing 0xD5. All source world, party, inventory, clock and
    // deterministic RNG changes happen in this isolated runtime copy.
    ExplorationRuntime staged=*this;
    staged.session_.coordinate_a=
        (static_cast<std::uint32_t>(
            AssassinRelicTransition::kStagingColumn)<<16U) |
            AssassinRelicTransition::kStagingFraction;
    staged.session_.coordinate_b=
        (static_cast<std::uint32_t>(
            AssassinRelicTransition::kStagingRow)<<16U) |
            AssassinRelicTransition::kStagingFraction;
    staged.session_.facing=AssassinRelicTransition::kStagingFacing;
    // 21F78 resets pending input before the original pose staging.
    // Do not carry a pre-event movement accumulator into Level23.
    staged.motion_state_=ExplorationRuntimeMotionState{};
    staged.motion_state_.integrator.heading=staged.session_.facing;

    const bool completed=Level22QuestPlayback::execute(
        [&](std::size_t frame)->bool {
            return present(frame,staged);
        },
        [&](std::uint32_t destination) {
            if(destination!=AssassinRelicTransition::kDestinationLevel)
                throw std::logic_error("unexpected original mist destination");
            staged.transition_to_level(
                source,LevelTransitionLocation{
                    static_cast<int>(destination),
                    AssassinRelicTransition::kDestinationCoordinateA,
                    AssassinRelicTransition::kDestinationCoordinateB,
                    AssassinRelicTransition::kDestinationFacing});
        },
        [&](std::uint16_t relic) {
            staged.inventory_.remove_object(
                staged.session_.party,staged.object_catalog_,
                static_cast<std::int16_t>(relic));
        });
    if(!completed)return false;
    // Only now make level, original RNG, party, relic consumption,
    // continuity snapshots, orientation and scene revision live.
    *this=std::move(staged);
    ++world_scene_revision_;
    return true;
}

ExplorationRuntime::Level22PickupProgress
ExplorationRuntime::pickup_and_advance_level22_at(
    const ExplorationRuntimeFrame& frame,
    int row,int column,
    const GameSource& source,
    const Level22MistPresenter& present,
    std::optional<std::uint32_t> original_irq_tick_30700) {
    // Copy before the first original 21704 mutation. The fourth pickup
    // and 21F70 event are ONE transaction: declining frame21, a missing
    // Level23 archive, a failed F540 removal or a thrown presenter MUST
    // retain the fourth loose item in the original world.
    ExplorationRuntime staged=*this;
    auto pickup=staged.pickup_level22_loose_item_at(
        frame,row,column,original_irq_tick_30700);
    if(!pickup)return {};
    const bool warped=pickup->level22_mist_sequence;
    if(warped && !staged.complete_level22_relic_transition(
            source,present)) {
        return {std::nullopt,true,false};
    }
    *this=std::move(staged);
    return {std::move(pickup),false,warped};
}

ExplorationRuntime::Level22WeaponAttackOutcome
ExplorationRuntime::attack_level22_assassin_at(
    const ExplorationRuntimeFrame& frame,int row,int column,
    std::size_t actor,std::size_t hand) {
    if(!gameplay_state_initialized_ ||
       session_.level_id!=Level22Story::kLevelId ||
       frame.world_scene_revision!=world_scene_revision_ ||
       frame.source_level_id!=session_.level_id ||
       frame.source_coordinate_a!=session_.coordinate_a ||
       frame.source_coordinate_b!=session_.coordinate_b ||
       frame.source_facing!=session_.facing)
        throw std::logic_error(
            "Level22 weapon attack requires current original scene");
    if(!combat_strength_source_ || !combat_burden_source_)
        throw std::logic_error(
            "Level22 player combat requires authentic CODE.2 tables");
    // 223F4 resolves original class0 NPC source record, never a
    // guessed 3D billboard coordinate or family-C floor item.
    const auto target=world_scene_target_at(
        frame.world_scene_records,row,column,0U);
    const auto index=Level22ViewportTargetModel::assassin_index(
        target,world_);
    if(!index)return {};
    const auto hand_plan=CombatLiveHandRequestModel::inspect(
        session_.party,object_catalog_,session_.level,actor,hand);
    if(!hand_plan.may_enter_player_attack())
        throw std::logic_error(
            "214B8/24478 did not authorize this equipped weapon");
    if(hand_plan.dispatch.object_id<0)
        throw std::logic_error(
            "original unarmed OBJECTS[-1] memory not authenticated");

    // Original E5F0 accepts an EXPLICIT source actor index. The
    // source DB94(-1) automatic-target buffers are still missing;
    // the native PC's source-backed two-click target is explicit.
    ExplorationRuntime staged=*this;
    auto& npc=staged.world_.npcs.at(*index);
    const auto& definition=staged.npc_catalog_.at(npc.npc_type_id);
    auto strike=CombatLevel22StrikeModel::resolve(
        staged.session_.party,staged.inventory_,
        staged.object_catalog_,*staged.combat_burden_source_,
        *staged.combat_strength_source_,definition,npc,
        staged.original_random_,actor,hand,
        hand_plan.dispatch.object_id,
        static_cast<std::int32_t>(*index),
        staged.session_.level.header[0x290U]);
    Level22WeaponAttackOutcome result{};
    result.found_assassin=true;
    result.strike=strike;
    if(strike.hit.hit) {
        // E963 always pushes reaction_mode=0. Reuse the existing
        // staged E284 Level22 type9 death/drop implementation.
        const auto injury=staged.damage_level22_assassin(
            *index,strike.damage_after_c910,0U,
            [&staged](std::uint32_t y,std::uint32_t x,
                      std::uint32_t radius) {
                return InventoryWorldDropModel::
                    placement_passable_2d5d0(
                        staged.session_,staged.world_,
                        staged.npc_catalog_,y,x,radius);
            });
        result.injury=injury;
        if(injury.died) {
            // E284:E53D invokes 1DDA4 with the definition's
            // original award. DDA4 writes class XP DWORDs into
            // the SAME staged party as the corpse and unique drops.
            // Class-level callback services 50DC/3EA4/3DE4
            // remain unimplemented; never fabricate a level.
            if(!staged.combat_progression_source_)
                throw std::logic_error(
                    "Assassin XP needs exact original CODE.2 class tables");
            auto awarded=CombatXpAwardService::apply(
                staged.session_.party,injury.experience_award,
                *staged.combat_progression_source_);
            if(!awarded)
                throw std::logic_error(
                    "original DDA4 XP class lanes cannot be validated");
            result.experience=*awarded;
        }
    }
    // C80C misses still consume their d20 and E578 effect changes.
    // Preserve both in the native save's party and RNG snapshot.
    // The original 24B48 action clocks/display are a distinct gate.
    ++staged.world_scene_revision_;
    *this=std::move(staged);
    return result;
}

InventoryPartyOrderResult ExplorationRuntime::exchange_party_order(
    std::size_t first,std::size_t second,
    std::array<bool,4>& per_actor_inventory_modes) {
    if(!gameplay_state_initialized_)
        throw std::logic_error(
            "CODE.2 party reorder requires live gameplay state");
    const auto result=InventoryPartyOrderModel::exchange(
        session_.party,world_,inventory_,
        per_actor_inventory_modes,first,second);
    if(result.changed)++world_scene_revision_;
    return result;
}

InventoryWorldDropResult ExplorationRuntime::drop_held_inventory_item(
    const InventoryWorldDropModel::PlacementProbe& can_place) {
    if(!gameplay_state_initialized_) {
        throw std::logic_error(
            "inventory drop requires initialized original world");
    }
    // In the live game, use the original CODE.2 2D5D0 mode-0x190
    // geometry/control/NPC/player predicate, NOT a guessed caller
    // rectangle. Keep the explicit probe injection for tests.
    auto placement=can_place;
    if(!placement) {
        placement=[this](std::uint32_t y,std::uint32_t x,
                         std::uint32_t actor_mode) {
            return InventoryWorldDropModel::placement_passable_2d5d0(
                session_,world_,npc_catalog_,y,x,actor_mode);
        };
    }
    const auto result=InventoryWorldDropModel::drop(
        world_,session_,inventory_,object_catalog_,renderer_tables_,
        placement,inventory_drop_c02f_center_fraction_);
    if(result.status==InventoryWorldDropStatus::Dropped)
        ++world_scene_revision_;
    return result;
}

Level16RitualCompletion
ExplorationRuntime::complete_pending_seed_ritual(
    const Level16RitualCompletionModel::Event8Playback& present_event_8,
    const InventoryWorldDropModel::PlacementProbe& can_place) {
    if(!gameplay_state_initialized_)
        throw std::logic_error(
            "Level16 event 8 requires initialized original world");

    auto placement=can_place;
    if(!placement) {
        placement=[this](std::uint32_t y,std::uint32_t x,
                         std::uint32_t actor_mode) {
            return InventoryWorldDropModel::placement_passable_2d5d0(
                session_,world_,npc_catalog_,y,x,actor_mode);
        };
    }
    try {
        const auto result=Level16RitualCompletionModel::complete_21880(
            session_,world_,inventory_,object_catalog_,renderer_tables_,
            placement,present_event_8,inventory_drop_c02f_center_fraction_);
        // A successful ritual appends the fourth seed, changes the
        // original actor/item and rewards the Holy Symbol. Aborted
        // presentation rolls back all persistent data but may have
        // temporarily displayed a different in-memory scene.
        // Force source render/hit-test freshness either way.
        if(result.status==Level16RitualCompletionStatus::Completed ||
           result.status==Level16RitualCompletionStatus::Escaped ||
           result.status==Level16RitualCompletionStatus::Quit)
            ++world_scene_revision_;
        return result;
    } catch(...) {
        ++world_scene_revision_;
        throw;
    }
}

LooseItemPickupResult ExplorationRuntime::pickup_level22_loose_item(
    std::size_t item_index,
    std::optional<std::uint32_t> original_irq_tick_30700) {
    if (!gameplay_state_initialized_) {
        throw std::logic_error("gameplay state is not initialized");
    }
    if (session_.level_id != Level22Story::kLevelId) {
        throw std::logic_error("Level 22 pickup outside Level 22");
    }
    // The live 21704 input route is a pickup, not an item swap.
    // Never destroy the existing cursor object or compact the source
    // family-C array when a click arrives with the hand already full.
    if(inventory_.held.object_id!=-1)
        throw std::logic_error(
            "Level 22 pickup requires an empty held-item cursor");
    // Preflight source object identity as well as the optional text
    // before either the world or cursor may be changed.
    const auto& next_item=world_.loose_items.at(item_index);
    (void)object_catalog_.at(next_item.object_type_id);
    // Original 22510..225DF composes the live item description
    // after 21704 transfer and before the relic completion branch.
    // Preflight source-only data without changing game state;
    // the eventual 1C21C queue write happens after the pickup.
    std::optional<std::vector<std::uint8_t>> pickup_text;
    if (original_irq_tick_30700 &&
        !original_pickup_ribbon_prefix_.empty()) {
        const auto& definition=object_catalog_.at(next_item.object_type_id);
        const bool known=(next_item.packed_runtime_state & 0x0400U)!=0U;
        pickup_text=OriginalPickupRibbonTextModel::compose_unenchanted(
            original_pickup_ribbon_prefix_,definition,known);
    }
    // The cursor and family-C list change together in this original
    // bounded pickup. Invalidate stale source targets only on success.
    const auto result=Level22EncounterModel::pickup_loose_item(
        world_,
        item_index,
        inventory_,
        session_.party,
        object_catalog_,
        (session_.persistent_event_flags[0x51U / 8U] &
         static_cast<std::uint8_t>(1U << (0x51U % 8U))) != 0U);
    ++world_scene_revision_;
    if(result.message_assignment_bf44.has_value())
        original_story_message_bf44_=*result.message_assignment_bf44;
    if(pickup_text && original_irq_tick_30700) {
        // 1C21C writes a source string into original 14580,
        // schedules its unsigned 30700 tick deadline +5Ah and
        // marks CF59. Existing CombatIrqMessageModel owns those
        // exact source semantics; no modern animation timer.
        original_pickup_ribbon_=CombatIrqMessageModel::queue({
            *original_irq_tick_30700,*pickup_text,
            std::vector<std::uint8_t>(pickup_text->size(),0U)});
    }
    return result;
}

std::optional<NpcDamageResult>
ExplorationRuntime::damage_level22_assassin_at(
    const ExplorationRuntimeFrame& frame,
    int row,
    int column,
    std::uint16_t amount,
    std::uint16_t reaction_mode,
    const Level22EncounterModel::PlacementProbe& can_place) {
    // 223F4's bit-0 source-class-2 gate must remain clear during
    // combat selection: loose items are pickup targets, not enemies.
    if (!gameplay_state_initialized_ ||
        frame.world_scene_revision != world_scene_revision_ ||
        frame.source_coordinate_a != session_.coordinate_a ||
        frame.source_coordinate_b != session_.coordinate_b ||
        frame.source_facing != session_.facing ||
        frame.source_level_id != session_.level_id) {
        throw std::logic_error(
            "Assassin target was selected from a stale world frame");
    }
    const auto selected = world_scene_target_at(
        frame.world_scene_records, row, column, 0U);
    const auto npc_index =
        Level22ViewportTargetModel::assassin_index(
            selected, world_);
    if (!npc_index.has_value()) {
        return std::nullopt;
    }
    return damage_level22_assassin(
        *npc_index, amount, reaction_mode, can_place);
}

std::optional<LooseItemPickupResult>
ExplorationRuntime::pickup_level22_loose_item_at(
    const ExplorationRuntimeFrame& frame,
    int row,
    int column,
    std::optional<std::uint32_t> original_irq_tick_30700) {
    // 224EF requires bit 0 for class-2 hit-test eligibility.
    if (!gameplay_state_initialized_ ||
        frame.world_scene_revision != world_scene_revision_ ||
        frame.source_coordinate_a != session_.coordinate_a ||
        frame.source_coordinate_b != session_.coordinate_b ||
        frame.source_facing != session_.facing ||
        frame.source_level_id != session_.level_id) {
        throw std::logic_error(
            "pickup target was selected from a stale world frame");
    }
    const auto selected = world_scene_target_at(
        frame.world_scene_records, row, column, 0x01U);
    const auto item_index =
        Level22ViewportTargetModel::loose_item_index(
            selected, world_);
    if (!item_index.has_value()) {
        return std::nullopt;
    }
    return pickup_level22_loose_item(
        *item_index,original_irq_tick_30700);
}

std::optional<WorldSceneTarget>
ExplorationRuntime::world_scene_target_at(
    const std::vector<WorldSceneRecord>& records,
    int row,
    int column,
    std::uint8_t action_flags) const {
    if (!gameplay_state_initialized_ ||
        !scene_shape_archive_.has_value()) {
        throw std::logic_error(
            "gameplay scene state is not initialized");
    }

    return WorldSceneSelectionModel::target_at(
        records,
        row,
        column,
        action_flags,
        [this](std::uint16_t resource_id) {
            return scene_shape_archive_->
                decode_type7_geometry(
                    static_cast<int>(
                        resource_id));
        },
        [this](std::uint16_t source_index) {
            const auto& placed =
                world_.placed_objects.at(
                    source_index);
            return WorldSceneSelectionModel::
                class1_type_is_interactive(
                    placed.object_type_id);
        });
}


ExplorationRuntimeMotionResult ExplorationRuntime::tick_motion(
    ExplorationMotionInputs inputs) {
    // The original 2D5D0 checks live NPC/placed-object footprints
    // after the map-cell geometry probe. Do not supply guessed sources
    // to startup fixtures that have not loaded real gameplay state.
    ExplorationLocomotionModel::EntityProbe probe{};
    if (gameplay_state_initialized_) {
        probe=[this](std::uint32_t a,std::uint32_t b) {
            return OriginalEntityCollisionModel::party_contact(
                world_,npc_catalog_,placed_scene_catalog_,
                session_.level_id,a,b);
        };
    }
    return ExplorationRuntimeMotionModel::tick(
        session_,motion_state_,inputs,renderer_tables_,probe);
}

ExplorationRuntimeFrame ExplorationRuntime::build_frame(
    int effect_fade,
    ViewSweepOptions sweep_options,
    const std::array<std::uint16_t, 32>& rng15) {
    ExplorationRuntimeFrame out{};
    out.world_scene_revision = world_scene_revision_;
    out.source_coordinate_a = session_.coordinate_a;
    out.source_coordinate_b = session_.coordinate_b;
    out.source_facing = session_.facing;
    out.source_level_id = session_.level_id;
    out.ground_source_verified=ground_source_.has_value();
    out.ground_source_failure=ground_source_failure_;
    if(ground_source_) {
        out.ground_res4_base=ground_source_->source_init_base;
        out.ground_res4_relative=ground_source_->signed_level_selector;
        out.ground_res4_absolute=ground_source_->absolute_archive_entry;
        out.ground_raster_source=ground_source_->raster_source;
    }
    // CODE.2 1F1F5..1F223 takes high 8 of each low
    // 16.16 coordinate word as its C044/C048 texture phase.
    // 34998 regenerates FF58 when heading changes; deriving it
    // here is idempotent and exact for this original C658 value.
    out.ground_phase_u=static_cast<std::uint8_t>(
        (session_.coordinate_a >> 8U)&0xFFU);
    out.ground_phase_v=static_cast<std::uint8_t>(
        (session_.coordinate_b >> 8U)&0xFFU);
    out.ground_ff58=OriginalGroundTextureRaster::generate_ff58(
        renderer_tables_,session_.facing);
    out.ground_perspective_ready=true;
    out.scene_video_page = scene_video_page_;
    out.initial_code2_ground_draw_page_c050 =
        initial_code2_ground_draw_page_c050_;
    out.scene_individual_columns_mode =
        scene_individual_columns_mode_;

    // For read-only field projections the source C6D8 and C738
    // counts must come from the current loaded original SINTAB,
    // never from native vector.size() or invented ray constants.
    out.original_c6d8_ray_count=renderer_tables_.c6d8();
    out.original_c738_scale_count=renderer_tables_.scale_block_count;
    out.sweep_records =
        ViewSweepTraversalModel::build(
            session_,
            renderer_tables_,
            sweep_options);

    ExplorationWorldModel::rebuild_automap_cache(
        session_.level,
        ExplorationWorldModel::party_cell(session_));

    out.scene = ExplorationSceneBuilder::build(
        session_,
        environment_profiles_,
        environment_phase_state_,
        rng15);

    if (gameplay_state_initialized_) {
        const auto defaults =
            OptionsState::defaults();
        const auto render_threshold =
            OptionsMenuModel::render_threshold(
                defaults.distancing(),
                session_.level.render_threshold_floor);
        auto npc_records =
            NpcSceneModel::build(
                world_,
                session_,
                npc_catalog_,
                renderer_tables_,
                scene_source_banks_,
                render_threshold);
        auto placed_records =
            PlacedObjectSceneModel::build(
                world_,
                session_,
                placed_scene_catalog_,
                renderer_tables_,
                scene_source_banks_,
                render_threshold);
        auto loose_records =
            LooseItemSceneModel::build(
                world_,
                session_,
                object_catalog_,
                renderer_tables_,
                scene_source_banks_,
                render_threshold);

        out.world_scene_records.reserve(
            npc_records.size() +
            placed_records.size() +
            loose_records.size());
        out.world_scene_records.insert(
            out.world_scene_records.end(),
            npc_records.begin(),
            npc_records.end());
        out.world_scene_records.insert(
            out.world_scene_records.end(),
            placed_records.begin(),
            placed_records.end());
        out.world_scene_records.insert(
            out.world_scene_records.end(),
            loose_records.begin(),
            loose_records.end());

        // CODE.2 applies each emitted source's optional scene-cell influence
        // after projection/threshold acceptance and before final rendering.
        for (const auto& record :
             out.world_scene_records) {
            std::uint8_t step = 0U;
            std::uint32_t cell_index = 0U;

            if (record.source_class == 0U) {
                const auto& live =
                    world_.npcs.at(
                        record.source_index);
                step =
                    npc_catalog_.at(
                        live.npc_type_id).
                        scene_influence_step;
                cell_index =
                    live.cached_cell_index;
            } else if (
                record.source_class == 1U) {
                const auto& live =
                    world_.placed_objects.at(
                        record.source_index);
                step =
                    placed_scene_catalog_.at(
                        live.object_type_id).
                        scene_influence_step;
                cell_index =
                    live.cached_cell_index;
            } else if (
                record.source_class == 2U) {
                const auto& live =
                    world_.loose_items.at(
                        record.source_index);
                step =
                    object_catalog_.at(
                        live.object_type_id).
                        scene_influence_step;
                cell_index =
                    live.cached_cell_index;
            }

            if (step != 0U) {
                ViewSweepInfluenceModel::apply(
                    out.sweep_records,
                    step,
                    cell_index,
                    session_.level.width);
            }
        }

        if (out.world_scene_records.size() >
            NpcSceneModel::kMaxSceneRecords) {
            out.world_scene_records.resize(
                NpcSceneModel::kMaxSceneRecords);
        }
        WorldSceneOrderModel::assign_render_order(
            out.world_scene_records);
        out.world_scene_draw_calls =
            WorldSceneDrawCallModel::build(
                out.world_scene_records,
                world_);
        if (!scene_shape_archive_.has_value()) {
            throw std::logic_error(
                "gameplay scene archive is not initialized");
        }
        // Scene RES3 shapes are immutable during a level session. Cache the
        // decoded display lists once, and preserve reference stability on
        // insertion through std::unordered_map.
        const auto shape_for_id =
            [this](std::uint16_t resource_id)
                -> const Type7SceneShape& {
                const auto existing =
                    scene_shape_cache_.find(resource_id);
                if (existing != scene_shape_cache_.end()) {
                    return existing->second;
                }
                const auto inserted =
                    scene_shape_cache_.emplace(
                        resource_id,
                        scene_shape_archive_->decode_type7(
                            static_cast<int>(resource_id)));
                return inserted.first->second;
            };
        // An unsupported individual RES3 asset should not discard
        // the complete ordered original scene; preserve its failed
        // slot and the original record identity for diagnostics.
        auto plans = WorldSceneRenderPlanModel::try_build(
            out.world_scene_draw_calls, renderer_tables_,
            out.sweep_records, shape_for_id,
            enhanced_pc_scene_rendering_);
        out.world_scene_render_plans = std::move(plans.plans);
        out.scene_plan_failures = std::move(plans.failures);
        if (scene_vga_masks_.has_value() &&
            !enhanced_pc_scene_rendering_) {
            // A malformed or unsupported diagnostic VGA trace must
            // never suppress walls, controls or game interaction.
            auto attempt =
                Type7VgaFrameTraceModel::try_build(
                    out.world_scene_render_plans,
                    renderer_tables_,
                    *scene_vga_masks_,
                    scene_video_page_,
                    scene_individual_columns_mode_,
                    shape_for_id);
            out.scene_vga_trace = std::move(attempt.trace);
            out.scene_vga_trace_failure =
                std::move(attempt.failure);
            out.scene_vga_scene_failures =
                std::move(attempt.scene_failures);
        }
    }

    out.sweep_runs =
        ViewSweepRunModel::build(
            out.sweep_records);

    out.surface_shades =
        SurfaceShadingModel::build(
            out.scene.environment_mode,
            effect_fade);

    for (const auto& run : out.sweep_runs) {
        if (run.pass != SweepRenderPass::Prepass ||
            !run.drawable()) {
            continue;
        }
        try {
            const auto plan =
                SurfacePrepassModel::build(
                    run,
                    session_.level,
                    renderer_tables_,
                    out.surface_shades);
            if (plan.has_value()) {
                out.prepass_plans.push_back(*plan);
            }
        } catch (const std::exception& error) {
            // A failed *optional* rendering plan does not invalidate
            // movement, world visibility, automap or interaction state.
            // Keep the original run pending, with its precise error.
            out.prepass_plan_failures.push_back(
                SurfacePrepassPlanFailure{
                    run.resource_id, run.first_ray, error.what(),
                });
        }
    }

    return out;
}

}  // namespace rloft
