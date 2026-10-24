// Defined below this include in the same startup translation unit.
void original_world_sweep_335dc() noexcept;
u32 original_world_sintab_end() noexcept;

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
    u32 polygon_vertex_copies=0u;
    u32 polygon_edges=0u;
    u32 polygon_steps=0u;
    u32 polygon_scanlines=0u;
    u32 polygon_drawn_scanlines=0u;
    u32 actor_update_returns=0u;
    u32 scene_projection_callbacks=0u;
    u32 scene_extent_callbacks=0u;
    u32 scene_wall_passes=0u;
    u32 scene_sprite_calls=0u;
    u32 scene_sprite_raster_returns=0u;
    u32 scene_sprite_visible_runs=0u;
    u32 scene_sprite_plane_passes=0u;
    u32 scene_sprite_indexed_writes=0u;
    u32 scene_sprite_off_aperture_writes=0u;
    u32 scene_projection_returns=0u;
    u32 compass_source_calls=0u;
    u32 compass_source_returns=0u;
    u32 compass_clips=0u;
    u32 compass_images=0u;
    u32 compass_vga_writes=0u;
    u32 frame_audio_queries=0u;
    u32 retained_page_returns=0u;
    u32 post_hud_return_witness=0u;
    u32 post_audio_random_callbacks=0u;
    u32 compositor_returns=0u;
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
    // The original 1F1C8 passes live 258A8 as the 314B0 view.
    // Canonical 314B0 dereferences the pointer at 2B764 first, then
    // indexes its heap table: rows=*( *(2B764)+view*4 ).
    // Do not mistake 2B764 itself for the table's first entry.
    if(!exterior_trace || view>=10u) {
        std::cerr<<"CODE2_ORIGINAL_LIVE_GAMEPLAY=314B0_SOURCE_VIEW_BOUNDARY"
                 <<" view="<<view
                 <<" camera_heading="<<code2::load_le32(current->data.data()+0xC658u)
                 <<" view_state_258A8="<<code2::load_le32(current->data.data()+0x258A8u)
                 <<" source_keyboard_code="<<current->original_1fc14_last_scan_code
                 <<" scene_returns="<<current->original_1fc14_source_scene_returns
                 <<" table_pointer="<<code2::load_le32(current->data.data()+0x2B764u)
                 <<" original_heap_lookup=REQUIRED\n"<<std::flush;
        stop("ORIGINAL_314B0_SOURCE_VIEW_OUTSIDE_ORIGINAL_TABLE");
    }
    const u32 table=code2::load_le32(
        current->data.data()+0x2B764u);
    if(!table || std::uint64_t(table)+4u*(view+1u)>current->data.size())
        stop("ORIGINAL_314B0_SOURCE_VIEW_POINTER_TABLE_MISSING");
    const u32 rows=code2::load_le32(
        current->data.data()+table+view*4u);
    if(!rows || std::uint64_t(rows)+8u>current->data.size())
        stop("ORIGINAL_314B0_SOURCE_VIEW_RECORDS_UNAVAILABLE");
    ++exterior_trace->active_actor_runs;
    std::cerr<<"CODE2_ORIGINAL_LIVE_GAMEPLAY=314B0_SOURCE_VIEW"
             <<" view="<<view<<" table="<<table<<" rows="<<rows
             <<" heading="<<code2::load_le32(current->data.data()+0xC658u)
             <<" original_table=2B750_2B764"
             <<" source_actor=RETAINED\n"<<std::flush;
    code2::Application57Boundary b{};
    b.call_2f29c=&original_full_exterior_resource;
    b.call_2a380=&original_full_exterior_unclipped;
    b.call_2a478=&original_full_exterior_subtractive;
    b.call_2a6b0=&original_full_exterior_unsupported_raster;
    b.call_2a8a0=&original_full_exterior_2a8a0;
    code2::Application73Boundary actor{};
    actor.app57=&b;
    code2::function_314b0(current->machine,actor,view);
    // The retained Level 22 baseline slot 1 has visible sprites.
    // Other authentic source slots may legitimately be empty.
    if(view==1u && exterior_trace->subtractive_draws==0u)
        stop("ORIGINAL_314B0_FULL_NO_SOURCE_SPRITES");
}

// Application43 is shared between party HUD and 1F1C8. Exterior
// oblique/polygon textures request RES4, not RES1 portraits. Forward
// these authentic requests to the retained 2F29C loader, preserving
// null image returns on valid archive entries.
u32 original_full_frame_resource(u32 id,u32 slot) noexcept {
    if(slot==0u || slot==1u)
        return original_scene_hud_res_image(id,slot);
    if((slot!=3u && slot!=4u && slot!=8u) ||
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

// Retained 34DE8 map scan and 35798 perspective projection.
// The original 33E6C, 3402C, 33FB4 and 34110 remain source-owned.
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
void original_full_polygon_copy(u32 from,u32 to,u32 count) noexcept {
    if(!original_full_frame_trace || count!=8u ||
       std::uint64_t(from)+32u>current->data.size() ||
       std::uint64_t(to)+32u>current->data.size())
        stop("ORIGINAL_33E6C_VERTEX_COPY_ABI_CHANGED");
    // Original REP MOVSD copies exactly eight guest dwords.
    std::copy_n(current->data.data()+from,32u,current->data.data()+to);
    ++original_full_frame_trace->polygon_vertex_copies;
}
void original_full_polygon_edge(u32 vertex,u32 state) noexcept {
    if(!original_full_frame_trace || vertex>3u ||
       (state!=0xFDC4u && state!=0xFDE8u))
        stop("ORIGINAL_3402C_EDGE_ARGUMENTS_CHANGED");
    (void)code2::function_3402c(current->machine,vertex,state);
    ++original_full_frame_trace->polygon_edges;
}
bool original_full_polygon_step() noexcept {
    if(!original_full_frame_trace)
        stop("ORIGINAL_33FB4_OUTSIDE_SOURCE_POLYGON");
    ++original_full_frame_trace->polygon_steps;
    return code2::function_33fb4(current->machine);
}
void original_full_polygon_scanline() noexcept {
    if(!original_full_frame_trace || !current->original_vga)
        stop("ORIGINAL_34110_OUTSIDE_SOURCE_POLYGON");
    try {
        rloft::source_video_boot::Bridge bridge(
            current->machine,*current->original_vga,current->data.size());
        const bool drawn=bridge.run_original_34110_polygon();
        ++original_full_frame_trace->polygon_scanlines;
        if(drawn)++original_full_frame_trace->polygon_drawn_scanlines;
    } catch(const std::exception& e) {
        std::cerr<<"CODE2_ORIGINAL_LIVE_BOOT=34110_POLYGON_SCANLINE_FAILURE"
                 <<" scan="<<code2::load_le32(current->data.data()+0xFD20u)
                 <<" page="<<code2::load_le32(current->data.data()+0xFDBCu)
                 <<" first="<<code2::load_le16(current->data.data()+0xFDE2u)
                 <<" last="<<code2::load_le16(current->data.data()+0xFE06u)
                 <<" reason="<<e.what()<<"\n"<<std::flush;
        stop("ORIGINAL_34110_SOURCE_POLYGON_RASTER_FAILED");
    }
}
void original_full_polygon_quad(
        code2::Machine& m,code2::Runtime103Boundary& raster,
        const u32* quad) noexcept {
    if(!original_full_frame_trace || !quad)
        stop("ORIGINAL_34DE8_SOURCE_STACK_QUAD_MISSING");
    // Original local quad is eight source dwords. Materialize a bounded
    // guest argument for synchronous 33E6C, then restore scratch bytes.
    constexpr u32 scratch=0x2BA00u;
    if(scratch+32u>current->data.size())
        stop("ORIGINAL_33E6C_QUAD_GUEST_ARGUMENT_OUT_OF_BOUNDS");
    std::array<u8,32u> previous{};
    std::copy_n(current->data.data()+scratch,32u,previous.data());
    for(u32 i=0u;i<8u;++i)
        code2::store_le32(current->data.data()+scratch+4u*i,quad[i]);
    ++original_full_frame_trace->polygon_quads;
    code2::function_33e6c(m,raster,scratch);
    std::copy(previous.begin(),previous.end(),
              current->data.data()+scratch);
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
    raster.rep_movsd_ds_es=&original_full_polygon_copy;
    raster.call_3402c=&original_full_polygon_edge;
    raster.call_33fb4=&original_full_polygon_step;
    raster.call_34110=&original_full_polygon_scanline;
    code2::function_34de8(current->machine,scan,quad,raster);
    ++original_full_frame_trace->polygon_scan_returns;
}
// Execute the original 1BDAC actor timers, state changes and record sweeps
// in the live retained guest. Unimplemented movement/asset dependencies
// stop at their real callback rather than returning fabricated values.
void original_full_actor_0d550(u32 actor,u32 definition,u32 index) noexcept {
    // Original 0D550: type 15 has a special path; type 8 enters its
    // special action when state high bits are 0x8000. Otherwise the
    // original compares both signed absolute camera/actor differences
    // with 0x15000 and returns immediately when either is greater.
    if(std::uint64_t(actor)+0x16u>current->data.size())
        stop("ORIGINAL_1BDAC_0D550_RECORD_OUT_OF_GUEST_BOUNDS");
    const u32 type=code2::load_le16(current->data.data()+actor+0xEu);
    const u32 state=code2::load_le16(current->data.data()+actor+0x14u);
    if(type==15u || (type==8u && (state&0xC000u)==0x8000u))
        stop("ORIGINAL_1BDAC_0D550_SPECIAL_ACTION_PENDING");
    // Model the original 32-bit SUB, CDQ, XOR, SUB, signed JG.
    // In particular INT_MIN must not be treated as a positive delta.
    const auto out_of_range=[](u32 camera,u32 position) noexcept {
        const u32 diff=camera-position;
        const u32 mask=0u-(diff>>31u);
        const u32 magnitude=(diff^mask)-mask;
        return (magnitude&0x80000000u)==0u && magnitude>0x15000u;
    };
    const u32 cx=code2::load_le32(current->data.data()+0x255D0u);
    const u32 cy=code2::load_le32(current->data.data()+0x255D4u);
    const u32 x=code2::load_le32(current->data.data()+actor+4u);
    const u32 y=code2::load_le32(current->data.data()+actor);
    if(out_of_range(cx,x) || out_of_range(cy,y)) {
        // Execute the actual reconstructed original 0D550 on its proven
        // callback-free distance-cull branch, not a host-side fake return.
        // All non-cull behavior still stops before calling a partial ABI.
        code2::Application19Boundary original_ai{};
        code2::function_0d550(current->machine,original_ai,actor,definition,index);
        std::cerr<<"CODE2_ORIGINAL_LIVE_GAMEPLAY=1BDAC_0D550_DISTANT_RET"
                 <<" actor="<<actor<<" definition="<<definition
                 <<" index="<<index<<" type="<<type
                 <<" camera_x="<<cx<<" camera_y="<<cy
                 <<" actor_x="<<x<<" actor_y="<<y
                 <<" original_machine_exit=0DAC4_NO_SIDE_EFFECTS\n"<<std::flush;
        return;
    }
    std::cerr<<"ORIGINAL_1BDAC_0D550_NEAR_ARGS="<<actor<<","
             <<definition<<","<<index<<" type="<<type
             <<" state="<<state<<"\n"<<std::flush;
    stop("ORIGINAL_1BDAC_0D550_NEAR_ACTION_PENDING");
}
void original_full_actor_30cd4(u32 dst,u32 src,u32 count) noexcept {
    const auto capacity=current->data.size();
    if(dst>capacity || src>capacity || count>capacity-dst || count>capacity-src)
        stop("ORIGINAL_1BDAC_30CD4_COPY_OUT_OF_GUEST_BOUNDS");
    (void)code2::function_30cd4(current->machine,dst,src,count);
}
void original_full_actor_missing2(u32,u32) noexcept {
    stop("ORIGINAL_1BDAC_EXTERNAL_2_ARGUMENT_DEPENDENCY");
}
void original_full_actor_missing1(code2::s32) noexcept {
    stop("ORIGINAL_1BDAC_RECORD_REMOVE_DEPENDENCY");
}
u32 original_full_actor_missing_predicate(u32,u32) noexcept {
    stop("ORIGINAL_1BDAC_RECORD_REMOVAL_PREDICATE_PENDING");
}
u32 original_full_actor_missing_collision(u32,u32,u32) noexcept {
    stop("ORIGINAL_1BDAC_COLLISION_CHECK_PENDING");
}
void original_full_actor_missing_movement(u32,u32,u32,u32) noexcept {
    stop("ORIGINAL_1BDAC_MOVE_PENDING");
}
void original_full_actor_missing_offsets(u32* dx,u32* dy,u32 radius,u32 heading) noexcept {
    if(!dx || !dy)stop("ORIGINAL_1BDAC_MOVEMENT_VECTOR_ARGUMENTS");
    code2::Application67Boundary movement{};
    movement.call_35680=&code2::function_35680;
    code2::function_2df2c(current->machine,movement,dx,dy,radius,heading);
}
void original_full_actor_fill_guest(u32 dst,u32 packed,u32 count) noexcept {
    if(std::uint64_t(dst)+count>current->data.size())
        stop("ORIGINAL_1BDAC_379D0_FILL_OUT_OF_BOUNDS");
    code2::function_379d0(current->data.data()+dst,count,packed);
}
void original_full_actor_missing_clear(u32 dst,u32 value,u32 count) noexcept {
    code2::Application72Boundary memory{};
    memory.call_379d0=&original_full_actor_fill_guest;
    (void)code2::function_30d30(memory,dst,value,count);
}
u32 original_full_actor_missing_query() noexcept {
    return code2::function_311e0(current->machine);
}
u32 original_full_actor_seed_callback(u32 value) noexcept {
    code2::function_30cc4(current->machine,value);
    return value;
}
void original_full_actor_missing_update(u32 seed) noexcept {
    code2::Application56Boundary timer{};
    timer.call_30cc4=&original_full_actor_seed_callback;
    (void)code2::function_290d0(current->machine,timer,seed);
}
void original_full_actor_divide_error() noexcept {
    stop("ORIGINAL_1BDAC_SOURCE_DIVIDE_ERROR");
}
u32 original_full_actor_rand() noexcept {
    return code2::function_30ca2(current->machine);
}
void original_full_1bdac_source() noexcept {
    if(!original_full_frame_trace ||
       original_full_frame_trace->polygon_scan_returns!=1u ||
       original_full_frame_trace->polygon_quads==0u ||
       original_full_frame_trace->polygon_scanlines==0u)
        stop("ORIGINAL_1BDAC_POLYGON_PREDECESSOR_MISSING");
    code2::Application36Boundary motion{};
    motion.call_30d30=&original_full_actor_missing_clear;
    motion.call_30ca2=&original_full_actor_rand;
    motion.call_2d5d0=&original_full_actor_missing_collision;
    motion.call_2df2c_local=&original_full_actor_missing_offsets;
    motion.call_28110=&original_full_actor_missing_movement;
    motion.call_311e0=&original_full_actor_missing_query;
    motion.call_290d0=&original_full_actor_missing_update;
    motion.divide_error=&original_full_actor_divide_error;
    code2::Application38Boundary actor{};
    actor.app36=&motion;
    actor.call_0d550=&original_full_actor_0d550;
    actor.call_2388c=&original_full_actor_missing_predicate;
    actor.call_30cd4=&original_full_actor_30cd4;
    actor.call_22e90=&original_full_actor_missing2;
    actor.call_08c88=&original_full_actor_missing1;
    actor.call_09070=&original_full_actor_missing2;
    actor.call_08cf4=&original_full_actor_missing2;
    code2::function_1bdac(current->machine,actor);
    ++original_full_frame_trace->actor_update_returns;
}
// Original 2C294 sprite-scene sorting, entity projections and wall pass.
// All projected vertices, source resource lookups and scene records remain
// owned by the retained CODE.2 Machine. A visible 36CEC sprite still
// fails closed; no synthetic sprite is emitted.
void original_full_scene_clear(u32 dst,u32 value,u32 count) noexcept {
    if(!original_full_frame_trace ||
       std::uint64_t(dst)+count>current->data.size() ||
       count!=0x1130u)
        stop("ORIGINAL_2C294_RECORD_CLEAR_CHANGED");
    code2::Application72Boundary memory{};
    memory.call_379d0=&original_full_actor_fill_guest;
    (void)code2::function_30d30(memory,dst,value,count);
}
u32 original_full_scene_random() noexcept {
    // After 1A510, original 1F120 reaches 30CA2 only after
    // its 2B1BC actor-count query and any intervening sound updates.
    auto* t=original_full_frame_trace;
    if(t && t->retained_page_returns==1u && t->frame_audio_queries==1u)
        ++t->post_audio_random_callbacks;
    return code2::function_30ca2(current->machine);
}
void original_full_scene_seed(u32 seed) noexcept {
    code2::function_30cc4(current->machine,seed);
}
u32 original_full_scene_tick() noexcept {
    return code2::function_30700(current->machine);
}
void original_full_scene_project(u32 x,u32 y,u32 screen,u32 scale) noexcept {
    if(!original_full_frame_trace ||
       std::uint64_t(screen)+4u>current->data.size() ||
       std::uint64_t(scale)+2u>current->data.size())
        stop("ORIGINAL_2C294_35798_ARGUMENTS_CHANGED");
    code2::function_35798(current->machine,x,y,screen,scale);
    ++original_full_frame_trace->scene_projection_callbacks;
}
void original_full_scene_extent(u32 record,u32 threshold) noexcept {
    if(!original_full_frame_trace ||
       std::uint64_t(record)+0x16u>current->data.size())
        stop("ORIGINAL_2C294_21BF0_ARGUMENTS_CHANGED");
    code2::Application48Boundary shape{};
    shape.call_2f29c=&original_full_frame_resource;
    (void)code2::function_21bf0(
        current->machine,shape,record,static_cast<code2::s32>(threshold));
    ++original_full_frame_trace->scene_extent_callbacks;
}
void original_full_scene_walls() noexcept {
    if(!original_full_frame_trace ||
       original_full_frame_trace->actor_update_returns!=1u)
        stop("ORIGINAL_2C294_WALL_PREDECESSOR_MISSING");
    original_world_sweep_335dc();
    ++original_full_frame_trace->scene_wall_passes;
}
void original_full_scene_save_clip(u32*) noexcept {
    stop("ORIGINAL_2C294_29738_CLIP_SAVE_PENDING");
}
void original_full_scene_clip(u32,u32,u32,u32) noexcept {
    stop("ORIGINAL_2C294_296C0_CLIP_PENDING");
}
void original_full_scene_overlay(u32,u32,u32,u32,u32) noexcept {
    stop("ORIGINAL_2C294_2978C_OVERLAY_PENDING");
}
void original_full_scene_sprite(
        u32 screen_x,u32 res3_id,u32 projected_size,u32 lookup_bias,
        u32 vertical_mode,u32 ray_depth) noexcept {
    if(!original_full_frame_trace || !current->original_vga ||
       current->original_03838_completed!=1u ||
       current->original_181b4_completed!=1u)
        stop("ORIGINAL_2C294_36CEC_RETAINED_GUEST_UNAVAILABLE");
    auto& trace=*original_full_frame_trace;
    ++trace.scene_sprite_calls;
    // Original 36D07 skips tiny projected sprites before loading RES3.
    if((projected_size>>3u)==0u) {
        ++trace.scene_sprite_raster_returns;
        return;
    }
    const u32 image=original_full_frame_resource(res3_id,3u);
    if(!image || std::uint64_t(image)+2u>current->data.size())
        stop("ORIGINAL_36CEC_RES3_RESOURCE_UNAVAILABLE");
    const u32 length=code2::load_le16(current->data.data()+image);
    if(length<14u ||
       std::uint64_t(image)+2u+length>current->data.size())
        stop("ORIGINAL_36CEC_RES3_DECODED_BODY_INVALID");
    const u32 code_end=original_world_sintab_end();
    if(code_end<=0x100204u || code_end>current->data.size())
        stop("ORIGINAL_36CEC_SINTAB_ARENA_INVALID");
    try {
        const rloft_recomp::Original36cecCall call{
            screen_x,res3_id,projected_size,lookup_bias,
            vertical_mode,ray_depth
        };
        const auto result=rloft_recomp::raster_original_36cec_scene(
            current->data,*current->original_vga,call,
            image+2u,length,0x100000u,code_end);
        ++trace.scene_sprite_raster_returns;
        trace.scene_sprite_visible_runs+=result.visible_runs;
        trace.scene_sprite_plane_passes+=result.plane_passes;
        trace.scene_sprite_indexed_writes+=result.indexed_writes;
        trace.scene_sprite_off_aperture_writes+=
            result.off_aperture_cpu_writes;
    } catch(const std::exception& error) {
        std::cerr<<"CODE2_ORIGINAL_LIVE_BOOT=36CEC_SPRITE_RASTER_FAILURE"
                 <<" source=2C294_36CEC_36B14_369D8"
                 <<" res3_id="<<res3_id
                 <<" screen_x="<<screen_x
                 <<" projected_size="<<projected_size
                 <<" lookup_bias="<<lookup_bias
                 <<" vertical_mode="<<vertical_mode
                 <<" ray_depth="<<ray_depth
                 <<" source_image="<<image
                 <<" decoded_length="<<length
                 <<" sintab_end="<<code_end
                 <<" reason="<<error.what()<<"\n"<<std::flush;
        stop("ORIGINAL_36CEC_RETAINED_SPRITE_RASTER_FAILED");
    }
}
void original_full_2c294_source() noexcept {
    if(!original_full_frame_trace ||
       original_full_frame_trace->actor_update_returns!=1u)
        stop("ORIGINAL_2C294_ACTOR_PREDECESSOR_MISSING");
    code2::Application64Boundary projection{};
    projection.call_30ca2=&original_full_scene_random;
    projection.call_30cc4=&original_full_scene_seed;
    projection.call_30700=&original_full_scene_tick;
    projection.call_35798=&original_full_scene_project;
    projection.call_21bf0=&original_full_scene_extent;
    code2::Application65Boundary scene{};
    scene.app64=&projection;
    scene.call_30d30=&original_full_scene_clear;
    scene.call_335dc=&original_full_scene_walls;
    scene.call_29738_stack4=&original_full_scene_save_clip;
    scene.call_296c0=&original_full_scene_clip;
    scene.call_36cec=&original_full_scene_sprite;
    scene.call_2978c=&original_full_scene_overlay;
    code2::function_2c294(current->machine,scene);
    ++original_full_frame_trace->scene_projection_returns;
}
// 274A4 -> 29804 -> 29414 -> 2A380/2A6B0 and 273F0.
// All source VGA ports operate through one Bridge::Scope. The source
// raster callbacks use the same guest machine and resident RES0 image.
u32 original_full_compass_clip(u32 top,u32 left,u32 bottom,u32 right) noexcept {
    if(!original_full_frame_trace)
        stop("ORIGINAL_274A4_CLIP_OUTSIDE_SOURCE");
    code2::function_296c0(current->machine,
        static_cast<code2::s32>(top),static_cast<code2::s32>(left),
        static_cast<code2::s32>(bottom),static_cast<code2::s32>(right));
    ++original_full_frame_trace->compass_clips;
    return 0u;
}
u32 original_full_compass_unclipped(
        u32 frame,u32 top,u32 left,u32 image) noexcept {
    if(!image || std::uint64_t(image)+8u>current->data.size())
        stop("ORIGINAL_274A4_IMAGE_UNAVAILABLE");
    auto gfx=rloft::source_video_boot::Bridge::cursor_gfx();
    code2::function_2a380(current->machine,gfx,frame,top,left,image);
    ++original_full_frame_trace->compass_images;
    return 1u;
}
u32 original_full_compass_clipped(
        u32 frame,u32 top,u32 left,u32 clip,u32 image) noexcept {
    if(!image || std::uint64_t(image)+8u>current->data.size() ||
       std::uint64_t(clip)+0x18u>current->data.size())
        stop("ORIGINAL_274A4_CLIPPED_IMAGE_UNAVAILABLE");
    auto gfx=rloft::source_video_boot::Bridge::app61();
    code2::function_2a6b0(current->machine,gfx,frame,top,left,clip,image);
    ++original_full_frame_trace->compass_images;
    return 1u;
}
u32 original_full_compass_29804(
        u32 frame,u32 top,u32 left,u32 image) noexcept {
    if(code2::load_le32(current->data.data()+0xF0C8u)!=0u)
        stop("ORIGINAL_274A4_IMAGE_DISPATCH_INTERRUPT_PENDING");
    code2::Application57Boundary draw{};
    draw.call_2a380=&original_full_compass_unclipped;
    draw.call_2a6b0=&original_full_compass_clipped;
    return code2::function_29804(
        current->machine,draw,frame,top,left,image);
}
void original_full_compass_pixel(u32 at,u8 value) noexcept {
    rloft::source_video_boot::Bridge::vga_write(at,value);
    ++original_full_frame_trace->compass_vga_writes;
}
void original_full_274a4_source() noexcept {
    if(!original_full_frame_trace ||
       original_full_frame_trace->scene_projection_returns!=1u ||
       !current->original_vga)
        stop("ORIGINAL_274A4_SCENE_PREDECESSOR_MISSING");
    ++original_full_frame_trace->compass_source_calls;
    try {
        rloft::source_video_boot::Bridge bridge(
            current->machine,*current->original_vga,current->data.size());
        {
            rloft::source_video_boot::Bridge::Scope scope(bridge);
            code2::Application52Boundary art{};
            art.call_296c0=&original_full_compass_clip;
            art.call_2f29c=&original_full_frame_resource;
            art.call_29804=&original_full_compass_29804;
            code2::Application54Boundary vga{};
            vga.out16=&rloft::source_video_boot::Bridge::out16;
            vga.phys_read8=&rloft::source_video_boot::Bridge::vga_read;
            vga.phys_write8=&original_full_compass_pixel;
            code2::function_274a4(current->machine,art,vga);
        }
        if(bridge.invalid_access || current->original_vga->unsupported_read())
            stop("ORIGINAL_274A4_SOURCE_VGA_IO_INVALID");
    } catch(const std::exception& e) {
        std::cerr<<"CODE2_ORIGINAL_LIVE_BOOT=274A4_COMPASS_FAILURE"
                 <<" heading="<<code2::load_le32(current->data.data()+0xC658u)
                 <<" page="<<unsigned(current->data[0xEF4Du])
                 <<" image_calls="<<original_full_frame_trace->compass_images
                 <<" vga_writes="<<original_full_frame_trace->compass_vga_writes
                 <<" reason="<<e.what()<<"\n"<<std::flush;
        stop("ORIGINAL_274A4_SOURCE_COMPASS_FAILED");
    }
    ++original_full_frame_trace->compass_source_returns;
}
u32 original_full_1f120_actor_count() noexcept {
    if(!original_full_frame_trace ||
       original_full_frame_trace->compass_source_returns!=1u)
        stop("ORIGINAL_1F120_COMPASS_PREDECESSOR_MISSING");
    ++original_full_frame_trace->frame_audio_queries;
    if(original_full_frame_trace->retained_page_returns==1u)
        ++original_full_frame_trace->post_hud_return_witness;
    return code2::function_2b1bc(current->machine);
}
u32 original_full_1f120_audio_mode() noexcept {
    return code2::function_29048(current->machine);
}
void original_full_1f120_audio_guard() noexcept {
    if(code2::load_le32(current->data.data()+0xEE78u)!=0u)
        stop("ORIGINAL_1F120_ACTIVE_AUDIO_DRIVER_UNCONNECTED");
}
void original_full_1f120_audio_start(u32 slot,u32 arg) noexcept {
    original_full_1f120_audio_guard();
    code2::function_28ae0(current->machine,current->audio_seed,slot,arg);
}
void original_full_1f120_audio_select(u32 slot) noexcept {
    original_full_1f120_audio_guard();
    code2::function_288fc(current->machine,current->audio_seed,slot);
}
void original_full_1f120_audio_volume(u32 volume) noexcept {
    original_full_1f120_audio_guard();
    code2::function_28f58(current->machine,current->audio_seed,volume);
}
void original_full_1f120_audio_tempo(u32 value) noexcept {
    original_full_1f120_audio_guard();
    code2::function_29008(current->machine,current->audio_seed,value);
}
void original_full_1f120_sound_event(
        u32 request_id,u32 aux,u32 channel,u32 priority) noexcept {
    // Original 28110 accepts the four cdecl arguments in this exact order
    // and updates the two twelve-byte source priority slots at EE48.
    // This is the game's sound REQUEST queue, not an AIL device playback.
    if(!current || channel>1u || !original_full_frame_trace)
        stop("ORIGINAL_1F120_28110_SOUND_QUEUE_CHANNEL_UNVERIFIED");
    const u32 offset=0xEE48u+channel*12u;
    if(std::uint64_t(offset)+12u>current->data.size())
        stop("ORIGINAL_1F120_28110_SOUND_QUEUE_OUT_OF_GUEST");
    const u32 enabled=code2::load_le32(current->data.data()+0xEE40u);
    const u32 prior_handle=code2::load_le32(current->data.data()+offset);
    code2::function_28110(
        current->machine,request_id,aux,channel,priority);
    const u32 after_handle=code2::load_le32(current->data.data()+offset);
    if(enabled==0u && after_handle!=prior_handle)
        stop("ORIGINAL_1F120_28110_DISABLED_QUEUE_CHANGED");
    std::cerr<<"CODE2_ORIGINAL_LIVE_GAMEPLAY=1F120_28110_SOUND_REQUEST"
             <<" channel="<<channel
             <<" handle="<<request_id
             <<" aux="<<aux
             <<" priority="<<priority
             <<" audio_queue_enabled="<<(enabled?1:0)
             <<" before_handle="<<prior_handle
             <<" after_handle="<<after_handle
             <<" original_source=28110"
             <<" hardware_playback=NOT_CERTIFIED\n"<<std::flush;
}

// Both the single-frame diagnostic and the retained 1FC14 tick must
// dispatch the same canonical CODE.2 Application43/44 source graph.
bool original_full_configure_scene(
        code2::Application43Boundary& ui,
        code2::Application44Boundary& scene) noexcept {
    ui={};
    scene={};
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
    const char* const polygon_source=
        std::getenv("RLOFT_RECOMP_EXPERIMENTAL_POLYGON_SOURCE");
    const bool source_polygons=through_actor && polygon_source &&
        std::strcmp(polygon_source,"1")==0;
    scene.call_34de8=source_polygons?
        &original_full_exterior_polygon_source:&original_exterior_polygon_frontier;
    scene.call_1bdac=source_polygons?&original_full_1bdac_source:nullptr;
    scene.call_2c294=source_polygons?&original_full_2c294_source:nullptr;
    scene.call_274a4=source_polygons?&original_full_274a4_source:nullptr;
    scene.call_2b1bc=source_polygons?&original_full_1f120_actor_count:nullptr;
    scene.call_29048=source_polygons?&original_full_1f120_audio_mode:nullptr;
    scene.call_28ae0=source_polygons?&original_full_1f120_audio_start:nullptr;
    scene.call_288fc=source_polygons?&original_full_1f120_audio_select:nullptr;
    scene.call_28f58=source_polygons?&original_full_1f120_audio_volume:nullptr;
    scene.call_29008=source_polygons?&original_full_1f120_audio_tempo:nullptr;
    scene.call_30ca2=source_polygons?&original_full_scene_random:nullptr;
    scene.call_28110=source_polygons?&original_full_1f120_sound_event:nullptr;
    scene.call_2f868=&original_scene_poll_source_events;
    scene.divide_error=&original_live_hud_divide_error;

    return source_polygons;
}

// These counters are guest-worker-thread state; unlike the one-shot
// diagnostic they survive across consecutive keyboard-driven source
// iterations and reset at the exact 30B40 keyboard boundary.
thread_local OriginalFullFrameTrace original_1fc14_tick_frame{};
thread_local ExteriorSourceTrace original_1fc14_tick_exterior{};
void original_full_prepare_1fc14_source_tick() noexcept {
    original_1fc14_tick_frame={};
    original_1fc14_tick_exterior={};
    original_full_frame_trace=&original_1fc14_tick_frame;
    exterior_trace=&original_1fc14_tick_exterior;
}
void original_full_mark_1fc14_page_return() noexcept {
    auto* t=original_full_frame_trace;
    if(!t || t!=&original_1fc14_tick_frame || t->retained_page_returns!=0u)
        stop("ORIGINAL_1FC14_POST_HANDOFF_SOURCE_TRACE_MISSING");
    ++t->retained_page_returns;
}
void original_full_audit_1fc14_previous_tick() noexcept {
    if(!current || current->original_1fc14_source_scene_returns==0u ||
       current->original_1fc14_source_scene_returns==
           current->original_1fc14_post_scene_cycles)
        return;
    auto* t=original_full_frame_trace;
    const bool trace_present=t==&original_1fc14_tick_frame;
    const u32 pages=trace_present?t->retained_page_returns:0u;
    const u32 hud=trace_present?t->post_hud_return_witness:0u;
    const u32 audio=trace_present?t->frame_audio_queries:0u;
    const u32 rng=trace_present?t->post_audio_random_callbacks:0u;
    const bool complete=trace_present && pages==1u && hud==1u &&
        audio==1u && rng==1u &&
        current->original_1fc14_source_scene_returns==
            current->original_1fc14_source_page_handoffs &&
        current->original_1fc14_source_scene_returns==
            current->original_1fc14_post_scene_cycles+1u;
    std::cerr<<"CODE2_ORIGINAL_LIVE_GAMEPLAY=1FC14_POST_SCENE_CYCLE_PROBE"
             <<" scenes="<<current->original_1fc14_source_scene_returns
             <<" pages="<<current->original_1fc14_source_page_handoffs
             <<" observed_page_return="<<pages
             <<" hud_return_before_audio="<<hud
             <<" audio_actor_queries="<<audio
             <<" audio_random_callbacks="<<rng
             <<" keyboard_loop_reentry=SOURCE"
             <<" acceptance="<<(complete?"BOUNDED_POST_SCENE":"DIAGNOSTIC_ONLY")
             <<" full_1F8FC_frames=NOT_CLAIMED\n"<<std::flush;
    if(complete)++current->original_1fc14_post_scene_cycles;
}
void original_full_validate_1fc14_scene_handoff(u32 page) noexcept {
    auto* t=original_full_frame_trace;
    auto* e=exterior_trace;
    if(!t || !e || page>1u ||
       t->polygon_scan_returns!=1u ||
       t->polygon_drawn_scanlines==0u ||
       t->actor_update_returns!=1u ||
       t->scene_projection_returns!=1u ||
       t->compass_source_returns!=1u ||
       t->scene_sprite_calls==0u ||
       t->scene_sprite_calls!=t->scene_sprite_raster_returns)
        stop("ORIGINAL_1FC14_SCENE_SOURCE_RASTER_INCOMPLETE");
    ++current->original_1fc14_source_scene_returns;
    std::cerr<<"CODE2_ORIGINAL_LIVE_GAMEPLAY=1FC14_SCENE_SOURCE_RETURN"
             <<" page="<<page
             <<" scene_returns="<<current->original_1fc14_source_scene_returns
             <<" source_keyboard_code="<<current->original_1fc14_last_scan_code
             <<" heading="<<code2::load_le32(current->data.data()+0xC658u)
             <<" camera_word_x="<<code2::load_le16(current->data.data()+0x255D0u)
             <<" camera_word_y="<<code2::load_le16(current->data.data()+0x255D4u)
             <<" polygon_quads="<<t->polygon_quads
             <<" polygon_drawn_scanlines="<<t->polygon_drawn_scanlines
             <<" original_actor_returns="<<t->actor_update_returns
             <<" res3_sprites="<<t->scene_sprite_calls
             <<" sprite_indexed_writes="<<t->scene_sprite_indexed_writes
             <<" original_compass_returns="<<t->compass_source_returns
             <<" 1A510_page_handoff=SOURCE"
             <<" HUD_and_sound_follow=NOT_YET_VERIFIED"
             <<" original_guest=PRESERVED\n"<<std::flush;
}

void original_full_attach_1fc14_scene(Session& guest) noexcept {
    if(!original_full_configure_scene(guest.live_ui,guest.live_scene))
        stop("ORIGINAL_1FC14_SCENE_SOURCE_GRAPH_MISSING");
    guest.presentation.app43=&guest.live_ui;
    guest.presentation.app44=&guest.live_scene;
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
    code2::Application44Boundary scene{};
    const bool source_polygons=original_full_configure_scene(ui,scene);
    const char* const polygon_advance=
        std::getenv("RLOFT_RECOMP_EXPERIMENTAL_POLYGON_FRONTIER");
    const bool through_actor=polygon_advance &&
        std::strcmp(polygon_advance,"1")==0;

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
        if(source_polygons) {
            ++frame.compositor_returns;
            stop("ORIGINAL_1F8FC_SOURCE_FRAME_RETURNED");
        }
        stop("ORIGINAL_1F8FC_CHAIN_RETURNED_WITHOUT_ORIGINAL_WORLD_RENDER");
    }
    current->active_escape=prior;
    original_full_frame_trace=nullptr;
    exterior_trace=nullptr;
    const char* const expected_stop=source_polygons?
        "ORIGINAL_1F8FC_SOURCE_FRAME_RETURNED":
        (through_actor?
            "ORIGINAL_1F1C8_34DE8_POLYGON_RASTER_PENDING":
            "ORIGINAL_1F1C8_314B0_FIRST_RES8_50_VGA_SPRITE_DRAWN");
    if(!current->stop_reason ||
       std::strcmp(current->stop_reason,expected_stop)!=0 ||
       frame.backgrounds!=2u || frame.releases!=2u ||
       frame.source_ids[0]!=expected || frame.source_ids[1]!=expected+1u ||
       exterior.active_actor_runs!=1u || exterior.subtractive_draws==0u ||
       (source_polygons &&
           (frame.polygon_cells==0u || frame.polygon_quads==0u ||
            frame.polygon_scan_returns!=1u || frame.polygon_scanlines==0u ||
            frame.polygon_edges==0u || frame.polygon_steps==0u ||
            frame.actor_update_returns!=1u ||
            frame.scene_wall_passes!=1u ||
            frame.scene_sprite_calls==0u ||
            frame.scene_sprite_raster_returns!=frame.scene_sprite_calls ||
            frame.scene_projection_returns!=1u ||
            frame.compass_source_calls!=1u ||
            frame.compass_source_returns!=1u ||
             frame.frame_audio_queries!=1u ||
             frame.compositor_returns!=1u)) ||
       exterior.resource_requests<exterior.subtractive_draws ||
       (exterior.angle_patches!=0u && exterior.angle_patches!=4u) ||
       current->data[0xC050u]!=(source_polygons?(initial_page^1u):initial_page) ||
       frame.presentations!=(source_polygons?1u:0u))
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
             <<" polygon_source_cells="<<frame.polygon_cells
             <<" polygon_source_projections="<<frame.polygon_projections
             <<" polygon_source_quads="<<frame.polygon_quads
             <<" polygon_scan_returns="<<frame.polygon_scan_returns
             <<" polygon_vertex_copies="<<frame.polygon_vertex_copies
             <<" polygon_edges="<<frame.polygon_edges
             <<" polygon_steps="<<frame.polygon_steps
             <<" polygon_scanlines="<<frame.polygon_scanlines
             <<" polygon_drawn_scanlines="<<frame.polygon_drawn_scanlines
             <<" actor_update_returns="<<frame.actor_update_returns
             <<" scene_projection_callbacks="<<frame.scene_projection_callbacks
             <<" scene_extent_callbacks="<<frame.scene_extent_callbacks
             <<" scene_wall_passes="<<frame.scene_wall_passes
             <<" scene_sprite_calls="<<frame.scene_sprite_calls
             <<" scene_sprite_raster_returns="<<frame.scene_sprite_raster_returns
             <<" scene_sprite_visible_runs="<<frame.scene_sprite_visible_runs
             <<" scene_sprite_plane_passes="<<frame.scene_sprite_plane_passes
             <<" scene_sprite_indexed_writes="<<frame.scene_sprite_indexed_writes
             <<" scene_sprite_off_aperture_writes="<<frame.scene_sprite_off_aperture_writes
             <<" scene_projection_returns="<<frame.scene_projection_returns
             <<" compass_source_calls="<<frame.compass_source_calls
             <<" compass_source_returns="<<frame.compass_source_returns
             <<" compass_clips="<<frame.compass_clips
             <<" compass_images="<<frame.compass_images
             <<" compass_vga_writes="<<frame.compass_vga_writes
             <<" frame_audio_queries="<<frame.frame_audio_queries
             <<" compositor_returns="<<frame.compositor_returns
             <<" angle_patches="<<exterior.angle_patches
             <<" presentations="<<frame.presentations
             <<" guest=PRESERVED"
             <<" next="<<(source_polygons?
                 "SOURCE_FRAME_PRESENTED":
                 (through_actor?"34DE8_POLYGON_RASTER":"EXTERIOR_REMAINING_SOURCE"))
             <<" complete_frames="<<(source_polygons?1u:0u)
             <<" DOS_VGA_PARITY=NOT_VERIFIED\n"<<std::flush;
    // Fingerprint the actual selected 320x400 original planar VGA scanout
    // and all 256 hardware DAC RGB6 triples. Never publish retail art or
    // mistake these hashes for an independent DOS parity measurement.
    if(source_polygons && current->original_vga) {
        const auto indices=current->original_vga->scanout_indices();
        if(indices.size()!=320u*400u)
            stop("ORIGINAL_1F8FC_SCANOUT_DIMENSIONS_CHANGED");
        std::vector<u8> rgb6;
        rgb6.reserve(768u);
        for(u32 i=0u;i<256u;++i) {
            const auto c=current->original_vga->palette(static_cast<u8>(i));
            if(c.r>63u || c.g>63u || c.b>63u)
                stop("ORIGINAL_1F8FC_DAC_RGB6_RANGE_CHANGED");
            rgb6.push_back(c.r); rgb6.push_back(c.g); rgb6.push_back(c.b);
        }
        std::cerr<<"CODE2_ORIGINAL_LIVE_BOOT=1F8FC_SOURCE_FRAME_FINGERPRINT"
                 <<" indices_sha256="<<rloft_recomp::sha256_hex(indices)
                 <<" dac_rgb6_sha256="<<rloft_recomp::sha256_hex(rgb6)
                 <<" source_vga_pixels="<<indices.size()
                 <<" dac_bytes="<<rgb6.size()
                 <<" crtc_start="<<current->original_vga->crtc_start()
                 <<" dos_reference_synchronized=NO\n"<<std::flush;
        // Explicit local-only comparison output: no artwork enters logs,
        // commits or CI artifacts. Default source execution writes none.
        if(const char* stem=std::getenv("RLOFT_RECOMP_PRIVATE_FRAME_STEM");
           stem && *stem) {
            const auto save_private=[&](const std::string& ext,
                                         const std::vector<u8>& bytes) {
                const std::filesystem::path target=
                    std::string(stem)+ext;
                bool written=false;
                {
                    std::ofstream output(target,
                        std::ios::binary|std::ios::trunc);
                    if(output) {
                        output.write(
                            reinterpret_cast<const char*>(bytes.data()),
                            static_cast<std::streamsize>(bytes.size()));
                        output.flush();
                        written=static_cast<bool>(output);
                    }
                }
                // Close the file before any source stop/longjmp.
                if(!written)
                    stop("ORIGINAL_1F8FC_PRIVATE_VGA_CAPTURE_WRITE_FAILED");
            };
            save_private(".idx",indices);
            save_private(".pal",rgb6);
            std::cerr<<"CODE2_ORIGINAL_LIVE_BOOT=1F8FC_PRIVATE_CAPTURE"
                     <<" frame_bytes="<<indices.size()
                     <<" palette_bytes="<<rgb6.size()
                     <<" original_dos_reference=NOT_SUPPLIED"
                     <<" media_payload=LOCAL_ONLY\n"<<std::flush;
        }
    }
    current->stop_reason=nullptr;
}
