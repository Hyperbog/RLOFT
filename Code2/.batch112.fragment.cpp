// Batch 112: 35418..3562E DOS/extender startup body. 3562F is a shared-entry continuation.
struct Runtime112Boundary {
    // Interrupt callbacks receive the register/segment image exactly at the encoded INT.
    void (*int21)(Machine&)=nullptr;
    void (*int31)(Machine&)=nullptr;

    // Access to non-DGROUP selectors used for PSP, command-tail, and environment data.
    u8  (*read_segment8)(u16,u32)=nullptr;
    u16 (*read_segment16)(u16,u32)=nullptr;
    u32 (*read_segment32)(u16,u32)=nullptr;
    void (*write_segment8)(u16,u32,u8)=nullptr;
    void (*write_segment16)(u16,u32,u16)=nullptr;

    void (*call_395e2)(Machine&)=nullptr;
    void (*call_39589)(Machine&)=nullptr;
    void (*tail_3562f)(Machine&)=nullptr;
};

static u8 app112_lo8(u32 v) noexcept { return static_cast<u8>(v); }
static u16 app112_lo16(u32 v) noexcept { return static_cast<u16>(v); }
static void app112_set_lo8(u32& r,u8 v) noexcept { r=(r&0xFFFFFF00u)|static_cast<u32>(v); }
static void app112_set_hi8(u32& r,u8 v) noexcept { r=(r&0xFFFF00FFu)|(static_cast<u32>(v)<<8); }
static void app112_set_lo16(u32& r,u16 v) noexcept { r=(r&0xFFFF0000u)|static_cast<u32>(v); }

static u8 app112_seg8(Machine& m,Runtime112Boundary& b,u16 dgroup,u16 seg,u32 off) noexcept {
    if(seg==dgroup)return *dg(m,off);
    return b.read_segment8?b.read_segment8(seg,off):0u;
}
static u16 app112_seg16(Machine& m,Runtime112Boundary& b,u16 dgroup,u16 seg,u32 off) noexcept {
    if(seg==dgroup)return load_le16(dg(m,off));
    return b.read_segment16?b.read_segment16(seg,off):0u;
}
static u32 app112_seg32(Machine& m,Runtime112Boundary& b,u16 dgroup,u16 seg,u32 off) noexcept {
    if(seg==dgroup)return load_le32(dg(m,off));
    return b.read_segment32?b.read_segment32(seg,off):0u;
}
static void app112_seg_store8(Machine& m,Runtime112Boundary& b,u16 dgroup,u16 seg,u32 off,u8 v) noexcept {
    if(seg==dgroup)*dg(m,off)=v;
    else if(b.write_segment8)b.write_segment8(seg,off,v);
}
static void app112_seg_store16(Machine& m,Runtime112Boundary& b,u16 dgroup,u16 seg,u32 off,u16 v) noexcept {
    if(seg==dgroup)store_le16(dg(m,off),v);
    else if(b.write_segment16)b.write_segment16(seg,off,v);
}

void function_35418(Machine& m,Runtime112Boundary& b) noexcept {
    const u16 dgroup=m.cpu.ds;
    m.cpu.interrupt_enable=true;                         // STI
    m.cpu.esp&=0xFFFFFFFCu;
    m.cpu.ebx=m.cpu.esp;
    store_le32(dg(m,0x12648u),m.cpu.ebx);
    store_le32(dg(m,0x12634u),m.cpu.ebx);
    app112_set_lo16(m.cpu.eax,0x0024u);
    store_le16(dg(m,0x12640u),app112_lo16(m.cpu.eax));

    m.cpu.ebx=0x50484152u;
    m.cpu.eax=0u;
    app112_set_hi8(m.cpu.eax,0x30u);
    if(b.int21)b.int21(m);
    *dg(m,0x1266Bu)=app112_lo8(m.cpu.eax);
    *dg(m,0x1266Cu)=static_cast<u8>(m.cpu.eax>>8);
    m.cpu.ecx=m.cpu.eax;
    m.cpu.esi=0u;
    m.cpu.edi=0x81u;
    m.cpu.eax>>=16;

    if(app112_lo16(m.cpu.eax)==0x4458u) {
        const u8 bl=static_cast<u8>(app112_lo8(m.cpu.ebx)-0x30u);
        app112_set_lo8(m.cpu.ebx,bl);
        app112_set_lo8(m.cpu.eax,bl);
        app112_set_hi8(m.cpu.eax,0u);
        const u32 saved_eax=m.cpu.eax;
        m.cpu.esp-=4u;
        m.cpu.es=load_le16(dg(m,0x12640u));
        m.cpu.ebx=app112_seg32(m,b,dgroup,m.cpu.es,0x5Cu);
        m.cpu.ebx+=0x0FFFu;
        m.cpu.ebx&=0xFFFFF000u;
        store_le32(dg(m,0x12634u),m.cpu.ebx);
        m.cpu.ebx>>=12;
        app112_set_lo16(m.cpu.eax,m.cpu.ds);
        m.cpu.es=app112_lo16(m.cpu.eax);
        app112_set_hi8(m.cpu.eax,0x4Au);
        if(b.int21)b.int21(m);
        m.cpu.eax=saved_eax;
        m.cpu.esp+=4u;
        app112_set_lo16(m.cpu.ebx,m.cpu.ds);
        app112_set_lo16(m.cpu.ecx,0x002Cu);
    } else if(app112_lo16(m.cpu.eax)==0x4243u) {
        store_le32(dg(m,0x10268u),m.cpu.edx);
        m.cpu.esi=m.cpu.edx;
        m.cpu.edx=load_le32(dg(m,m.cpu.esi+0x1Cu));
        m.cpu.ebx=m.cpu.esp-m.cpu.edx;
        app112_set_hi8(m.cpu.eax,0x4Au);
        if(b.int21)b.int21(m);
        app112_set_lo16(m.cpu.ebx,m.cpu.ds);
        store_le16(dg(m,0x12640u),m.cpu.ds);
        m.cpu.eax=load_le32(dg(m,m.cpu.esi+0x10u));
        m.cpu.edi+=m.cpu.eax;
        m.cpu.esi=0u;
        app112_set_lo16(m.cpu.esi,load_le16(dg(m,m.cpu.eax+0x2Cu)));
        m.cpu.esi<<=4;
        app112_set_lo16(m.cpu.ecx,m.cpu.ds);
        app112_set_lo8(m.cpu.eax,9u);
        app112_set_hi8(m.cpu.eax,0u);
    } else {
        app112_set_lo16(m.cpu.edx,0x0078u);
        app112_set_lo16(m.cpu.eax,0xFF00u);
        if(b.int21)b.int21(m);
        if(app112_lo8(m.cpu.eax)!=0u) {
            app112_set_lo16(m.cpu.eax,m.cpu.gs);
            if(app112_lo16(m.cpu.eax)!=0u)store_le16(dg(m,0x1026Cu),app112_lo16(m.cpu.eax));
            app112_set_lo16(m.cpu.eax,0x0006u);
            app112_set_lo16(m.cpu.ebx,m.cpu.ds);
            if(b.int31)b.int31(m);
            app112_set_lo8(m.cpu.eax,1u);
            app112_set_hi8(m.cpu.eax,0u);
            const u16 joined=static_cast<u16>(app112_lo16(m.cpu.edx)|app112_lo16(m.cpu.ecx));
            app112_set_lo16(m.cpu.edx,joined);
            if(joined!=0u)app112_set_hi8(m.cpu.eax,1u);
            store_le16(dg(m,0x12640u),m.cpu.es);
            app112_set_lo16(m.cpu.ecx,app112_seg16(m,b,dgroup,m.cpu.es,0x2Cu));
        } else {
            app112_set_lo16(m.cpu.edx,m.cpu.ds);
            app112_set_lo16(m.cpu.ecx,0x0024u);
            m.cpu.ds=app112_lo16(m.cpu.ecx);
            app112_set_lo16(m.cpu.ecx,0x002Cu);
            m.cpu.ds=app112_lo16(m.cpu.edx);
            app112_set_lo16(m.cpu.ebx,0x0017u);
            app112_set_lo8(m.cpu.eax,0u);
            app112_set_hi8(m.cpu.eax,0u);
        }
    }

    *dg(m,0x12662u)=app112_lo8(m.cpu.eax);
    *dg(m,0x12663u)=static_cast<u8>(m.cpu.eax>>8);
    m.cpu.es=app112_lo16(m.cpu.ebx);
    app112_seg_store16(m,b,dgroup,m.cpu.es,0x35675u,m.cpu.ds);
    store_le32(dg(m,0x12665u),m.cpu.esi);
    store_le16(dg(m,0x12669u),app112_lo16(m.cpu.ecx));

    const u32 saved_esi=m.cpu.esi;
    m.cpu.esp-=4u;                                      // PUSH ESI
    m.cpu.es=load_le16(dg(m,0x12640u));
    m.cpu.edx=0x0002BD0Cu;
    m.cpu.edx+=0x0Fu;
    app112_set_lo8(m.cpu.edx,static_cast<u8>(app112_lo8(m.cpu.edx)&0xF0u));
    bool zf=(app112_lo8(m.cpu.edx)==0u);                // AND DL,F0 supplies ZF if REP count is zero.
    m.cpu.ecx=0u;
    app112_set_lo8(m.cpu.ecx,app112_seg8(m,b,dgroup,m.cpu.es,m.cpu.edi-1u));
    app112_set_lo8(m.cpu.eax,0x20u);
    while(m.cpu.ecx!=0u) {
        const u8 value=app112_seg8(m,b,dgroup,m.cpu.es,m.cpu.edi);
        ++m.cpu.edi;
        --m.cpu.ecx;
        zf=(value==app112_lo8(m.cpu.eax));
        if(!zf)break;
    }
    m.cpu.esi=m.cpu.edi-1u;
    m.cpu.edi=m.cpu.edx;
    app112_set_lo16(m.cpu.ebx,m.cpu.es);
    app112_set_lo16(m.cpu.edx,m.cpu.ds);
    m.cpu.ds=app112_lo16(m.cpu.ebx);
    m.cpu.es=app112_lo16(m.cpu.edx);
    if(!zf)++m.cpu.ecx;
    while(m.cpu.ecx!=0u) {
        const u8 value=app112_seg8(m,b,dgroup,m.cpu.ds,m.cpu.esi++);
        app112_seg_store8(m,b,dgroup,m.cpu.es,m.cpu.edi++,value);
        --m.cpu.ecx;
    }
    app112_set_lo8(m.cpu.eax,0u);
    app112_seg_store8(m,b,dgroup,m.cpu.es,m.cpu.edi++,0u);
    app112_seg_store8(m,b,dgroup,m.cpu.es,m.cpu.edi++,0u);
    m.cpu.esi=saved_esi;
    m.cpu.esp+=4u;                                      // POP ESI
    --m.cpu.edi;
    const u32 saved_edi=m.cpu.edi;
    m.cpu.esp-=4u;                                      // PUSH EDI
    const u32 saved_edx=m.cpu.edx;
    m.cpu.esp-=4u;                                      // PUSH EDX
    m.cpu.ds=app112_seg16(m,b,dgroup,m.cpu.es,0x12669u);
    m.cpu.ebp=0u;

    for(;;) {
        const u32 head=app112_seg32(m,b,dgroup,m.cpu.ds,m.cpu.esi)|0x20202020u;
        if(head==0x37386F6Eu && app112_seg8(m,b,dgroup,m.cpu.ds,m.cpu.esi+4u)==0x3Du)++m.cpu.ebp;
        for(;;) {
            const u8 value=app112_seg8(m,b,dgroup,m.cpu.ds,m.cpu.esi);
            app112_set_lo8(m.cpu.eax,value);
            ++m.cpu.esi;
            if(value==0u)break;
        }
        if(app112_seg8(m,b,dgroup,m.cpu.ds,m.cpu.esi)!=0u)continue;
        app112_set_lo8(m.cpu.eax,app112_seg8(m,b,dgroup,m.cpu.ds,m.cpu.esi));
        ++m.cpu.esi;
        m.cpu.esi+=2u;
        break;
    }
    for(;;) {
        const u8 value=app112_seg8(m,b,dgroup,m.cpu.ds,m.cpu.esi++);
        app112_set_lo8(m.cpu.eax,value);
        app112_seg_store8(m,b,dgroup,m.cpu.es,m.cpu.edi++,value);
        if(value==0u)break;
    }

    m.cpu.ds=app112_lo16(saved_edx);                    // POP DS consumes the saved EDX dword.
    m.cpu.esp+=4u;
    m.cpu.esi=saved_edi;                               // POP ESI consumes the saved EDI dword.
    m.cpu.esp+=4u;
    m.cpu.ebx=m.cpu.esp;
    store_le16(dg(m,0x12660u),app112_lo16(m.cpu.ebp));
    store_le32(dg(m,0x12644u),m.cpu.edi);
    store_le32(dg(m,0x12630u),m.cpu.ebx);

    m.cpu.ecx=0x0002BD0Cu;
    m.cpu.edi=0x00012740u;
    m.cpu.ecx-=m.cpu.edi;
    if(*dg(m,0x12662u)==1u && m.cpu.ecx>0x1000u)m.cpu.ecx=0x1000u;
    app112_set_lo8(m.cpu.edx,app112_lo8(m.cpu.ecx));
    const u32 clear_count=m.cpu.ecx;
    m.cpu.eax=0u;
    for(u32 i=0u;i<clear_count;++i)app112_seg_store8(m,b,dgroup,m.cpu.es,m.cpu.edi+i,0u);
    m.cpu.edi+=clear_count;
    m.cpu.ecx=0u;

    m.cpu.eax=0x0002BD0Cu;
    m.cpu.eax+=0x0Fu;
    app112_set_lo8(m.cpu.eax,static_cast<u8>(app112_lo8(m.cpu.eax)&0xF0u));
    store_le32(dg(m,0x12638u),m.cpu.eax);
    store_le32(dg(m,0x1263Cu),m.cpu.esi);
    m.cpu.eax=0x000000FFu;
    if(b.call_395e2)b.call_395e2(m);
    m.cpu.ebp=0u;
    if(b.call_39589)b.call_39589(m);

    // 3562F starts with POP EAX and has an independent caller; it remains outside this fold.
    if(b.tail_3562f)b.tail_3562f(m);
}
static_assert(0x3562Fu-0x35418u==535u);
