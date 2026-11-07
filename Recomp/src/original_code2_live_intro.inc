// Retained original CODE.2 20440 -> 19888 front-end.
// All callbacks use current's authentic guest, DOS owner, Watcom heap, and VGA.
// The first unapplied DOS timing/input service stops explicitly: no replacement
// slideshow, synthetic tick count, or host-authored title behavior.
void intro_require_no_active_sound() noexcept {
    if(code2::load_le32(current->data.data()+0xEE78u)!=0u ||
       code2::load_le32(current->data.data()+0xEE40u)!=0u)
        stop("ORIGINAL_INTRO_ACTIVE_AIL_DEVICE_NOT_CONNECTED");
}
u32 intro_tick() noexcept { return code2::function_30700(current->machine); }
void intro_pump() noexcept { original_scene_poll_source_events(); }
void intro_wait(u32 delay,u32 flags) noexcept {
    original_game_over_2e614_timing(delay,flags);
}
u32 intro_keyboard() noexcept {
    code2::Application71Boundary b{};
    b.call_2f868=&intro_pump;
    return code2::function_30b40(current->machine,b);
}
u32 intro_read_scalar(u32 handle,u32 width) noexcept {
    if((width!=2u&&width!=4u) || handle>0xFFFFu ||
       !current->host_files ||
       current->host_descriptor_flags.count(handle)==0u)
        stop("ORIGINAL_INTRO_312EC_SCALAR_ABI_INVALID");
    u8 buffer[4]{};
    const auto r=current->host_files->read(
        static_cast<u16>(handle),buffer,width);
    if(r.carry||r.ax!=width)
        stop("ORIGINAL_INTRO_312EC_SCALAR_SHORT_READ");
    return width==2u?code2::load_le16(buffer):code2::load_le32(buffer);
}
void intro_read_guest(u32 dest,u32 size,u32 handle) noexcept {
    if(dest>current->data.size() || size>current->data.size()-dest ||
       handle>0xFFFFu || !current->host_files ||
       current->host_descriptor_flags.count(handle)==0u)
        stop("ORIGINAL_INTRO_DOS_GUEST_BUFFER_INVALID");
    // Original cinematic packet reads may exceed the 16-bit DOS CX word.
    // Preserve the original source 312EC/38106 wrapper for every chunk.
    for(u32 done=0;done<size;) {
        const u32 n=std::min<u32>(size-done,0xFFFFu);
        if(code2::function_312ec(current->dos_file_io,
                                 dest+done,n,handle)!=n)
            stop("ORIGINAL_INTRO_312EC_SHORT_PACKET");
        done+=n;
    }
}
void intro_read_local(void* dst,u32 size,u32 handle) noexcept {
    if(!dst || size>0xFFFFu || handle>0xFFFFu ||
       !current->host_files ||
       current->host_descriptor_flags.count(handle)==0u)
        stop("ORIGINAL_INTRO_DOS_LOCAL_BUFFER_INVALID");
    const auto r=current->host_files->read(
        static_cast<u16>(handle),static_cast<u8*>(dst),size);
    if(r.carry||r.ax!=size)stop("ORIGINAL_INTRO_DOS_LOCAL_SHORT_READ");
}
void intro_decode_text(u32 dst,u32 index) noexcept {
    code2::Application39Boundary b{};
    b.call_3131c=&original_level_source_seek;
    b.call_312ec_local=&intro_read_local;
    b.call_312ec_dgroup=&intro_read_guest;
    code2::function_1c368(current->machine,b,dst,index);
}
code2::Application54Boundary intro_vga() noexcept {
    code2::Application54Boundary b{};
    b.out16=+[](u16 port,u16 data) noexcept {
        if(!current->original_vga->write_indexed_port(port,data))
            stop("ORIGINAL_INTRO_VGA_INDEXED_PORT_UNSUPPORTED");
    };
    b.phys_read8=+[](u32 addr) noexcept -> u8 {
        if(addr<0xA0000u||addr>=0xB0000u)
            stop("ORIGINAL_INTRO_VGA_READ_OUTSIDE_APERTURE");
        return current->original_vga->read_aperture(
            static_cast<u16>(addr-0xA0000u));
    };
    b.phys_write8=+[](u32 addr,u8 value) noexcept {
        if(addr<0xA0000u||addr>=0xB0000u ||
           !current->original_vga->write_aperture(
               static_cast<u16>(addr-0xA0000u),value))
            stop("ORIGINAL_INTRO_VGA_WRITE_OUTSIDE_APERTURE");
    };
    return b;
}
void intro_raster_rle(u32 page) noexcept {
    if(page>1u)stop("ORIGINAL_INTRO_RLE_PAGE_INVALID");
    auto b=intro_vga();code2::function_27bd8(current->machine,b,page);
}
void intro_raster_sparse(u32 page) noexcept {
    if(page>1u)stop("ORIGINAL_INTRO_SPARSE_PAGE_INVALID");
    auto b=intro_vga();code2::function_27aec(current->machine,b,page);
}
void intro_raster_planes(u32 page) noexcept {
    if(page>1u)stop("ORIGINAL_INTRO_RAW_PAGE_INVALID");
    auto b=intro_vga();code2::function_27b84(current->machine,b,page);
}
void intro_page(u32 page) noexcept { original_291b0_source_page(page); }
void intro_scanout(u32 page) noexcept { original_scene_select_scanout(page); }
void intro_clear(u32 y,u32 x,u32 rows,u32 cols,u32 value) noexcept {
    try {
        rloft::source_video_boot::Bridge b(
            current->machine,*current->original_vga,current->data.size());
        b.run_original_29b40(y,x,rows,cols,value);
    } catch(const std::exception&) {stop("ORIGINAL_INTRO_VGA_CLEAR_UNSUPPORTED");}
}
void intro_copy_page(u32 page) noexcept {
    original_scene_copy_page(page);
}
void intro_palette_stream(u32 palette) noexcept {
    try {
        rloft::source_video_boot::Bridge b(
            current->machine,*current->original_vga,current->data.size());
        b.stream_original_29234(palette);
    } catch(const std::exception&) {stop("ORIGINAL_INTRO_VGA_DAC_UNSUPPORTED");}
}
void intro_color_bank(u32 dst,u32 start,u32 bank,u32 name,u32 count) noexcept {
    if(!current->host_files || dst!=0xC054u || start!=0u ||
       count!=0x100u || bank>=15u ||
       (name!=0xCF4u && name!=0x654u))
        stop("ORIGINAL_INTRO_COLORS_SOURCE_ABI_CHANGED");
    code2::Application81Boundary b{};
    b.app72=&current->dos_file_io;
    (void)code2::function_325b0(
        current->machine,b,dst,start,bank,name,count);
}
void intro_reset_pages() noexcept {
    try {
        rloft::source_video_boot::Bridge b(
            current->machine,*current->original_vga,current->data.size());
        b.run_original_20294();
    } catch(const std::exception&) {stop("ORIGINAL_INTRO_20294_SOURCE_VGA_FAILED");}
}
void intro_audio_stop(u32 id) noexcept {
    intro_require_no_active_sound();
    code2::function_28dcc(current->machine,current->audio_seed,id);
}
void intro_audio_wait() noexcept {
    intro_require_no_active_sound();
    code2::function_28ef0(current->machine,current->audio_seed);
}
void intro_audio_volume(u32 value) noexcept {
    intro_require_no_active_sound();
    code2::function_29008(current->machine,current->audio_seed,value);
}
void intro_audio_rate(u32 value) noexcept {
    intro_require_no_active_sound();
    code2::function_28f58(current->machine,current->audio_seed,value);
}
void intro_audio_play(code2::s32 id,code2::s32 slot,u32 aux) noexcept {
    intro_require_no_active_sound();
    code2::function_28e58(current->machine,current->audio_seed,
                          static_cast<u32>(id),static_cast<u32>(slot),aux);
}
void intro_audio_queue(u32 id,u32 aux,u32 slot,u32 priority) noexcept {
    intro_require_no_active_sound();
    code2::function_28110(current->machine,id,aux,slot,priority);
}
void intro_audio_service() noexcept {
    intro_require_no_active_sound();
    code2::Application55Boundary b{};
    code2::function_28168(current->machine,b);
}
void intro_sound_event(code2::s32) noexcept {
    // Original 146B8 dispatches device-dependent 27F24. This branch is
    // gated by the same absent-device source state as 28E58/28168.
    intro_require_no_active_sound();
}
void intro_state_push(u32 value) noexcept {
    code2::function_19ccc(current->machine,value);
}
void intro_state_pop() noexcept {code2::function_19cf4(current->machine);}
void intro_original_cursor_restore() noexcept {
    code2::function_1aacc(
        current->machine,current->original_mouse_draw_callbacks);
}
void intro_no_title_input() noexcept {
    stop("ORIGINAL_INTRO_TITLE_MOUSE_INPUT_PENDING");
}
u32 intro_no_title_click() noexcept {
    stop("ORIGINAL_INTRO_TITLE_CLICK_INPUT_PENDING");
}
u32 intro_not_yet_new_game() noexcept {
    stop("ORIGINAL_INTRO_NEW_GAME_07B94_NOT_CONNECTED");
}
void original_19888_live_intro() noexcept {
    if(!current || !current->live_continuation || !current->host_files ||
       !current->original_vga || current->original_16d04_completed!=1u ||
       current->original_03838_completed!=1u ||
       current->original_31218_checks!=1u ||
       current->original_16cd0_completed!=0u ||
       !current->host_files->has_readonly_media())
        stop("ORIGINAL_19888_RETAINED_SOURCE_PREREQUISITES_MISSING");
    intro_require_no_active_sound();
    if(current->code.size()<0x131DBu ||
       std::memcmp(current->code.data()+0x131D0u,"CINE00.ANM",11u)!=0)
        stop("ORIGINAL_INTRO_CODE2_CINE_FILENAME_TEMPLATE_MISMATCH");
    code2::Application25Boundary packets{};
    packets.read_312ec_value=&intro_read_scalar;
    packets.call_312ec=&intro_read_guest;
    packets.call_3131c=&original_level_source_seek;
    packets.call_291b0=&intro_page;
    packets.call_291c8=&intro_scanout;
    packets.call_27bd8=&intro_raster_rle;
    packets.call_27aec=&intro_raster_sparse;
    packets.call_27b84=&intro_raster_planes;
    packets.call_29234=&intro_palette_stream;
    packets.call_29b40=&intro_clear;
    packets.call_2a020=&intro_copy_page;
    packets.call_30d30=&original_checked_memset;
    packets.call_310b0=&original_level_heap_release;
    packets.call_1c368=&intro_decode_text;
    packets.call_28dcc=&intro_audio_stop;
    packets.call_28ef0=&intro_audio_wait;
    packets.call_29008=&intro_audio_volume;
    packets.call_28f58=&intro_audio_rate;
    packets.call_28e58=&intro_audio_play;
    packets.call_28110=&intro_audio_queue;
    packets.call_28168=&intro_audio_service;
    packets.call_146b8=&intro_sound_event;
    packets.call_19ccc=&intro_state_push;
    packets.call_19cf4=&intro_state_pop;
    packets.call_2e608=&original_game_over_2e608_timing;
    packets.call_2f868=&intro_pump;
    packets.call_2e614=&intro_wait;
    packets.call_30700=&intro_tick;
    packets.call_30b40=&intro_keyboard;
    code2::Application26Boundary movie{};
    movie.app25=&packets;
    movie.read_cs8=+[](u32 offset) noexcept -> u8 {
        if(offset>=current->code.size())
            stop("ORIGINAL_INTRO_CS_FILENAME_OUT_OF_GUEST");
        return current->code[offset];
    };
    movie.call_312a0=+[](u32 name) noexcept -> code2::s32 {
        return static_cast<code2::s32>(original_boot_file_open(name));
    };
    movie.call_3104c=&original_boot_heap_alloc;
    movie.call_312dc=&original_boot_file_close;
    movie.call_325b0=&intro_color_bank;
    movie.call_29048=+[]() noexcept -> u32 {
        return code2::function_29048(current->machine);
    };
    movie.call_29050=+[]() noexcept -> code2::s32 {
        return static_cast<code2::s32>(
            code2::function_29050(current->machine));
    };
    movie.call_29098=+[]() noexcept -> u32 {
        return code2::function_29098(current->machine);
    };
    movie.call_290a0=+[]() noexcept -> u32 {
        return code2::function_290a0(current->machine);
    };
    movie.call_1aacc=&intro_original_cursor_restore;
    movie.call_20294=&intro_reset_pages;
    code2::Application33Boundary menu{};
    menu.app26=&movie;
    menu.call_30d30=&original_checked_memset;
    menu.call_29234=&intro_palette_stream;
    menu.call_325b0=&intro_color_bank;
    menu.call_291b0=&intro_page;
    menu.call_291c8=&intro_scanout;
    menu.call_20294=&intro_reset_pages;
    menu.call_2e510=&original_scene_frame_base;
    menu.call_2e614=&intro_wait;
    menu.call_30700=&intro_tick;
    menu.call_30b40=&intro_keyboard;
    menu.call_1a1f4=&intro_no_title_click;
    menu.call_7b94=&intro_not_yet_new_game;
    menu.call_1a0b0=&intro_no_title_input;
    code2::Application34Boundary frontend{};
    frontend.app33=&menu;
    frontend.call_2f844=+[]() noexcept {
        code2::function_2f844(current->machine);
    };
    frontend.call_28e58=&intro_audio_play;
    std::cerr<<"CODE2_RETAINED_19888_SOURCE=ENTER"
             <<" memory=SAME_HEAP VGA=SAME_DEVICE"
             <<" media=READ_ONLY_CD\n"<<std::flush;
    code2::function_19888(current->machine,frontend);
    stop("ORIGINAL_19888_RETURN_UNEXPECTED_WITHOUT_TITLE_HANDOFF");
}
