// Retained Level22 exterior source frontier: execute actual 1F1C8,
// its 334C4->32954 camera/ray preparation, original 34998 view coefficients,
// and authentic 314B0->29838 RES8 active-environment branch until the
// first not-yet-connected 2A478 subtractive planar sprite raster.
// Also retain true 33DE0/33E28 VGA REP STOSB callbacks for other profiles.
// Do NOT equate this bounded prefix with a completed exterior/game frame.
void original_world_camera_334c4() noexcept;

struct ExteriorSourceTrace {
    u32 sky_runs=0u,ground_runs=0u;
    u32 write_calls=0u,write_bytes=0u,angle_patches=0u;
    u32 active_actor_runs=0u,resource_requests=0u;
    u32 resource_id=0u,resource_slot=0u;
};
thread_local ExteriorSourceTrace* exterior_trace=nullptr;

void original_exterior_out16(u16 port,u16 value) noexcept {
    if(!exterior_trace || port!=0x3C4u || value!=0x0F02u ||
       !current->original_vga ||
       !current->original_vga->write_indexed_port(port,value))
        stop("ORIGINAL_1F1C8_33E28_VGA_SEQUENCER_UNSUPPORTED");
}
void original_exterior_stosb(u32 absolute,u8 color,u32 count) noexcept {
    if(!exterior_trace || count!=0x40u ||
       absolute<0xA0000u || std::uint64_t(absolute)+count>0xB0000u ||
       !current->original_vga || current->original_vga->map_mask()!=0x0Fu)
        stop("ORIGINAL_1F1C8_33E28_ES_VGA_APERTURE_OUT_OF_RANGE");
    for(u32 byte=0u;byte<count;++byte) {
        if(!current->original_vga->write_aperture(
             static_cast<u16>(absolute-0xA0000u+byte),color))
            stop("ORIGINAL_1F1C8_33E28_PLANAR_VGA_WRITE_FAILED");
    }
    ++exterior_trace->write_calls;
    exterior_trace->write_bytes+=count;
}
void original_exterior_sky(u32 color) noexcept {
    code2::Runtime102Boundary b{};
    b.out16=&original_exterior_out16;
    b.rep_stosb_es=&original_exterior_stosb;
    code2::function_33e28(current->machine,b,color);
    if(!exterior_trace || exterior_trace->write_calls!=128u)
        stop("ORIGINAL_1F1C8_33E28_SOURCE_ROWS_INVALID");
    ++exterior_trace->sky_runs;
}
void original_exterior_ground(u32 color) noexcept {
    code2::Runtime102Boundary b{};
    b.out16=&original_exterior_out16;
    b.rep_stosb_es=&original_exterior_stosb;
    const u32 before=exterior_trace?exterior_trace->write_calls:0u;
    code2::function_33de0(current->machine,b,color);
    if(!exterior_trace || exterior_trace->write_calls!=before+128u)
        stop("ORIGINAL_1F1C8_33DE0_SOURCE_ROWS_INVALID");
    ++exterior_trace->ground_runs;
}
void original_exterior_angle_patch(u32 at,u16) noexcept {
    if(!exterior_trace ||
       (at!=0x34A11u && at!=0x34A27u &&
        at!=0x34A3Du && at!=0x34A54u))
        stop("ORIGINAL_1F1C8_34998_PATCH_TARGET_CHANGED");
    // Record the genuine 34998 source callbacks. The actual 34A74
    // self-modified raster is NOT executed by this bounded probe.
    ++exterior_trace->angle_patches;
}
void original_exterior_angle_refresh() noexcept {
    code2::Runtime107Boundary b{};
    b.patch_code16=&original_exterior_angle_patch;
    const u32 before=exterior_trace?exterior_trace->angle_patches:0u;
    code2::function_34998(current->machine,b);
    if(!exterior_trace || exterior_trace->angle_patches!=before+4u)
        stop("ORIGINAL_1F1C8_34998_SOURCE_PATCHES_NOT_OBSERVED");
}
u32 original_exterior_actor_resource(u32 key,u32 slot) noexcept {
    if(!exterior_trace)stop("ORIGINAL_1F1C8_314B0_RESOURCE_CALLBACK_OUTSIDE_SOURCE");
    ++exterior_trace->resource_requests;
    exterior_trace->resource_id=key;
    exterior_trace->resource_slot=slot;
    // Fail before substituting a RES sprite or skipping the clipped raster.
    stop("ORIGINAL_1F1C8_314B0_RESOURCE_SPRITE_RASTER_PENDING");
}
void original_exterior_active_actor(u32 view) noexcept {
    if(!exterior_trace || view!=1u)
        stop("ORIGINAL_1F1C8_314B0_ACTIVE_WORLD_VIEW_CHANGED");
    ++exterior_trace->active_actor_runs;
    code2::Application57Boundary sprite{};
    sprite.call_2f29c=&original_exterior_actor_resource;
    code2::Application73Boundary actor{};
    actor.app57=&sprite;
    code2::function_314b0(current->machine,actor,view);
    stop("ORIGINAL_1F1C8_314B0_SOURCE_RETURNED_WITHOUT_SPRITES");
}
void original_exterior_oblique_sprite() noexcept {
    stop("ORIGINAL_1F1C8_34A74_OBLIQUE_SPRITE_PENDING");
}
void original_exterior_polygon_frontier() noexcept {
    stop("ORIGINAL_1F1C8_34DE8_POLYGON_RASTER_PENDING");
}

void original_live_1f1c8_source_prefix_probe() noexcept {
    if(!current->original_vga || !current->live_continuation ||
       current->original_181b4_completed!=1u ||
       code2::load_le32(current->data.data()+0xC034u)!=3u)
        stop("ORIGINAL_1F1C8_RETAINED_GUEST_PREREQUISITES");

    auto indexed_draw=[]() {
        auto video=*current->original_vga;
        if(!video.write_indexed_port(0x3D4u,0x800Cu) ||
           !video.write_indexed_port(0x3D4u,0x000Du) ||
           video.crtc_start()!=0x8000u)
            stop("ORIGINAL_1F1C8_RETAINED_VGA_DRAW_PAGE_UNAVAILABLE");
        return video.scanout_indices();
    };
    const auto before=indexed_draw();
    ExteriorSourceTrace trace{};
    exterior_trace=&trace;
    code2::Application43Boundary ui{};
    ui.call_29b40=&original_live_hud_rect;
    ui.call_2f29c=&original_scene_hud_res_image;
    code2::Application44Boundary b{};
    b.app43=&ui;
    b.call_291b0=&original_291b0_source_page;
    b.call_334c4=&original_world_camera_334c4;
    b.call_34998=&original_exterior_angle_refresh;
    b.call_314b0=&original_exterior_active_actor;
    b.call_33e28=&original_exterior_sky;
    b.call_33de0=&original_exterior_ground;
    b.call_34a74=&original_exterior_oblique_sprite;
    b.call_34de8=&original_exterior_polygon_frontier;

    std::jmp_buf scoped{};
    auto* const previous=current->active_escape;
    const int stopped=setjmp(scoped);
    if(stopped==0) {
        current->active_escape=&scoped;
        code2::function_1f1c8(current->machine,b);
        stop("ORIGINAL_1F1C8_PREFIX_UNEXPECTED_FULL_RETURN");
    }
    current->active_escape=previous;
    exterior_trace=nullptr;
    if(!current->stop_reason ||
       std::strcmp(current->stop_reason,
                   "ORIGINAL_1F1C8_314B0_RESOURCE_SPRITE_RASTER_PENDING")!=0) {
        std::cerr<<"CODE2_ORIGINAL_LIVE_BOOT=1F1C8_PREFIX_FRONTIER_DIAGNOSTIC"
                 <<" first_unsupported="<<(current->stop_reason?current->stop_reason:"NONE")
                 <<" sky_runs="<<trace.sky_runs
                 <<" ground_runs="<<trace.ground_runs
                 <<" raster_bytes="<<trace.write_bytes
                 <<" angle_patches="<<trace.angle_patches
                 <<" actor_runs="<<trace.active_actor_runs
                 <<" resource_requests="<<trace.resource_requests
                 <<" resource_id="<<trace.resource_id
                 <<" resource_slot="<<trace.resource_slot
                 <<" c026="<<unsigned(current->data[0xC026u])
                 <<" c027="<<unsigned(current->data[0xC027u])
                 <<" c028="<<unsigned(current->data[0xC028u])
                 <<" active="<<unsigned(current->data[0x258A8u])
                 <<" kind="<<unsigned(current->data[0x258B0u])
                 <<" world_kind="<<unsigned(current->data[0x258B1u])
                 <<" page="<<unsigned(current->data[0xC050u])
                 <<"\\n"<<std::flush;
        stop("ORIGINAL_1F1C8_PREFIX_UNEXPECTED_DEPENDENCY");
    }
    const auto after=indexed_draw();
    u32 changed=0u;
    for(u32 y=0u;y<400u;++y)
        for(u32 x=0u;x<320u;++x)
            changed+=before[y*320u+x]!=after[y*320u+x];
    if(trace.sky_runs!=0u || trace.ground_runs!=0u ||
       trace.write_calls!=0u || trace.write_bytes!=0u ||
       trace.active_actor_runs!=1u || trace.resource_requests!=1u ||
       trace.angle_patches!=4u)
        stop("ORIGINAL_1F1C8_SOURCE_ACTIVE_RENDERER_ACCEPTANCE_FAILED");
    std::cerr<<"CODE2_ORIGINAL_LIVE_BOOT=1F1C8_EXTERIOR_SOURCE_PREFIX"
             <<" original_source=1F1C8_1EDAC_334C4_32954_34998_314B0_29838_OR_2986C"
             <<" active_world_cell=1 actor_calls="<<trace.active_actor_runs
             <<" resource_requests="<<trace.resource_requests
             <<" resource_id="<<trace.resource_id
             <<" resource_slot="<<trace.resource_slot
             <<" 34998_patch_callbacks="<<trace.angle_patches
             <<" changed_indexed_pixels="<<changed
             <<" retained_guest=YES"
             <<" frontier=314B0_RESOURCE_SPRITE_RASTER_PENDING"
             <<" full_1F8FC=NOT_ENABLED\\n"<<std::flush;
    // This diagnostic owns no scene-mode transition or palette changes.
    current->stop_reason=nullptr;
}
