#include "rloft_native/native_media.hpp"
#include "rloft_native/native_startup_visuals.hpp"
#include "rloft_native/native_world_payload.hpp"
#include "rloft_native/native_world_runtime.hpp"
#include "rloft_native/native_party_start.hpp"
#include "rloft_native/native_scene_instances.hpp"
#include "rloft_native/native_assets.hpp"
#include "rloft/startup_presenter.hpp"
#include "rloft/character_creation_presenter.hpp"
#include "rloft/character_creation_flow.hpp"
#include "rloft/character_transfer.hpp"
#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>
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
        const auto res3_count=rloft_native::NativeResArchive(
            media.read("RES3")).entry_count();
        const auto banks=rloft_native::decode_native_res3_scene_banks(
            media.read("INIT"),res3_count);
        const auto definitions=rloft_native::decode_native_scene_definitions(
            banks,media.read("NPCS.DAT"),media.read("OBJECTS.DAT"));
        const auto placements=rloft_native::derive_native_authored_scene_instances(
            world.original(),definitions);
        const auto& initial=placements.levels.at(22U);
        if(initial.size()!=111U || first.families[0].size()!=1U ||
           first.families[1].size()!=110U ||
           !first.families[2].empty())
            throw std::runtime_error(
                "original Level22 source actor/placed placement count changed");
        for(const auto& item:initial) {
            if(item.level!=22U || item.absolute_res3_base>=res3_count)
                throw std::runtime_error(
                    "Level22 authentic RES3 scene instance out of range");
        }
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
                 <<"\"original_first_level_res3_bases_valid\":true,"
                 <<"\"character_equipment_rows_verified\":90,"
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
