// Diagnostic-only entry into an AUTHENTIC type-8 portal record from the
// owner's unmodified original LEVELS.DAT. The host does not construct a
// destination or synthesize movement; compiled CODE2 2D3D8 owns the
// camera, heading, 181B4 transition and post-portal actor checks.
void original_live_archived_portal_entry(u32 requested) noexcept {
    if(!current || !current->original_vga || !current->live_continuation ||
       !current->host_files ||
       !current->level_overlay.idle() ||
       current->original_level_transition_active)
        stop("ORIGINAL_2D3D8_ARCHIVE_PORTAL_OWNER_NOT_READY");
    const u32 source=code2::load_le32(current->data.data()+0xC6C8u);
    if(!((source==23u&&requested==26u)||
         (source==26u&&requested==23u)))
        stop("ORIGINAL_2D3D8_ARCHIVE_PORTAL_PAIR_NOT_VERIFIED");
    std::vector<u8> archive{};
    rloft::original_levels::LevelGrid world{};
    rloft::original_levels::LevelGrid dest{};
    try {
        archive=rloft::original_levels::read_archive(
            (current->original_installation_root/"LEVELS.DAT").string());
        world=rloft::original_levels::decode_grid(archive,source);
        dest=rloft::original_levels::decode_grid(archive,requested);
    } catch(const std::exception&) {
        stop("ORIGINAL_2D3D8_ARCHIVE_PORTAL_MEDIA_INVALID");
    }
    if(code2::load_le16(current->data.data()+0x25619u)!=world.width ||
       code2::load_le16(current->data.data()+0x2561Bu)!=world.height)
        stop("ORIGINAL_2D3D8_ARCHIVE_PORTAL_LIVE_LEVEL_LAYOUT_CHANGED");
    const u32 start=rloft::original_levels::le32(
        archive.data()+4u+source*4u);
    const u32 end=(source+1u<32u)?
        rloft::original_levels::le32(archive.data()+4u+(source+1u)*4u):
        static_cast<u32>(archive.size());
    std::uint64_t at=std::uint64_t(start)+0x1Du+0x2A4u+
                     2u*world.width*world.height;
    bool found=false;
    u32 portal_col=0u,portal_row=0u,exit_x=0u,exit_y=0u,turn=0u;
    for(u32 col=0u;col<world.width&&!found;++col) {
        if(at+2u>end)stop("ORIGINAL_2D3D8_ARCHIVE_PORTAL_COUNT_TRUNCATED");
        const u32 count=rloft::original_levels::le16(
            archive.data()+static_cast<std::size_t>(at));
        at+=2u;
        if(count>512u || at+std::uint64_t(count)*10u>end)
            stop("ORIGINAL_2D3D8_ARCHIVE_PORTAL_RECORDS_TRUNCATED");
        for(u32 j=0u;j<count;++j) {
            const u8* rec=archive.data()+at+j*10u;
            if(rec[0]!=8u || rec[6]!=requested)continue;
            if(rec[5]!=col || rec[4]>=world.height ||
               rec[7]>=dest.width || rec[8]>=dest.height)
                stop("ORIGINAL_2D3D8_ARCHIVE_PORTAL_COORDINATES_INVALID");
            // Verify that this ORIGINAL portal still exists in the
            // source guest's own dynamic record column. A removed or
            // altered in-game portal cannot be resurrected by the host.
            const u32 list=code2::load_le32(
                current->data.data()+0xCB3Cu+col*4u);
            if(!list)continue;
            bool in_guest=false;
            for(u32 k=0u;k<4096u;++k) {
                const std::uint64_t p=std::uint64_t(list)+k*10u;
                if(p+10u>current->data.size())
                    stop("ORIGINAL_2D3D8_ARCHIVE_PORTAL_GUEST_LIST_INVALID");
                const u8* guest=current->data.data()+p;
                if(guest[0u]==0u && (guest[1u]&0x10u)==0u)break;
                if(guest[0u]==8u && guest[4u]==rec[4u] &&
                   guest[5u]==rec[5u] && guest[6u]==rec[6u] &&
                   guest[7u]==rec[7u] && guest[8u]==rec[8u] &&
                   guest[9u]==rec[9u]) {
                    in_guest=true;break;
                }
            }
            if(!in_guest)continue;
            portal_col=col;portal_row=rec[4u];
            exit_x=rec[7u];exit_y=rec[8u];turn=rec[9u];
            found=true;break;
        }
        at+=std::uint64_t(count)*10u;
    }
    if(!found)stop("ORIGINAL_2D3D8_ARCHIVE_PORTAL_NOT_PRESENT_IN_GUEST");
    code2::Application66Boundary b{};
    b.call_181b4=&original_full_movement_event;
    b.call_1c0c0=&original_full_movement_heading;
    b.call_e0f8=&original_live_actor_reposition;
    code2::function_2d3d8(
        current->machine,b,portal_col,portal_row,
        exit_x,exit_y,turn,requested);
    if(code2::load_le32(current->data.data()+0xC6C8u)!=requested ||
       current->original_pending_level_frame!=requested)
        stop("ORIGINAL_2D3D8_ARCHIVE_PORTAL_SOURCE_DID_NOT_TRANSFER");
    std::cerr<<"CODE2_ORIGINAL_GAMEPLAY_LEVEL=2D3D8_AUTHENTIC_PORTAL_SOURCE_RETURN"
             <<" from="<<source<<" to="<<requested
             <<" portal_x="<<portal_col<<" portal_y="<<portal_row
             <<" target_x="<<exit_x<<" target_y="<<exit_y
             <<" turn="<<turn
             <<" source_portal_record=UNMODIFIED_LEVELS_DAT"
             <<" next_1FC14_frame=NOT_YET_OBSERVED\n"<<std::flush;
}
// Original 20850 actor reposition callback graph. It operates on the
// destination world's compiled CODE2 actor, tile and RNG functions. The
// result is not a host-selected "safe" or "no collision" surrogate.
code2::s16 original_live_actor_2d5d0(
        u32 y,u32 x,u32 index) noexcept {
    if(index>=0x190u)
        stop("ORIGINAL_20850_SOURCE_ACTOR_INDEX_OUT_OF_BOUNDS");
    code2::Application66Boundary collision{};
    collision.call_1e8c8=&original_full_movement_record;
    collision.call_1e930=&original_full_movement_marker;
    collision.call_20c0c=&original_full_movement_effect;
    collision.call_181b4=&original_full_movement_event;
    collision.call_002a8=&original_full_movement_script;
    collision.call_e0f8=&original_live_actor_reposition;
    collision.call_1c0c0=&original_full_movement_heading;
    collision.call_1ea78=&original_full_movement_refresh;
    code2::Application65Boundary secondary{};
    secondary.call_181b4=&original_full_movement_event;
    secondary.call_1c0c0=&original_full_movement_heading;
    secondary.call_1c1dc=&original_full_movement_bit_query;
    secondary.call_1c124=&original_full_movement_bit_set;
    secondary.call_1c160=&original_full_movement_bit_clear;
    secondary.call_2acdc=&code2::function_2acdc;
    secondary.call_1ea78=&original_full_movement_refresh;
    collision.app65=&secondary;
    return static_cast<code2::s16>(
        code2::function_2d5d0(current->machine,collision,y,x,
                              static_cast<code2::s32>(index)));
}
u32 original_live_actor_random_direction(u32& /*x*/,u32& /*y*/) noexcept {
    // The encoded 20850 call returns the source's 15-bit RNG value;
    // existing x/y registers are intentionally preserved.
    return code2::function_30ca2(current->machine);
}
void original_live_actor_vector(
        u32& x,u32& y,u32 distance,u32 heading) noexcept {
    code2::Application67Boundary vector{};
    vector.call_35680=&code2::function_35680;
    code2::function_2df2c(current->machine,vector,&x,&y,distance,heading);
}
u32 original_live_actor_seed(u32 value) noexcept {
    code2::function_30cc4(current->machine,value);
    return value; // ABI caller 290D0 discards 30CC4 return.
}
void original_live_actor_music_rng(u32 seed) noexcept {
    code2::Application56Boundary source{};
    source.call_30cc4=&original_live_actor_seed;
    (void)code2::function_290d0(current->machine,source,seed);
}
u32 original_live_actor_roll(
        u32 count,u32 sides,u32 add,u32 flat) noexcept {
    return original_level_actor_roll(count,sides,add,flat);
}
u32 original_live_actor_reposition(u32 index) noexcept {
    if(index>=static_cast<u32>(
          code2::load_le16(current->data.data()+0x258A2u)) ||
       index>=150u)
        stop("ORIGINAL_0E0F8_ACTOR_INDEX_INVALID");
    code2::Application19Boundary source{};
    source.call_30700=+[]() noexcept -> u32 {
        return code2::function_30700(current->machine);
    };
    source.call_290d0=&original_live_actor_music_rng;
    source.call_30ca2=&original_level_actor_random;
    source.call_1c328=&original_live_actor_roll;
    return code2::function_0e0f8(current->machine,source,index);
}
void original_live_actor_reposition_void(u32 index) noexcept {
    (void)original_live_actor_reposition(index);
}
void original_live_actor_20850() noexcept {
    const auto count=code2::load_le16(current->data.data()+0x258A2u);
    if(count>150u)
        stop("ORIGINAL_20850_UNBOUNDED_ACTOR_COUNT");
    code2::Application46Boundary source{};
    source.call_2acdc=&code2::function_2acdc;
    source.call_2d5d0=&original_live_actor_2d5d0;
    source.call_30ca2=&original_live_actor_random_direction;
    source.call_2df2c=&original_live_actor_vector;
    source.call_e0f8=&original_live_actor_reposition_void;
    code2::function_20850(current->machine,source);
}
// Owner-live source 171FC / 181B4 transition.
using live_transition_s32=code2::s32;
void original_181b4_live_transition(u32 requested) noexcept {
    const char* enabled=std::getenv("RLOFT_RECOMP_EXPERIMENTAL_LIVE_LEVEL_TRANSITIONS");
    if(!enabled || std::strcmp(enabled,"1")!=0)
        stop("ORIGINAL_181B4_CROSS_LEVEL_REQUIRES_EXPLICIT_OPT_IN");
    if(!current || !current->live_continuation || !current->original_vga ||
       !current->host_files || !current->game_audio.app68 ||
       current->original_181b4_completed!=1u ||
       current->original_level_transition_active)
        stop("ORIGINAL_181B4_LIVE_OWNER_PREREQUISITE_MISSING");
    const u32 prior=code2::load_le32(current->data.data()+0xC6C8u);
    // 22/23 have a preserved-source save/restore gate. 23/26 have
    // an independently authenticated *real portal record* 2D3D8
    // roundtrip gate. No other arbitrary transitions are enabled.
    if(!((prior==22u && requested==23u)||
         (prior==23u && requested==22u)||
         (prior==23u && requested==26u)||
         (prior==26u && requested==23u)))
        stop("ORIGINAL_181B4_LIVE_UNVERIFIED_LEVEL_PAIR");
    if(!current->level_overlay.idle() || current->data[0xA79Du]!=0u ||
       current->data[0xA7ECu]>=14u)
        stop("ORIGINAL_171FC_LIVE_SAVE_STACK_UNSAFE");
    const auto* g=current->data.data();
    const live_transition_s32 width=static_cast<code2::s16>(code2::load_le16(g+0x25619u));
    const live_transition_s32 height=static_cast<code2::s16>(code2::load_le16(g+0x2561Bu));
    if(width<=0 || width>128 || height<=0 || height>128 ||
       std::uint64_t(width)*height>0x4000u ||
       code2::load_le32(g+0xC038u)>150u)
        stop("ORIGINAL_171FC_LIVE_WORLD_INVALID");
    const u32 scripts=code2::load_le32(g+0xCF44u);
    if(!scripts || std::uint64_t(scripts)+0x514u>current->data.size())
        stop("ORIGINAL_171FC_LIVE_SCRIPT_POINTER_UNMAPPED");
    // Original 171FC traverses each ten-byte list up to a source
    // terminator. A truncated pointer cannot be permitted to stream
    // guest bytes into the private save.
    for(live_transition_s32 col=0;col<width;++col) {
        for(u32 family=0u;family<2u;++family) {
            const u32 ptr=code2::load_le32(g+
                    (family?0xCD3Cu:0xCB3Cu)+static_cast<u32>(col)*4u);
            if(!ptr)continue;
            bool end=false;
            for(u32 i=0u;i<6554u;++i) {
                const std::uint64_t offset=std::uint64_t(ptr)+i*10u;
                if(offset+10u>current->data.size())break;
                const u8 first=g[static_cast<std::size_t>(offset)];
                const u8 flags=g[static_cast<std::size_t>(offset)+1u];
                if(first==0u&&(family!=0u||(flags&0x10u)==0u)){
                    end=true;break;
                }
            }
            if(!end)stop("ORIGINAL_171FC_LIVE_COLUMN_LIST_UNTERMINATED");
        }
    }
    for(const u32 at:{0x258A2u,0x258A4u,0x258A6u}) {
        const live_transition_s32 count=static_cast<code2::s16>(code2::load_le16(g+at));
        if(count<0 || count>150)
            stop("ORIGINAL_171FC_LIVE_ENTITY_COUNT_INVALID");
    }
    // Source I/O is preflighted before writing the old level. This is
    // the original same-level owner with real archival data, not a
    // rebuilt or synthetic world.
    rloft::original_levels::LevelGrid target{};
    try {
        if(current->level_overlay.has(requested))
            target=rloft::original_levels::decode_saved_grid(
                current->level_overlay.levels.at(requested),requested);
        else
            target=rloft::original_levels::decode_grid(
                rloft::original_levels::read_archive(
                    (current->original_installation_root/"LEVELS.DAT").string()),
                requested);
    }catch(const std::exception&) {
        stop("ORIGINAL_181B4_LIVE_DESTINATION_MEDIA_PREFLIGHT_FAILED");
    }
    if(target.cells.size()!=std::size_t(target.width)*target.height ||
       target.width>128u || target.height>128u)
        stop("ORIGINAL_181B4_LIVE_DESTINATION_WORLD_INVALID");
    // The compiled original 1F8E0 chooses mode from 81AC and runs
    // 1F8FC when it differs. Validate the authentic RES0 backdrop pair
    // and COLORS bank BEFORE source 171FC mutates the departing world.
    const u32 destination_mode=current->data[0x81ACu+requested*3u];
    const u32 first_res=code2::load_le32(g+0x258DCu);
    const u32 catalog=code2::load_le32(g+0xF0B8u);
    if(destination_mode>=15u ||
       std::uint64_t(catalog)+16u>current->data.size() ||
       first_res>0xFFFFFFFFu-2u-destination_mode*3u)
        stop("ORIGINAL_181B4_LIVE_GRAPHICS_RESOURCE_INDEX_INVALID");
    const u32 catalog_count=code2::load_le32(g+catalog+8u);
    const u32 backdrop=first_res+1u+destination_mode*3u;
    if(backdrop+1u>=catalog_count)
        stop("ORIGINAL_181B4_LIVE_GRAPHICS_BACKDROP_NOT_IN_RES0");
    const u32 active_mode=code2::load_le32(g+0xC034u);
    if(active_mode>=15u)
        stop("ORIGINAL_181B4_LIVE_PREVIOUS_GRAPHICS_MODE_INVALID");
    const u16 future_moving=code2::load_le16(target.header.data()+0x28Au);
    if(future_moving>150u)
        stop("ORIGINAL_181B4_LIVE_ACTOR_COUNT_ABOVE_SOURCE_MAX");
    // When the authentic EE78 AIL device is disabled, the compiled
    // 28DCC, 28EF0 and 28E58 functions all immediately return, even if
    // the destination changes music bank. Keep that source behavior;
    // reject active real devices until the original AIL service is wired.
    if(code2::load_le32(g+0xEE78u)!=0u)
        stop("ORIGINAL_181B4_LIVE_ACTIVE_AIL_DEVICE_CALLBACKS_OPEN");
    // The original archive fallback reads the CODE.2 BSS filename.
    // Reuse the exact authenticated C-runtime literal restoration that
    // the initial original Level22 loader already requires.
    original_restore_181b4_archive_literal();
    original_live_level_no_real_save(prior);
    original_live_level_no_real_save(requested);
    const std::size_t handles_before=current->host_files->open_handles();
    current->original_level_transition_active=true;
    current->original_level_state_pushes=0u;
    current->original_level_state_pops=0u;
    current->original_level_party_terminal_checks=0u;
    code2::Application31Boundary save{};
    save.call_31380=&original_live_level_remove;
    save.call_312c8=&original_live_level_old_open;
    save.call_312b4=&original_live_level_create;
    save.call_3131c=&original_live_level_seek;
    save.call_31304=&original_live_level_write_guest;
    save.call_31304_local=&original_live_level_write_local;
    save.call_312dc=&original_live_level_close;
    save.call_2f580=&original_live_level_io_error;
    // Compiled original 01558 regenerates the party/exploration
    // automap from the live map/record flags before 171FC writes it.
    // Skipping this step would serialize stale annotations.
    code2::Application04Boundary explored{};
    explored.call_1e8c8=+[](u32 a,u32 b) noexcept -> u32 {
        return original_full_movement_record(
            static_cast<code2::s32>(a),static_cast<code2::s32>(b));
    };
    explored.call_1e930=&original_full_movement_marker;
    save.app04=&explored;
    save.call_1c1dc=&original_level_actor_quest;
    save.call_2acdc=&original_level_actor_type;
    save.call_30ca2=&original_level_actor_random;
    save.call_1c328=&original_level_actor_roll;
    save.call_1c77c=&original_level_actor_dialog;
    code2::Application32Boundary load{};
    load.app31=&save;
    load.call_312a0=&original_live_level_open_saved;
    load.call_312a0_local=&original_level_archive_open_local;
    load.call_3131c=&original_live_level_seek;
    load.call_312ec=&original_live_level_read_guest;
    load.call_312ec_local=&original_live_level_read_local;
    load.call_312dc=&original_live_level_close;
    load.call_2f580=&original_live_level_io_error;
    load.call_30d30=&original_checked_memset;
    load.call_3104c=&original_boot_heap_alloc;
    load.call_310b0=&original_level_heap_release;
    load.call_29050=&original_level_sound_bank;
    load.call_29048=&original_level_sound_mode;
    load.call_28dcc=+[](u32 handle) noexcept {
        if(code2::load_le32(current->data.data()+0xEE78u)!=0u)
            stop("ORIGINAL_181B4_AIL_STOP_REQUIRES_SOURCE_DRIVER");
        code2::Application56Boundary music{};
        code2::function_28dcc(current->machine,music,handle);
    };
    load.call_28ef0=+[]() noexcept {
        if(code2::load_le32(current->data.data()+0xEE78u)!=0u)
            stop("ORIGINAL_181B4_AIL_RESET_REQUIRES_SOURCE_DRIVER");
        code2::Application56Boundary music{};
        code2::function_28ef0(current->machine,music);
    };
    load.call_28e58=+[](live_transition_s32 bank,
                          live_transition_s32 slot,u32 id) noexcept {
        if(code2::load_le32(current->data.data()+0xEE78u)!=0u)
            stop("ORIGINAL_181B4_AIL_BANK_REQUIRES_SOURCE_DRIVER");
        code2::Application56Boundary music{};
        code2::function_28e58(current->machine,music,
                              static_cast<u32>(bank),
                              static_cast<u32>(slot),id);
    };
    load.call_19ccc=&original_level_19ccc_push;
    load.call_19cf4=&original_level_19cf4_pop;
    load.call_0ccf4=&original_level_0ccf4_party_terminal;
    load.call_20850=&original_live_actor_20850;
    load.call_1f8e0=+[](u32 mode) noexcept {
        if(mode>=15u || !current->original_level_transition_active ||
           !current->original_vga || !current->game_audio.app68)
            stop("ORIGINAL_181B4_1F8FC_MODE_SERVICE_PREREQUISITES");
        // Real source 1F8E0 performs the unchanged-mode return; on
        // a change it falls into the full original 1F8FC compositor.
        // Its callbacks call the existing RES0/RES1 resource decoders,
        // planar VGA host, world renderer, original HUD and page flips.
        code2::Application45Boundary render=current->presentation;
        render.app43=&current->live_ui;
        render.app44=&current->live_scene;
        render.call_291b0=&original_291b0_source_page;
        render.call_2e510=&original_scene_frame_base;
        render.call_2efd0=&original_scene_res_release;
        render.call_30070=&original_scene_capture;
        render.call_30040=&original_scene_capture_release;
        render.call_325b0=&original_scene_palette;
        render.call_29234=&original_video_palette_29234;
        render.call_1a5b0=&original_scene_1a5b0;
        render.call_22d70=&original_scene_clear_event_ring;
        render.call_2f868=&original_scene_poll_source_events;
        render.call_30b2c=&original_scene_finish_keyboard;
        const u32 old_mode=code2::load_le32(current->data.data()+0xC034u);
        const auto prior_frames=current->original_scene_presentations;
        const auto prior_dac=current->original_source_dac_streams;
        code2::function_1f8e0(current->machine,render,mode);
        if(code2::load_le32(current->data.data()+0xC034u)!=mode)
            stop("ORIGINAL_181B4_1F8FC_GRAPHICS_MODE_NOT_COMMITTED");
        if(old_mode!=mode) {
            // The source 1F8FC compositor must really present a new VGA
            // frame and stream the source 29234 palette once; host-side
            // incrementing without those witnesses would be imitation.
            if(current->original_scene_presentations!=prior_frames+1u ||
               current->original_source_dac_streams!=prior_dac+1u)
                stop("ORIGINAL_181B4_1F8FC_SOURCE_COMPOSITOR_INCOMPLETE");
            ++current->original_live_graphics_mode_switches;
        } else if(current->original_scene_presentations!=prior_frames ||
                  current->original_source_dac_streams!=prior_dac)
            stop("ORIGINAL_181B4_1F8E0_UNCHANGED_MODE_MUTATED_VGA");
        std::cerr<<"CODE2_ORIGINAL_GAMEPLAY_LEVEL=1F8E0_SOURCE_RETURN"
                 <<" mode="<<mode<<" source_compositor=1F8FC"
                 <<" framebuffer_parity=NOT_CERTIFIED\n"<<std::flush;
    };
    load.call_1c124=+[](u32 bit) noexcept {
        code2::function_1c124(current->machine,bit);
    };
    load.call_31edc=+[](u32 value) noexcept {
        code2::function_31edc(current->machine,static_cast<live_transition_s32>(value));
    };
    load.call_31f38=+[](u32 value) noexcept {
        code2::function_31f38(current->machine,static_cast<live_transition_s32>(value));
    };
    load.call_31f94=+[]() noexcept {
        code2::function_31f94(current->machine);
    };
    // The reconstructed original 181B4 calls 171FC to persist the
    // departing world, then reconstructs actors, scripts and the target
    // map on this retained machine. No second host game loop is created.
    code2::function_181b4(current->machine,load,static_cast<live_transition_s32>(requested));
    if(!current->level_overlay.idle() ||
       current->host_files->open_handles()!=handles_before ||
       current->original_level_state_pushes!=1u ||
       current->original_level_state_pops!=1u ||
       current->original_level_party_terminal_checks!=1u ||
       !current->level_overlay.has(prior) ||
       code2::load_le32(current->data.data()+0xC6C8u)!=requested ||
       code2::load_le16(current->data.data()+0x25619u)!=target.width ||
       code2::load_le16(current->data.data()+0x2561Bu)!=target.height ||
       code2::load_le32(current->data.data()+0xCF44u)==0u)
        stop("ORIGINAL_181B4_LIVE_SOURCE_TRANSITION_DIVERGED");
    ++current->original_live_level_transitions;
    current->original_pending_level_frame=requested;
    current->original_pending_level_scene_returned=false;
    current->original_level_transition_active=false;
    std::cerr<<"CODE2_ORIGINAL_GAMEPLAY_LEVEL=171FC_181B4_LIVE_SOURCE_RETURN"
             <<" from="<<prior<<" to="<<requested
             <<" source_write_calls="<<current->level_overlay.source_writes
             <<" source_read_calls="<<current->level_overlay.source_reads
             <<" npc_postload=ORIGINAL_17A68"
             <<" same_guest=YES"
             <<" awaited_next_1FC14_frame=YES\n"<<std::flush;
}
