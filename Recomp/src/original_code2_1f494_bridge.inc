// Included inside WorldFrame: original compiled 1F494/1EA90 host callback bridge.
// Uses exact CODE.2 entry points, guest state, RES0/RES1, native VGA plane bus.
std::string party_res0_path{},party_res1_path{};
u32 party_resource_calls=0,party_masked_blits=0,party_direct_blits=0;
u32 party_rect_fills=0,party_status_frames=0,party_missing_text=0;
u32 party_decimal_formats=0,party_centered_text_draws=0,party_glyph_blits=0;
u32 party_decimal_bytes=0;
static u32 party_lookup(u32 id,u32 slot) noexcept {
    auto& w=self();
    if(w.failure)return 0u;
    try {
        if(slot>1u)throw std::out_of_range("original party RES archive slot");
        const auto& path=slot==0u?w.party_res0_path:w.party_res1_path;
        if(path.empty())throw std::runtime_error("missing original party RES path");
        const auto name=std::filesystem::path(path).filename().string();
        if(w.gfx.expected_archive_basename!=name ||
           w.gfx.resident_slot!=slot)
            w.gfx.open_readonly_resource(path,0u,slot);
        if(id>=w.gfx.entry_offsets.size()-1u)
            throw std::out_of_range("party RES ID outside original archive");
        ++w.party_resource_calls;
        const u32 ptr=code2::function_2f29c(
            w.gfx.machine,w.gfx.resources,id,slot);
        if(!ptr||w.gfx.stream_error)
            throw std::runtime_error("original party resource decode");
        return ptr;
    } catch(const std::exception& e) {
        std::cerr<<"CODE2_ORIGINAL_1F494_RESOURCE_ERROR slot="<<slot
                 <<" id="<<id<<" reason="<<e.what()<<"\\n";
        w.failure=true;return 0u;
    }
}
static u32 party_draw_direct(u32 frame,u32 row,u32 x,u32 image) noexcept {
    if(!image||self().failure)return 0u;
    ++self().party_direct_blits;
    return WorldGraphicsSource::draw_image(frame,row,x,image);
}
static u32 party_draw_masked(u32 frame,u32 row,u32 x,u32 image,u32 lut) noexcept {
    auto& w=self();
    if(!image||w.failure)return 0u;
    if(lut<0xCF70u||lut+256u>0xD370u) {
        w.failure=true;return 0u;
    }
    code2::Application60Boundary adapter{};
    adapter.out16=&WorldGraphicsSource::out16;
    adapter.vga_read8=&WorldGraphicsSource::read8;
    adapter.vga_write8=&WorldGraphicsSource::write8;
    code2::function_2a584(w.gfx.machine,adapter,frame,row,x,image,lut);
    ++w.party_masked_blits;
    return 1u;
}
static void party_2975c(u32 frame,u32 row,u32 x,u32 id,u32 slot) noexcept {
    code2::Application57Boundary b{};
    b.call_2f29c=&party_lookup;
    b.call_2a380=&party_draw_direct;
    (void)code2::function_2975c(b,frame,row,x,id,slot);
}
static void party_298f0(u32 frame,u32 row,u32 x,u32 id,u32 slot,u32 lut) noexcept {
    code2::Application57Boundary b{};
    b.call_2f29c=&party_lookup;
    b.call_2a584=&party_draw_masked;
    (void)code2::function_298f0(b,frame,row,x,id,slot,lut);
}
static void party_2a258(u32 frame,u32 image) noexcept {
    if(image)code2::function_2a258(self().gfx.machine,frame,image);
}
static u32 party_24478(u32 actor,u32 hand) noexcept {
    return code2::function_24478(
        self().gfx.machine,static_cast<std::int32_t>(actor),hand);
}
static u32 party_clock() noexcept {
    return code2::function_19cc4(self().gfx.machine);
}
static void party_rect(u32 row,u32 x,u32 rows,u32 cols,u32 color) noexcept {
    auto& w=self();
    if(row>=400u||x>=320u||rows>400u-row||cols>320u-x||
       std::uint64_t(rows)*cols>128000u){w.failure=true;return;}
    // Host primitive is the original CODE.2 29AA4 planar pixel store.
    for(u32 r=row;r<row+rows;++r)
        for(u32 c=x;c<x+cols;++c)
            code2::function_29aa4(w.gfx.machine,w.gfx.gfx,r,c,color);
    ++w.party_rect_fills;
}
static void party_vertical(u32 row,u32 col,u32 height,u32 color) noexcept {
    party_rect(row,col,height,1u,color);
}
static void party_horizontal(u32 row,u32 col,u32 width,u32 color) noexcept {
    party_rect(row,col,1u,width,color);
}
static void party_border(u32 row,u32 col,u32 rows,u32 cols,u32 color) noexcept {
    code2::Application67Boundary b{};
    b.call_291e0=&party_vertical;
    b.call_29208=&party_horizontal;
    code2::function_2e374(b,row,col,rows,cols,color);
    ++self().party_status_frames;
}
static void party_frame_direct(u32 frame,u32 row,u32 x,u32 image) noexcept {
    (void)party_draw_direct(frame,row,x,image);
}
static void party_2e510(u32 row,u32 col,u32 id,u32 slot) noexcept {
    auto& w=self();
    code2::Application67Boundary b{};
    b.call_2f29c=&party_lookup;
    b.call_2939c=&party_frame_direct;
    code2::function_2e510(w.gfx.machine,b,row,col,id,slot);
}
static void party_2e4a4(u32 row,u32 col,u32 start,u32 count,u32 img) noexcept {
    auto& w=self();
    if(!img){w.failure=true;return;}
    code2::Application67Boundary b{};
    b.call_2939c=&party_frame_direct;
    code2::function_2e4a4(w.gfx.machine,b,row,col,start,count,img);
}
static u32 party_ef14(u32 slot) noexcept {
    return static_cast<u32>(code2::function_0ef14(self().gfx.machine,slot));
}
static void party_22f40(u32 slot,u32 state) noexcept {
    auto& w=self();
    code2::Application50Boundary b{};
    b.call_298f0=&party_298f0;
    code2::function_22f40(w.gfx.machine,b,static_cast<std::int32_t>(slot),state);
}
static void party_23024(u32,u32) noexcept {
    // Group-list UI needs original 23024-specific fonts / navigation.
    self().failure=true;
}
// 2E6F0 -> retail 373BF: signed decimal into CODE.2 guest 2B64E.
// The original 10-byte scratch buffer belongs to CODE.2, not the host.
static void party_decimal_write8(u32 at,u8 value) noexcept {
    auto& w=self();
    if(at<0x2B64Eu || at>=0x2B658u) {
        w.failure=true;return;
    }
    w.gfx.memory[at]=value;
    ++w.party_decimal_bytes;
}
static void party_decimal_format(u32 raw,u32 target,u32 radix) noexcept {
    auto& w=self();
    code2::Runtime124Boundary b{};
    b.write8=&party_decimal_write8;
    (void)code2::function_373bf(w.gfx.machine,b,
        static_cast<std::int32_t>(raw),target,radix);
}
static u32 party_2e6f0(u32 value) noexcept {
    auto& w=self();
    constexpr u32 text=0x2B64Eu,capacity=10u;
    std::fill_n(w.gfx.memory.begin()+text,capacity,0u);
    w.party_decimal_bytes=0u;
    code2::Application67Boundary b{};
    b.call_373bf=&party_decimal_format;
    const u32 result=code2::function_2e6f0(b,value);
    if(result!=text || !w.party_decimal_bytes ||
       w.party_decimal_bytes>=capacity || w.failure) {
        ++w.party_missing_text; w.failure=true; return 0u;
    }
    // 373BF's generated ASCII loop does not store a terminal zero;
    // the zeroed remaining guest buffer is the C-string sentinel.
    ++w.party_decimal_formats;
    return result;
}
static u32 party_glyph_direct(u32 frame,u32 row,u32 col,u32 image) noexcept {
    auto& w=self();
    if(!image || frame>255u || w.failure) {
        w.failure=true;return 0u;
    }
    ++w.party_glyph_blits;
    return party_draw_direct(frame,row,col,image);
}
static u32 party_glyph_masked(
        u32 frame,u32 row,u32 col,u32 image,u32 palette) noexcept {
    auto& w=self();
    if(!image || frame>255u || w.failure) {
        w.failure=true;return 0u;
    }
    ++w.party_glyph_blits;
    return party_draw_masked(frame,row,col,image,palette);
}
static void party_30508(u32 row,u32 col,u32 width,u32 text,u32 font_id) noexcept {
    auto& w=self();
    if(w.failure)return;
    if(text!=0x2B64Eu || font_id!=0u || row>=400u ||
       col>=320u || width>320u-col) {
        ++w.party_missing_text;w.failure=true;return;
    }
    // 30508 uses app70_font -> 2F29C(RES0 resource font_id,0).
    // Make archive slot 0 current BEFORE entering compiled CODE.2;
    // the same original cache/decoder is used as every other RES image.
    const u32 font=party_lookup(font_id,0u);
    if(!font || std::uint64_t(font)+132u>=w.gfx.heap_cursor) {
        ++w.party_missing_text;w.failure=true;return;
    }
    // Retail font starts with an IMAGE pointer, followed by signed-byte
    // glyph widths at font+4+char. ASCII decimal/minus are the only
    // characters accepted by the 2E6F0 decimal formatter.
    const u32 image=code2::load_le32(w.gfx.memory.data()+font);
    if(!image || std::uint64_t(image)+8u>=w.gfx.heap_cursor ||
       w.gfx.memory[image+7u]>0xFFu) {
        ++w.party_missing_text;w.failure=true;return;
    }
    u32 measured=0u,chars=0u;
    for(u32 at=text; at<text+10u && w.gfx.memory[at];++at) {
        const u8 c=w.gfx.memory[at];
        if(!((c>='0'&&c<='9')||c=='-')) {
            w.failure=true;++w.party_missing_text;return;
        }
        measured+=w.gfx.memory[font+4u+c];
        ++chars;
    }
    if(chars==0u||chars>=10u || measured>width) {
        ++w.party_missing_text;w.failure=true;return;
    }
    code2::Application56Boundary glyph{};
    glyph.call_2a380=&party_glyph_direct;
    glyph.call_2a584=&party_glyph_masked;
    code2::Application70Boundary text_bound{};
    text_bound.app69=&w.gfx.resources;
    text_bound.app56=&glyph;
    // The source handles centering, 2B680 cursor, 2B684 row,
    // per-glyph widths, 2939C/293EC and the original VGA store.
    code2::function_30508(w.gfx.machine,text_bound,
                          row,col,width,text,font_id);
    if(w.failure || !w.party_glyph_blits) {
        ++w.party_missing_text;w.failure=true;return;
    }
    ++w.party_centered_text_draws;
}
void render_original_1f494_party(const std::string& res0,
                                 const std::string& res1) {
    CHECK(std::filesystem::path(res0).filename()=="RES0" &&
          std::filesystem::path(res1).filename()=="RES1");
    party_res0_path=res0;party_res1_path=res1;
    // Original 07938 produced the saved 4 x 0x248 character records.
    copy_guest(0x218F0u,4u*0x248u);
    copy_guest(0xCF60u,0x810u); // original palette/LUT state from source
    copy_guest(0xD681u,2u);
    copy_guest(0xD8A8u,8u);
    copy_guest(0x6DAEu,2u);
    copy_guest(0xA7E4u,4u);
    u32 active_character_records=0u;
    for(u32 i=0u;i<4u;++i) {
        gfx.memory[0xCF5Au+i]=1u;
        const u16 flags=code2::load_le16(
            gfx.memory.data()+0x218F0u+i*0x248u);
        active_character_records+=((flags&0x8000u)==0u);
    }
    const u32 lut_nonzero_bytes=static_cast<u32>(
        std::count_if(gfx.memory.begin()+0xCF70u,
                      gfx.memory.begin()+0xD370u,
                      [](u8 value){return value!=0u;}));
    code2::Application43Boundary b43{};
    b43.call_19cc4=&party_clock;
    b43.call_2f29c=&party_lookup;
    b43.call_29b40=&party_rect;
    b43.call_24478=&party_24478;
    b43.call_2a258=&party_2a258;
    b43.call_298f0=&party_298f0;
    b43.call_2e374=&party_border;
    b43.call_2975c=&party_2975c;
    b43.call_2e6f0=&party_2e6f0;
    b43.call_30508=&party_30508;
    b43.divide_error=[](){self().failure=true;};
    code2::Application44Boundary b44{};
    b44.app43=&b43;
    b44.call_291b0=&source_select_draw_page;
    b44.call_ef14=&party_ef14;
    b44.call_2e510=&party_2e510;
    b44.call_2e4a4=&party_2e4a4;
    b44.call_22f40=&party_22f40;
    b44.call_23024=&party_23024;
    b44.divide_error=[](){self().failure=true;};
    code2::function_1f494(gfx.machine,b44,0u);
    std::cout<<"CODE2_ORIGINAL_1F494_1EA90_PARTY_HUD"
             <<" active_original_characters="<<active_character_records
             <<" source_state_lut_nonzero="<<lut_nonzero_bytes
             <<" resource_calls="<<party_resource_calls
             <<" masked_298f0_2a584="<<party_masked_blits
             <<" direct_2975c="<<party_direct_blits
             <<" fills_29aa4="<<party_rect_fills
             <<" borders_2e374="<<party_status_frames
             <<" hand_slots_1ea90=8"
             <<" decimal_2e6f0="<<party_decimal_formats
             <<" centered_30508="<<party_centered_text_draws
             <<" glyph_303c8="<<party_glyph_blits
             <<" unsupported_text="<<party_missing_text
             <<" callback_failure="<<failure
             <<" DOS_PARITY=NOT_VERIFIED\\n";
    CHECK(!failure && !gfx.unsupported && !gfx.stream_error);
    gfx.open_readonly_resource(res0,0u,0u);
}
