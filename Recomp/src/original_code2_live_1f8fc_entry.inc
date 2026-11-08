// Authentic retained 1F8FC frame-entry boundary. The original function
// performs two RES0 background 2E510 draws before it calls 2EFD0.
// Stop at that exact first cache-release callback: never skip the
// incomplete 1F1C8/1F120/presentation graph or synthesize a frame.
struct OriginalFrameEntryTrace {
    u32 backgrounds=0u;
    u32 releases=0u;
    u32 resource_ids[2]{};
    u32 rows[2]{};
    u32 columns[2]{};
};
thread_local OriginalFrameEntryTrace* original_frame_entry_trace=nullptr;
void original_frame_entry_background(u32 row,u32 x,u32 resource,u32 slot) noexcept {
    if(!original_frame_entry_trace || slot!=0u ||
       original_frame_entry_trace->backgrounds>=2u)
        stop("ORIGINAL_1F8FC_FRAME_ENTRY_UNEXPECTED_DRAW");
    const u32 index=original_frame_entry_trace->backgrounds++;
    original_frame_entry_trace->resource_ids[index]=resource;
    original_frame_entry_trace->rows[index]=row;
    original_frame_entry_trace->columns[index]=x;
    original_scene_frame_base(row,x,resource,slot);
}
void original_frame_entry_release(u32 resource,u32 slot) noexcept {
    if(!original_frame_entry_trace || slot!=0u ||
       original_frame_entry_trace->backgrounds!=2u ||
       resource!=original_frame_entry_trace->resource_ids[0])
        stop("ORIGINAL_1F8FC_FRAME_ENTRY_RELEASE_ABI_CHANGED");
    ++original_frame_entry_trace->releases;
    stop("ORIGINAL_1F8FC_FRAME_ENTRY_FIRST_RESOURCE_RELEASE_FRONTIER");
}
void original_live_1f8fc_entry_probe() noexcept {
    if(!current->original_vga || !current->live_continuation ||
       current->original_181b4_completed!=1u ||
       code2::load_le32(current->data.data()+0xC034u)!=3u)
        stop("ORIGINAL_1F8FC_ENTRY_GUEST_PRECONDITION_CHANGED");
    OriginalFrameEntryTrace trace{};
    original_frame_entry_trace=&trace;
    code2::Application45Boundary entry{};
    entry.call_291b0=&original_291b0_source_page;
    entry.call_2e510=&original_frame_entry_background;
    entry.call_2efd0=&original_frame_entry_release;
    const u32 original_mode=code2::load_le32(current->data.data()+0xC034u);
    const u32 expected_first=code2::load_le32(current->data.data()+0x258DCu)+1u+original_mode*3u;
    const u32 expected_second=expected_first+1u;
    std::jmp_buf scope{};
    auto* prior=current->active_escape;
    const int stopped=setjmp(scope);
    if(stopped==0) {
        current->active_escape=&scope;
        code2::function_1f8fc(current->machine,entry);
        stop("ORIGINAL_1F8FC_FRAME_ENTRY_UNEXPECTED_RETURN");
    }
    current->active_escape=prior;
    original_frame_entry_trace=nullptr;
    if(!current->stop_reason ||
       std::strcmp(current->stop_reason,
          "ORIGINAL_1F8FC_FRAME_ENTRY_FIRST_RESOURCE_RELEASE_FRONTIER")!=0 ||
       trace.backgrounds!=2u || trace.releases!=1u ||
       trace.resource_ids[0]!=expected_first ||
       trace.resource_ids[1]!=expected_second ||
       trace.rows[0]!=0u || trace.columns[0]!=0u ||
       trace.rows[1]!=0xC8u || trace.columns[1]!=0u)
        stop("ORIGINAL_1F8FC_ENTRY_ORIGINAL_RESOURCE_ORDER_INVALID");
    std::cerr<<"CODE2_ORIGINAL_LIVE_BOOT=1F8FC_SOURCE_FRAME_ENTRY"
             <<" original_source=1F8FC_2E510_2F29C_2939C_2A380"
             <<" backgrounds="<<trace.backgrounds
             <<" first_RES0_id="<<trace.resource_ids[0]
             <<" second_RES0_id="<<trace.resource_ids[1]
             <<" original_release_boundary=2EFD0"
             <<" original_guest=PRESERVED"
             <<" scene=NOT_YET_CONNECTED"
             <<" completed_frames=0\n"<<std::flush;
    current->stop_reason=nullptr;
}
