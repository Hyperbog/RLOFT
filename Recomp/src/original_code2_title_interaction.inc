// Original 19464 title mouse bridge. Same CODE.2 session; 19D18 owns
// pointer handling, guest data, and source menu outcomes.
// Native input reaches original INT33 fn03 through retained 1A1F4.
// The host invokes reconstructed 27894 on the same CODE.2 worker after
// observing a change; this does NOT certify asynchronous DOS fn0C IRQ timing.
// Original 27894 invokes source 19D18; no host menu behavior is substituted.
// Host only binds original source 19D18 arguments. The canonical CODE.2
// 27894 ISR, not a direct UI-side call, invokes this registered callback.
u32 intro_title_source_19d18(u32 flags,u32 buttons,u32 arg3,
                              u32 arg4,u32 sample_ptr) noexcept {
    auto& m=current->machine;
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
    code2::function_19d18(m,mouse,flags,buttons,arg3,arg4,sample_ptr);
    return 0u;
}
void intro_title_mouse_event() noexcept {
    if(!current->original_vga ||
       current->original_20440_mouse_inits!=1u ||
       current->original_20440_mouse_callback_installs!=1u)
        stop("ORIGINAL_19464_27894_SOURCE_NOT_REGISTERED");
    rloft::ui::title_interaction::MouseEvent sample{};
    if(!rloft::ui::title_interaction::read_source_mouse_event(
           current->data.data(),current->data.size(),sample))
        stop("ORIGINAL_19464_TITLE_MOUSE_GUEST_SNAPSHOT_OUT_OF_BOUNDS");
    const auto flags=current->original_title_dispatch.event_mask(sample);
    if(flags==0u)return;
    if(current->data[0xD910u]!=0u)
        stop("ORIGINAL_19464_27894_NESTED_GUEST_ISR");
    // Original 27894 consumes EAX=event reason, EBX=buttons,
    // ECX=source half-X*2, EDX=Y, ESI=real-mode pointer segment.
    // The no-aux-data fn0C title event uses a zero ESI segment.
    auto& machine=current->machine;
    const code2::CpuState before=machine.cpu;
    machine.cpu.eax=flags;
    machine.cpu.ebx=sample.buttons;
    machine.cpu.ecx=sample.dos_x_doubled;
    machine.cpu.edx=sample.dos_y;
    machine.cpu.esi=0u;
    auto v=intro_vga();
    v.in8=+[](u16 port) noexcept -> u8 {
        if(port!=0x3C4u && port!=0x3C5u &&
           port!=0x3CEu && port!=0x3CFu)
            stop("ORIGINAL_19464_27894_VGA_PORT_READ_UNSUPPORTED");
        return current->original_vga->read_port8(port);
    };
    v.out8=+[](u16 port,u8 value) noexcept {
        if(!current->original_vga->write_port8(port,value))
            stop("ORIGINAL_19464_27894_VGA_PORT_WRITE_UNSUPPORTED");
    };
    v.call_19d18=&intro_title_source_19d18;
    code2::function_27894(machine,v);
    machine.cpu=before;
    if(current->data[0xD910u]!=0u)
        stop("ORIGINAL_19464_27894_SOURCE_STACK_NOT_RESTORED");
    ++current->original_title_source_27894_events;
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
