// Original CODE.2 VGA/presentation callback tables.
// Compiles inside attach_original_vga on the only original CODE.2 guest.
// Retained source handlers, experimental gating and registration order unchanged.
    source.presentation.call_2e590=&original_2e590_presentation_reset;
    source.presentation.call_291b0=&original_291b0_source_page;
    source.presentation.call_29b40=&original_29b40_source_rectangle;
    source.presentation.call_2e5d4=&original_2e5d4_source_page_pop;
    source.presentation.call_30b40=&original_30b40_keyboard_poll;
    source.game.call_5e38=&original_5e38_boot_options;
    source.game.call_28f58=&original_28f58_volume;
    source.game.call_31218=&original_31218_chars_probe;
    source.game.call_312a0=&original_312a0_next_dos_open;
    source.game.call_16d04=&original_16d04_after_dos_open;
    source.game.call_3838=&original_03838_sintab_load;
    source.game.call_16cd0=&original_16cd0_chars_load;
    source.game.call_19888=&original_19888_live_intro;
    source.game.call_325b0=&original_325b0_live_palette;
    source.game.call_291b0=&original_291b0_boot_page;
    source.game.call_291c8=&original_291c8_boot_scanout;
    source.game.call_2ecb4=&original_20440_res0_image_load;
    source.game.call_2f184=&original_20440_res0_image_retain;
    source.game.call_1a0b0=&original_20440_mouse_init;
    source.game.call_1a288=&original_20440_mouse_render;
    source.original_mouse_resources.call_2f29c=&original_20440_mouse_image;
    source.original_mouse_resources.call_30cd4=&original_20440_mouse_copy;
    source.original_mouse_init_callbacks.app33=&source.original_mouse_resources;
    source.original_mouse_init_callbacks.call_2e340=&original_20440_mouse_probe;
    source.original_mouse_init_callbacks.call_33d38=&original_20440_mouse_set_y;
    source.original_mouse_init_callbacks.call_33d18=&original_20440_mouse_set_x;
    source.original_mouse_init_callbacks.call_33c90=&original_20440_mouse_set_position;
    source.original_mouse_init_callbacks.call_33cb0=&original_20440_mouse_get_position;
    source.original_mouse_init_callbacks.call_296c0=&original_20440_mouse_clip;
    source.original_mouse_init_callbacks.call_33a7c=&original_20440_mouse_irq;
    source.original_mouse_draw_callbacks.app34=&source.original_mouse_init_callbacks;
    source.original_mouse_draw_callbacks.call_2e590=&original_20440_mouse_page_push;
    source.original_mouse_draw_callbacks.call_2e5d4=&original_20440_mouse_page_pop;
    source.original_mouse_draw_callbacks.call_292bc=&original_20440_mouse_capture;
    source.original_mouse_draw_callbacks.call_2ae30=&original_20440_mouse_hit;
    source.original_mouse_draw_callbacks.call_29804=&original_20440_mouse_draw;
    source.game.call_2f844=&original_2f844_source_enable;
    source.game.call_181b4=&original_181b4_level_load;
    source.presentation.call_2e510=&original_scene_frame_base;
    source.presentation.call_2efd0=&original_scene_res_release;
    source.presentation.call_30070=&original_scene_capture;
    source.presentation.call_30040=&original_scene_capture_release;
    source.presentation.call_325b0=&original_scene_palette;
    source.presentation.call_29234=&original_video_palette_29234;
    source.presentation.call_22d70=&original_scene_clear_event_ring;
    source.presentation.call_2f868=&original_scene_poll_source_events;
    source.presentation.call_30b2c=&original_scene_finish_keyboard;
    // Opt-in actual original motion integration. Not a host camera
    // assignment or an IRQ9 timing assertion; unsupported collision
    // events/level transitions are hard stops until source-attested.
    if(const char* motion=std::getenv("RLOFT_RECOMP_EXPERIMENTAL_1FC14_MOVEMENT_SOURCE");
       motion && std::strcmp(motion,"1")==0)
        source.presentation.call_2e120=&original_full_2e120_movement_service;
    if(const char* live=std::getenv("RLOFT_RECOMP_EXPERIMENTAL_1FC14_SCENE_SOURCE");
       live && std::strcmp(live,"1")==0)
        source.presentation.call_28168=&original_full_28168_audio_service;
    source.presentation.call_1a5b0=&original_scene_1a5b0;
    // Independent experimental source 1FC14 tick: the 29804 cursor clip
    // result is not yet accepted. Keep the validated default loop intact.
    // This flag enables diagnostic execution without claiming a drawn frame.
    if(const char* trial=std::getenv("RLOFT_RECOMP_EXPERIMENTAL_1FC14_PAGE_HANDOFF")) {
        if(std::strcmp(trial,"1")==0)
            source.presentation.call_1a510=&original_1fc14_source_page_handoff;
    }
    if(const char* live=std::getenv("RLOFT_RECOMP_EXPERIMENTAL_1FC14_SCENE_SOURCE");
       live && std::strcmp(live,"1")==0) {
        const char* page=std::getenv("RLOFT_RECOMP_EXPERIMENTAL_1FC14_PAGE_HANDOFF");
        if(!page || std::strcmp(page,"1")!=0)
            stop("ORIGINAL_1FC14_SCENE_REQUIRES_ORIGINAL_1A510");
        original_full_attach_1fc14_scene(source);
    }
    // Party RES0/RES1 source decoders and portrait blitters are attached
    // independently of enabling the incomplete full 1F8FC frame.
    source.original_mouse_resources.call_2975c=&original_scene_hud_2975c;
    source.game.app45=&source.presentation;
