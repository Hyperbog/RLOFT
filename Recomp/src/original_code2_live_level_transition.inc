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
    if(!((prior==22u && requested==23u)||
         (prior==23u && requested==22u)))
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
    // Original 1F8E0 -> 1F8FC requires a separate fully sourced mode
    // switch if the target differs. Only identical mode-3 graphics are
    // admitted; the retained 1FC14 frame stack renders on return.
    if(code2::load_le32(g+0xC034u)!=3u ||
       current->data[0x81ACu+requested*3u]!=3u)
        stop("ORIGINAL_181B4_LIVE_DESTINATION_GRAPHICS_MODE_OPEN");
    const u16 future_moving=code2::load_le16(target.header.data()+0x28Au);
    if(current->data[0xC02Fu]!=0u && future_moving!=0u)
        stop("ORIGINAL_181B4_LIVE_20850_NPC_REPOSITION_OPEN");
    if(target.header[0x29Cu]!=code2::function_29050(current->machine) ||
       code2::function_29048(current->machine)!=1u)
        stop("ORIGINAL_181B4_LIVE_DESTINATION_AUDIO_BANK_OPEN");
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
    explored.call_1e8c8=&original_full_movement_record;
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
    load.call_19ccc=&original_level_19ccc_push;
    load.call_19cf4=&original_level_19cf4_pop;
    load.call_0ccf4=&original_level_0ccf4_party_terminal;
    load.call_20850=+[]() noexcept {
        if(code2::load_le16(current->data.data()+0x258A2u)!=0u)
            stop("ORIGINAL_181B4_LIVE_20850_ACTOR_CALLBACK_PENDING");
        code2::Application46Boundary actor{};
        actor.call_2acdc=&code2::function_2acdc;
        code2::function_20850(current->machine,actor);
    };
    load.call_1f8e0=+[](u32 mode) noexcept {
        if(mode!=3u || code2::load_le32(current->data.data()+0xC034u)!=mode)
            stop("ORIGINAL_181B4_LIVE_1F8FC_MODE_REDRAW_PENDING");
        code2::function_1f8e0(current->machine,current->presentation,mode);
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
    current->original_level_transition_active=false;
    std::cerr<<"CODE2_ORIGINAL_GAMEPLAY_LEVEL=171FC_181B4_LIVE_SOURCE_RETURN"
             <<" from="<<prior<<" to="<<requested
             <<" source_write_calls="<<current->level_overlay.source_writes
             <<" source_read_calls="<<current->level_overlay.source_reads
             <<" npc_postload=ORIGINAL_17A68"
             <<" same_guest=YES"
             <<" awaited_next_1FC14_frame=YES\n"<<std::flush;
}
