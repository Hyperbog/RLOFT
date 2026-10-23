// Original 1F8FC chained into both original cache releases, full party
// 1F494 and 1F1C8 until the independently verified first world sprite.
// This is a source-worker diagnostic, invoked AFTER archive parity checks.
// It never reports a full frame or bypasses missing polygon/scene callbacks.
struct OriginalFullFrameTrace {
    u32 backgrounds=0u;
    u32 releases=0u;
    u32 captures=0u;
    u32 presentations=0u;
    u32 source_ids[2]{};
};
thread_local OriginalFullFrameTrace* original_full_frame_trace=nullptr;

void original_full_frame_background(u32 row,u32 x,u32 id,u32 slot) noexcept {
    auto* t=original_full_frame_trace;
    if(!t || t->backgrounds>=2u || slot!=0u ||
       row!=(t->backgrounds?0xC8u:0u) || x!=0u)
        stop("ORIGINAL_1F8FC_CHAIN_BACKGROUND_ORDER_CHANGED");
    t->source_ids[t->backgrounds++]=id;
    original_scene_frame_base(row,x,id,slot);
}
void original_full_frame_release(u32 id,u32 slot) noexcept {
    auto* t=original_full_frame_trace;
    if(!t || slot!=0u || t->backgrounds!=2u ||
       t->releases>=2u || id!=t->source_ids[t->releases])
        stop("ORIGINAL_1F8FC_CHAIN_RELEASE_ORDER_CHANGED");
    original_scene_res_release(id,slot); // Actual compiled 2EFD0.
    ++t->releases;
}
u32 original_full_frame_capture(u32 y,u32 x,u32 h,u32 w,u32 tag) noexcept {
    if(!original_full_frame_trace)
        stop("ORIGINAL_1F8FC_CHAIN_CAPTURE_OUTSIDE_SOURCE");
    const u32 result=original_scene_capture(y,x,h,w,tag);
    ++original_full_frame_trace->captures;
    return result;
}
void original_full_frame_present(u32 page,code2::s32 cursor,u32 variant) noexcept {
    if(!original_full_frame_trace)
        stop("ORIGINAL_1F8FC_CHAIN_PRESENT_OUTSIDE_SOURCE");
    original_scene_1a5b0(page,cursor,variant);
    ++original_full_frame_trace->presentations;
}
void original_live_1f8fc_source_chain() noexcept {
    if(!current || !current->live_continuation ||
       current->original_181b4_completed!=1u ||
       !current->game_audio.app68 || !current->original_vga ||
       code2::load_le32(current->data.data()+0xC034u)!=3u)
        stop("ORIGINAL_1F8FC_CHAIN_NOT_ON_RETAINED_LEVEL22_GUEST");
    OriginalFullFrameTrace frame{};
    ExteriorSourceTrace exterior{};
    original_full_frame_trace=&frame;
    exterior_trace=&exterior;
    code2::Application43Boundary ui{};
    ui.call_19cc4=&original_live_hud_clock;
    ui.call_2f29c=&original_scene_hud_res_image;
    ui.call_29b40=&original_live_hud_rect;
    ui.call_24478=&original_live_hud_item;
    ui.call_2a258=&original_live_hud_2a258;
    ui.call_298f0=&original_scene_hud_298f0;
    ui.call_2e374=&original_live_hud_border;
    ui.call_2975c=&original_scene_hud_2975c;
    ui.call_2e6f0=&original_live_hud_text_number;
    ui.call_30508=&original_live_hud_text;
    ui.call_2939c=&original_scene_res_blit;
    ui.divide_error=&original_live_hud_divide_error;

    code2::Application44Boundary scene{};
    scene.app43=&ui;
    scene.call_291b0=&original_291b0_source_page;
    scene.call_ef14=&original_live_hud_hp;
    scene.call_2e510=&original_scene_frame_base;
    scene.call_2e4a4=&original_live_hud_2e4a4;
    scene.call_22f40=&original_live_hud_22f40;
    scene.call_23024=&original_live_hud_group;
    scene.call_334c4=&original_world_camera_334c4;
    scene.call_34998=&original_exterior_angle_refresh;
    scene.call_314b0=&original_exterior_active_actor;
    scene.call_33e28=&original_exterior_sky;
    scene.call_33de0=&original_exterior_ground;
    scene.call_34a74=&original_exterior_oblique_sprite;
    scene.call_34de8=&original_exterior_polygon_frontier;
    scene.call_2f868=&original_scene_poll_source_events;
    scene.divide_error=&original_live_hud_divide_error;

    code2::Application45Boundary compositor{};
    compositor.app43=&ui;
    compositor.app44=&scene;
    compositor.call_291b0=&original_291b0_source_page;
    compositor.call_2e510=&original_full_frame_background;
    compositor.call_2efd0=&original_full_frame_release;
    compositor.call_30040=&original_scene_capture_release;
    compositor.call_30070=&original_full_frame_capture;
    compositor.call_325b0=&original_scene_palette;
    compositor.call_29234=&original_video_palette_29234;
    compositor.call_1a5b0=&original_full_frame_present;
    compositor.call_22d70=&original_scene_clear_event_ring;
    compositor.call_2f868=&original_scene_poll_source_events;
    compositor.call_30b2c=&original_scene_finish_keyboard;

    const u32 mode=code2::load_le32(current->data.data()+0xC034u);
    const u32 expected=code2::load_le32(current->data.data()+0x258DCu)+1u+mode*3u;
    const u32 initial_page=current->data[0xC050u];
    std::jmp_buf scope{};
    auto* prior=current->active_escape;
    const int stopped=setjmp(scope);
    if(stopped==0) {
        current->active_escape=&scope;
        code2::function_1f8fc(current->machine,compositor);
        stop("ORIGINAL_1F8FC_CHAIN_RETURNED_WITHOUT_ORIGINAL_WORLD_RENDER");
    }
    current->active_escape=prior;
    original_full_frame_trace=nullptr;
    exterior_trace=nullptr;
    if(!current->stop_reason ||
       std::strcmp(current->stop_reason,
           "ORIGINAL_1F1C8_314B0_FIRST_RES8_50_VGA_SPRITE_DRAWN")!=0 ||
       frame.backgrounds!=2u || frame.releases!=2u ||
       frame.source_ids[0]!=expected || frame.source_ids[1]!=expected+1u ||
       exterior.actor_runs!=1u || exterior.subtractive_draws!=1u ||
       exterior.resource_requests!=1u || exterior.angle_patches!=4u ||
       current->data[0xC050u]!=initial_page ||
       frame.presentations!=0u)
    {
        std::cerr<<"CODE2_ORIGINAL_LIVE_BOOT=1F8FC_CHAIN_DIAGNOSTIC"
                 <<" stop="<<(current->stop_reason?current->stop_reason:"NONE")
                 <<" background="<<frame.backgrounds
                 <<" release="<<frame.releases
                 <<" captures="<<frame.captures
                 <<" exterior_actor="<<exterior.actor_runs
                 <<" world_sprites="<<exterior.subtractive_draws
                 <<" angle_patches="<<exterior.angle_patches
                 <<" presentations="<<frame.presentations<<"\n"<<std::flush;
        stop("ORIGINAL_1F8FC_CHAIN_SCENE_SEQUENCE_UNVERIFIED");
    }
    for(u32 i=0u;i<4u;++i)
        if(current->data[0xCF5Au+i]!=0u)
            stop("ORIGINAL_1F8FC_CHAIN_PARTY_HUD_NOT_CLEARED");
    std::cerr<<"CODE2_ORIGINAL_LIVE_BOOT=1F8FC_COMPOSITOR_CHAIN"
             <<" original_source=1F8FC_2E510_2EFD0_1F494_1EA90_1F1C8_314B0_2A478"
             <<" backgrounds=2 releases=2"
             <<" party_dirty_flags=CLEARED"
             <<" world_sprite_draws="<<exterior.subtractive_draws
             <<" captures="<<frame.captures
             <<" presentations="<<frame.presentations
             <<" guest=PRESERVED"
             <<" next=EXTERIOR_REMAINING_SOURCE"
             <<" complete_frames=0\n"<<std::flush;
    current->stop_reason=nullptr;
}
