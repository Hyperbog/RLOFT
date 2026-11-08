// Original 19464 title mouse bridge. Same CODE.2 session; 19D18 owns
// pointer handling, guest data, and source menu outcomes.
// A real DOS INT33 callback drives original 19D18 asynchronously.
// The native host samples INT33 through the retained source 1A1F4
// boundary and dispatches the ORIGINAL 19D18 body on this same worker.
// It is an explicit host-mouse adaptation, not fabricated title motion.
void intro_title_mouse_event() noexcept {
    auto& m=current->machine;
    rloft::ui::title_interaction::MouseEvent event{};
    if(!rloft::ui::title_interaction::read_source_mouse_event(
           current->data.data(),current->data.size(),event))
        stop("ORIGINAL_19464_TITLE_MOUSE_GUEST_SNAPSHOT_OUT_OF_BOUNDS");
    code2::Application33Boundary video=current->original_mouse_resources;
    video.call_30700=&intro_tick;
    video.call_1aacc=&original_game_over_1aacc_cursor_restore;
    video.call_1a288=&intro_title_redraw;
    code2::Application34Boundary mouse=current->original_mouse_init_callbacks;
    mouse.app33=&video;
    mouse.call_2e34c=+[]() noexcept -> u32 {
        return code2::function_2e34c(current->machine);
    };
    mouse.call_1f820=+[]() noexcept {
        code2::function_1f820(current->machine);
    };
    mouse.call_29738=+[](u32 p) noexcept {
        code2::function_29738(current->machine,p);
    };
    mouse.call_296c0=+[](u32 y,u32 x,u32 h,u32 w) noexcept {
        code2::function_296c0(current->machine,
            static_cast<code2::s32>(y),static_cast<code2::s32>(x),
            static_cast<code2::s32>(h),static_cast<code2::s32>(w));
    };
    code2::function_19d18(m,mouse,event.event,event.buttons,
                          event.dos_x_doubled,event.dos_y,event.reserved);
}
u32 intro_title_click() noexcept {
    const auto pressed=original_game_over_1a1f4_mouse_button();
    ++current->original_title_mouse_polls;
    intro_title_mouse_event();
    if(!rloft::ui::title_interaction::source_title_presented(
           current->original_title_presentations))
        stop("ORIGINAL_19464_TITLE_CLICK_BEFORE_SOURCE_FRAME");
    if(const char* gate=std::getenv("RLOFT_RECOMP_TITLE_RENDER_INPUT_GATE");
       gate && std::strcmp(gate,"1")==0) {
        std::cerr<<"CODE2_ORIGINAL_19464_RENDER_AND_INPUT_PASS"
                 <<" original_menu=192CC_19464"
                 <<" source_click_poll=1A1F4"
                 <<" title_page_flips="<<current->original_title_presentations
                 <<" mouse_buttons="<<pressed
                 <<" original_guest=SAME\n"<<std::flush;
        stop("ORIGINAL_19464_TITLE_RENDER_AND_MOUSE_POLL_VERIFIED");
    }
    return pressed;
}
