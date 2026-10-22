// Retained CODE.2 Level 22 pre-transition HUD diagnostic.
// Executes compiled original 1F494 and 1EA90 on the live guest and VGA.
// NOT a substitute for the missing 1F8FC mode-four compositor.
void original_live_hud_rect(u32 row,u32 col,u32 rows,u32 cols,u32 color) noexcept {
    if(!current->original_vga || row>=400u || col>=320u ||
       rows==0u || cols==0u || rows>400u-row || cols>320u-col ||
       color>255u)
        stop("ORIGINAL_LIVE_1F494_RECTANGLE_OUT_OF_BOUNDS");
    try {
        rloft::source_video_boot::Bridge bridge(
            current->machine,*current->original_vga,current->data.size());
        bridge.run_original_29b40(row,col,rows,cols,color);
    }catch(const std::exception&) {
        stop("ORIGINAL_LIVE_1F494_29B40_PLANAR_FILL_FAILED");
    }
}
void original_live_hud_vertical(u32 row,u32 col,u32 height,u32 color) noexcept {
    original_live_hud_rect(row,col,height,1u,color);
}
void original_live_hud_horizontal(u32 row,u32 col,u32 width,u32 color) noexcept {
    original_live_hud_rect(row,col,1u,width,color);
}
void original_live_hud_border(u32 row,u32 col,u32 rows,u32 cols,u32 color) noexcept {
    code2::Application67Boundary b{};
    b.call_291e0=&original_live_hud_vertical;
    b.call_29208=&original_live_hud_horizontal;
    code2::function_2e374(b,row,col,rows,cols,color);
}
u32 original_live_hud_clock() noexcept {
    return code2::function_19cc4(current->machine);
}
u32 original_live_hud_item(u32 actor,u32 hand) noexcept {
    return code2::function_24478(
        current->machine,static_cast<code2::s32>(actor),hand);
}
u32 original_live_hud_hp(u32 slot) noexcept {
    return static_cast<u32>(code2::function_0ef14(current->machine,slot));
}
void original_live_hud_2a258(u32 frame,u32 image) noexcept {
    if(image)code2::function_2a258(current->machine,frame,image);
}
void original_live_hud_blit(u32 frame,u32 row,u32 col,u32 image) noexcept {
    original_scene_res_blit(frame,row,col,image);
}
void original_live_hud_2e4a4(
    u32 row,u32 col,u32 start,u32 count,u32 image) noexcept {
    if(!image)stop("ORIGINAL_LIVE_1F494_2E4A4_IMAGE_MISSING");
    code2::Application67Boundary b{};
    b.call_2939c=&original_live_hud_blit;
    code2::function_2e4a4(
        current->machine,b,row,col,start,count,image);
}
void original_live_hud_22f40(u32 slot,u32 state) noexcept {
    code2::Application50Boundary b{};
    b.call_298f0=&original_scene_hud_298f0;
    code2::function_22f40(
        current->machine,b,static_cast<code2::s32>(slot),state);
}
void original_live_hud_group(u32,u32) noexcept {
    stop("ORIGINAL_LIVE_1F494_23024_GROUP_UI_NOT_CONNECTED");
}
u32 original_live_hud_text_number(u32) noexcept {
    stop("ORIGINAL_LIVE_1F494_2E6F0_TEXT_FORMAT_NOT_CONNECTED");
}
void original_live_hud_text(u32,u32,u32,u32,u32) noexcept {
    stop("ORIGINAL_LIVE_1F494_30508_FONT_NOT_CONNECTED");
}
void original_live_hud_divide_error() noexcept {
    stop("ORIGINAL_LIVE_1F494_SOURCE_DIVIDE_FAULT");
}
// Diagnostics operate on authentic post-181B4 state. The pending mode
// transition would set all party dirty flags in 1F8FC, but it is not yet
// safe to enter that routine. Force exactly this redraw precondition in
// the diagnostic, then restore the original flags after the source pass.
void original_live_1f494_party_probe() noexcept {
    if(current->original_181b4_completed!=1u ||
       !current->live_continuation || !current->game_audio.app68 ||
       !current->original_vga ||
       code2::load_le32(current->data.data()+0xC034u)!=3u)
        stop("ORIGINAL_LIVE_1F494_PRETRANSITION_STATE_INVALID");
    const auto read_draw_page=[]() {
        auto scan=*current->original_vga;
        if(!scan.write_indexed_port(0x3D4u,0x800Cu) ||
           !scan.write_indexed_port(0x3D4u,0x000Du) ||
           scan.crtc_start()!=0x8000u)
            stop("ORIGINAL_LIVE_1F494_DRAW_PAGE_UNAVAILABLE");
        return scan.scanout_indices();
    };
    const auto before=read_draw_page();
    std::array<u8,4> dirty{};
    for(u32 i=0u;i<4u;++i) {
        dirty[i]=current->data[0xCF5Au+i];
        current->data[0xCF5Au+i]=1u;
    }
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
    ui.divide_error=&original_live_hud_divide_error;
    code2::Application44Boundary hud{};
    hud.app43=&ui;
    hud.call_291b0=&original_291b0_source_page;
    hud.call_ef14=&original_live_hud_hp;
    hud.call_2e510=&original_scene_frame_base;
    hud.call_2e4a4=&original_live_hud_2e4a4;
    hud.call_22f40=&original_live_hud_22f40;
    hud.call_23024=&original_live_hud_group;
    hud.divide_error=&original_live_hud_divide_error;
    code2::function_1f494(current->machine,hud,0u);
    for(u32 i=0u;i<4u;++i) {
        if(current->data[0xCF5Au+i]!=0u)
            stop("ORIGINAL_LIVE_1F494_SOURCE_DIRTY_BIT_NOT_CLEARED");
        current->data[0xCF5Au+i]=dirty[i];
    }
    const auto after=read_draw_page();
    unsigned changed=0u;
    for(u32 y=0x138u;y<400u;++y)
        for(u32 x=0u;x<320u;++x)
            changed+=before[y*320u+x]!=after[y*320u+x];
    if(!changed)
        stop("ORIGINAL_LIVE_1F494_SOURCE_HUD_NO_VGA_PIXEL_CHANGE");
    std::cerr<<"CODE2_ORIGINAL_LIVE_BOOT=1F494_1EA90_RETAINED_HUD_PROBE"
             <<" original_source=1F494_1EA90_22F40_298F0_2975C"
             <<" normal_party=2 terminal_party=2"
             <<" changed_draw_page_pixels="<<changed
             <<" mode=3_PRETRANSITION_ONLY"
             <<" frame_status=HUD_ONLY_NOT_ORIGINAL_1F8FC\\n"
             <<std::flush;
}
