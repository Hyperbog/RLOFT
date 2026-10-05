// -----------------------------------------------------------------------------
// Batch 70: 2F580..305A5 presentation/input path, buffered/RLE resource reader,
// image materialization, and text metrics/drawing helpers.
// All addresses are CODE.2 object-relative.
// -----------------------------------------------------------------------------

struct Application70Boundary {
    Application69Boundary* app69=nullptr;
    Application67Boundary* app67=nullptr;
    Application56Boundary* app56=nullptr;
    Application59Boundary* app59=nullptr;

    u32 (*call_3104c)(u32)=nullptr;
    void (*call_310b0)(u32)=nullptr;
    void (*call_311e8)(u32)=nullptr;
    void (*call_311e8_host)(const u8*)=nullptr;
    u32 (*call_30aec)()=nullptr;
    u32 (*call_312ec)(u32,u32,u32)=nullptr;
    u32 (*call_3131c)(u32,u32,u32)=nullptr;
    void (*call_30d30)(u32,u32,u32)=nullptr;
    u32 (*call_37700)()=nullptr;
    void (*call_37810)(u32)=nullptr;
    void (*call_indirect_f0c8)()=nullptr;
    void (*divide_error)()=nullptr;
};

static u32 function_2f5f4(Machine&,Application70Boundary&,u32,u32) noexcept;
static u32 app70_present(Machine&,Application70Boundary&,u32,const u8*,u32) noexcept;
static void function_2f868(Machine&,Application70Boundary&) noexcept;
static u8 function_2f9e4(Machine&,Application70Boundary&) noexcept;
static u8 function_2fc48(Machine&,Application70Boundary&) noexcept;
static u32 function_2fd9c(Machine&,Application70Boundary&,u32,u32) noexcept;
static u32 function_30288(Machine&,Application70Boundary&,u32,u32) noexcept;
static void function_30354(Machine&,Application70Boundary&,s8,u32) noexcept;
static void function_303c8(Machine&,Application70Boundary&,u32,u32) noexcept;
static void function_304f4(Machine&,u32,u32) noexcept;
static u32 function_3058c(Machine&) noexcept;
static u32 function_30594(Machine&) noexcept;

static void app70_copy_dg_z_pairs(Machine& m,u8* dst,u32 src) noexcept {
    u32 d=0;
    for(;;){
        const u8 a=*dg(m,src); dst[d]=a;
        if(a==0u)return;
        const u8 c=*dg(m,src+1u); dst[d+1u]=c;
        src+=2u; d+=2u;
        if(c==0u)return;
    }
}
static void app70_append_dg_z_pairs(Machine& m,u8* dst,u32 src) noexcept {
    u32 d=0; while(dst[d]!=0u)++d;
    app70_copy_dg_z_pairs(m,dst+d,src);
}
static u32 app70_font(Machine& m,Application70Boundary& b,u32 item) noexcept {
    if(!b.app69)return 0u;
    return function_2f29c(m,*b.app69,item,0u);
}
static u32 app70_text_width_from_font(Machine& m,u32 text,u32 font,u32 limit,bool bounded) noexcept {
    u32 width=0u;
    if(text==0u)return 0u;
    u32 n=0u;
    while(*dg(m,text)!=0u && (!bounded || n<limit)){
        const s32 ch=static_cast<s8>(*dg(m,text++));
        width+=*dg(m,static_cast<u32>(static_cast<s32>(font)+ch+4));
        ++n;
    }
    return width;
}

// 2F580: presentation/error wrapper.  The two paths that bypass the interactive
// prompt still emit the supplied string through 311E8 before restoring backend state.
void function_2f580(Machine& m,Application70Boundary& b,u32 text) noexcept {
    if(load_le32(dg(m,0xF0C0u))!=0xFFFFFFFFu && ((*dg(m,0xF0B1u)&0x80u)==0u)){
        const u32 key=function_2f5f4(m,b,text,0u);
        if(b.app69 && b.app69->app56)function_290f4(m,*b.app69->app56,0xFFu);
        if(b.app69)function_2f4e0(m,*b.app69);
        if(key==0x54u && b.call_311e8)b.call_311e8(text);
    } else if(b.call_311e8) b.call_311e8(text);
    if(b.app69)function_2f540(*b.app69,1u);
}
static_assert(0x2F5F1u-0x2F580u==113u);

// 2F5F4: centered prompt/input presenter.  ESI is not initialized on the
// delay-only path, so the encoded EAX result there is the caller's incoming ESI.
static u32 app70_present(Machine& m,Application70Boundary& b,u32 text,const u8* host_text,u32 mode) noexcept {
    if(*dg(m,0xEF3Cu)==0xFFu){
        if(host_text){if(b.call_311e8_host)b.call_311e8_host(host_text);}
        else if(b.call_311e8)b.call_311e8(text);
        return 0u;
    }

    const u8 saved_page=*dg(m,0xEF4Du);
    u8 saved_aux=0xFFu;
    u32 result=m.cpu.esi;
    const u32 saved_y=function_3058c(m);
    const u32 saved_x=function_30594(m);
    s32 bottom=0x140;

    if(*dg(m,0xEF3Cu)!=5u && *dg(m,0xEF4Du)!=*dg(m,0xEF4Eu)){
        if(b.app56)function_291b0(m,*b.app56,*dg(m,0xEF4Eu));
    } else if(*dg(m,0xEF3Cu)==5u){
        if(b.app56)function_291b0(m,*b.app56,0u);
        bottom-=8;
    }

    const u32 height=*dg(m,0xEF3Eu);
    const u32 scratch=b.call_3104c?b.call_3104c(height*0x140u):0u;
    if(b.app56)function_292bc(m,*b.app56,scratch,0,0,static_cast<s32>(height),0x140);
    --bottom;
    for(u32 i=0;i<height;++i){
        if(b.app59)function_29cec(m,*b.app59,i,0u,i,static_cast<u32>(bottom),static_cast<u32>(*dg(m,0xF0C4u)-static_cast<u8>(i)));
    }

    if(load_le32(dg(m,0xF0C0u))!=0xFFFFFFFFu && (host_text!=nullptr || text!=0u)){
        u32 width=0u;
        if(host_text){
            const u32 font=app70_font(m,b,load_le32(dg(m,0xF0C0u)));
            for(u32 i=0;host_text[i]!=0u;++i){const s32 ch=static_cast<s8>(host_text[i]);width+=*dg(m,static_cast<u32>(static_cast<s32>(font)+ch+4));}
        } else width=function_30288(m,b,text,load_le32(dg(m,0xF0C0u)));
        const s32 half=(static_cast<s32>(width)-(static_cast<s32>(width)>>31))/2; // encoded SUB sign; SAR 1 form
        function_304f4(m,0u,static_cast<u32>(0xA0-half));
        if(host_text){
            for(u32 i=0;host_text[i]!=0u;++i)function_30354(m,b,static_cast<s8>(host_text[i]),load_le32(dg(m,0xF0C0u)));
        } else function_303c8(m,b,text,load_le32(dg(m,0xF0C0u)));
    }

    if(*dg(m,0xEF3Cu)==5u && *dg(m,0xEF4Eu)!=0u){
        saved_aux=*dg(m,0xEF47u);
        const s32 limit=0xC8-static_cast<s32>(*dg(m,0xEF3Eu));
        if(static_cast<s16>(load_le16(dg(m,0xEF47u)))>limit && b.app59)
            function_2a104(*b.app59,static_cast<u32>(limit));
    }

    if(static_cast<s32>(mode)>1){
        if(b.app67)(void)function_2e614(m,*b.app67,mode,0u);
    } else if(mode==1u){
        for(;;){
            result=b.call_30aec?b.call_30aec():0u;
            if(result==0x20u || result==0x54u)break;
        }
    } else {
        for(;;){
            result=b.call_30aec?b.call_30aec():0u;
            if(result==0x4Eu || result==0x59u || result==0x53u)break;
        }
        if(result==0x53u)result=0x59u;
    }

    if(b.app56)function_2932c(m,*b.app56,scratch,0,0,static_cast<s32>(height),0x140);
    if(b.call_310b0)b.call_310b0(scratch);
    if(b.app56)function_291b0(m,*b.app56,saved_page);
    function_304f4(m,saved_y,saved_x);
    if(*dg(m,0xEF3Cu)==5u && saved_aux!=0xFFu && *dg(m,0xEF4Eu)!=0u && b.app59)
        function_2a104(*b.app59,saved_aux);
    return result;
}
static u32 function_2f5f4(Machine& m,Application70Boundary& b,u32 text,u32 mode) noexcept {
    return app70_present(m,b,text,nullptr,mode);
}
static_assert(0x2F839u-0x2F5F4u==581u);

void function_2f83c(Machine& m) noexcept { *dg(m,0xF0B1u)=static_cast<u8>(*dg(m,0xF0B1u)|0x80u); }
static_assert(0x2F844u-0x2F83Cu==8u);
void function_2f844(Machine& m) noexcept { *dg(m,0xF0B1u)=static_cast<u8>(*dg(m,0xF0B1u)&0x7Fu); }
static_assert(0x2F84Cu-0x2F844u==8u);
[[nodiscard]] u32 function_2f84c(Machine& m) noexcept { return ((*dg(m,0xF0B1u)&0x80u)==0u)?1u:0u; }
static_assert(0x2F85Fu-0x2F84Cu==19u);

// 2F860 falls through into 2F868 in the binary.
void function_2f860(Machine& m,Application70Boundary& b) noexcept {
    *dg(m,0xF0B1u)=static_cast<u8>(*dg(m,0xF0B1u)|1u);
    function_2f868(m,b);
}
static_assert(0x2F868u-0x2F860u==8u);

static void function_2f868(Machine& m,Application70Boundary& b) noexcept {
    if(load_le32(dg(m,0xF0C8u))!=0u && b.call_indirect_f0c8)b.call_indirect_f0c8();
    const u8 flags=*dg(m,0xF0B1u);
    if(flags==0u || (flags&0x80u)!=0u || *dg(m,0xF0B2u)!=0u)return;

    *dg(m,0xF0B2u)=1u;
    *dg(m,0xF0B1u)=0u;
    u8 msg[0x50]{};
    for(u32 i=0;i<6u;++i)msg[i]=*dg(m,0x1448u+i);
    if(load_le32(dg(m,0xF0BCu))!=0u)app70_append_dg_z_pairs(m,msg,load_le32(dg(m,0xF0BCu)));
    app70_append_dg_z_pairs(m,msg,0x1450u);

    const u32 key=app70_present(m,b,0u,msg,1u);
    if(key==0x59u){
        if(b.app69 && b.app69->app56)function_290f4(m,*b.app69->app56,0xFFu);
        if(b.app69)function_2f4e0(m,*b.app69);
        if(b.app69)function_2f540(*b.app69,1u);
    }
    *dg(m,0xF0B2u)=0u;
    *dg(m,0xF0B1u)=0u;
}
static_assert(0x2F960u-0x2F868u==248u);

void function_2f960(Machine& m,u32 stream) noexcept { store_le32(dg(m,0xF568u),stream); }
static_assert(0x2F96Au-0x2F960u==10u);

void function_2f96c(Machine& m,Application70Boundary& b) noexcept {
    const u32 pos=b.call_3131c?b.call_3131c(load_le32(dg(m,0xF568u)),1u,0u):0u;
    const u32 need=0x400u-(pos&0x3FFu);
    const u32 got=b.call_312ec?b.call_312ec(0xF0D4u,need,load_le32(dg(m,0xF568u))):0u;
    store_le32(dg(m,0xF4D4u),0xF0D4u);
    store_le32(dg(m,0xF4D8u),got);
}
static_assert(0x2F9AEu-0x2F96Cu==66u);

void function_2f9b0(Machine& m,Application70Boundary& b) noexcept {
    if(load_le32(dg(m,0xF4D8u))!=0u)return;
    const u32 got=b.call_312ec?b.call_312ec(0xF0D4u,0x400u,load_le32(dg(m,0xF568u))):0u;
    store_le32(dg(m,0xF4D4u),0xF0D4u);
    store_le32(dg(m,0xF4D8u),got);
}
static_assert(0x2F9E1u-0x2F9B0u==49u);

static u8 function_2f9e4(Machine& m,Application70Boundary& b) noexcept {
    if(load_le32(dg(m,0xF4D8u))==0u)function_2f9b0(m,b); // duplicated JNE at 2F9ED is preserved semantically.
    const u32 p=load_le32(dg(m,0xF4D4u));
    const u8 value=*dg(m,p);
    store_le32(dg(m,0xF4D4u),p+1u);
    store_le32(dg(m,0xF4D8u),load_le32(dg(m,0xF4D8u))-1u);
    return value;
}
static_assert(0x2FA2Fu-0x2F9E4u==75u);

void function_2fa30(Machine& m,Application70Boundary& b,u32 value) noexcept {
    u32 n=load_le32(dg(m,0x2B68Cu))+1u;
    store_le32(dg(m,0x2B68Cu),n);
    if(*dg(m,0x2B6C3u)!=0u){
        if(*dg(m,0x2B6C2u)!=0u)return;
        *dg(m,load_le32(dg(m,0x2B694u))+n-1u)=static_cast<u8>(value);
        if(n>=load_le32(dg(m,0x2B6B4u))){
            function_2f868(m,b);
            const u32 desc=load_le32(dg(m,0x2B6A4u));
            function_2a20c(m,load_le32(dg(m,0x2B688u)),load_le32(dg(m,0x2B6B4u)),load_le32(dg(m,0x2B694u)),load_le32(dg(m,0x2B6BCu)),*dg(m,desc+5u));
            store_le32(dg(m,0x2B68Cu),0u);
            store_le32(dg(m,0x2B688u),load_le32(dg(m,0x2B688u))+1u);
        }
        const u32 desc=load_le32(dg(m,0x2B6A4u));
        if(static_cast<s16>(load_le16(dg(m,desc)))<=static_cast<s32>(load_le32(dg(m,0x2B688u))))
            *dg(m,0x2B6C2u)=1u;
    } else {
        if(*dg(m,0x2B6C2u)!=0u)return;
        *dg(m,load_le32(dg(m,0x2B698u))+n-1u)=static_cast<u8>(value);
        if(n>=load_le32(dg(m,0x2B6B4u))){
            function_2f868(m,b);
            store_le32(dg(m,0x2B68Cu),0u);
            *dg(m,0x2B6C2u)=1u;
        }
    }
}
static_assert(0x2FB01u-0x2FA30u==209u);

[[nodiscard]] u32 function_2fb04(Machine& m,Application70Boundary& b,u32 out_or_desc,u32 descriptor,u32 data,u32 count,u32 mode) noexcept {
    *dg(m,0x2B6C3u)=static_cast<u8>(mode);
    *dg(m,0x2B6C2u)=0u;
    store_le32(dg(m,0x2B6A4u),descriptor);
    store_le32(dg(m,0x2B68Cu),0u);
    store_le32(dg(m,0x2B6B4u),count);
    store_le32(dg(m,0x2B688u),0u);
    store_le32(dg(m,0x2B6BCu),out_or_desc);
    if(static_cast<u8>(mode)!=0u)store_le32(dg(m,0x2B694u),data);
    else store_le32(dg(m,0x2B698u),data);
    return b.call_37700?b.call_37700():0u; // encoded tail JMP.
}
static_assert(0x2FB66u-0x2FB04u==98u);

void function_2fb68(Machine& m,Application70Boundary& b,u32 raw_count,u32 dst) noexcept {
    u32 count=raw_count;
    if(load_le32(dg(m,0xF4D8u))==0u)function_2f9b0(m,b);
    const u32 avail=load_le32(dg(m,0xF4D8u));
    auto copy=[&](u32 d,u32 s,u32 n){for(u32 i=0;i<n;++i)*dg(m,d+i)=*dg(m,s+i);};
    if(static_cast<s32>(count)<=static_cast<s32>(avail)){
        copy(dst,load_le32(dg(m,0xF4D4u)),count);
        store_le32(dg(m,0xF4D4u),load_le32(dg(m,0xF4D4u))+count);
        store_le32(dg(m,0xF4D8u),load_le32(dg(m,0xF4D8u))-count);
        return;
    }
    copy(dst,load_le32(dg(m,0xF4D4u)),avail);
    count-=avail; dst+=avail;
    const u32 got=b.call_312ec?b.call_312ec(0xF0D4u,0x400u,load_le32(dg(m,0xF568u))):0u;
    store_le32(dg(m,0xF4D8u),got);
    copy(dst,0xF0D4u,count);
    store_le32(dg(m,0xF4D4u),0xF0D4u+count);
    store_le32(dg(m,0xF4D8u),load_le32(dg(m,0xF4D8u))-count);
}
static_assert(0x2FC46u-0x2FB68u==222u);

static u8 function_2fc48(Machine& m,Application70Boundary& b) noexcept {
    if(*dg(m,0xF4DCu)==0u){
        store_le32(dg(m,0xF4E0u),0xF4E4u);
        const s8 ctrl=static_cast<s8>(function_2f9e4(m,b));
        if(ctrl>=0){
            const u8 n=static_cast<u8>(ctrl+1);
            *dg(m,0xF4DCu)=n;
            function_2fb68(m,b,n,0xF4E4u);
        } else {
            const u8 n=static_cast<u8>(static_cast<u8>(0u-static_cast<u8>(ctrl))+1u);
            *dg(m,0xF4DCu)=n;
            const u8 value=function_2f9e4(m,b);
            *dg(m,0xF4E4u)=value;
            if(b.call_30d30)b.call_30d30(0xF4E5u,value,static_cast<u32>(n-1u));
        }
    }
    const u32 p=load_le32(dg(m,0xF4E0u));
    const u8 value=*dg(m,p);
    store_le32(dg(m,0xF4E0u),p+1u);
    *dg(m,0xF4DCu)=static_cast<u8>(*dg(m,0xF4DCu)-1u);
    return value;
}
static_assert(0x2FD0Cu-0x2FC48u==196u);

void function_2fd0c(Machine& m,Application70Boundary& b,u32 dst,s32 a,s32 c) noexcept {
    const s32 total=static_cast<s32>(static_cast<u32>(a)*static_cast<u32>(c)*8u);
    for(s32 i=0;i<total;++i)*dg(m,dst+static_cast<u32>(i))=function_2fc48(m,b);
}
static_assert(0x2FD3Bu-0x2FD0Cu==47u);

void function_2fd3c(Machine& m,Application70Boundary& b,u32 dst,s32 a,s32 c) noexcept {
    const s32 total=static_cast<s32>(static_cast<u32>(a)*static_cast<u32>(c)*8u);
    for(s32 i=0;i<total;++i)*dg(m,dst+static_cast<u32>(i))=function_2f9e4(m,b);
}
static_assert(0x2FD8Au-0x2FD3Cu==78u);

static constexpr u32 kApp70DecodeJumpTable[4]={0x2FEBCu,0x2FF32u,0x2FFA4u,0x2FFABu};
static_assert(sizeof(kApp70DecodeJumpTable)==16u);

static u32 app70_load_image_from_header(Machine& m,Application70Boundary& b,u32 hdr,u32 stream) noexcept {
    store_le32(dg(m,0xF568u),stream);
    const s32 frames=static_cast<s16>(load_le16(dg(m,hdr)));
    if(frames==0)function_2f580(m,b,0x145Cu);
    const s32 rows=static_cast<s16>(load_le16(dg(m,hdr+2u)));
    const u32 planes=*dg(m,hdr+4u);
    const u32 tag=*dg(m,hdr+5u);
    const u32 image_size=function_2a1e8(static_cast<u32>(frames),static_cast<u32>(rows),planes,8u,tag);
    if(frames==0){ if(b.divide_error)b.divide_error(); return 0u; }
    const s32 q=static_cast<s32>(image_size)/frames;
    const u32 image=b.call_3104c?b.call_3104c(image_size+8u):0u;
    store_le16(dg(m,image),static_cast<u16>(q));
    store_le16(dg(m,image+2u),static_cast<u16>(frames));
    store_le16(dg(m,image+4u),static_cast<u16>(rows));
    *dg(m,image+6u)=static_cast<u8>(planes);
    *dg(m,image+7u)=tag==0xFFu?0u:0x80u;

    const u32 row_bytes=static_cast<u32>(rows)*planes*8u;
    const u32 scratch=b.call_3104c?b.call_3104c(row_bytes):0u;
    if(scratch==0u)function_2f580(m,b,0x1480u);
    function_2f96c(m,b);

    const u8 mode=*dg(m,hdr+6u);
    if(mode==0u || mode==1u){
        for(s32 frame=0;frame<frames;++frame){
            function_2f868(m,b);
            for(u32 i=0;i<row_bytes;++i)*dg(m,scratch+i)=mode==0u?function_2f9e4(m,b):function_2fc48(m,b);
            function_2a20c(m,static_cast<u32>(frame),row_bytes,scratch,image,tag);
        }
    } else if(mode==2u){
        function_2f580(m,b,0x14A4u);
    } else if(mode==3u){
        (void)function_2fb04(m,b,image,hdr,scratch,row_bytes,1u);
    } else {
        function_2f580(m,b,0x14C0u);
    }

    if(b.call_310b0)b.call_310b0(scratch);
    if(*dg(m,hdr+7u)!=0u && b.call_37810)b.call_37810(image);
    return image;
}
static u32 function_2fd9c(Machine& m,Application70Boundary& b,u32 item_record,u32 stream) noexcept {
    return app70_load_image_from_header(m,b,load_le32(dg(m,item_record+5u)),stream);
}
static_assert(0x30040u-0x2FD9Cu==676u);

void function_30040(Application70Boundary& b,u32 p) noexcept { if(b.call_310b0)b.call_310b0(p); }
static_assert(0x3004Du-0x30040u==13u);

[[nodiscard]] u32 function_30050(Machine& m,u32 p) noexcept {
    u32 eax=p;
    const s32 prod=static_cast<s16>(load_le16(dg(m,p)))*static_cast<s16>(load_le16(dg(m,p+2u)));
    eax=(eax&0xFFFF0000u)|static_cast<u16>(prod);
    eax+=8u;
    if((*dg(m,p+7u)&0x80u)!=0u)++eax;
    return eax;
}
static_assert(0x30070u-0x30050u==32u);

[[nodiscard]] u32 function_30070(Machine& m,Application70Boundary& b,s32 x,s32 y,s32 w,s32 h,u32 tag) noexcept {
    if((static_cast<u8>(y)&3u)!=0u || (static_cast<u8>(h)&7u)!=0u)function_2f580(m,b,0x14E4u);
    const s32 q=h/8;
    const u32 size=function_2a1e8(1u,static_cast<u32>(w),static_cast<u32>(q),8u,tag);
    const u32 image=b.call_3104c?b.call_3104c(static_cast<u16>(size)+8u):0u;
    store_le16(dg(m,image),static_cast<u16>(size));
    store_le16(dg(m,image+2u),1u);
    store_le16(dg(m,image+4u),static_cast<u16>(w));
    *dg(m,image+6u)=static_cast<u8>(q);
    *dg(m,image+7u)=tag==0xFFu?0u:0x80u;
    u32 target=image+8u+(tag==0xFFu?0u:1u);
    if(b.app56)function_292bc(m,*b.app56,target,x,y,w,h);
    if(tag!=0xFFu)*dg(m,image+8u)=static_cast<u8>(tag);
    return image;
}
static_assert(0x3012Cu-0x30070u==188u);

[[nodiscard]] u32 function_3012c(Machine& m,Application70Boundary& b,u32 image,u32 header_word,s32 x,s32 y,s32 w,s32 h,u32 tag) noexcept {
    const s32 q=h/8;
    store_le16(dg(m,image),static_cast<u16>(header_word));
    store_le16(dg(m,image+4u),static_cast<u16>(w));
    store_le16(dg(m,image+2u),1u);
    *dg(m,image+6u)=static_cast<u8>(q);
    *dg(m,image+7u)=tag==0xFFu?0u:0x80u;
    u32 target=image+8u+(tag==0xFFu?0u:1u);
    if(b.app56)function_292bc(m,*b.app56,target,x,y,w,h);
    if(tag!=0xFFu)*dg(m,image+8u)=static_cast<u8>(tag);
    return image;
}
static_assert(0x301A8u-0x3012Cu==124u);

[[nodiscard]] u32 function_301a8(Machine& m,Application70Boundary& b,u32 rec,u32 stream) noexcept {
    bool restore=false;
    if(function_2f84c(m)!=0u){function_2f83c(m);restore=true;}
    const u32 payload=load_le32(dg(m,rec+5u));
    const u32 hdr=static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,rec+3u)))))+payload-8u;
    const s32 len=static_cast<s16>(load_le16(dg(m,hdr)));
    const u32 block=b.call_3104c?b.call_3104c(static_cast<u32>(len+4)):0u;
    for(s32 i=0;i<len;++i)*dg(m,block+3u+static_cast<u32>(i))=*dg(m,payload+static_cast<u32>(i));
    // The compiler copies the nine-byte descriptor to a local, rewrites local
    // word +3 to 8 and local dword +5 to hdr, then 2FD9C consumes only +5.
    const u32 image=app70_load_image_from_header(m,b,hdr,stream);
    store_le32(dg(m,block),image);
    if(restore)function_2f844(m);
    return block;
}
static_assert(0x30249u-0x301A8u==161u);

void function_3024c(Machine& m,Application70Boundary& b,u32 p) noexcept {
    const u32 inner=load_le32(dg(m,p));
    if(b.call_310b0)b.call_310b0(inner);
    if(b.call_310b0)b.call_310b0(p);
}
static_assert(0x30268u-0x3024Cu==28u);

[[nodiscard]] u32 function_30268(Machine& m,u32 text,s8 delta) noexcept {
    if(text==0u)return 0u;
    for(u32 p=text;*dg(m,p)!=0u;++p)*dg(m,p)=static_cast<u8>(*dg(m,p)+static_cast<u8>(delta));
    return text;
}
static_assert(0x30286u-0x30268u==30u);

static u32 function_30288(Machine& m,Application70Boundary& b,u32 text,u32 font_item) noexcept {
    const u32 saved=load_le32(dg(m,0x2B67Cu));
    const u32 font=app70_font(m,b,font_item);
    const u32 width=app70_text_width_from_font(m,text,font,0u,false);
    store_le32(dg(m,0x2B67Cu),font);
    (void)saved; // load is architecturally present but overwritten before return.
    return width;
}
static_assert(0x302C9u-0x30288u==65u);

[[nodiscard]] u32 function_302cc(Machine& m,Application70Boundary& b,u32 text,u32 font_item,u32 count) noexcept {
    const u32 font=app70_font(m,b,font_item);
    const u32 width=app70_text_width_from_font(m,text,font,count,true);
    store_le32(dg(m,0x2B67Cu),font);
    return width;
}
static_assert(0x30318u-0x302CCu==76u);

[[nodiscard]] u32 function_30318(Machine& m,Application70Boundary& b,s8 ch,u32 font_item) noexcept {
    const u32 font=app70_font(m,b,font_item); store_le32(dg(m,0x2B67Cu),font);
    return *dg(m,static_cast<u32>(static_cast<s32>(font)+static_cast<s32>(ch)+4));
}
static_assert(0x30336u-0x30318u==30u);

[[nodiscard]] u32 function_30338(Machine& m,Application70Boundary& b,u32 font_item) noexcept {
    const u32 font=app70_font(m,b,font_item); store_le32(dg(m,0x2B67Cu),font);
    const u32 p=load_le32(dg(m,font)); return load_le16(dg(m,p+4u));
}
static_assert(0x30352u-0x30338u==26u);

static void function_30354(Machine& m,Application70Boundary& b,s8 ch,u32 font_item) noexcept {
    const u32 font=app70_font(m,b,font_item); store_le32(dg(m,0x2B67Cu),font);
    const u32 image=load_le32(dg(m,font));
    if(load_le32(dg(m,0xF56Cu))!=0u){ if(b.app56)function_293ec(*b.app56,static_cast<u32>(static_cast<s32>(ch)),load_le32(dg(m,0x2B684u)),load_le32(dg(m,0x2B680u)),image,load_le32(dg(m,0xF56Cu))); }
    else { if(b.app56)function_2939c(*b.app56,static_cast<u32>(static_cast<s32>(ch)),load_le32(dg(m,0x2B684u)),load_le32(dg(m,0x2B680u)),image); }
    const u32 adv=*dg(m,static_cast<u32>(static_cast<s32>(font)+static_cast<s32>(ch)+4));
    store_le32(dg(m,0x2B680u),load_le32(dg(m,0x2B680u))+adv);
}
static_assert(0x303C7u-0x30354u==115u);

static void function_303c8(Machine& m,Application70Boundary& b,u32 text,u32 font_item) noexcept {
    const u32 font=app70_font(m,b,font_item); store_le32(dg(m,0x2B67Cu),font);
    if(text==0u)return;
    while(*dg(m,text)!=0u){
        const s8 ch=static_cast<s8>(*dg(m,text++));
        const u32 image=load_le32(dg(m,font));
        if(load_le32(dg(m,0xF56Cu))!=0u){if(b.app56)function_293ec(*b.app56,static_cast<u32>(static_cast<s32>(ch)),load_le32(dg(m,0x2B684u)),load_le32(dg(m,0x2B680u)),image,load_le32(dg(m,0xF56Cu)));}
        else {if(b.app56)function_2939c(*b.app56,static_cast<u32>(static_cast<s32>(ch)),load_le32(dg(m,0x2B684u)),load_le32(dg(m,0x2B680u)),image);}
        const u32 adv=*dg(m,static_cast<u32>(static_cast<s32>(font)+static_cast<s32>(ch)+4));
        store_le32(dg(m,0x2B680u),load_le32(dg(m,0x2B680u))+adv);
    }
}
static_assert(0x30457u-0x303C8u==143u);

void function_30458(Machine& m,Application70Boundary& b,u32 text,u32 font_item,u32 count) noexcept {
    const u32 font=app70_font(m,b,font_item); store_le32(dg(m,0x2B67Cu),font);
    if(text==0u)return;
    while(*dg(m,text)!=0u && count!=0u){
        const s8 ch=static_cast<s8>(*dg(m,text++)); --count;
        const u32 image=load_le32(dg(m,font));
        if(load_le32(dg(m,0xF56Cu))!=0u){if(b.app56)function_293ec(*b.app56,static_cast<u32>(static_cast<s32>(ch)),load_le32(dg(m,0x2B684u)),load_le32(dg(m,0x2B680u)),image,load_le32(dg(m,0xF56Cu)));}
        else {if(b.app56)function_2939c(*b.app56,static_cast<u32>(static_cast<s32>(ch)),load_le32(dg(m,0x2B684u)),load_le32(dg(m,0x2B680u)),image);}
        const u32 adv=*dg(m,static_cast<u32>(static_cast<s32>(font)+static_cast<s32>(ch)+4));
        store_le32(dg(m,0x2B680u),load_le32(dg(m,0x2B680u))+adv);
    }
}
static_assert(0x304F4u-0x30458u==156u);

static void function_304f4(Machine& m,u32 y,u32 x) noexcept { store_le32(dg(m,0x2B684u),y); store_le32(dg(m,0x2B680u),x); }
static_assert(0x30507u-0x304F4u==19u);

void function_30508(Machine& m,Application70Boundary& b,u32 y,u32 x,u32 width,u32 text,u32 font_item) noexcept {
    if(text==0u)return;
    (void)app70_font(m,b,font_item);
    store_le32(dg(m,0x2B684u),y);
    const u32 font=app70_font(m,b,font_item); store_le32(dg(m,0x2B67Cu),font);
    const u32 tw=app70_text_width_from_font(m,text,font,0u,false);
    const s32 centered=static_cast<s32>(width-tw)>>1;
    store_le32(dg(m,0x2B680u),x+static_cast<u32>(centered));
    function_303c8(m,b,text,font_item);
}
static_assert(0x3058Bu-0x30508u==131u);

static u32 function_3058c(Machine& m) noexcept { return load_le32(dg(m,0x2B684u)); }
static_assert(0x30592u-0x3058Cu==6u);
static u32 function_30594(Machine& m) noexcept { return load_le32(dg(m,0x2B680u)); }
static_assert(0x3059Au-0x30594u==6u);
void function_3059c(Machine& m,u32 color_or_mask) noexcept { store_le32(dg(m,0xF56Cu),color_or_mask); }
static_assert(0x305A6u-0x3059Cu==10u);

static_assert(
    (0x2F5F1u-0x2F580u)+(0x2F839u-0x2F5F4u)+(0x2F844u-0x2F83Cu)+
    (0x2F84Cu-0x2F844u)+(0x2F85Fu-0x2F84Cu)+(0x2F868u-0x2F860u)+
    (0x2F960u-0x2F868u)+(0x2F96Au-0x2F960u)+(0x2F9AEu-0x2F96Cu)+
    (0x2F9E1u-0x2F9B0u)+(0x2FA2Fu-0x2F9E4u)+(0x2FB01u-0x2FA30u)+
    (0x2FB66u-0x2FB04u)+(0x2FC46u-0x2FB68u)+(0x2FD0Cu-0x2FC48u)+
    (0x2FD3Bu-0x2FD0Cu)+(0x2FD8Au-0x2FD3Cu)+(0x30040u-0x2FD9Cu)+
    (0x3004Du-0x30040u)+(0x30070u-0x30050u)+(0x3012Cu-0x30070u)+
    (0x301A8u-0x3012Cu)+(0x30249u-0x301A8u)+(0x30268u-0x3024Cu)+
    (0x30286u-0x30268u)+(0x302C9u-0x30288u)+(0x30318u-0x302CCu)+
    (0x30336u-0x30318u)+(0x30352u-0x30338u)+(0x303C7u-0x30354u)+
    (0x30457u-0x303C8u)+(0x304F4u-0x30458u)+(0x30507u-0x304F4u)+
    (0x3058Bu-0x30508u)+(0x30592u-0x3058Cu)+(0x3059Au-0x30594u)+
    (0x305A6u-0x3059Cu)==4070u);
