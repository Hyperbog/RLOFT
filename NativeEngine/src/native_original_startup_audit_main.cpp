#include "rloft_native/native_media.hpp"
#include "rloft_native/native_startup_visuals.hpp"
#include "rloft_native/native_world_payload.hpp"
#include "rloft_native/native_world_runtime.hpp"
#include "rloft_native/native_party_start.hpp"
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
