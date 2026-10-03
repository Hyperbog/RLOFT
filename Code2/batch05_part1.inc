// -----------------------------------------------------------------------------
// Batch 05: 021D8..03630 modal text deletion, map export/load, control panel,
// and viewport movement. CODE.2 bytes are the controlling evidence.
// -----------------------------------------------------------------------------

struct Application05Boundary {
    Application04Boundary* ui=nullptr;
    Application03Boundary* app=nullptr;
    Graphics2Boundary* gfx=nullptr;
    void (*call_30cd4)(u32,u32,u32)=nullptr;
    void (*call_30d30)(u32,u32,u32)=nullptr;
    void (*call_30d52)(u32,u8*,u8*)=nullptr;
    u32 (*call_3104c)(u32)=nullptr;
    void (*call_31107)(u32,u32)=nullptr;
    void (*call_31304)(u32,u32,u32)=nullptr;
    void (*call_310b0)(u32)=nullptr;
    void (*call_2e6d0)()=nullptr;
    void (*call_31491)(u32)=nullptr;
    void (*call_31380)(u32)=nullptr;
    u32 (*call_312c8)(u32)=nullptr;
    void (*call_312dc)(u32)=nullptr;
    u32 (*call_312b4)(u32)=nullptr;
    u32 (*call_312a0)(u32)=nullptr;
    void (*call_3131c)(u32,u32,u32)=nullptr;
    u32 (*call_312ec_local32)(u32)=nullptr;
    void (*call_312ec_guest)(u32,u32,u32)=nullptr;
    void (*call_298f0)(u32,u32,u32,u32,u32,u32)=nullptr;
    void (*call_1f8fc)()=nullptr;
    void (*call_1ae30)()=nullptr;
};

static void app05_copy(Machine& m,Application05Boundary& b,u32 dst,u32 src,u32 count) noexcept {
    if(b.call_30cd4){b.call_30cd4(dst,src,count);return;}
    for(u32 i=0;i<count;++i)*dg(m,dst+i)=*dg(m,src+i);
}
static void app05_fill(Machine& m,Application05Boundary& b,u32 dst,u8 value,u32 count) noexcept {
    if(b.call_30d30){b.call_30d30(dst,value,count);return;}
    for(u32 i=0;i<count;++i)*dg(m,dst+i)=value;
}
static void app05_putc(Application05Boundary& b,u32 ch,u32 stream) noexcept {
    if(b.call_31107)b.call_31107(ch,stream);
}
static void app05_write(Application05Boundary& b,u32 src,u32 count,u32 sink) noexcept {
    if(b.call_31304)b.call_31304(src,count,sink);
}
static s32 app05_s16(Machine& m,u32 a) noexcept {
    return static_cast<s32>(static_cast<s16>(load_le16(dg(m,a))));
}

// 021D8..02352: wait for a mouse press, locate a packed text record under the
// cursor, delete it with the encoded forward-copy/clear arithmetic, redraw,
// and repeat until the exit rectangle is selected.
void function_021d8(Machine& m,Application05Boundary& b) noexcept {
    if(!b.ui)return;
    if(b.ui->call_1a1f4)while(b.ui->call_1a1f4()!=0u){}
    for(;;) {
        if(b.ui->call_1a1f4)while(b.ui->call_1a1f4()==0u){}
        const s32 sx=app05_s16(m,0x9E31u), sy=app05_s16(m,0x9E33u);
        const s32 ex0=app05_s16(m,0x5F8Au), ex1=app05_s16(m,0x5F8Eu);
        const s32 ey0=app05_s16(m,0x5F8Cu), ey1=app05_s16(m,0x5F90u);
        if(sx>=ex0&&sx<ex1&&sy>=ey0&&sy<ey1)break;
        if(sx<0x20||sx>0x16F||sy<0x10||sy>0xE3)continue;
        const s32 x=static_cast<s32>(static_cast<u32>(sx)+load_le32(dg(m,0x12744u))*8u);
        const s32 y=static_cast<s32>(static_cast<u32>(sy)+load_le32(dg(m,0x12740u))*5u);
        u32 selected=0u, record=0x5760u;
        for(;;) {
            const s32 len=app05_s16(m,record+4u);
            if(len==0)break;
            const u32 text=record+6u;
            const s32 rx=app05_s16(m,record), ry=app05_s16(m,record+2u);
            const s32 extent=b.ui->call_302cc?b.ui->call_302cc(text,1u,static_cast<u32>(len)):0;
            if(rx<x&&static_cast<s32>(static_cast<u32>(rx)+0x0Bu)>x&&
               ry<y&&static_cast<s32>(static_cast<u32>(ry)+static_cast<u32>(extent))>y)
                selected=record;
            record=static_cast<u32>(text+static_cast<u32>(len));
        }
        if(selected==0u)continue;
        const u32 len_bits=static_cast<u32>(app05_s16(m,selected+4u));
        const u32 terminator=function_01c08(m);
        const u32 end=selected+6u+len_bits;
        const u32 tail=(terminator-0x5760u)-(end-0x5760u);
        app05_copy(m,b,selected,end,tail);
        const u32 clear=(end-0x5760u)+selected;
        app05_fill(m,b,clear,0u,len_bits+6u);
        if(b.ui&&b.app&&b.gfx)
            function_0139c(m,*b.ui,*b.app,*b.gfx,static_cast<s32>(load_le32(dg(m,0x12744u))),
                           static_cast<s32>(load_le32(dg(m,0x12740u))),0x30,0x2F,1u);
        if(b.ui->call_1a510)b.ui->call_1a510(load_le32(dg(m,0xC050u)));
        *dg(m,0xC050u)=static_cast<u8>(*dg(m,0xC050u)^1u);
    }
    if(b.ui->call_22d70)b.ui->call_22d70();
}
static_assert(0x02353u-0x021D8u==379u);

// 02354..023B5: query the encoded 17h status frame and accept exactly the
// 80h+10h set / 20h+08h clear combination in byte one.
[[nodiscard]] u32 function_02354(Application05Boundary& b) noexcept {
    u8 frame[0x1Cu]{};
    store_le16(frame,0x0200u);
    store_le16(frame+0x0Cu,0u);
    if(b.call_30d52)b.call_30d52(0x17u,frame,frame);
    const u8 f=frame[1];
    return ((f&0x80u)!=0u&&(f&0x20u)==0u&&(f&0x10u)!=0u&&(f&0x08u)==0u)?1u:0u;
}
static_assert(0x023B6u-0x02354u==98u);

// 023B8..02472: locate the packed text record whose signed coordinates map to
// the supplied /8 and /5 cell, while advancing the binary's A..Z/A.. label.
struct App05LabelResult { u32 found=0u; u8 first=0x41u,second=0x20u; };
[[nodiscard]] static App05LabelResult app05_label_for_cell(Machine& m,s32 want_x,s32 want_y) noexcept {
    App05LabelResult r{};u32 record=0x5760u;
    for(;;) {
        const s32 len=app05_s16(m,record+4u);
        if(len==0)break;
        const s32 qx=app05_s16(m,record)/8;
        const s32 qy=app05_s16(m,record+2u)/5;
        if(qx==want_x&&qy==want_y){r.found=1u;break;}
        if(r.first==0x5Au){r.first=0x41u;if(r.second==0x20u)r.second=0x41u;else ++r.second;}
        else ++r.first;
        record=static_cast<u32>(record+6u+static_cast<u32>(len));
    }
    return r;
}
[[nodiscard]] u32 function_023b8(Machine& m,s32 want_x,s32 want_y,u32 out) noexcept {
    const App05LabelResult r=app05_label_for_cell(m,want_x,want_y);
    *dg(m,out)=r.first;*dg(m,out+1u)=r.second;return r.found;
}
static_assert(0x02473u-0x023B8u==187u);

// 02474..026E5: serialize packed text records either as character output or
// fixed-width block writes. Labels and CR/LF behavior follow the original.
void function_02474(Machine& m,Application05Boundary& b,u32 stream,u32 sink,u32 block_mode) noexcept {
    u8 first=0x41u, second=0x20u;
    const u32 buffer=b.call_3104c?b.call_3104c(0x4Fu):0u;
    u32 record=0x5760u;
    for(;;) {
        const s32 len=app05_s16(m,record+4u);
        if(len==0)break;
        const u32 text=record+6u;
        app05_fill(m,b,buffer,0x20u,0x4Fu);
        *dg(m,buffer)=first;*dg(m,buffer+1u)=second;*dg(m,buffer+2u)=0x3Au;
        if(first==0x5Au){first=0x41u;if(second==0x20u)second=0x41u;else ++second;}else ++first;
        u32 column=4u;u32 dirty=0u;
        for(s32 i=0;i<len;++i) {
            const u8 ch=*dg(m,text+static_cast<u32>(i));
            if(ch==0x0Du) {
                if(block_mode==0u) {
                    for(u32 p=buffer;p!=buffer+0x4Fu;++p)app05_putc(b,*dg(m,p),stream);
                    app05_putc(b,*dg(m,0x5F60u),stream);app05_putc(b,*dg(m,0x5F61u),stream);
                } else {
                    app05_write(b,buffer,0x4Fu,sink);app05_write(b,0x5F60u,2u,sink);
                }
                dirty=0u;app05_fill(m,b,buffer,0x20u,0x4Fu);column=4u;
            } else {
                *dg(m,buffer+column)=ch;++column;dirty=1u;
            }
        }
        if(block_mode==0u) {
            if(dirty!=0u){for(u32 p=buffer;p!=buffer+0x4Fu;++p)app05_putc(b,*dg(m,p),stream);app05_putc(b,*dg(m,0x5F60u),stream);app05_putc(b,*dg(m,0x5F61u),stream);}
            app05_putc(b,*dg(m,0x5F60u),stream);app05_putc(b,*dg(m,0x5F61u),stream);
        } else {
            if(dirty!=0u){app05_write(b,buffer,0x4Fu,sink);app05_write(b,0x5F60u,2u,sink);}
            *dg(m,buffer)=0x0Au;app05_write(b,buffer,1u,sink);
        }
        record=static_cast<u32>(record+6u+static_cast<u32>(len));
    }
    if(block_mode==0u){app05_putc(b,*dg(m,0x5F60u),stream);app05_putc(b,*dg(m,0x5F61u),stream);}
    else app05_write(b,0x5F60u,2u,sink);
    if(b.call_310b0)b.call_310b0(buffer);
}
static_assert(0x026E6u-0x02474u==626u);
