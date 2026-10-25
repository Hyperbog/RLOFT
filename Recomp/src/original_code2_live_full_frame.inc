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
// Original CODE.2 1FC14 -> 2E120 -> 2DF2C -> 2DBAC movement
// edge. Opt-in only, because retail collision event/teleport callbacks
// and original time progression remain incomplete. No host writes to
// guest camera/heading occur here: only the compiled original routines
// can change those values.
u32 original_full_movement_record(code2::s32 x,code2::s32 y) noexcept {
    const code2::s32 width=static_cast<code2::s16>(
        code2::load_le16(current->data.data()+0x25619u));
    const code2::s32 height=static_cast<code2::s16>(
        code2::load_le16(current->data.data()+0x2561Bu));
    if(x<0 || y<0 || x>=width || y>=height)
        stop("ORIGINAL_2D5D0_MOVEMENT_RECORD_OUT_OF_MAP");
    return code2::function_1e8c8(
        current->machine,static_cast<u32>(x),static_cast<u32>(y));
}
u32 original_full_movement_marker(u32 tile) noexcept {
    const code2::s32 width=static_cast<code2::s16>(
        code2::load_le16(current->data.data()+0x25619u));
    const code2::s32 height=static_cast<code2::s16>(
        code2::load_le16(current->data.data()+0x2561Bu));
    if(width<=0 || height<=0 ||
       std::uint64_t(tile)>=std::uint64_t(width)*height)
        stop("ORIGINAL_2D5D0_MOVEMENT_MARKER_OUT_OF_MAP");
    code2::Application43Boundary originals{};
    return code2::function_1e930(current->machine,originals,tile);
}
void original_full_movement_event(u32 level) noexcept {
    if(!current || !current->original_vga ||
       current->original_181b4_completed!=1u ||
       !original_full_frame_trace)
        stop("ORIGINAL_2DBAC_EVENT_OUTSIDE_RETAINED_LEVEL22");
    // Same-level portal calls are legal: original 181B4 checks old==target
    // and returns before file I/O, world release or guest mutation.
    const u32 previous=code2::load_le32(current->data.data()+0xC6C8u);
    if(level==previous && level==22u) {
        code2::Application32Boundary original{};
        code2::function_181b4(current->machine,original,
                              static_cast<code2::s32>(level));
        std::cerr<<"CODE2_ORIGINAL_GAMEPLAY_LEVEL=181B4_SAME_LEVEL_SOURCE_RETURN"
                 <<" target="<<level<<" media_read=NO state_reset=NO\n"
                 <<std::flush;
        return;
    }
    // Cross-level source 171FC must first save mutable old-world records;
    // original 181B4 then releases map/script allocs, reads target data,
    // runs 17A68 NPC transforms, and rebinds graphics/music. Startup's
    // one-shot Level22 loader cannot yet guarantee those live callbacks.
    // Fail closed; do not invoke it on another level or fake a world load.
    if(level!=22u)
        stop("ORIGINAL_2DBAC_CROSS_LEVEL_TRANSITION_NEEDS_171FC_181B4_SERVICES");
    original_181b4_level_load(level);
}
// Original 20C0C is the precise animated-tile interaction path;
// callbacks route into the original 2C800/2C8E4/2C9DC entity probes and
// the original 28110 sound-queue setter. No host opens doors by hand.
u32 original_full_movement_entity_probe(u32 y,u32 x,code2::s32 exclude) noexcept {
    return code2::function_2c800(current->machine,y,x,
                                 static_cast<u32>(exclude));
}
u32 original_full_movement_special_actor_probe(
        u32 y,u32 x,code2::s32 exclude) noexcept {
    code2::Application65Boundary actors{};
    actors.call_2acdc=&code2::function_2acdc;
    return code2::function_2c8e4(
        current->machine,actors,y,x,static_cast<u32>(exclude));
}
u32 original_full_movement_party_probe(u32 y,u32 x) noexcept {
    return code2::function_2c9dc(current->machine,y,x);
}
void original_full_movement_special_audio(
        u32 request,u32 aux,u32 channel,u32 priority) noexcept {
    if(channel>1u)stop("ORIGINAL_20A24_SOUND_CHANNEL_UNSUPPORTED");
    code2::function_28110(current->machine,request,aux,channel,priority);
}
void original_full_movement_other_audio(u32,u32,u32) noexcept {
    stop("ORIGINAL_20A24_UNRECONSTRUCTED_28164_DIRECT_AUDIO");
}
void original_full_movement_effect(
        code2::s32 y,code2::s32 x,u32 record) noexcept {
    if(!record || std::uint64_t(record)+10u>current->data.size())
        stop("ORIGINAL_20C0C_TILE_RECORD_OUT_OF_GUEST");
    code2::Application46Boundary source{};
    source.call_2c800=&original_full_movement_entity_probe;
    source.call_2c8e4=&original_full_movement_special_actor_probe;
    source.call_2c9dc=&original_full_movement_party_probe;
    source.call_28110=&original_full_movement_special_audio;
    source.call_28164=&original_full_movement_other_audio;
    (void)code2::function_20c0c(current->machine,source,
            static_cast<u32>(y),static_cast<u32>(x),record);
}
// Connect the original fixed-size 002A8 descriptor dispatcher. A rejected
// script is refused BEFORE any source opcode has changed the guest.
u32 original_full_script_record(u32 row,u32 col) noexcept {
    const u32 record=code2::function_1e8c8(current->machine,row,col);
    if(record && std::uint64_t(record)+10u>current->data.size())
        stop("ORIGINAL_002A8_SOURCE_OBJECT_RECORD_OUT_OF_GUEST");
    return record;
}
void original_full_script_tile(u32 row,u32 col,u32 record) noexcept {
    original_full_movement_effect(static_cast<code2::s32>(row),
                                  static_cast<code2::s32>(col),record);
}
void original_full_script_clear_bit(u32 bit,u32 /*original live EBX*/) noexcept {
    code2::function_1c160(current->machine,bit);
}
void original_full_script_toggle_bit(u32 bit) noexcept {
    code2::function_1c1a0(current->machine,bit);
}
void original_full_script_set_bit(u32 bit) noexcept {
    code2::function_1c124(current->machine,bit);
}
// Only original 08A80 type-8 moving records are admitted by the
// whole-descriptor preflight. No host actor or projectile is synthesized.
void original_full_script_moving_owner(u32 slot) noexcept {
    if(slot!=4u)
        stop("ORIGINAL_08A80_MOVING_RECORD_OWNER_CHANGED");
    code2::function_0e578(current->machine,slot);
}
u32 original_full_script_moving_random() noexcept {
    return code2::function_30ca2(current->machine);
}
void original_full_script_moving_record(
        u32 type,code2::s32 angle,u32 owner,u32 packed_x,u32 packed_y) noexcept {
    if(type!=8u || owner!=4u)
        stop("ORIGINAL_08A80_MOVING_RECORD_PRECHECK_BYPASSED");
    code2::Application12Boundary b{};
    b.call_e578=&original_full_script_moving_owner;
    b.call_30ca2=&original_full_script_moving_random;
    b.call_28110=&original_full_movement_special_audio;
    code2::function_08a80(current->machine,b,type,angle,
                           static_cast<code2::s32>(owner),packed_x,packed_y);
}
void original_full_movement_script(code2::s32 index) noexcept {
    if(!current || !current->original_vga || !original_full_frame_trace)
        stop("ORIGINAL_002A8_SCRIPT_OUTSIDE_RETAINED_GAMEPLAY");
    const auto table=code2::load_le32(current->data.data()+0xCF44u);
    const auto checked=rloft::source_script::validate(
        current->data.data(),current->data.size(),table,index);
    if(!checked.accepted) {
        std::cerr<<"CODE2_ORIGINAL_GAMEPLAY_SCRIPT=PRE_EXECUTION_REJECTED"
                 <<" index="<<index<<" reason="<<checked.reason
                 <<" original_guest_mutated=NO\n"<<std::flush;
        stop("ORIGINAL_002A8_SCRIPT_REQUIRES_UNCONNECTED_SOURCE");
    }
    if(index==-1)return; // Original 002A8 sentinel.
    code2::Dispatch002A8Boundary dispatch{};
    dispatch.call_1e8c8=&original_full_script_record;
    dispatch.call_20c0c=&original_full_script_tile;
    dispatch.call_28110=&original_full_movement_special_audio;
    dispatch.call_1c124=&original_full_script_set_bit;
    dispatch.call_1c160=&original_full_script_clear_bit;
    dispatch.call_1c1a0=&original_full_script_toggle_bit;
    dispatch.call_08a80=&original_full_script_moving_record;
    code2::function_002a8(current->machine,dispatch,index);
    std::cerr<<"CODE2_ORIGINAL_GAMEPLAY_SCRIPT=002A8_SOURCE_RETURN"
             <<" index="<<index<<" commands="<<checked.commands
             <<" supported_event_scope=BOUNDED"
             <<" source_guest=RETAINED\n"<<std::flush;
}
u32 original_full_movement_predicate(u32) noexcept {
    stop("ORIGINAL_2D5D0_SPECIAL_TILE_PREDICATE_NOT_CONNECTED");
}
void original_full_movement_refresh() noexcept {
    // Original 1EA78 swaps the retained display page and invokes the
    // original 1A510 source presentation callback. A special-event
    // redraw is still bounded by the source page-handoff trace checks.
    if(!current || !current->original_vga || !original_full_frame_trace)
        stop("ORIGINAL_2DBAC_MOVEMENT_REFRESH_OUTSIDE_SOURCE_FRAME");
    code2::Application43Boundary ui{};
    ui.call_1a510=&original_1fc14_source_page_handoff;
    code2::function_1ea78(current->machine,ui);
}
void original_full_movement_heading(u32 heading) noexcept {
    // Original 1C0C0 quantizes heading only when the grid-mode byte
    // requests it; no host-side pose assignment or guessed orientation.
    code2::function_1c0c0(current->machine,heading);
}
u32 original_full_movement_bit_query(u32 bit) noexcept {
    return code2::function_1c1dc(current->machine,bit);
}
void original_full_movement_bit_set(u32 bit) noexcept {
    code2::function_1c124(current->machine,bit);
}
void original_full_movement_bit_clear(u32 bit) noexcept {
    code2::function_1c160(current->machine,bit);
}
void original_full_2e120_movement_service() noexcept {
    if(!current || !current->original_vga ||
       !current->game_audio.app68 ||
       !original_full_frame_trace)
        stop("ORIGINAL_2E120_MOVEMENT_NOT_AT_RETAINED_GUEST");
    const auto* const guest=current->data.data();
    const u32 before_x=code2::load_le32(guest+0x255D0u);
    const u32 before_y=code2::load_le32(guest+0x255D4u);
    const u32 before_heading=code2::load_le32(guest+0xC658u);
    const u32 before_dx=code2::load_le32(guest+0xC6E8u);
    const u32 before_dy=code2::load_le32(guest+0xC6ECu);
    const u32 before_turn=code2::load_le32(guest+0xC6F0u);

    code2::Application66Boundary collision{};
    collision.call_1e8c8=&original_full_movement_record;
    collision.call_1e930=&original_full_movement_marker;
    collision.call_20c0c=&original_full_movement_effect;
    collision.call_002a8=&original_full_movement_script;
    collision.call_181b4=&original_full_movement_event;
    collision.call_e0f8=&original_full_movement_predicate;
    collision.call_1c0c0=&original_full_movement_heading;
    collision.call_1ea78=&original_full_movement_refresh;
    // Attach the original 2CC88 secondary record-object collision.
    // This is not a "no objects" fallback: source 2CC88 sees the
    // authentic 258A6 object count and 22238/2223C world coordinates.
    // Rare transitions keep their original event callback, which stops
    // unless the preserved level's source loader is supported.
    code2::Application65Boundary secondary{};
    secondary.call_181b4=&original_full_movement_event;
    secondary.call_1c0c0=&original_full_movement_heading;
    secondary.call_1c1dc=&original_full_movement_bit_query;
    secondary.call_1c124=&original_full_movement_bit_set;
    secondary.call_1c160=&original_full_movement_bit_clear;
    secondary.call_2acdc=&code2::function_2acdc;
    secondary.call_1ea78=&original_full_movement_refresh;
    collision.app65=&secondary;
    code2::Application67Boundary movement{};
    movement.app66=&collision;
    movement.call_35680=&code2::function_35680;
    movement.call_369a7=&code2::function_369a7;
    code2::function_2e120(current->machine,movement);
    if(current->data.data()!=guest)
        stop("ORIGINAL_2E120_MOVEMENT_GUEST_RELOCATED");
    const u32 after_x=code2::load_le32(guest+0x255D0u);
    const u32 after_y=code2::load_le32(guest+0x255D4u);
    const u32 after_heading=code2::load_le32(guest+0xC658u);
    if(after_heading>=360u)
        stop("ORIGINAL_2E120_MOVEMENT_HEADING_OUT_OF_RANGE");
    std::cerr<<"CODE2_ORIGINAL_LIVE_GAMEPLAY=2E120_MOVEMENT_SOURCE_RETURN"
             <<" pre_x="<<before_x<<" post_x="<<after_x
             <<" pre_y="<<before_y<<" post_y="<<after_y
             <<" pre_heading="<<before_heading
             <<" post_heading="<<after_heading
             <<" input_dx="<<before_dx
             <<" input_dy="<<before_dy
             <<" input_turn="<<before_turn
             <<" camera_displaced="<<((after_x!=before_x || after_y!=before_y)?1:0)
             <<" heading_changed="<<(after_heading!=before_heading?1:0)
             <<" source_2e120=EXECUTED"
             <<" source_2dbac=PRIMARY_AND_SECONDARY_COLLISION"
             <<" secondary_actor_collision=ORIGINAL_2CC88"
             <<" cross_level_events=GUARDED"
             <<" host_pose_writes=NONE\n"<<std::flush;
}
// Stable, host-private adapter across consecutive original 1FC14 cycles.
// No original bytes leave the active process, logs or published artifacts.
thread_local rloft::source_audio::WinMMSourceSfx original_live_sfx_device{};
thread_local u32 original_live_sfx_packet=0u;
thread_local u32 original_live_sfx_length=0u;
thread_local u32 original_live_sfx_bank=0u;
thread_local u32 original_live_sfx_id=0u;
thread_local bool original_live_sfx_staged=false;
u32 original_live_sfx_resource(u32 id,u32 bank) noexcept {
    if(!original_live_sfx_staged || id!=original_live_sfx_id ||
       bank!=original_live_sfx_bank || !original_live_sfx_packet)
        stop("ORIGINAL_27F24_SFX_RESOURCE_ID_CHANGED");
    return original_live_sfx_packet;
}
u32 original_live_sfx_status(u32 device) noexcept {
    if(device!=code2::load_le32(current->data.data()+0xEE6Cu))
        stop("ORIGINAL_27F24_SFX_DEVICE_HANDLE_CHANGED");
    const bool playing=original_live_sfx_device.status_playing();
    return playing?2u:0u;  // Host WinMM emulation, NOT AIL status.
}
u32 original_live_sfx_stop(u32 device) noexcept {
    if(device!=code2::load_le32(current->data.data()+0xEE6Cu))
        stop("ORIGINAL_27F24_SFX_STOP_DEVICE_CHANGED");
    original_live_sfx_device.stop_active();
    return 0u;
}
u32 original_live_sfx_sample(u32 device,u32 dest,u32 ds,u32 flags) noexcept {
    if(!original_live_sfx_staged ||
       device!=code2::load_le32(current->data.data()+0xEE6Cu) ||
       dest!=code2::load_le32(current->data.data()+0xEE60u) ||
       ds!=static_cast<u32>(current->machine.cpu.ds) ||
       flags!=0xFFFFFFFFu ||
       std::uint64_t(dest)+original_live_sfx_length>current->data.size() ||
       std::memcmp(current->data.data()+dest,
           current->data.data()+original_live_sfx_packet+4u,
           original_live_sfx_length)!=0)
        stop("ORIGINAL_27F24_SFX_SOURCE_COPY_OR_ABI_MISMATCH");
    return 0u;
}
u32 original_live_sfx_format(u32 device,u32 dest,u32 ds,u32 fmt,u32 flags) noexcept {
    if(original_live_sfx_sample(device,dest,ds,flags)!=0u ||
       fmt!=code2::load_le32(current->data.data()+0xEE64u))
        stop("ORIGINAL_27F24_SFX_FORMAT_ABI_CHANGED");
    return 0u;
}
u32 original_live_sfx_start(u32 device) noexcept {
    if(!original_live_sfx_staged ||
       device!=code2::load_le32(current->data.data()+0xEE6Cu))
        stop("ORIGINAL_27F24_SFX_START_ABI_CHANGED");
    try {
        original_live_sfx_device.start_staged();
    }catch(const std::exception& error) {
        std::cerr<<"CODE2_ORIGINAL_GAMEPLAY_SFX=HOST_SUBMIT_FAILED reason="
                 <<error.what()<<" original_AIL=NOT_CERTIFIED\n"<<std::flush;
        stop("ORIGINAL_27F24_SFX_HOST_DEVICE_SUBMIT_FAILED");
    }
    return 0u;
}
void original_full_28168_audio_service() noexcept {
    if(!current || !original_full_frame_trace)
        stop("ORIGINAL_28168_SOURCE_AUDIO_TRACE_MISSING");
    const u32 enabled=code2::load_le32(current->data.data()+0xEE40u);
    const u32 sound_enabled=current->data[0xEE70u];
    const u32 request0=code2::load_le32(current->data.data()+0xEE48u);
    const u32 request1=code2::load_le32(current->data.data()+0xEE54u);
    const bool queued=request0!=0xFFFFFFFFu || request1!=0xFFFFFFFFu;
    code2::Application55Boundary audio{};
    if(enabled && sound_enabled && queued) {
        const char* requested=std::getenv(
            "RLOFT_RECOMP_EXPERIMENTAL_SOURCE_SFX_WINMM");
        if(!requested || std::strcmp(requested,"1")!=0) {
            std::cerr<<"CODE2_ORIGINAL_LIVE_GAMEPLAY=28168_ACTIVE_AIL_PENDING"
                     <<" slot0="<<request0<<" slot1="<<request1
                     <<" AIL_driver=UNCONNECTED software_queue=PRESERVED\n"
                     <<std::flush;
            stop("ORIGINAL_28168_27F24_ACTIVE_AIL_DRIVER_UNCONNECTED");
        }
        if(!current->game_audio.app68 ||
           !current->original_vga || original_live_sfx_staged)
            stop("ORIGINAL_27F24_SFX_NOT_AT_RETAINED_GUEST");
        // Mirror original signed-priority choice, but only for preflight.
        // The compiled original 28168 still chooses and clears the queue.
        const bool both=request0!=0xFFFFFFFFu&&request1!=0xFFFFFFFFu;
        const bool select0=request1==0xFFFFFFFFu ||
             (both && static_cast<std::int32_t>(
               code2::load_le32(current->data.data()+0xEE50u)) <
               static_cast<std::int32_t>(
               code2::load_le32(current->data.data()+0xEE5Cu)));
        const u32 id=select0?request0:request1;
        const u32 bank=code2::load_le32(current->data.data()+0xEE44u);
        const u32 catalog=code2::load_le32(current->data.data()+0xF0B8u);
        if(bank>=16u || !catalog ||
           std::uint64_t(catalog)+(std::uint64_t(bank)+1u)*16u>
                   current->data.size())
            stop("ORIGINAL_27F24_SOUND_RESOURCE_ARCHIVE_NOT_INITIALIZED");
        const u32 catalog_record=catalog+bank*16u;
        if(id>=code2::load_le32(current->data.data()+catalog_record+8u))
            stop("ORIGINAL_27F24_SOUND_RESOURCE_ID_NOT_IN_ARCHIVE");
        // Source-owned resource cache and exact original packet pointer.
        const u32 packet=code2::function_2f29c(
            current->machine,current->game_audio,id,bank);
        if(!packet || std::uint64_t(packet)+4u>current->data.size())
            stop("ORIGINAL_27F24_SOUND_RESOURCE_PACKET_MISSING");
        const u32 len=code2::load_le32(current->data.data()+packet);
        const u32 dest=code2::load_le32(current->data.data()+0xEE60u);
        if(len==0u || len>16u*1024u*1024u ||
           std::uint64_t(packet)+4u+len>current->data.size() ||
           std::uint64_t(dest)+len>current->data.size())
            stop("ORIGINAL_27F24_SFX_PACKET_OR_DEST_BOUNDS_INVALID");
        // Reject aliases: CODE.2 uses a forward MOVSD/MOVSB copy, and
        // sharing source/destination can destroy the authenticated packet.
        if(std::uint64_t(dest)<std::uint64_t(packet)+4u+len &&
           std::uint64_t(packet)<std::uint64_t(dest)+len)
            stop("ORIGINAL_27F24_SFX_RESOURCE_DESTINATION_ALIAS");
        std::vector<u8> payload(current->data.data()+packet,
                                current->data.data()+packet+4u+len);
        // Do not consume the source queue until the real Windows host
        // device and exact VOC codec have passed all preflight checks.
        try {
            original_live_sfx_device.preflight(payload);
        }catch(const std::exception& error) {
            std::cerr<<"CODE2_ORIGINAL_GAMEPLAY_SFX=PRE_FLIGHT_REFUSED"
                     <<" reason="<<error.what()
                     <<" original_28168_queue=PRESERVED"
                     <<" native_Miles_AIL=NOT_CERTIFIED\n"<<std::flush;
            stop("ORIGINAL_27F24_SOUND_DEVICE_PREFLIGHT_NOT_VERIFIED");
        }
        original_live_sfx_packet=packet;
        original_live_sfx_length=len;
        original_live_sfx_bank=bank;
        original_live_sfx_id=id;
        original_live_sfx_staged=true;
        audio.call_2f29c=&original_live_sfx_resource;
        audio.call_36803=&original_live_sfx_status;
        audio.call_36817=&original_live_sfx_stop;
        audio.call_367f9=&original_live_sfx_sample;
        audio.call_367ef=&original_live_sfx_format;
        audio.call_3680d=&original_live_sfx_start;
        // Runs unmodified 28168 -> 27F24 and its complete original
        // callback sequence, not a host-side reimplementation of queue.
        code2::function_28168(current->machine,audio);
        original_live_sfx_staged=false;
        std::cerr<<"CODE2_ORIGINAL_GAMEPLAY_SFX=SOURCE_28168_27F24_WINMM_SUBMITTED"
                 <<" selected_id="<<id<<" bank="<<bank
                 <<" source_packet_bytes="<<len
                 <<" host_device=WINMM_WAVEOUT"
                 <<" native_Miles_AIL=NOT_CERTIFIED"
                 <<" original_irq_dma=NOT_CERTIFIED"
                 <<" completion=AWAIT_WAVEHDR_DONE\n"<<std::flush;
    } else {
        code2::function_28168(current->machine,audio);
    }
    // Poll real host completion even on source ticks without a new SFX
    // request; WHDR_DONE (not a source queue empty condition) owns status.
    if(const char* host=std::getenv("RLOFT_RECOMP_EXPERIMENTAL_SOURCE_SFX_WINMM");
       host && std::strcmp(host,"1")==0)
        (void)original_live_sfx_device.status_playing();
    const auto completed=original_live_sfx_device.completions();
    std::cerr<<"CODE2_ORIGINAL_LIVE_GAMEPLAY=28168_SOURCE_SERVICE_RETURN"
             <<" original_28168=EXECUTED"
             <<" enabled="<<enabled
             <<" audible="<<sound_enabled
             <<" prior_queue0="<<request0
             <<" prior_queue1="<<request1
             <<" after_queue0="
             <<code2::load_le32(current->data.data()+0xEE48u)
             <<" after_queue1="
             <<code2::load_le32(current->data.data()+0xEE54u)
             <<" host_winmm_completions="<<completed
             <<" original_AIL_playback=NOT_CERTIFIED\n"<<std::flush;
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
void original_full_private_capture_1fc14_tick() noexcept {
    const char* raw=std::getenv("RLOFT_RECOMP_PRIVATE_1FC14_FRAME_DIR");
    if(!raw || !*raw)return;
    if(!current || !current->original_vga ||
       code2::load_le32(current->data.data()+0xC6C8u)!=22u)
        stop("ORIGINAL_1FC14_PRIVATE_CAPTURE_LEVEL22_PRECONDITION");
    const u32 crtc=current->original_vga->crtc_start();
    if(crtc!=0u && crtc!=0x8000u)
        stop("ORIGINAL_1FC14_PRIVATE_CAPTURE_UNKNOWN_SCANOUT_PAGE");
    const auto indices=current->original_vga->scanout_indices();
    if(indices.size()!=320u*400u)
        stop("ORIGINAL_1FC14_PRIVATE_CAPTURE_WRONG_INDEXED_FRAME_SIZE");
    std::vector<u8> palette;
    palette.reserve(768u);
    for(u32 i=0;i<256u;++i) {
        const auto rgb=current->original_vga->palette(static_cast<u8>(i));
        if(rgb.r>63u || rgb.g>63u || rgb.b>63u)
            stop("ORIGINAL_1FC14_PRIVATE_CAPTURE_DAC_NOT_RGB6");
        palette.push_back(rgb.r);
        palette.push_back(rgb.g);
        palette.push_back(rgb.b);
    }
    const std::filesystem::path directory(raw);
    std::error_code ec;
    std::filesystem::create_directories(directory,ec);
    if(ec || !std::filesystem::is_directory(directory,ec) || ec)
        stop("ORIGINAL_1FC14_PRIVATE_CAPTURE_DIRECTORY_NOT_WRITABLE");
    const auto name=std::string("level22_source_tick_")+
                    std::to_string(current->original_1fc14_post_scene_cycles);
    const auto destination=directory/name;
    const auto write_private=[&](const std::string& extension,
                                 const std::vector<u8>& bytes) {
        bool wrote=false;
        {
            std::ofstream out(destination.string()+extension,
                              std::ios::binary|std::ios::trunc);
            if(out) {
                out.write(reinterpret_cast<const char*>(bytes.data()),
                          static_cast<std::streamsize>(bytes.size()));
                out.flush();
                wrote=static_cast<bool>(out);
            }
        }
        return wrote;
    };
    if(!write_private(".idx",indices) || !write_private(".pal",palette))
        stop("ORIGINAL_1FC14_PRIVATE_CAPTURE_BYTES_WRITE_FAILED");
    const auto input_hash=rloft_recomp::sha256_hex(
        current->original_1fc14_source_keyboard_trace);
    const auto idx_hash=rloft_recomp::sha256_hex(indices);
    const auto palette_hash=rloft_recomp::sha256_hex(palette);
    bool wrote_receipt=false;
    {
        std::ofstream json(destination.string()+".json",
                           std::ios::binary|std::ios::trunc);
        if(json) {
            json<<"{\n"
                <<"  \"schema\": \"rloft-vga-observation-v1\",\n"
                <<"  \"role\": \"compiled_recomp\",\n"
                <<"  \"level\": 22,\n"
                <<"  \"frame_boundary\": \"1FC14_1A510\",\n"
                <<"  \"camera_x_16_16\": "
                <<code2::load_le32(current->data.data()+0x255D0u)<<",\n"
                <<"  \"camera_y_16_16\": "
                <<code2::load_le32(current->data.data()+0x255D4u)<<",\n"
                <<"  \"heading\": "
                <<code2::load_le32(current->data.data()+0xC658u)<<",\n"
                <<"  \"crtc_start\": "<<crtc<<",\n"
                <<"  \"source_tick\": "
                <<current->original_1fc14_post_scene_cycles<<",\n"
                <<"  \"keyboard_trace_sha256\": \""<<input_hash<<"\",\n"
                <<"  \"indices_sha256\": \""<<idx_hash<<"\",\n"
                <<"  \"rgb6_sha256\": \""<<palette_hash<<"\",\n"
                <<"  \"capture_phase\": \"NEXT_30B40_AFTER_PRIOR_SOURCE_TICK\",\n"
                <<"  \"input_origin\": \"HOST_30B40_EXPLICIT_REPLY\",\n"
                <<"  \"original_dos_executed\": false\n"
                <<"}\n";
            json.flush();
            wrote_receipt=static_cast<bool>(json);
        }
    }
    if(!wrote_receipt)
        stop("ORIGINAL_1FC14_PRIVATE_CAPTURE_RECEIPT_WRITE_FAILED");
    std::cerr<<"CODE2_ORIGINAL_LIVE_GAMEPLAY=1FC14_PRIVATE_FRAME_CAPTURE"
             <<" source_tick="<<current->original_1fc14_post_scene_cycles
             <<" crtc_start="<<crtc
             <<" indices_sha256="<<idx_hash
             <<" rgb6_sha256="<<palette_hash
             <<" input_trace_sha256="<<input_hash
             <<" private_only=YES"
             <<" independent_dos_parity=NOT_CERTIFIED\n"<<std::flush;
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
    if(complete) {
        ++current->original_1fc14_post_scene_cycles;
        original_full_private_capture_1fc14_tick();
    }
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
