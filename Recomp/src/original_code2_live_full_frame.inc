// Original 1F8FC chained into both original cache releases, full party
// 1F494 and 1F1C8 until the independently verified first world sprite.
// This is a source-worker diagnostic, invoked AFTER archive parity checks.
// It never reports a full frame or bypasses missing polygon/scene callbacks.
struct OriginalFullFrameTrace {
    u32 backgrounds=0u;
    u32 releases=0u;
    u32 captures=0u;
    u32 presentations=0u;
    u32 original_res4_requests=0u;
    u32 oblique_rasters=0u;
    u32 polygon_cells=0u;
    u32 polygon_projections=0u;
    u32 polygon_quads=0u;
    u32 polygon_scan_returns=0u;
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
// Optional deeper original 314B0 branch. Unlike the first-sprite prefix,
// the real 314B0 loops over its retained actor records until exhausted.
// Only original 2A478's genuine un-clipped raster is accepted here; any
// other raster variant must fail closed instead of vanishing.
u32 original_full_exterior_resource(u32 id,u32 slot) noexcept {
    if(!exterior_trace || slot!=8u || !current->game_audio.app68)
        stop("ORIGINAL_314B0_FULL_RES8_RESOURCE_ABI_CHANGED");
    const u32 catalog=code2::load_le32(current->data.data()+0xF0B8u);
    if(catalog==0u || std::uint64_t(catalog)+9u*16u>current->data.size())
        stop("ORIGINAL_314B0_FULL_RES8_ARCHIVE_TABLE_MISSING");
    const u32 count=code2::load_le32(current->data.data()+catalog+8u*16u+8u);
    if(id>=count)
        stop("ORIGINAL_314B0_FULL_RES8_RECORD_OUT_OF_BOUNDS");
    // Source 2F29C is allowed to return null for a valid record, and
    // compiled 2986C/29564 tests image==0 and omits that raster.
    // Do not turn an authentic absent image into a fatal host exception.
    const u32 image=code2::function_2f29c(
        current->machine,current->game_audio,id,slot);
    if(image && std::uint64_t(image)+8u>current->data.size())
        stop("ORIGINAL_314B0_FULL_RES8_NONZERO_POINTER_INVALID");
    if(!image) {
        const u32 rec=catalog+8u*16u;
        const u32 dir=code2::load_le32(current->data.data()+rec+12u);
        const u32 entry=dir+id*9u;
        std::cerr<<"CODE2_ORIGINAL_LIVE_BOOT=314B0_RES8_NULL_IMAGE"
                 <<" id="<<id<<" archive=8"
                 <<" cache_type="<<unsigned(current->data[entry]&0x0Fu)
                 <<" cache_status="<<unsigned(current->data[entry+2u])
                 <<" source_return=0_ALLOWED"
                 <<" retain_guest=YES\\n"<<std::flush;
    }
    ++exterior_trace->resource_requests;
    exterior_trace->resource_id=id;
    exterior_trace->resource_slot=slot;
    exterior_trace->resource_image=image;
    return image;
}
u32 original_full_exterior_subtractive(
    u32 frame,u32 row,u32 x,u32 image,u32 delta) noexcept {
    if(!exterior_trace || !image || delta>8u ||
       std::uint64_t(image)+8u>current->data.size())
        stop("ORIGINAL_314B0_FULL_2A478_RASTER_ARGUMENTS_INVALID");
    try {
        rloft::source_video_boot::Bridge bridge(
            current->machine,*current->original_vga,current->data.size());
        bridge.run_original_2a478_subtractive(frame,row,x,image,delta);
    }catch(const std::exception& e) {
        std::cerr<<"CODE2_ORIGINAL_LIVE_BOOT=314B0_NEXT_RASTER_FRONTIER"
                 <<" frame="<<frame<<" row="<<row<<" x="<<x
                 <<" image="<<image<<" delta="<<delta
                 <<" reason="<<e.what()<<"\\n"<<std::flush;
        stop("ORIGINAL_314B0_FULL_2A478_SOURCE_RASTER_FAILED");
    }
    ++exterior_trace->subtractive_draws;
    exterior_trace->palette_delta=delta;
    return 0u;
}
u32 original_full_exterior_unclipped(
    u32 frame,u32 row,u32 x,u32 image) noexcept {
    original_scene_res_blit(frame,row,x,image);
    return 0u;
}
u32 original_full_exterior_unsupported_raster(
    u32,u32,u32,u32,u32) noexcept {
    stop("ORIGINAL_314B0_FULL_2A6B0_CLIPPED_RASTER_PENDING");
}
u32 original_full_exterior_2a8a0(
    u32 frame,u32 row,u32 x,u32 clip,u32 image,u32 delta) noexcept {
    if(!exterior_trace || !image || delta>8u)
        stop("ORIGINAL_314B0_2A8A0_CLIPPED_ARGUMENTS_CHANGED");
    try {
        rloft::source_video_boot::Bridge bridge(
            current->machine,*current->original_vga,current->data.size());
        bridge.run_original_2a8a0_clipped(frame,row,x,clip,image,delta);
    } catch(const std::exception& e) {
        std::cerr<<"CODE2_ORIGINAL_LIVE_BOOT=314B0_CLIPPED_RASTER_FRONTIER"
                 <<" row="<<row<<" x="<<x
                 <<" clip="<<clip<<" image="<<image
                 <<" delta="<<delta<<" reason="<<e.what()<<"\\n"<<std::flush;
        stop("ORIGINAL_314B0_FULL_2A8A0_SOURCE_RASTER_FAILED");
    }
    ++exterior_trace->subtractive_draws;
    ++exterior_trace->clipped_rasters;
    exterior_trace->palette_delta=delta;
    return 0u;
}
void original_full_exterior_actor(u32 view) noexcept {
    if(!exterior_trace || view!=1u)
        stop("ORIGINAL_314B0_FULL_VIEW_CHANGED");
    ++exterior_trace->active_actor_runs;
    code2::Application57Boundary b{};
    b.call_2f29c=&original_full_exterior_resource;
    b.call_2a380=&original_full_exterior_unclipped;
    b.call_2a478=&original_full_exterior_subtractive;
    b.call_2a6b0=&original_full_exterior_unsupported_raster;
    b.call_2a8a0=&original_full_exterior_2a8a0;
    code2::Application73Boundary actor{};
    actor.app57=&b;
    code2::function_314b0(current->machine,actor,view);
    if(exterior_trace->subtractive_draws==0u)
        stop("ORIGINAL_314B0_FULL_NO_SOURCE_SPRITES");
}

// Application43 is shared between party HUD and 1F1C8. Exterior
// oblique/polygon textures request RES4, not RES1 portraits. Forward
// these authentic requests to the retained 2F29C loader, preserving
// null image returns on valid archive entries.
u32 original_full_frame_resource(u32 id,u32 slot) noexcept {
    if(slot==0u || slot==1u)
        return original_scene_hud_res_image(id,slot);
    if((slot!=4u && slot!=8u) ||
       !current->game_audio.app68 || !current->original_vga)
        stop("ORIGINAL_1F8FC_SHARED_RESOURCE_ABI_CHANGED");
    const u32 catalog=code2::load_le32(current->data.data()+0xF0B8u);
    if(!catalog || std::uint64_t(catalog)+(slot+1u)*16u>current->data.size())
        stop("ORIGINAL_1F8FC_SHARED_ARCHIVE_DIRECTORY_NOT_READY");
    const u32 rec=catalog+slot*16u;
    const u32 count=code2::load_le32(current->data.data()+rec+8u);
    if(id>=count)
        stop("ORIGINAL_1F8FC_SHARED_RESOURCE_RECORD_OUT_OF_BOUNDS");
    if(slot==4u && original_full_frame_trace)
        ++original_full_frame_trace->original_res4_requests;
    const u32 image=code2::function_2f29c(
        current->machine,current->game_audio,id,slot);
    if(image && std::uint64_t(image)+8u>current->data.size())
        stop("ORIGINAL_1F8FC_SHARED_RESOURCE_IMAGE_INVALID");
    return image;
}

void original_full_exterior_oblique_sprite() noexcept {
    if(!exterior_trace || !original_full_frame_trace ||
       !current->original_vga)
        stop("ORIGINAL_1F1C8_34A74_RETAINED_GUEST_ABI_CHANGED");
    try {
        rloft::source_video_boot::Bridge bridge(
            current->machine,*current->original_vga,current->data.size());
        bridge.run_original_34a74_oblique();
    } catch(const std::exception& e) {
        std::cerr<<"CODE2_ORIGINAL_LIVE_BOOT=34A74_RES4_TEXTURE_FRONTIER"
                 <<" image="<<code2::load_le32(current->data.data()+0xC040u)
                 <<" line_count="<<code2::load_le32(current->data.data()+0xFF50u)
                 <<" reason="<<e.what()<<"\\n"<<std::flush;
        stop("ORIGINAL_1F1C8_34A74_SOURCE_RASTER_FAILED");
    }
    ++original_full_frame_trace->oblique_rasters;
}

// Actual compiled 34DE8 map scan and original 35798 perspective
// projection. The first projected polygon remains fail-closed at
// the unconnected 33E6C raster edge/scanline callbacks.
u32 original_full_polygon_record(code2::s32 x,code2::s32 y) noexcept {
    if(!original_full_frame_trace || x<0 || y<0)
        stop("ORIGINAL_34DE8_LEVEL_RECORD_QUERY_INVALID");
    ++original_full_frame_trace->polygon_cells;
    return code2::function_1e8c8(
        current->machine,static_cast<u32>(x),static_cast<u32>(y));
}
void original_full_polygon_project(code2::Machine& m,
        u32 x,u32 y,u32 screen,u32 depth) noexcept {
    if(!original_full_frame_trace ||
       std::uint64_t(depth)+2u>current->data.size())
        stop("ORIGINAL_34DE8_35798_PROJECTOR_INVALID");
    ++original_full_frame_trace->polygon_projections;
    code2::function_35798(m,x,y,screen,depth);
}
void original_full_polygon_quad(
        code2::Machine&,code2::Runtime103Boundary&,
        const u32* quad) noexcept {
    if(!original_full_frame_trace || !quad)
        stop("ORIGINAL_34DE8_SOURCE_STACK_QUAD_MISSING");
    ++original_full_frame_trace->polygon_quads;
    std::cerr<<"CODE2_ORIGINAL_LIVE_BOOT=34DE8_ORIGINAL_VERTEX_QUAD"
             <<" ordinal="<<original_full_frame_trace->polygon_quads
             <<" source=34DE8_35798_34BB0"
             <<" vertex0_x="<<quad[0]
             <<" vertex0_y="<<quad[1]
             <<" original_guest=PRESERVED\n"<<std::flush;
    stop("ORIGINAL_34DE8_33E6C_POLYGON_RASTER_PENDING");
}
void original_full_exterior_polygon_source() noexcept {
    if(!original_full_frame_trace || !current->original_vga)
        stop("ORIGINAL_34DE8_SOURCE_GUEST_PREREQUISITES");
    code2::Runtime110Boundary scan{};
    scan.call_1e8c8=&original_full_polygon_record;
    scan.call_35798=&original_full_polygon_project;
    code2::Runtime109Boundary quad{};
    quad.call_2f29c=&original_full_frame_resource;
    quad.call_33e6c_local_quad=&original_full_polygon_quad;
    code2::Runtime103Boundary raster{};
    code2::function_34de8(current->machine,scan,quad,raster);
    ++original_full_frame_trace->polygon_scan_returns;
}
void original_full_1bdac_frontier() noexcept {
    if(!original_full_frame_trace ||
       original_full_frame_trace->polygon_scan_returns!=1u)
        stop("ORIGINAL_1BDAC_POLYGON_PREDECESSOR_MISSING");
    stop("ORIGINAL_1BDAC_POST_POLYGON_ACTOR_CALLBACK_PENDING");
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
    ui.call_2f29c=&original_full_frame_resource;
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
    const char* const polygon_advance=
        std::getenv("RLOFT_RECOMP_EXPERIMENTAL_POLYGON_FRONTIER");
    const bool through_actor=polygon_advance &&
        std::strcmp(polygon_advance,"1")==0;
    scene.call_314b0=through_actor?
        &original_full_exterior_actor:&original_exterior_active_actor;
    scene.call_33e28=&original_exterior_sky;
    scene.call_33de0=&original_exterior_ground;
    scene.call_34a74=through_actor?
        &original_full_exterior_oblique_sprite:&original_exterior_oblique_sprite;
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
    const char* const expected_stop=through_actor?
        "ORIGINAL_1F1C8_34DE8_POLYGON_RASTER_PENDING":
        "ORIGINAL_1F1C8_314B0_FIRST_RES8_50_VGA_SPRITE_DRAWN";
    if(!current->stop_reason ||
       std::strcmp(current->stop_reason,expected_stop)!=0 ||
       frame.backgrounds!=2u || frame.releases!=2u ||
       frame.source_ids[0]!=expected || frame.source_ids[1]!=expected+1u ||
       exterior.active_actor_runs!=1u || exterior.subtractive_draws==0u ||
       exterior.resource_requests<exterior.subtractive_draws ||
       (exterior.angle_patches!=0u && exterior.angle_patches!=4u) ||
       current->data[0xC050u]!=initial_page ||
       frame.presentations!=0u)
    {
        std::cerr<<"CODE2_ORIGINAL_LIVE_BOOT=1F8FC_CHAIN_DIAGNOSTIC"
                 <<" stop="<<(current->stop_reason?current->stop_reason:"NONE")
                 <<" background="<<frame.backgrounds
                 <<" release="<<frame.releases
                 <<" captures="<<frame.captures
                 <<" source_resource_requests="<<exterior.resource_requests
                 <<" exterior_actor="<<exterior.active_actor_runs
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
             <<" clipped_source_rasters="<<exterior.clipped_rasters
             <<" captures="<<frame.captures
             <<" res4_source_requests="<<frame.original_res4_requests
             <<" oblique_source_rasters="<<frame.oblique_rasters
             <<" angle_patches="<<exterior.angle_patches
             <<" presentations="<<frame.presentations
             <<" guest=PRESERVED"
             <<" next="<<(through_actor?"34DE8_POLYGON_RASTER":"EXTERIOR_REMAINING_SOURCE")
             <<" complete_frames=0\n"<<std::flush;
    current->stop_reason=nullptr;
}
