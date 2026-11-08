// CODE.2 owner-side callback registration.
// This source fragment must compile INSIDE code2_startup_gate.cpp:
// all callbacks reference the same original CODE.2 Machine and Session.
// No callback dispatch path, branch order, or heap ownership is replaced.
    state.entry.tail_35418=&invoke_startup;
    // The canonical 352EE implementation delegates case-folded comparison
    // to the real canonical 39526, not a host std::getenv/strtol substitute.
    state.entry.call_39526=&code2::function_39526;
    state.game.call_352ee=&original_getenv;
    state.game.call_35349=&original_atoi;
    state.game.call_2f2ec=&original_audio_init;
    state.game.call_35286=&original_disk_free_from_35286;
    if(stackful_worker) {
        // Explicitly represent source 20440 dependencies; the older
        // null callbacks silently skipped 2F468/290F4 and made 20608
        // return without ever reaching its 1FC14 game loop.
        state.game.call_2f468=&original_source_resource_selection;
        state.game.call_290f4=&original_video_mode_290f4;
        state.game.call_30d30=&original_checked_memset;
        state.game.call_29234=&original_video_palette_29234;
    }
    state.entry.int21_36=&stop_at_dos_disk_free;
    state.game_audio.call_375fe_far=&original_far_install;
    state.dos_irq.install_interrupt=&original_vector_install;
    state.keyboard_irq.get_vector=&guest_get_vector;
    state.keyboard_irq.set_vector=&guest_set_vector;
    state.audio_seed.call_30cc4=&original_random_seed;
    state.game_audio.call_30bb4=&original_keyboard_vectors;
    state.game_audio.app56=&state.audio_seed;
    state.game_audio.call_3104c=&original_checked_allocation;
    state.game_audio.call_30d30=&original_checked_memset;
    state.game_audio.call_312a0=&original_sound_file;
    state.game_audio.call_35349=&original_sound_pointer_from_argv;
    state.dos_file_io.call_37b34=&original_checked_allocation_header;
    state.dos_file_io.call_379d0=&original_linear_dword_fill;
    state.dos_file_io.call_37e77=&original_checked_source_open;
    state.dos_file_open.read8=&original_dos_open_guest_read8;
    state.dos_file_open.dos_open=&stop_at_real_dos_ah3d_open;
    state.dos_file_open.descriptor_flags=&original_read_descriptor_flags;
    state.dos_file_open.descriptor_is_device=&original_descriptor_is_device;
    state.dos_file_open.descriptor_publish=&original_publish_descriptor_flags;
    state.dos_file_read.descriptor_flags=&original_read_descriptor_flags;
    state.dos_file_read.dos_read=&original_dos_read_fixture;
    state.dos_file_close.descriptor_close=&original_dos_close_fixture;
    state.dos_file_io.call_38106=&original_binary_read;
    state.dos_file_io.call_380f8=&original_source_descriptor_close;
    state.dos_file_io.call_38407=&original_level_file_seek_host;
    state.dos_file_io.call_37c37=&original_level_heap_release_header;
    state.game_audio.call_312ec=&original_sound_config_read;
    state.game_audio.call_312dc=&original_sound_config_close;
    state.mouse_init.int33_reset=&controlled_mouse_reset;
    state.mouse_init.call_30d80=&original_mouse_memory_via_30d80;
    state.mouse_init.call_33aa0=&original_mouse_33aa0;
    state.mouse_init.call_30de8=&original_mouse_free_via_30de8;
    state.mouse_range.call_2e340=&source_mouse_present_2e340;
    state.mouse_range.int33=&interrupt_mouse_range_boundary;
    state.game_audio.call_33d18=&original_mouse_range_x;
    state.game_audio.call_33d38=&original_mouse_range_y;
    state.dos_file_io.call_37a73=&original_dpmi_register_context;
    state.dos_file_io.call_33a7c=&stop_at_original_dpmi31;
    state.mouse_init.call_311e8=&original_mouse_warning_via_311e8;
    state.dos_file_io.dos_ah09=&stop_at_dos_mouse_warning_ah09;
    state.game_audio.call_33b44=&original_mouse_initialization;
    state.startup.int21=&dos21;
    state.startup.int31=&dpmi31;
    state.startup.read_segment8=&seg8;
    state.startup.read_segment16=&seg16;
    state.startup.read_segment32=&seg32;
    state.startup.write_segment8=&put8;
    state.startup.write_segment16=&put16;
    state.startup.call_395e2=&initializers;
    state.startup.call_39589=&after_initializers;
    state.init.call_guest=&guest;
    if(extended) {
        if(source_host_heap) {
            state.heap_provider.dpmi_alloc=&host_heap_0501;
            if(source_dos_fallback) {
                state.heap_provider.dos_alloc_paragraphs=&host_heap_dos_allocate;
                state.heap_provider.dos_free_selector=&host_heap_dos_release;
                state.heap_provider.selector_base=&host_heap_dos_selector_base;
                state.heap_provider.release_dos_block=&host_heap_dos_release;
            }
            state.heap_provider.release_dpmi_block=&host_heap_0502;
            state.heap_growth.alloc_39c7b=&host_heap_39c7b;
            state.heap_growth.init_arena_39bbc=&host_heap_39bbc;
            state.heap_growth.free_37c45=&host_heap_free_initial;
            state.heap.arena_alloc=&host_heap_alloc;
            state.heap.arena_free=&host_heap_free;
            if(source_break_growth) {
                state.heap_growth.lsl_ds=&host_break_limit;
                state.heap_growth.limit_3a467=&host_break_extend;
                state.heap.grow_heap=&host_heap_39e1c;
            }else state.heap.grow_heap=&host_heap_39d4d;
        }else state.heap.arena_alloc=&fixture_arena_alloc;
        state.environment.alloc=&fixture_alloc;
        state.environment.memset_30d30=&fixture_fill;
        state.environment.free_37c37=&fixture_free;
        state.streams.alloc_primary=&fixture_alloc;
        state.streams.alloc_fallback=&rejected_fallback;
        state.streams.fatal_3a536=&rejected_fatal;
        state.argv.alloc=&fixture_alloc;
        state.post.call_3a165=&post_stack_capacity;
        state.post.alloca_guest=&post_stack_alloc;
        state.post.call_3a38a=&post_protection;
        state.post.call_20608=&enter_game;
        state.post.call_352be=&unexpected_exit;
    }
