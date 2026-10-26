// Owner-private original CODE.2 07938 save restoration. The input is an
// unmodified real SAVE.DAT/LEV023.DAT pair, copied into retained guest-owned
// buffers. All parsing, game-state mutations and level dispatch are compiled
// original CODE.2 callbacks. No original installation file is modified.
// This is an explicitly opt-in engineering path, not a DOS save menu.
namespace {
constexpr u32 source_saved_game_handle=0xFFFCu;
void live_07938_guest_path(u32 ptr) noexcept {
    if(ptr!=0x12990u || std::uint64_t(ptr)+14u>current->data.size())
        stop("ORIGINAL_07938_SAVE_FILENAME_POINTER");
    constexpr char expected[]="WORK\\SAVE.DAT";
    for(u32 i=0;i<14u;++i) {
        u8 c=current->data[ptr+i];
        if(c>='a'&&c<='z')c=static_cast<u8>(c-('a'-'A'));
        if(c!=static_cast<u8>(expected[i]))
            stop("ORIGINAL_07938_SOURCE_SAVE_FILENAME_CHANGED");
    }
}
u32 live_07938_open(u32 ptr) noexcept {
    live_07938_guest_path(ptr);
    if(!current->original_live_restore_active ||
       current->original_live_save_open ||
       current->original_live_save_opens ||
       current->original_live_save_bytes.empty())
        stop("ORIGINAL_07938_SAVE_OPEN_NOT_READY");
    current->original_live_save_open=true;
    current->original_live_save_position=0u;
    ++current->original_live_save_opens;
    return source_saved_game_handle;
}
u32 live_07938_read_local(u8* dst,u32 count,u32 handle) noexcept {
    if(handle!=source_saved_game_handle ||
       !current->original_live_save_open ||
       (!dst && count) ||
       std::uint64_t(current->original_live_save_position)+count>
           current->original_live_save_bytes.size())
        stop("ORIGINAL_07938_SAVE_LOCAL_READ_OUT_OF_RANGE");
    if(count)
        std::copy_n(current->original_live_save_bytes.data()+
                    current->original_live_save_position,count,dst);
    current->original_live_save_position+=count;
    ++current->original_live_save_reads;
    return count;
}
u32 live_07938_read_guest(u32 dst,u32 count,u32 handle) noexcept {
    if(std::uint64_t(dst)+count>current->data.size())
        stop("ORIGINAL_07938_SAVE_GUEST_READ_OUT_OF_RANGE");
    return live_07938_read_local(current->data.data()+dst,count,handle);
}
u32 live_07938_seek(u32 displacement,u32 origin,u32 handle) noexcept {
    if(handle!=source_saved_game_handle ||
       !current->original_live_save_open || origin>2u)
        stop("ORIGINAL_07938_SAVE_SEEK_HANDLE");
    const std::int64_t base=origin==0u?0:
        origin==1u?static_cast<std::int64_t>(current->original_live_save_position):
        static_cast<std::int64_t>(current->original_live_save_bytes.size());
    const std::int64_t next=base+
        static_cast<std::int64_t>(static_cast<std::int32_t>(displacement));
    if(next<0 || next>static_cast<std::int64_t>(
                        current->original_live_save_bytes.size()))
        stop("ORIGINAL_07938_SAVE_SEEK_OUT_OF_RANGE");
    current->original_live_save_position=static_cast<u32>(next);
    return static_cast<u32>(next);
}
void live_07938_close(u32 handle) noexcept {
    if(handle!=source_saved_game_handle ||
       !current->original_live_save_open)
        stop("ORIGINAL_07938_SAVE_CLOSE_HANDLE");
    current->original_live_save_open=false;
    ++current->original_live_save_closes;
}
void live_07938_copy(u32 dst,u32 src,u32 size) noexcept {
    if(std::uint64_t(dst)+size>current->data.size() ||
       std::uint64_t(src)+size>current->data.size())
        stop("ORIGINAL_07938_COPY_OUT_OF_GUEST");
    (void)code2::function_30cd4(current->machine,dst,src,size);
}
void live_07938_heading(u32 heading) noexcept {
    code2::function_1c0c0(current->machine,heading);
}
void live_07938_interrupt_ticks(u32 ticks) noexcept {
    code2::function_30738(current->machine,ticks);
}
void live_07938_game_ticks(u32 ticks) noexcept {
    code2::function_1e74c(current->machine,ticks);
}
void live_07938_cursor(code2::s32 item,u32 known) noexcept {
    code2::Application35Boundary b{};
    code2::function_1a344(current->machine,b,static_cast<u32>(item),known);
}
void live_07938_environment(u32 raw) noexcept {
    code2::function_31edc(current->machine,
                          static_cast<code2::s32>(raw));
}
void live_07938_light(u32 raw) noexcept {
    code2::function_31f38(current->machine,
                          static_cast<code2::s32>(raw));
}
void live_07938_level(u32 requested) noexcept {
    ++current->original_live_save_level_callbacks;
    if(requested!=23u || !current->level_overlay.has(23u))
        stop("ORIGINAL_07938_SOURCE_SAVED_LEVEL_NOT_23");
    // Original 181B4 performs the actual level load on the same guest,
    // using the saved-world overlay before the archive fallback.
    original_181b4_live_transition(requested);
}
} // namespace

void original_live_07938_restore_level23() noexcept {
    const char* enabled=std::getenv(
        "RLOFT_RECOMP_EXPERIMENTAL_LEVEL23_SAVE_RESTORE");
    const char* dir_name=std::getenv("RLOFT_RECOMP_ORIGINAL_LEVEL23_SAVE_DIR");
    if(!enabled || std::strcmp(enabled,"1")!=0 ||
       !dir_name || !*dir_name)
        stop("ORIGINAL_07938_RESTORE_REQUIRES_EXPLICIT_SAVE_DIR");
    if(!current || !current->live_continuation || !current->host_files ||
       !current->original_vga || !current->level_overlay.idle() ||
       current->original_181b4_completed!=1u ||
       current->original_live_restore_active ||
       current->original_live_level_transitions!=0u ||
       current->original_live_save_opens!=0u ||
       current->original_pending_level_frame!=0xFFFFFFFFu ||
       code2::load_le32(current->data.data()+0xC6C8u)!=22u)
        stop("ORIGINAL_07938_RETAINED_LEVEL22_OWNER_NOT_READY");
    std::vector<u8> save, saved_world;
    try {
        const std::filesystem::path dir(dir_name);
        save=rloft::original_levels::read_archive(
            (dir/"SAVE.DAT").string());
        saved_world=rloft::original_levels::read_archive(
            (dir/"LEV023.DAT").string());
        const auto grid=rloft::original_levels::decode_saved_grid(
            saved_world,23u);
        if(grid.cells.size()!=std::size_t(grid.width)*grid.height ||
           !grid.width || !grid.height || grid.width>128u ||
           grid.height>128u)
            stop("ORIGINAL_07938_SAVED_LEVEL23_LAYOUT_INVALID");
    } catch(const std::exception&) {
        stop("ORIGINAL_07938_OWNER_SAVED_MEDIA_READ_FAILED");
    }
    if((save.size()!=9288u && save.size()!=9310u) ||
       code2::load_le32(save.data()+0x52u)!=23u ||
       current->level_overlay.has(23u))
        stop("ORIGINAL_07938_SAVED_GAME_NOT_AUTHENTIC_LEVEL23");
    // No guest camera, actor, map or event byte is synthesized here.
    current->level_overlay.levels.emplace(23u,std::move(saved_world));
    current->original_live_save_bytes=std::move(save);
    current->original_live_save_position=0u;
    current->original_live_save_opens=0u;
    current->original_live_save_closes=0u;
    current->original_live_save_reads=0u;
    current->original_live_save_level_callbacks=0u;
    current->original_live_save_open=false;
    current->original_live_restore_active=true;
    code2::Application10Boundary input{};
    code2::Application09Boundary ui{};
    input.call_312a0=&live_07938_open;
    input.call_3131c=&live_07938_seek;
    input.call_312ec=&live_07938_read_guest;
    input.call_312ec_local=&live_07938_read_local;
    input.call_312dc=&live_07938_close;
    input.call_3104c=&original_boot_heap_alloc;
    input.call_310b0=&original_level_heap_release;
    input.call_30cd4=&live_07938_copy;
    input.call_1c0c0=&live_07938_heading;
    input.call_30738=&live_07938_interrupt_ticks;
    input.call_1a344=&live_07938_cursor;
    input.call_1e74c=&live_07938_game_ticks;
    input.call_31edc=&live_07938_environment;
    input.call_31f38=&live_07938_light;
    input.call_181b4=&live_07938_level;
    ui.call_1e8c8=&original_full_movement_record;
    const u32 result=code2::function_07938(current->machine,input,ui);
    if(result!=0u || current->original_live_save_open ||
       current->original_live_save_opens!=1u ||
       current->original_live_save_closes!=1u ||
       !current->original_live_save_reads ||
       current->original_live_save_level_callbacks!=1u ||
       current->original_live_level_transitions!=1u ||
       current->original_pending_level_frame!=23u ||
       !current->level_overlay.idle() ||
       code2::load_le32(current->data.data()+0xC6C8u)!=23u ||
       code2::load_le32(current->data.data()+0xC658u)>=360u)
        stop("ORIGINAL_07938_RETAINED_SOURCE_RESTORE_INCOMPLETE");
    current->original_live_restore_active=false;
    std::cerr<<"CODE2_ORIGINAL_GAMEPLAY_LEVEL=07938_RETAINED_LEVEL23_RESTORE_PASS"
             <<" level=23 source=07938_181B4"
             <<" original_save_reads="<<current->original_live_save_reads
             <<" same_original_1FC14_guest=YES"
             <<" next_source_frame=PENDING"
             <<" original_media=READ_ONLY\n"<<std::flush;
}
