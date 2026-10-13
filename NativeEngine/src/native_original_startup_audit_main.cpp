#include "rloft_native/native_media.hpp"
#include "rloft_native/native_startup_visuals.hpp"
#include "rloft_native/native_world_payload.hpp"
#include "rloft_native/native_world_runtime.hpp"
#include "rloft_native/native_party_start.hpp"
#include "rloft_native/native_party_inventory.hpp"
#include "rloft_native/native_party_item_icon.hpp"
#include "rloft_native/native_palette_context.hpp"
#include "rloft_native/native_scene_instances.hpp"
#include "rloft_native/native_scene_target.hpp"
#include "rloft_native/native_scene_frame_ranges.hpp"
#include "rloft_native/native_assets.hpp"
#include "rloft/startup_presenter.hpp"
#include "rloft/character_creation_presenter.hpp"
#include "rloft/character_creation_flow.hpp"
#include "rloft/character_transfer.hpp"
#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <set>
#include <utility>
#include <vector>
namespace {
std::uint64_t checksum(const std::vector<std::uint8_t>& pixels) {
    std::uint64_t hash=14695981039346656037ULL;
    for(const auto b:pixels) {
        hash^=b;
        hash*=1099511628211ULL;
    }
    return hash;
}
}
int main(int argc,char** argv) {
    if(argc!=2) {
        std::cerr<<"usage: rloft_native_startup_audit <original-media-dir-or-iso>\n";
        return 2;
    }
    try {
        const auto media=rloft_native::NativeMedia::open(argv[1]);
        rloft_native::NativeStartupVisualSource visuals(media);
        rloft::StartupPresenter scene(visuals);
        unsigned source_frames=0U;
        std::array<std::uint64_t,4> logo_hashes{};
        scene.first_logo();
        logo_hashes[0]=checksum(scene.framebuffer().pixels());
        scene.second_logo();
        logo_hashes[1]=checksum(scene.framebuffer().pixels());
        scene.third_logo();
        logo_hashes[2]=checksum(scene.framebuffer().pixels());
        scene.title_background();
        logo_hashes[3]=checksum(scene.framebuffer().pixels());
        for(int index=1;index<=14;++index) {
            scene.menu_animation_frame(index);
            if(scene.framebuffer().pixels().size()!=320U*400U)
                throw std::runtime_error("original menu animation is not 320x400 indexed");
            ++source_frames;
        }
        for(int option=0;option<4;++option) {
            scene.menu_frame(option);
            if(scene.framebuffer().pixels().size()!=320U*400U)
                throw std::runtime_error("original menu composition not 320x400");
            ++source_frames;
        }
        // An original source RES0 record ID must decode through NativeMedia
        // and a second independent INDEXED presenter, not a fabricated image.
        for(int id=94;id<=105;++id) {
            const auto raster=visuals.res0_type1(id);
            if(raster.count<=0||raster.width<=0||raster.height<=0||
               raster.pixels.empty())
                throw std::runtime_error("original RES0 source ID is not displayable");
        }
        for(const auto bank:{9,12,14}) {
            const auto palette=visuals.colors_palette(bank);
            for(const auto channel:palette.rgb) {
                if(channel>63)
                    throw std::runtime_error("original startup DAC6 palette channel outside range");
            }
        }
        auto levels=rloft_native::load_native_world_payload(media);
        rloft_native::NativeWorldRuntime world(std::move(levels));
        const auto& first=world.original().level(
            rloft_native::NativePartyStart::kFirstLevel);
        if(first.width!=30U||first.height!=15U ||
           world.normal_collision(rloft_native::NativePartyStart::kFirstLevel,4U,4U)!=
               rloft_native::NativeCollision::Passable ||
           first.cell(4U,4U)!=0x0000U)
            throw std::runtime_error(
                "original Level22 first-party spawn conflicts with LEVELS.DAT");
        // Bounded source facts: the supplied initial map does not contain
        // kind-1 door controls or map-local event-slot commands. Original
        // executable-driven scene/assassin logic must be implemented from
        // CODE.2, not invented as a conventional map event.
        if(first.controls.size()!=10U ||
           first.families[0].size()!=1U ||
           first.families[1].size()!=110U ||
           !first.families[2].empty())
            throw std::runtime_error("original Level22 source record family count changed");
        for(const auto& control:first.controls)
            if(control.bytes[0]!=0U)
                throw std::runtime_error("original Level22 has unexpected controller kind");
        for(const auto& slot:first.event_slots)
            for(const auto byte:slot)
                if(byte!=0U)
                    throw std::runtime_error("original Level22 event slot is not empty");
        // The normal new-game GPU session now consumes the authentic
        // NPC/INIT/OBJECTS → RES3 scene definition and 11-byte placement
        // directory. Fail closed if the initial original scene changes.
        const rloft_native::NativeResArchive source_res3(media.read("RES3"));
        const auto res3_count=source_res3.entry_count();
        const auto banks=rloft_native::decode_native_res3_scene_banks(
            media.read("INIT"),res3_count);
        const auto definitions=rloft_native::decode_native_scene_definitions(
            banks,media.read("NPCS.DAT"),media.read("OBJECTS.DAT"));
        const auto placements=rloft_native::derive_native_authored_scene_instances(
            world.original(),definitions);
        const auto& initial=placements.levels.at(22U);
        // Preserve the source NPC's exact 16.16 pose rather than treating
        // the one Level22 NPC as a made-up actor at the party start.
        const auto& original_npc=initial.at(0U);
        if(original_npc.source_class!=0U ||
           original_npc.original_type_id!=9U ||
           original_npc.absolute_res3_base!=315U ||
           original_npc.cell_column!=13U ||
           original_npc.cell_row!=7U ||
           original_npc.x_position_16_16!=
               static_cast<std::int32_t>(13U*65536U+31744U) ||
           original_npc.y_position_16_16!=
               static_cast<std::int32_t>(7U*65536U+30720U) ||
           original_npc.compact_facing_0_to_7!=2U)
            throw std::runtime_error(
                "original Level22 NPC9 authored pose does not match source");
        // Portable native-host inspector must recognize the *actual*
        // original NPC9 16.16 record from a nearby unobstructed viewpoint.
        // Selection is an explicit diagnostics-only rule, not original
        // CODE.2 NPC activation, dialogue, combat or sprite frame selection.
        const double npc_x=double(original_npc.x_position_16_16)/65536.0;
        const double npc_y=double(original_npc.y_position_16_16)/65536.0;
        if(world.normal_collision(22U,13U,7U)!=
               rloft_native::NativeCollision::Passable)
            throw std::runtime_error(
                "Level22 authentic NPC9 source cell is not ordinary passable");
        const auto target=rloft_native::select_native_authored_scene_target(
            world,22U,{original_npc},npc_x-0.4,npc_y,0.0);
        if(!target||target->original.original_type_id!=9U)
            throw std::runtime_error(
                "native source-bound Level22 NPC9 inspection target failed");
        if(initial.size()!=111U || first.families[0].size()!=1U ||
           first.families[1].size()!=110U ||
           !first.families[2].empty())
            throw std::runtime_error(
                "original Level22 source actor/placed placement count changed");
        std::array<std::size_t,4> first_scene_source_counts{};
        for(const auto& item:initial) {
            if(item.level!=22U || item.absolute_res3_base>=res3_count)
                throw std::runtime_error(
                    "Level22 authentic RES3 scene instance out of range");
            const unsigned bucket=
                item.source_class==0U&&item.original_type_id==9U?0U:
                item.source_class==1U&&item.original_type_id==62U?1U:
                item.source_class==1U&&item.original_type_id==63U?2U:
                item.source_class==1U&&item.original_type_id==64U?3U:4U;
            if(bucket>=first_scene_source_counts.size())
                throw std::runtime_error(
                    "Level22 authentic scene record has unexpected source type");
            ++first_scene_source_counts[bucket];
        }
        if(first_scene_source_counts!=
           std::array<std::size_t,4>{{1U,52U,42U,16U}})
            throw std::runtime_error(
                "Level22 original source entity types/counts changed");
        // For each *distinct* Level22 original RES3 base, decode the
        // source-authenticated EH7 geometry. Source frame ranges are legal
        // allocations only; current animation/view is still unproven.
        const auto legal_frames=rloft_native::derive_native_authored_frame_ranges(
            placements,definitions);
        const auto& legal_first=legal_frames.levels.at(22U);
        if(legal_first.size()!=initial.size())
            throw std::runtime_error(
                "original Level22 scene/frame ledger alignment changed");
        std::set<std::size_t> decoded_base_ids;
        for(std::size_t index=0;index<initial.size();++index) {
            const auto& actor=initial.at(index);
            const auto& range=legal_first.at(index);
            if(range.source_class!=actor.source_class ||
               range.compact_record_index!=actor.compact_record_index ||
               range.original_type_id!=actor.original_type_id ||
               range.first_absolute_res3_id!=actor.absolute_res3_base)
                throw std::runtime_error(
                    "original Level22 typed entity/frame provenance mismatch");
            const auto id=rloft_native::select_native_authored_res3_frame(
                range,banks,0U,0U);
            if(!decoded_base_ids.insert(id).second)continue;
            const auto shape=rloft_native::probe_native_authored_base_eh7(
                range,banks,source_res3);
            if(shape.absolute_res3_id!=id ||
               shape.original_entry_sha256.size()!=64U ||
               shape.original_body_sha256.size()!=64U ||
               shape.live_view_subframe_verified)
                throw std::runtime_error(
                    "original Level22 RES3 EH7 base evidence invalid");
            if(index==0U &&
               (id!=315U || range.npc_animation_stride!=8U ||
                range.npc_cycle_divisor!=4U ||
                range.npc_animation_span!=3U ||
                shape.source_width!=64U ||
                shape.x_min!=15 || shape.x_max!=51 ||
                shape.y_min!=27 || shape.y_max!=128))
                throw std::runtime_error(
                    "original Assassin NPC9 RES3:315 base EH7 bounds or animation source changed");
        }
        if(decoded_base_ids.empty()||decoded_base_ids.size()>4U)
            throw std::runtime_error(
                "original Level22 unique base-frame count outside source type bound");
        // This source-data check NEVER assigns sprite animation frames,
        // encounter AI, pickup eligibility or in-world interactivity.
        const auto original_tables=rloft::CharacterTransferTables::from_original_bytes(
            media.read("CODE.1"),media.read("OBJECTS.DAT"));
        // Validate all real 90 family/race rows and original object ID
        // bounds. The serializer's actor data remains a source-only fixture;
        // the game runtime never fabricates a player party.
        for(std::size_t family=0;family<15;++family)
            for(std::size_t race=0;race<6;++race)
                static_cast<void>(original_tables.starting_loadout(family,race));
        // Exercise the actual CODE.1 generator against the owner-supplied
        // 90-row original starting-item table, and feed its output into the
        // independent native CHARS inventory decoder. Actors below are
        // explicitly synthetic AUDIT identities, never gameplay defaults.
        std::array<rloft::CharacterTransferActor,2> transfer_fixture{};
        for(unsigned i=0;i<2U;++i) {
            auto& actor=transfer_fixture[i];
            actor.draft.actor=static_cast<int>(i);
            actor.draft.name=i==0U?"VERIFY A":"VERIFY B";
            actor.draft.sex=static_cast<int>(i);
            actor.draft.race=0; // Both class families use populated source rows
            actor.draft.classes={{static_cast<int>(i),-1,-1}};
            actor.draft.option=static_cast<int>(i);
            actor.draft.face=static_cast<int>(i);
            actor.stats.abilities.fill(12);
            actor.stats.levels[0]=1;
            actor.stats.hit_points=8+static_cast<int>(i);
        }
        const auto actual_transferred=rloft::build_character_transfer(
            transfer_fixture,original_tables);
        const auto actual_inventory=rloft_native::inspect_native_party_inventory(
            rloft_native::decode_native_party_start(actual_transferred),
            media.read("OBJECTS.DAT"));
        if(actual_inventory.object_definitions!=336U ||
           actual_inventory.actors[0].actor_name!="VERIFY A" ||
           actual_inventory.actors[1].actor_name!="VERIFY B")
            throw std::runtime_error(
                "original CODE.1-generated actor CHARS inventory identity failed");
        std::size_t original_starter_items=0U;
        for(unsigned actor=0;actor<2U;++actor) {
            const auto original=original_tables.starting_loadout(actor,0U);
            for(unsigned slot=0;slot<21U;++slot) {
                const auto& item=actual_inventory.actors[actor].slots[slot];
                if(original.slots[slot]==-1) {
                    if(item.occupied||item.object_id!=-1)
                        throw std::runtime_error(
                            "original CODE.1 empty actor item slot diverges");
                    continue;
                }
                if(!item.occupied || item.object_id!=original.slots[slot])
                    throw std::runtime_error(
                        "CODE.1 starting loadout CHARS-to-native inventory mismatch");
                ++original_starter_items;
            }
        }
        // The pinned supplied CODE.1 rows class-family0/race0 and
        // class-family1/race0 each contain five original source items.
        // This explicitly covers populated *both-actor* transfer.
        if(actual_inventory.occupied_total!=original_starter_items ||
           original_starter_items!=10U)
            throw std::runtime_error(
                "original CODE.1 generated party has no validated starting gear");
        // Inspect the *whole* original INIT[+02] RES1 inventory UI
        // namespace, including serialized empty records. Never infer an
        // animation frame or substitute placeholder icons in the native
        // first-party HUD. Context source bank comes from CODE.2 level22.
        if(rloft_native::native_world_palette_bank(22U)!=4U)
            throw std::runtime_error("original Level22 palette table drifted");
        const rloft_native::NativeResArchive res1(media.read("RES1"));
        const auto first_level_palette=
            rloft_native::decode_native_palette(media.read("COLORS"),4U);
        const auto icon_corpus=
            rloft_native::audit_native_party_item_icon_corpus(
                res1,first_level_palette);
        if(icon_corpus.examined!=168U ||
           icon_corpus.nonempty<100U ||
           icon_corpus.zero_length>21U ||
           icon_corpus.decoded_pixels==0U ||
           icon_corpus.original_ui_frame_palette_parity_verified)
            throw std::runtime_error(
                "original RES1 object UI bank EH1 frame0 source audit failed");
        // Verify each genuine CODE.1 starting equipped UI ID passes the
        // exact same original RES1 path as GPU F7 inventory presentation.
        std::size_t starter_icon_slots=0U;
        for(const auto& actor:actual_inventory.actors)
            for(const auto& slot:actor.slots)
                if(slot.occupied) {
                    const auto icon=rloft_native::decode_native_party_item_icon(
                        res1,slot.absolute_res1_ui_id,first_level_palette);
                    if(icon.original_res1_id!=slot.absolute_res1_ui_id)
                        throw std::runtime_error(
                            "original CODE.1 starting item RES1 icon identity drifted");
                    ++starter_icon_slots;
                }
        if(starter_icon_slots!=10U)
            throw std::runtime_error(
                "original CODE.1 starter icon link count changed");
        // Real original OBJECTS.DAT corpus + portable native read-only
        // inventory decoder. The CHARS values below are an AUDIT FIXTURE,
        // never a player party or a source save. Only object names, IDs,
        // categories and RES1 graphic bases are source-asserted.
        rloft_native::NativeCharsImage inventory_fixture{};
        for(unsigned actor=0;actor<2U;++actor) {
            const auto base=std::size_t(actor)*0x248U;
            const std::string name=actor==0U?"AUDIT ONE":"AUDIT TWO";
            for(std::size_t i=0;i<name.size();++i)
                inventory_fixture[base+i]=static_cast<std::uint8_t>(name[i]);
            inventory_fixture[base+0x1CU]=8U;
            inventory_fixture[base+0x1EU]=8U;
            for(std::size_t slot=0;slot<21U;++slot) {
                inventory_fixture[base+0x40U+2U*slot]=0xffU;
                inventory_fixture[base+0x41U+2U*slot]=0xffU;
            }
        }
        inventory_fixture[0x40U]=6U; // original QUIVER type, uint16
        inventory_fixture[0x41U]=0U;
        inventory_fixture[0x12BU]=1U; // slot0 known
        inventory_fixture[0x248U+0x40U]=12U; // original LEATHER ARMOR, uint16
        inventory_fixture[0x248U+0x41U]=0U;
        const auto inventory=rloft_native::inspect_native_party_inventory(
            rloft_native::decode_native_party_start(inventory_fixture),
            media.read("OBJECTS.DAT"));
        if(inventory.object_definitions!=336U ||
           inventory.occupied_total!=2U ||
           inventory.live_inventory_emulation_verified ||
           inventory.actors[0].slots[0].object_id!=6 ||
           inventory.actors[0].slots[0].source_name!="QUIVER" ||
           inventory.actors[0].slots[0].source_category!=1U ||
           inventory.actors[0].slots[0].absolute_res1_ui_id!=174U ||
           !inventory.actors[0].slots[0].identified ||
           inventory.actors[1].slots[0].object_id!=12 ||
           inventory.actors[1].slots[0].source_name!="LEATHER ARMOR" ||
           inventory.actors[1].slots[0].source_category!=4U ||
           inventory.actors[1].slots[0].absolute_res1_ui_id!=180U ||
           inventory.actors[1].slots[0].identified)
            throw std::runtime_error(
                "original OBJECTS.DAT item identities or CHARS equipment field mapping changed");
        // Original CODE.1: validate the same source-backed presenter that
        // the Windows supervisor uses, including the full 70-frame opening.
        rloft::CharacterCreationPresenter character(visuals);
        std::array<std::uint64_t,3> character_hashes{};
        for(int frame=0;
            frame<rloft::CharacterCreationPresenter::kOpeningFrameCount;
            ++frame) {
            character.opening_frame(frame);
            if(character.framebuffer().pixels().size()!=320U*400U)
                throw std::runtime_error("original CODE.1 opening frame size");
            if(frame==0)character_hashes[0]=checksum(character.framebuffer().pixels());
            if(frame==69)character_hashes[1]=checksum(character.framebuffer().pixels());
        }
        rloft::CharacterCreationFlow creation;
        character.screen(creation);
        character_hashes[2]=checksum(character.framebuffer().pixels());
        // Original CODE.1 event 3 selects the first unoccupied actor slot.
        if(!creation.apply_event(3)||
           creation.state()!=rloft::CharacterCreationState::SexChoice)
            throw std::runtime_error("original CODE.1 actor choice handoff");
        character.screen(creation);
        if(character.framebuffer().pixels().size()!=320U*400U)
            throw std::runtime_error("original CODE.1 first choice screen size");
        std::cout<<"{\"schema\":\"native-source-startup-v1\","
                 <<"\"original_first_party_level\":22,"
                 <<"\"original_first_party_spawn_cell_passable\":true,"
                 <<"\"original_first_level_controls\":10,"
                 <<"\"original_first_level_kind1_controls\":0,"
                 <<"\"original_first_level_nonempty_event_slots\":0,"
                 <<"\"original_first_level_family_records\":[1,110,0],"
                 <<"\"original_first_level_authored_scene_instances\":111,"
                 <<"\"original_first_level_source_type_counts\":[1,52,42,16],"
                 <<"\"original_first_level_npc9_original_pose_verified\":true,"
                 <<"\"original_first_level_npc9_inspection_target_verified\":true,"
                 <<"\"original_first_level_assassin_res3_315_base_verified\":true,"
                 <<"\"original_first_level_distinct_eh7_bases_decoded\":"
                 <<decoded_base_ids.size()<<","
                 <<"\"original_first_level_live_eh7_frames_verified\":false,"
                 <<"\"original_first_level_res3_bases_valid\":true,"
                 <<"\"character_equipment_rows_verified\":90,"
                 <<"\"original_party_inventory_definitions\":336,"
                 <<"\"original_party_inventory_slots_per_actor\":21,"
                 <<"\"original_party_inventory_object6_12_verified\":true,"
                 <<"\"original_party_inventory_gameplay_verified\":false,"
                 <<"\"original_generated_party_inventory_verified\":true,"
                 <<"\"original_generated_party_starting_items\":"
                 <<original_starter_items<<","
                 <<"\"original_res1_inventory_icon_records\":"
                 <<icon_corpus.examined<<","
                 <<"\"original_res1_inventory_icons_decoded\":"
                 <<icon_corpus.nonempty<<","
                 <<"\"original_res1_inventory_icons_empty\":"
                 <<icon_corpus.zero_length<<","
                 <<"\"original_res1_inventory_starter_icon_links\":"
                 <<starter_icon_slots<<","
                 <<"\"original_inventory_icon_live_ui_parity_verified\":false,"

                 <<"\"character_opening_frames\":70,"
                 <<"\"character_controller_actor_and_sex\":true,"
                 <<"\"character_frame_hashes\":["
                 <<character_hashes[0]<<','<<character_hashes[1]<<','
                 <<character_hashes[2]<<"],"
                 <<"\"original_res0_images_verified\":12,"
                 <<"\"menu_animation_and_selection_frames\":"<<source_frames<<","
                 <<"\"indexed_width\":320,\"indexed_height\":400,"
                 <<"\"logo_hashes\":[";
        for(unsigned i=0;i<logo_hashes.size();++i) {
            if(i)std::cout<<',';
            std::cout<<logo_hashes[i];
        }
        std::cout<<"],\"writes_to_original\":0,"
                 <<"\"dos_frame_parity_claim\":false}\n";
        return 0;
    }catch(const std::exception& error) {
        std::cerr<<"original startup audit FAILED: "<<error.what()<<'\n';
        return 1;
    }
}
