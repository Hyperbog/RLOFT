
// -----------------------------------------------------------------------------
// Batch 10: 06CAD shared save-load tail, 07070 resource-panel helpers,
// background renderer, FCB delete wrapper, wildcard file copy, and 07938 loader.
// -----------------------------------------------------------------------------

struct Application10Boundary {
    u32 (*call_3131c)(u32,u32,u32)=nullptr;
    u32 (*call_312ec)(u32,u32,u32)=nullptr;
    u32 (*call_312ec_local)(u8*,u32,u32)=nullptr;
    void (*call_1c0c0)(u32)=nullptr;
    void (*call_30738)(u32)=nullptr;
    void (*call_1a344)(s32,u32)=nullptr;
    void (*call_1e74c)(u32)=nullptr;
    u32 (*call_3104c)(u32)=nullptr;
    void (*call_30cd4)(u32,u32,u32)=nullptr;
    void (*call_310b0)(u32)=nullptr;
    void (*call_312dc)(u32)=nullptr;
    void (*call_31edc)(u32)=nullptr;
    void (*call_31f38)(u32)=nullptr;
    void (*call_181b4)(u32)=nullptr;
    u32 (*call_312a0)(u32)=nullptr;
    u32 (*call_30070)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_2975c)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_3060c)(u32,u32,u32,u32)=nullptr;
    void (*call_30040)(u32)=nullptr;
    u32 (*call_30ca2)()=nullptr;
    void (*call_291b0)(u32)=nullptr;
    void (*call_1a510)(u32)=nullptr;
    void (*call_1a9f8)()=nullptr;
    void (*call_33971)(u32,u32,u32)=nullptr;
    void (*dos_int21_13)(u32)=nullptr;
    u32 (*call_339a2_local)(u32,u32,u8*)=nullptr;
    u32 (*call_339cf_local)(u8*)=nullptr;
    u32 (*call_3127c)(u32)=nullptr;
    void (*call_31380)(u32)=nullptr;
    u32 (*call_312c8)(u32)=nullptr;
    u32 (*call_312b4)(u32)=nullptr;
    u32 (*call_31304)(u32,u32,u32)=nullptr;
};

static u32 app10_read_local_u32(Application10Boundary& b,u32 h) noexcept {
    u8 raw[4]{};
    if(b.call_312ec_local)(void)b.call_312ec_local(raw,4u,h);
    return load_le32(raw);
}

static u16 app10_read_local_u16(Application10Boundary& b,u32 h) noexcept {
    u8 raw[2]{};
    if(b.call_312ec_local)(void)b.call_312ec_local(raw,2u,h);
    return load_le16(raw);
}

static u8 app10_read_local_u8(Application10Boundary& b,u32 h) noexcept {
    u8 raw=0u;
    if(b.call_312ec_local)(void)b.call_312ec_local(&raw,1u,h);
    return raw;
}

// 06CAD is a backward-jump shared tail of 07938, not an independently called
// routine. EBX and ESI both carry the already-open save handle at entry.
static u32 app10_load_tail_06cad(Machine& m,Application10Boundary& b,Application09Boundary& a9,u32 h) noexcept {
    if(b.call_3131c)(void)b.call_3131c(0x32u,0u,h);
    auto read_guest=[&](u32 ptr,u32 n)->u32{return b.call_312ec?b.call_312ec(ptr,n,h):0u;};

    (void)read_guest(0x22200u,0x20u);
    const u32 saved_level=app10_read_local_u32(b,h);
    (void)read_guest(0x255D0u,4u);
    (void)read_guest(0x255D4u,4u);
    const u32 saved_mode=app10_read_local_u32(b,h);
    if(b.call_1c0c0)b.call_1c0c0(saved_mode);
    if(*dg(m,0xC02Fu)!=0u){store_le16(dg(m,0x255D0u),0x8000u);store_le16(dg(m,0x255D4u),0x8000u);}

    (void)read_guest(0xF03Cu,2u);
    (void)read_guest(0xF03Eu,2u);
    (void)read_guest(0xFD08u,4u);
    (void)read_guest(0x6DAEu,2u);
    const u32 runtime_state=app10_read_local_u32(b,h);
    if(b.call_30738)b.call_30738(runtime_state);
    (void)read_guest(0xCF4Cu,4u);
    const u16 word_state=app10_read_local_u16(b,h);
    const u8 byte_state=app10_read_local_u8(b,h);
    if(b.call_1a344)b.call_1a344(static_cast<s16>(word_state),byte_state);

    (void)read_guest(0x218E0u,0x920u);
    const u32 record_count=app10_read_local_u32(b,h);
    (void)read_guest(0x248ECu,record_count*0x16u);
    for(u32 off=0u;off<0x920u;off+=0x248u){
        if((load_le16(dg(m,0x218F0u+off))&0x8000u)!=0u)continue;
        const s8 a=static_cast<s8>(*dg(m,0x218F5u+off));
        const s8 c=static_cast<s8>(*dg(m,0x218F6u+off));
        const s8 d=static_cast<s8>(*dg(m,0x218F7u+off));
        if(a==1)*dg(m,0x21A11u+off)=static_cast<u8>(*dg(m,0x21A11u+off)|4u);
        if(a==4||c==4||d==4)*dg(m,0x21A11u+off)=static_cast<u8>(*dg(m,0x21A11u+off)|2u);
        if(a==3||c==3||d==3)*dg(m,0x21A11u+off)=static_cast<u8>(*dg(m,0x21A11u+off)|1u);
    }

    (void)read_guest(0xD8A8u,8u);
    const u32 elapsed_state=app10_read_local_u32(b,h);
    if(b.call_1e74c)b.call_1e74c(elapsed_state);
    for(u32 i=0u;i<0x3Eu;++i){
        const u32 d=0x7330u+i*10u;
        (void)read_guest(load_le32(dg(m,d+5u)),static_cast<u32>(*dg(m,d))*2u);
        (void)read_guest(d+9u,1u);
    }
    const u32 state_31edc=app10_read_local_u32(b,h);
    const u32 state_31f38=app10_read_local_u32(b,h);
    if(read_guest(0x12990u,0x0Fu)==0x0Fu&&b.call_30cd4)b.call_30cd4(0x67ECu,0x12990u,0x0Fu);

    const u32 temp=b.call_3104c?b.call_3104c(0x248u):0u;
    for(u32 i=0u;i<10u;++i){
        const u32 n=read_guest(temp,0x248u);
        if(n==0x248u&&b.call_30cd4)b.call_30cd4(0xA808u+i*0x248u,temp,n);
    }
    const u32 tail_n=read_guest(temp,0x10u);
    if(tail_n==0x10u&&b.call_30cd4)b.call_30cd4(0x6FA0u,temp,tail_n);
    if(b.call_310b0)b.call_310b0(temp);
    if(b.call_312dc)b.call_312dc(h);

    for(u32 i=0u;i<4u;++i)*dg(m,0xCF5Au+i)=1u;
    if(b.call_31edc)b.call_31edc(state_31edc);
    if(b.call_31f38)b.call_31f38(state_31f38);
    store_le32(dg(m,0x6F90u),0u);store_le32(dg(m,0x6F94u),0u);
    store_le32(dg(m,0x6F98u),0u);store_le32(dg(m,0x6F9Cu),0u);
    store_le32(dg(m,0xC6C8u),0xFFFFFFFFu);
    if(b.call_181b4)b.call_181b4(saved_level);
    store_le32(dg(m,0xBF44u),0x270Fu);
    store_le32(dg(m,0xC038u),record_count);
    function_06c68(m,a9);
    return 0u;
}
static_assert(0x0706Eu-0x06CADu==961u);

// Ten 50-byte labels/resources backed by numbered files 01..10. Missing files
// fall back to the C string pointed to by 60EC.
void function_07070(Machine& m,Application10Boundary& b,u32 image) noexcept {
    u32 sprite=0u,x=0x15u,handle_off=0u,text_y=0x1Du;
    u32 buf=0x6461u;
    for(s32 i=0;i<10;++i){
        buf+=0x32u;
        const s32 n=i+1;
        *dg(m,0x6690u)=static_cast<u8>(n/10+0x30);
        *dg(m,0x6691u)=static_cast<u8>(n%10+0x30);
        const u32 h=b.call_312a0?b.call_312a0(0x668Cu):0xFFFFFFFFu;
        if(h==0xFFFFFFFFu){app09_copy_cstr(m,buf,load_le32(dg(m,0x60ECu)));}
        else {if(b.call_312ec)(void)b.call_312ec(buf,0x32u,h);if(b.call_312dc)b.call_312dc(h);}
        const u32 resource=b.call_30070?b.call_30070(text_y,0x64u,0x0Fu,0xD8u,0xFFu):0u;
        store_le32(dg(m,0x12B30u+handle_off),resource);
        if(b.call_2975c)b.call_2975c(sprite,x,0x14u,image,0u);
        if(b.call_3060c)b.call_3060c(text_y,0x64u,buf,1u);
        sprite+=2u;x+=0x25u;handle_off+=4u;text_y+=0x25u;
    }
}
static_assert(0x0719Cu-0x07070u==300u);

void function_0719c(Machine& m,Application10Boundary& b) noexcept {
    for(u32 off=0u;off<0x28u;off+=4u)if(b.call_30040)b.call_30040(load_le32(dg(m,0x12B30u+off)));
}
static_assert(0x071B8u-0x0719Cu==28u);

void function_071b8(Machine& m,Application10Boundary& b,u32 image) noexcept {
    if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
    for(u32 x=0u;x<0x190u;x+=0x10u){
        for(u32 y=0u;y<0x140u;y+=8u){
            const u32 r=b.call_30ca2?b.call_30ca2():0u;
            if(b.call_2975c)b.call_2975c((r*4u)>>15,x,y,image+0x35u,0u);
        }
    }
    for(u32 y=8u;y<0x138u;y+=8u){
        u32 r=b.call_30ca2?b.call_30ca2():0u;
        if(b.call_2975c)b.call_2975c(((r*2u)>>15)+9u,0u,y,image+0x35u,0u);
        r=b.call_30ca2?b.call_30ca2():0u;
        if(b.call_2975c)b.call_2975c(((r*2u)>>15)+5u,0x180u,y,image+0x35u,0u);
    }
    for(u32 x=0x10u;x<0x180u;x+=0x10u){
        u32 r=b.call_30ca2?b.call_30ca2():0u;
        if(b.call_2975c)b.call_2975c(((r*2u)>>15)+0x0Cu,x,0u,image+0x35u,0u);
        r=b.call_30ca2?b.call_30ca2():0u;
        if(b.call_2975c)b.call_2975c(((r*2u)>>15)+0x0Eu,x,0x138u,image+0x35u,0u);
    }
    if(b.call_2975c){
        b.call_2975c(8u,0u,0u,image+0x35u,0u);
        b.call_2975c(0x0Bu,0u,0x138u,image+0x35u,0u);
        b.call_2975c(4u,0x180u,0u,image+0x35u,0u);
        b.call_2975c(7u,0x180u,0x138u,image+0x35u,0u);
    }
    function_07070(m,b,image);
    if(b.call_1a510)b.call_1a510(load_le32(dg(m,0xC050u)));
    *dg(m,0xC050u)=static_cast<u8>(*dg(m,0xC050u)^1u);
    if(b.call_1a9f8)b.call_1a9f8();
    if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
}
static_assert(0x07376u-0x071B8u==446u);

void function_07378(Machine& m,Application10Boundary& b) noexcept {
    *dg(m,0x12B70u)=0u;
    if(b.call_33971)b.call_33971(0x12B71u,0x3B0u,0x0Bu);
    if(b.dos_int21_13)b.dos_int21_13(0x12B70u);
}
static_assert(0x0739Du-0x07378u==37u);

// Copy every wildcard match under src_root into dst_root. The DOS find-state is
// the exact 2Ch-byte local block; its filename field begins at +1Eh.
[[nodiscard]] u32 function_073a0(Machine& m,Application10Boundary& b,u32 src_root,u32 dst_root) noexcept {
    u8 find_state[0x2C]{};
    app09_copy_cstr(m,0x128F0u,src_root);
    app09_append_cstr(m,0x128F0u,0x3BCu);
    if(!b.call_339a2_local||b.call_339a2_local(0x128F0u,0u,find_state)!=0u)return 0u;
    for(;;){
        app09_copy_cstr(m,0x12850u,src_root);
        app09_append_cstr(m,0x12850u,0x398u);
        u32 end=0x12850u;while(*dg(m,end)!=0u)++end;
        for(u32 i=0u;;++i){const u8 c=find_state[0x1Eu+i];*dg(m,end+i)=c;if(c==0u)break;}
        const u32 size=b.call_3127c?b.call_3127c(0x12850u):0u;
        const u32 in=b.call_312a0?b.call_312a0(0x12850u):0xFFFFFFFFu;
        if(in==0xFFFFFFFFu)return 1u;
        const u32 buf=b.call_3104c?b.call_3104c(size):0u;
        if(b.call_312ec)(void)b.call_312ec(buf,size,in);
        if(b.call_312dc)b.call_312dc(in);

        app09_copy_cstr(m,0x127B0u,dst_root);
        app09_append_cstr(m,0x127B0u,0x398u);
        end=0x127B0u;while(*dg(m,end)!=0u)++end;
        for(u32 i=0u;;++i){const u8 c=find_state[0x1Eu+i];*dg(m,end+i)=c;if(c==0u)break;}
        if(b.call_31380)b.call_31380(0x127B0u);
        const u32 probe=b.call_312c8?b.call_312c8(0x127B0u):0u;
        if(b.call_312dc)b.call_312dc(probe);
        const u32 out=b.call_312b4?b.call_312b4(0x127B0u):0xFFFFFFFFu;
        if(out==0xFFFFFFFFu){if(b.call_310b0)b.call_310b0(buf);return 1u;}
        const u32 wrote=b.call_31304?b.call_31304(buf,size,out):0u;
        if(wrote!=size){if(b.call_310b0)b.call_310b0(buf);if(b.call_312dc)b.call_312dc(out);return 1u;}
        if(b.call_312dc)b.call_312dc(out);
        if(b.call_310b0)b.call_310b0(buf);
        if(!b.call_339cf_local||b.call_339cf_local(find_state)!=0u)return 0u;
    }
}
static_assert(0x075C9u-0x073A0u==553u);

// 07938 opens the constructed save pathname and jumps backward to 06CAD on
// success. The C++ helper above preserves that shared-tail ownership.
[[nodiscard]] u32 function_07938(Machine& m,Application10Boundary& b,Application09Boundary& a9) noexcept {
    *dg(m,0xD681u)=0u;*dg(m,0xD682u)=0u;
    app09_copy_cstr(m,0x12990u,0xC020u);
    app09_append_cstr(m,0x12990u,0x398u);
    app09_append_cstr(m,0x12990u,0x39Cu);
    const u32 h=b.call_312a0?b.call_312a0(0x12990u):0xFFFFFFFFu;
    if(h==0xFFFFFFFFu)return 1u;
    return app10_load_tail_06cad(m,b,a9,h);
}
static_assert(0x079E0u-0x07938u==168u);
