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
void intro_publish_original_vga() noexcept {
    if(current->original_intro_frame_observer) {
        if(!current->original_vga ||
           !current->original_intro_frame_observer(
               current->original_intro_frame_context,*current->original_vga))
            stop("ORIGINAL_INTRO_NATIVE_WINDOW_CLOSED");
    }
}
// Original 30708 calls 19C80 with the *pre-increment* source tick.
// The existing original mouse graph supplies 19C80's timer-side state.
void intro_irq8_dispatch(u32) noexcept {
    if(current->data[0xA79Cu]!=0u)
        stop("ORIGINAL_CINE_IRQ8_19C80_PENDING_SOUND_EVENT");
    auto mouse=current->original_mouse_init_callbacks;
    auto video=current->original_mouse_resources;
    video.call_30700=&intro_tick;
    mouse.app33=&video;
    code2::function_19c80(current->machine,mouse);
}
// Original 30BB4 saved the host fixture's prior IRQ8 vector. The host
// acknowledges it explicitly; this is NOT execution of a real DOS BIOS ISR.
void intro_irq8_prior_bios(u32 offset,u16 selector) noexcept {
    if(offset!=0x9008u || selector!=0x20u ||
       code2::load_le32(current->data.data()+0xF582u)!=offset ||
       code2::load_le16(current->data.data()+0xF586u)!=selector)
        stop("ORIGINAL_CINE_IRQ8_PREVIOUS_VECTOR_UNKNOWN");
    ++current->original_intro_bios_chain_calls;
}
void intro_arm_irq8() noexcept {
    if(current->source_int08_scheduler.armed() ||
       current->keyboard_irq.call_19c80 ||
       current->keyboard_irq.far_chain ||
       !current->live_continuation ||
       current->data[0xF575u]!=1u)
        stop("ORIGINAL_CINE_IRQ8_SETUP_CHANGED");
    current->keyboard_irq.call_19c80=&intro_irq8_dispatch;
    current->keyboard_irq.far_chain=&intro_irq8_prior_bios;
    const auto contract=rloft::source_int08::validate(
        current->machine,current->data.size(),current->ivt,
        current->keyboard_irq);
    if(!contract.ready)stop(contract.reason);
    if(!current->source_int08_scheduler.arm_bios_pit(
        rloft::original_irq8::ElapsedScheduler::Clock::now(),8192u))
        stop("ORIGINAL_CINE_IRQ8_PIT_RATE_INVALID");
    std::cerr<<"CODE2_RETAINED_INT08_SOURCE=ARMED"
             <<" source_isr=30708 source_tick=F58C"
             <<" upstream_vector=HOST_FIXTURE_9008_20"
             <<" pit_hz=1193182 divisor=65536"
             <<" dos_bios_parity=NOT_CLAIMED\n"<<std::flush;
}
void intro_pump() noexcept {
    original_scene_poll_source_events();
    if(current->source_int08_scheduler.armed())std::this_thread::yield();
}
void intro_wait(u32 delay,u32 flags) noexcept {
    original_game_over_2e614_timing(delay,flags);
    intro_publish_original_vga();
    ++current->original_intro_frame_waits;
    if(code2::load_le32(current->data.data()+0x7F78u)==0u)
        ++current->original_intro_cine00_frames;
    if((current->original_intro_frame_waits%50u)==0u)
        std::cerr<<"CODE2_RETAINED_CINE_ORIGINAL_FRAMES="
                 <<current->original_intro_frame_waits
                 <<" mode="<<code2::load_le32(current->data.data()+0x7F78u)
                 <<" source_irq8="
                 <<current->source_int08_scheduler.delivered()
                 <<" old_vector_ack="
                 <<current->original_intro_bios_chain_calls
                 <<"\n"<<std::flush;
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
void intro_text_seek(u32 offset,u32 origin,u32 handle) noexcept {
    (void)original_level_source_seek(offset,origin,handle);
}
void intro_decode_text(u32 dst,u32 index) noexcept {
    code2::Application39Boundary b{};
    b.call_3131c=&intro_text_seek;
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
       (name!=0xCF4u && name!=0x654u && name!=0xCFCu))
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
// Original 19464 is reached before 20440's gameplay cursor initialization.
// Run the SAME 1A0B0, 1A288, 1A510, 1A9F8 and 1A1F4 mouse code as
// the existing retained gameplay graph; never substitute a title bitmap.
void intro_title_mouse_init() noexcept {
    if(current->original_intro_cine00_frames!=405u ||
       current->original_intro_bios_chain_calls==0u ||
       current->original_intro_bios_chain_calls!=
           current->source_int08_scheduler.delivered() ||
       current->original_20440_mouse_inits!=0u)
        stop("ORIGINAL_INTRO_19464_BEFORE_COMPLETE_CINE00");
    std::cerr<<"CODE2_LIVE_20440_CINE00_TO_19464_SOURCE_PASS"
             <<" cine00_frames="<<current->original_intro_cine00_frames
             <<" total_film_frames="<<current->original_intro_frame_waits
             <<" source_irq8="<<current->source_int08_scheduler.delivered()
             <<" original_guest=SAME"
             <<" DOS_BIOS_chain=HOST_FIXTURE\n"<<std::flush;
    original_20440_mouse_init();
}
void intro_title_copy(u32 dst,u32 src,u32 count) noexcept {
    if(!count || std::uint64_t(dst)+count>current->data.size() ||
       std::uint64_t(src)+count>current->data.size())
        stop("ORIGINAL_19464_SOURCE_COPY_OUTSIDE_GUEST");
    (void)code2::function_30cd4(current->machine,dst,src,count);
}
void intro_title_remap(u32 frame,u32 table,u32 image) noexcept {
    if(frame!=0u || table!=0x759Cu || image!=0x880Du ||
       current->original_20440_mouse_cursor_loads!=1u)
        stop("ORIGINAL_19378_CURSOR_PALETTE_TABLE_INVALID");
    code2::function_2aa9c(current->machine,frame,table,image);
}
void intro_title_text(u32 y,u32 x,u32 text,u32 font) noexcept {
    if(y!=0u || x!=0u || text!=0x82C0u || font!=1u)
        stop("ORIGINAL_192CC_TITLE_TEXT_ARGUMENTS_CHANGED");
    code2::Application71Boundary wrapper{};
    wrapper.call_303c8=+[](u32 ptr,u32 item) noexcept {
        if(!current->game_audio.app68)
            stop("ORIGINAL_19464_TITLE_FONT_RESOURCE_OWNER_MISSING");
        const u32 font_ptr=code2::function_2f29c(
            current->machine,current->game_audio,item,0u);
        const u32 image_ptr=(font_ptr && std::uint64_t(font_ptr)+8u<=current->data.size())
            ?code2::load_le32(current->data.data()+font_ptr):0u;
        const u32 table=code2::load_le32(current->data.data()+0xF0B8u);
        const u32 records=table?code2::load_le32(current->data.data()+table+12u):0u;
        const u32 record=records+9u*item;
        std::cerr<<"CODE2_ORIGINAL_19464_FONT_RESOURCE_DIAG"
                 <<" font_id="<<item
                 <<" font_ptr="<<font_ptr
                 <<" linked_image="<<image_ptr
                 <<" record_type="<<unsigned(current->data[record])
                 <<" record_loaded="<<unsigned(current->data[record+2u])
                 <<" record_pointer="<<code2::load_le32(current->data.data()+record+5u)
                 <<" font_first8=";
        if(font_ptr && std::uint64_t(font_ptr)+8u<=current->data.size())
            for(u32 i=0;i<8u;++i)
                std::cerr<<unsigned(current->data[font_ptr+i])<<",";
        std::cerr<<"\n"<<std::flush;
        if(!font_ptr || !image_ptr ||
           std::uint64_t(image_ptr)+8u>current->data.size() ||
           !code2::load_le16(current->data.data()+image_ptr+2u) ||
           !code2::load_le16(current->data.data()+image_ptr+4u))
            stop("ORIGINAL_19464_FONT_TYPE2_301A8_SOURCE_DECODE_MISSING");
        code2::Application56Boundary glyphs{};
        glyphs.call_2a380=+[](u32 frame,u32 row,u32 column,u32 image) noexcept -> u32 {
            original_scene_res_blit(frame,row,column,image);
            return 0u;
        };
        code2::Application70Boundary b{};
        b.app56=&glyphs;
        b.app69=&current->game_audio;
        code2::function_303c8(current->machine,b,ptr,item);
    };
    code2::function_3060c(current->machine,wrapper,y,x,text,font);
}
void intro_title_page() noexcept {
    code2::Application43Boundary title{};
    title.call_1a510=+[](u32 page) noexcept {
        code2::Application33Boundary v{};
        code2::Application34Boundary mouse{};
        auto draw=original_game_over_cursor_callbacks(v,mouse);
        code2::function_1a510(current->machine,draw,page);
    };
    code2::function_1ea78(current->machine,title);
    ++current->original_title_presentations;
    intro_publish_original_vga();
    std::cerr<<"CODE2_ORIGINAL_19464_SOURCE_TITLE_PRESENT"
             <<" page_flips="<<current->original_title_presentations
             <<" guest_vga=RETAINED"
             <<" source_192CC_ART=YES"
             <<" source_3060C_TEXT=YES\n"<<std::flush;
}
void intro_title_redraw() noexcept {
    original_game_over_1a288_cursor_redraw();
    intro_publish_original_vga();
}
void intro_title_reset() noexcept {
    original_game_over_1a9f8_mouse_reset();
}
void intro_title_fade(bool fade_in,u32 source,u32 first,
                      u32 last,u32 red,u32 green,u32 blue) noexcept {
    code2::Application56Boundary b{};
    b.call_29ca0=+[](u32 palette,u32 start,u32 count) noexcept -> u32 {
        if(start!=0u||count!=0x300u)
            stop("ORIGINAL_19464_FADE_DAC_SLICE_CHANGED");
        intro_palette_stream(palette);
        return 0u;
    };
    if(fade_in)code2::function_322ec(current->machine,b,source,
        static_cast<code2::s32>(first),static_cast<code2::s32>(last),red,green,blue);
    else code2::function_32220(current->machine,b,source,
        static_cast<code2::s32>(first),static_cast<code2::s32>(last),red,green,blue);
}
#include "original_code2_title_interaction.inc"
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
    intro_arm_irq8();
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
    menu.call_2aa9c=&intro_title_remap;
    menu.call_2f29c=&original_scene_res_image;
    menu.call_30cd4=&intro_title_copy;
    menu.call_1aacc=&original_game_over_1aacc_cursor_restore;
    menu.call_1a288=&intro_title_redraw;
    menu.call_3060c=&intro_title_text;
    menu.call_1ea78=&intro_title_page;
    menu.call_1a9f8=&intro_title_reset;
    menu.call_1a1f4=&intro_title_click;
    menu.call_7b94=&intro_not_yet_new_game;
    menu.call_1a0b0=&intro_title_mouse_init;
    menu.call_32220=+[](u32 p,u32 first,u32 last,u32 r,u32 g,u32 b) noexcept {
        intro_title_fade(false,p,first,last,r,g,b);
    };
    menu.call_322ec=+[](u32 p,u32 first,u32 last,u32 r,u32 g,u32 b) noexcept {
        intro_title_fade(true,p,first,last,r,g,b);
    };
    menu.call_205b4=+[](code2::s32) noexcept {
        stop("ORIGINAL_19464_SUPERVISOR_TRANSITION_NOT_CONNECTED");
    };
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
