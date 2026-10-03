// -----------------------------------------------------------------------------
// Batch 04: 011A8..021D7 text/map UI, tile-classification, and editor helpers.
// Every routine below was reconciled directly against fresh CODE.2 object-1
// disassembly; address-named calls remain explicit until separately proved.
// -----------------------------------------------------------------------------

struct Application04Boundary {
    void (*call_296c0)(u32,u32,u32,u32)=nullptr;
    void (*call_291b0)(u32)=nullptr;
    void (*call_2978c)(u32,u32,u32,u32,u32)=nullptr;
    u32 (*call_2f29c)(u32,u32)=nullptr;
    s32 (*call_302cc)(u32,u32,u32)=nullptr;
    void (*call_29804)(u32,u32,u32,u32)=nullptr;
    void (*call_1c368)(u32,u32)=nullptr;
    void (*call_30508)(u32,u32,u32,u32,u32)=nullptr;
    u32 (*call_1e8c8)(u32,u32)=nullptr;
    u32 (*call_1e930)(u32)=nullptr;
    u32 (*call_30700)()=nullptr;
    void (*call_290d0)(u32)=nullptr;
    u32 (*call_30ca2)(u32,u32,u32,u32)=nullptr;
    void (*call_3060c)(u32,u32,u32,u32)=nullptr;
    s32 (*call_3058c)()=nullptr;
    s32 (*call_30594)()=nullptr;
    void (*call_304f4)(u32,u32)=nullptr;
    void (*call_30354)(s32,u32)=nullptr;
    s32 (*call_30318)(s32,u32)=nullptr;
    u32 (*call_30aec)()=nullptr;
    void (*call_1a8c8)(u32,u32,u32,u32,u32)=nullptr;
    u32 (*call_1a1f4)()=nullptr;
    void (*call_1a510)(u32)=nullptr;
    void (*call_1aacc)()=nullptr;
    void (*call_2e590)(u32)=nullptr;
    void (*call_2a020)(u32)=nullptr;
    void (*call_2e5d4)()=nullptr;
    void (*call_1a288)()=nullptr;
    u32 (*call_30070)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_30040)(u32)=nullptr;
    void (*call_22d70)()=nullptr;
};

[[nodiscard]] static s32 app04_add_s32(s32 a,s32 b) noexcept {
    return static_cast<s32>(static_cast<u32>(a)+static_cast<u32>(b));
}
[[nodiscard]] static u32 app04_mask_low_byte(u32 v,u8 mask) noexcept {
    return (v&0xFFFFFF00u)|static_cast<u32>(static_cast<u8>(v)&mask);
}

void function_011a8(Machine& m,Application04Boundary& b) noexcept {
    if(b.call_296c0)b.call_296c0(6u,4u,0x189u,0xEFu);
    if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
    for(u32 x=0u;x!=0x120u;x+=0x60u) {
        if(b.call_2978c)b.call_2978c(0u,0u,x,load_le32(dg(m,0x258DCu))+0x3Eu,0u);
        if(b.call_2978c)b.call_2978c(0u,0xC8u,x,load_le32(dg(m,0x258DCu))+0x3Eu,0u);
    }
    if(b.call_296c0)b.call_296c0(0u,0u,0x18Fu,0x13Fu);
}
static_assert(0x01227u-0x011A8u==127u);

void function_01228(Machine& m,Application04Boundary& b,s32 grid_x,s32 grid_y) noexcept {
    const s32 x0=static_cast<s32>(static_cast<u32>(grid_x)*8u);
    const s32 y0=static_cast<s32>(static_cast<u32>(grid_y)*5u);
    const s32 x1=static_cast<s32>(static_cast<u32>(x0)+0x184u);
    const s32 y1=static_cast<s32>(static_cast<u32>(y0)+0xECu);
    const u32 font=b.call_2f29c?b.call_2f29c(1u,0u):0u;
    if(b.call_296c0)b.call_296c0(6u,4u,0x189u,0xEFu);
    u32 cursor=0x5760u;
    for(;;) {
        const u32 header=cursor;
        cursor+=6u;
        const s32 len=static_cast<s32>(static_cast<s16>(load_le16(dg(m,header+4u))));
        if(len==0)break;
        const s32 rx=static_cast<s32>(static_cast<s16>(load_le16(dg(m,header))));
        const s32 ry=static_cast<s32>(static_cast<s16>(load_le16(dg(m,header+2u))));
        const s32 extent=b.call_302cc?b.call_302cc(cursor,1u,static_cast<u32>(len)):0;
        bool visible=rx>=x0&&rx<=x1;
        if(visible&&(ry<y0||ry>y1)) {
            const s32 end=static_cast<s32>(static_cast<u32>(ry)+static_cast<u32>(extent));
            visible=end>=y0&&end<=y1;
        }
        if(visible) {
            s32 draw_x=static_cast<s32>(static_cast<u32>(rx)-static_cast<u32>(x0));
            s32 draw_y=static_cast<s32>(static_cast<u32>(ry)-static_cast<u32>(y0));
            const s32 reset_y=draw_y;
            for(s32 i=0;i<len;++i) {
                const u8 ch=*dg(m,cursor+static_cast<u32>(i));
                if(ch==0x0Du) {
                    draw_x=static_cast<s32>(static_cast<u32>(draw_x)+0x0Cu);
                    draw_y=reset_y;
                } else {
                    if(b.call_29804)b.call_29804(ch,static_cast<u32>(draw_x),static_cast<u32>(draw_y),load_le32(dg(m,font)));
                    draw_y=static_cast<s32>(static_cast<u32>(draw_y)+*dg(m,font+static_cast<u32>(ch)+4u));
                }
            }
        }
        cursor+=static_cast<u32>(len);
    }
    if(b.call_296c0)b.call_296c0(0u,0u,0x18Fu,0x13Fu);
}
static_assert(0x01399u-0x01228u==369u);

void function_0139c(Machine& m,Application04Boundary& b,Application03Boundary& a3,Graphics2Boundary& g,
                     s32 col,s32 row,s32 cols,s32 rows,u32 decorate) noexcept {
    function_011a8(m,b);
    const s32 end_col=app04_add_s32(col,cols);
    const s32 end_row=app04_add_s32(row,rows);
    for(u32 cbits=static_cast<u32>(col);static_cast<s32>(cbits)<end_col;++cbits) {
        const s32 c=static_cast<s32>(cbits);
        if(c<0||c>=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x12756u)))))continue;
        for(u32 rbits=static_cast<u32>(row);static_cast<s32>(rbits)<end_row;++rbits) {
            const s32 r=static_cast<s32>(rbits);
            if(r>=0&&r<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x12754u)))))
                function_00b6c(m,a3,c,r,6,4);
        }
    }
    for(u32 cbits=static_cast<u32>(col);static_cast<s32>(cbits)<end_col;++cbits) {
        const s32 c=static_cast<s32>(cbits);
        if(c<0||c>=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x12756u)))))continue;
        for(u32 rbits=static_cast<u32>(row);static_cast<s32>(rbits)<end_row;++rbits) {
            const s32 r=static_cast<s32>(rbits);
            if(r>=0&&r<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x12754u)))))
                function_00cb0(m,a3,g,c,r,6,4);
        }
    }
    if(*dg(m,0x5FD9u)!=0u)function_01228(m,b,col,row);
    if(decorate!=0u) {
        if(*dg(m,0x6058u)==0u&&b.call_1c368)
            b.call_1c368(0x6058u,load_le32(dg(m,0x1274Cu))+0x26Du);
        if(b.call_30508)b.call_30508(0x26u,4u,0xECu,0x6058u,1u);
        function_010c4(m,a3);
    }
}
static_assert(0x01557u-0x0139Cu==443u);

[[nodiscard]] static bool app04_empty_neighbor(Machine& m,u32 index) noexcept {
    const u32 a=0x198E0u+index*2u;
    return (*dg(m,a+1u)&4u)!=0u&&(*dg(m,a)&0x3Fu)==0u;
}

void function_01558(Machine& m,Application04Boundary& b) noexcept {
    store_le16(dg(m,0x12756u),load_le16(dg(m,0x25619u)));
    store_le16(dg(m,0x12754u),load_le16(dg(m,0x2561Bu)));
    for(s32 col=0;col<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x12756u))));++col) {
        for(s32 row=0;row<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x12754u))));++row) {
            const s32 width=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x12756u))));
            const s32 height=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x12754u))));
            const u32 index=static_cast<u32>(width)*static_cast<u32>(row)+static_cast<u32>(col);
            const u32 tile=0x198E0u+index*2u;
            u32 code=0xFFu;
            const bool player=row==static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D6u))))&&
                              col==static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D2u))));
            if(player) {
                code=(*dg(m,0x258A8u)==0xFFu)?0x2Fu:0x2Eu;
            } else if((*dg(m,tile+1u)&4u)!=0u) {
                const u32 low=*dg(m,tile)&0x3Fu;
                if(low==0u)code=(*dg(m,0x258A8u)==0xFFu)?0x2Fu:0x2Eu;
                else {
                    const s32 type=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x25622u+low*2u))));
                    if(type==0x93)code=(*dg(m,0x258A8u)==0xFFu)?0x2Fu:0x2Eu;
                    else if((*dg(m,tile+1u)&8u)!=0u)code=5u;
                    else if((*dg(m,tile)&0x80u)==0u)code=1u;
                    else {
                        const u32 rec=b.call_1e8c8?b.call_1e8c8(static_cast<u32>(col),static_cast<u32>(row)):0u;
                        if(rec==0u)code=3u;
                        else code=(static_cast<u16>(b.call_1e930?b.call_1e930(index):0u)==0x8000u)?4u:2u;
                    }
                }
            } else {
                const u32 low=*dg(m,tile)&0x3Fu;
                if(low!=0u) {
                    const s32 type=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x25622u+low*2u))));
                    if(type==0x93)code=(*dg(m,0x258A8u)==0xFFu)?0x2Fu:0x2Eu;
                    else {
                        bool open=false;
                        if(col>0) {
                            if(row>0&&app04_empty_neighbor(m,index-1u-static_cast<u32>(width)))open=true;
                            if(app04_empty_neighbor(m,index-1u))open=true;
                            if(row<height-1&&app04_empty_neighbor(m,index-1u+static_cast<u32>(width)))open=true;
                        }
                        if(row>0&&app04_empty_neighbor(m,index-static_cast<u32>(width)))open=true;
                        if(row<height-1&&app04_empty_neighbor(m,index+static_cast<u32>(width)))open=true;
                        if(col<width-1) {
                            if(row>0&&app04_empty_neighbor(m,index+1u-static_cast<u32>(width)))open=true;
                            if(app04_empty_neighbor(m,index+1u))open=true;
                            if(row<height-1&&app04_empty_neighbor(m,index+1u+static_cast<u32>(width)))open=true;
                        }
                        if(open)code=1u;
                    }
                }
            }
            *dg(m,0x1760u+index)=static_cast<u8>(code);
        }
    }
}
static_assert(0x01831u-0x01558u==729u);

[[nodiscard]] static u32 app04_scale15_4(u32 v) noexcept { return (v*4u)>>15; }
[[nodiscard]] static u32 app04_scale15_2(u32 v) noexcept { return (v*2u)>>15; }

void function_01834(Machine& m,Application04Boundary& b,Application03Boundary& a3) noexcept {
    if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
    for(u32 x=0u;x!=0x120u;x+=0x60u) {
        if(a3.call_2975c)a3.call_2975c(0u,0u,x,load_le32(dg(m,0x258DCu))+0x3Eu,0u);
        if(a3.call_2975c)a3.call_2975c(0u,0xC8u,x,load_le32(dg(m,0x258DCu))+0x3Eu,0u);
    }
    const u32 r0=b.call_30700?b.call_30700():0u;
    store_le32(dg(m,0x12748u),r0);
    if(b.call_290d0)b.call_290d0(load_le16(dg(m,0x12748u)));
    for(u32 y=0xF0u;y<0x140u;y+=8u)for(u32 x=0u;x<0x190u;x+=0x10u) {
        const u32 p=load_le32(dg(m,0x258DCu))+0x35u;
        const u32 r=b.call_30ca2?b.call_30ca2(x,y,p,0u):0u;
        if(a3.call_2975c)a3.call_2975c(x,y,p,0u,app04_scale15_4(r));
    }
    u32 y=0x1Au,label=0xE2u;
    for(u32 i=0u;i<0x0Du;++i,++label) {
        if(i==9u&&*dg(m,0x258A8u)!=0xFFu)continue;
        if(b.call_1c368)b.call_1c368(0x769Cu,label);
        const u32 p=load_le32(dg(m,0x258DCu))+0x34u;
        if(a3.call_2975c)a3.call_2975c(*dg(m,0x5FCCu+i),y,0xF6u,p,0u);
        if(b.call_3060c)b.call_3060c(y,0xFFu,0x769Cu,1u);
        y+=0x0Au;
    }
    const u32 r1=b.call_30700?b.call_30700():0u;
    store_le32(dg(m,0x12750u),r1);
    if(b.call_290d0)b.call_290d0(load_le16(dg(m,0x12750u)));
    for(u32 x=8u;x<0x138u;x+=8u) {
        u32 p=load_le32(dg(m,0x258DCu))+0x35u;
        u32 r=b.call_30ca2?b.call_30ca2(0u,x,p,0u):0u;
        if(a3.call_2975c)a3.call_2975c(0u,x,p,0u,app04_scale15_2(r)+9u);
        p=load_le32(dg(m,0x258DCu))+0x35u;
        r=b.call_30ca2?b.call_30ca2(0x180u,x,p,0u):0u;
        if(a3.call_2975c)a3.call_2975c(0x180u,x,p,0u,app04_scale15_2(r)+5u);
    }
    for(u32 x=0x10u;x<0x180u;x+=0x10u) {
        u32 p=load_le32(dg(m,0x258DCu))+0x35u;
        u32 r=b.call_30ca2?b.call_30ca2(x,0u,p,0u):0u;
        if(a3.call_2975c)a3.call_2975c(x,0u,p,0u,app04_scale15_2(r)+0x0Cu);
        p=load_le32(dg(m,0x258DCu))+0x35u;
        r=b.call_30ca2?b.call_30ca2(x,0xF0u,p,0u):0u;
        if(a3.call_2975c)a3.call_2975c(x,0xF0u,p,0u,app04_scale15_2(r)+0x12u);
        p=load_le32(dg(m,0x258DCu))+0x35u;
        r=b.call_30ca2?b.call_30ca2(x,0x138u,p,0u):0u;
        if(a3.call_2975c)a3.call_2975c(x,0x138u,p,0u,app04_scale15_2(r)+0x0Eu);
    }
    auto draw=[&](u32 a0,u32 a1,u32 a2,u32 off) noexcept {if(a3.call_2975c)a3.call_2975c(a0,a1,a2,load_le32(dg(m,0x258DCu))+off,0u);};
    draw(8u,0u,0u,0x35u); draw(0x0Bu,0u,0x138u,0x35u); draw(4u,0x180u,0u,0x35u); draw(7u,0x180u,0x138u,0x35u);
    draw(0x10u,0u,0xF0u,0x35u); draw(0x11u,0x180u,0xF0u,0x35u); draw(0u,0x11Du,0xF0u,0x37u); draw(1u,0x11Du,0x130u,0x37u);
    draw(*dg(m,0x5FD9u),0x11Au,0x100u,0x38u);
    u32 sx=0x136u; for(u32 sprite=2u;sprite!=8u;sprite+=2u,sx+=0x1Cu)draw(sprite,sx,0x100u,0x38u);
    draw(0u,3u,0xF3u,0x39u); draw(0u,0xAEu,0x120u,0x3Au); draw(0u,0xA8u,0xF8u,0x3Bu); draw(0u,0xF3u,0xF5u,0x3Fu);
}
static_assert(0x01C08u-0x01834u==980u);

[[nodiscard]] u32 function_01c08(Machine& m) noexcept {
    u32 p=0x5760u;
    if(load_le16(dg(m,0x5764u))==0u)return p;
    do {p+=6u;const s32 len=static_cast<s32>(static_cast<s16>(load_le16(dg(m,p-2u))));p+=static_cast<u32>(len);} while(load_le16(dg(m,p+4u))!=0u);
    return p;
}
static_assert(0x01C28u-0x01C08u==32u);

[[nodiscard]] u32 function_01c28(Machine& m) noexcept {
    u32 p=0x5760u;
    if(load_le16(dg(m,0x5764u))!=0u)do {p+=6u;const s32 len=static_cast<s32>(static_cast<s16>(load_le16(dg(m,p-2u))));p+=static_cast<u32>(len);} while(load_le16(dg(m,p+4u))!=0u);
    return p-0x5760u;
}
static_assert(0x01C4Du-0x01C28u==37u);

void function_01c50(Machine& m,Application04Boundary& b,u32 text,u32 style,u32 count) noexcept {
    s32 cursor0=b.call_3058c?b.call_3058c():0;
    const s32 cursor1=b.call_30594?b.call_30594():0;
    if(text==0u||*dg(m,text)==0u||count==0u)return;
    u32 p=text,remaining=count;
    while(*dg(m,p)!=0u&&remaining!=0u) {
        const s32 ch=static_cast<s32>(static_cast<s8>(*dg(m,p)));
        if(ch==0x0D) {cursor0=static_cast<s32>(static_cast<u32>(cursor0)+0x0Cu);if(b.call_304f4)b.call_304f4(static_cast<u32>(cursor0),static_cast<u32>(cursor1));}
        else if(b.call_30354)b.call_30354(ch,style);
        ++p;--remaining;
    }
}
static_assert(0x01CA8u-0x01C50u==88u);

[[nodiscard]] s32 function_01ca8(Machine& m,Application04Boundary& b,u32 text,u32 style) noexcept {
    s32 total=0;if(text==0u||*dg(m,text)==0u)return total;
    for(u32 p=text;*dg(m,p)!=0u;++p) {if(*dg(m,p)==0x0Du)total=0;else {const s32 ch=static_cast<s32>(static_cast<s8>(*dg(m,p)));const s32 w=b.call_30318?b.call_30318(ch,style):0;total=static_cast<s32>(static_cast<u32>(total)+static_cast<u32>(w));}}
    return total;
}
static_assert(0x01CE2u-0x01CA8u==58u);

void function_01ce4(Machine& m,Application04Boundary& b,Graphics2Boundary& g,u32 origin0,u32 origin1,u32 max_lines,u32 image) noexcept {
    u32 key=0u;bool redraw=true;u32 record=0x5760u;
    if(load_le16(dg(m,0x5764u))!=0u)do {record+=6u;const s32 len=static_cast<s32>(static_cast<s16>(load_le16(dg(m,record-2u))));record+=static_cast<u32>(len);} while(load_le16(dg(m,record+4u))!=0u);
    const u32 text=record+6u;s32 line=0;if(function_01c08(m)-0x5760u>=0x7EEu)return;
    const u32 xbits=static_cast<u32>(static_cast<u16>(load_le16(dg(m,0x12744u))))<<3;
    store_le16(dg(m,record),static_cast<u16>(origin0+xbits));
    const u16 ydelta=static_cast<u16>(static_cast<u16>(load_le16(dg(m,0x12740u)))*5u);
    store_le16(dg(m,record+2u),static_cast<u16>(origin1+static_cast<u32>(ydelta)));store_le16(dg(m,record+4u),0u);*dg(m,text)=0u;
    const s32 line_limit=static_cast<s32>(static_cast<u32>(max_lines)-1u);const u32 masked_origin1=app04_mask_low_byte(origin1,0xF8u);const u32 right_extent=0x140u-origin1;const u32 line_span=max_lines*0x0Cu;
    while(key!=0x1Bu) {
        if(redraw) {function_2a380(m,g,0u,origin0,masked_origin1,image);if(b.call_304f4)b.call_304f4(origin0,origin1);function_01c50(m,b,text,1u,static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,record+4u))))));if(b.call_30354)b.call_30354(0x7Cu,1u);if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),origin0,origin1,line_span,right_extent);redraw=false;}
        key=b.call_30aec?b.call_30aec():0u;bool mutated=false;
        if(key==8u) {const u16 len=load_le16(dg(m,record+4u));if(len!=0u){store_le16(dg(m,record+4u),static_cast<u16>(len-1u));mutated=true;}}
        else if(key==0x0Du) {if(function_01c08(m)-0x5760u<0x7EEu){if(line<line_limit){const s32 old_len=static_cast<s32>(static_cast<s16>(load_le16(dg(m,record+4u))));store_le16(dg(m,record+4u),static_cast<u16>(load_le16(dg(m,record+4u))+1u));*dg(m,text+static_cast<u32>(old_len))=0x0Du;line=app04_add_s32(line,1);mutated=true;}else key=0x1Bu;}}
        else {const u8 idx=static_cast<u8>(static_cast<u8>(key)+1u);const u8 cls=*dg(m,0x165Cu+idx);const bool accepted=(cls&0xE0u)!=0u||(cls&4u)!=0u||key==0x20u;if(accepted&&function_01c08(m)-0x5760u<0x7EEu){const s32 width=function_01ca8(m,b,text,1u);if(static_cast<s32>(static_cast<u32>(width)+origin1)<0xE3){const s32 old_len=static_cast<s32>(static_cast<s16>(load_le16(dg(m,record+4u))));store_le16(dg(m,record+4u),static_cast<u16>(load_le16(dg(m,record+4u))+1u));*dg(m,text+static_cast<u32>(old_len))=static_cast<u8>(key);mutated=true;}}}
        if(mutated){const s32 len=static_cast<s32>(static_cast<s16>(load_le16(dg(m,record+4u))));*dg(m,text+static_cast<u32>(len))=0u;redraw=true;}
    }
    if(load_le16(dg(m,record+4u))==0u){*dg(m,text)=0u;store_le16(dg(m,record+4u),0u);store_le16(dg(m,record+2u),0u);store_le16(dg(m,record),0u);}
    function_2a380(m,g,0u,origin0,masked_origin1,image);if(b.call_304f4)b.call_304f4(origin0,origin1);function_01c50(m,b,text,1u,static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,record+4u))))));if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),origin0,origin1,line_span,0x140u-origin1);
}
static_assert(0x01F6Bu-0x01CE4u==647u);

void function_01f6c(Machine& m,Application04Boundary& b,Application03Boundary& a3,Graphics2Boundary& g,u32 decorate) noexcept {
    if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));if(b.call_290d0)b.call_290d0(load_le16(dg(m,0x12750u)));
    for(u32 x=8u;x<0xB8u;x+=8u){u32 p=load_le32(dg(m,0x258DCu))+0x35u;u32 r=b.call_30ca2?b.call_30ca2(0u,x,p,0u):0u;if(a3.call_2975c)a3.call_2975c(0u,x,p,0u,app04_scale15_2(r)+9u);p=load_le32(dg(m,0x258DCu))+0x35u;r=b.call_30ca2?b.call_30ca2(0x180u,x,p,0u):0u;if(a3.call_2975c)a3.call_2975c(0x180u,x,p,0u,app04_scale15_2(r)+5u);}
    for(u32 x=0x10u;x<0x180u;x+=0x10u){u32 p=load_le32(dg(m,0x258DCu))+0x35u;u32 r=b.call_30ca2?b.call_30ca2(x,0u,p,0u):0u;if(a3.call_2975c)a3.call_2975c(x,0u,p,0u,app04_scale15_2(r)+0x0Cu);p=load_le32(dg(m,0x258DCu))+0x35u;r=b.call_30ca2?b.call_30ca2(x,0xF0u,p,0u):0u;if(a3.call_2975c)a3.call_2975c(x,0xF0u,p,0u,app04_scale15_2(r)+0x12u);}
    function_0139c(m,b,a3,g,static_cast<s32>(load_le32(dg(m,0x12744u))),static_cast<s32>(load_le32(dg(m,0x12740u))),0x30,0x2F,decorate);if(b.call_1a510)b.call_1a510(load_le32(dg(m,0xC050u)));*dg(m,0xC050u)=static_cast<u8>(*dg(m,0xC050u)^1u);
}
static_assert(0x02079u-0x01F6Cu==269u);

void function_0207c(Machine& m,Application04Boundary& b,Application03Boundary& a3,Graphics2Boundary& g) noexcept {
    if(b.call_1a1f4)while(b.call_1a1f4()!=0u){}function_01f6c(m,b,a3,g,0u);if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
    for(;;){if(b.call_1a1f4)while(b.call_1a1f4()==0u){}const s32 x=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x9E31u))));const s32 y=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x9E33u))));const bool exit_rect=x>=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x5F82u))))&&x<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x5F86u))))&&y>=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x5F84u))))&&y<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x5F88u))));if(exit_rect)break;if(x<8||x>0x188||y<4||y>0xCE)continue;if(b.call_1aacc)b.call_1aacc();if(b.call_2e590)b.call_2e590(*dg(m,0xEF4Eu));if(b.call_2a020)b.call_2a020(static_cast<u8>(*dg(m,0xEF4Eu)^1u));s32 lines=(0x188-x)/0x0C;if(lines==0){if(b.call_2e5d4)b.call_2e5d4();if(b.call_1a288)b.call_1a288();continue;}if(lines>4)lines=4;const s32 aligned_y=static_cast<s32>(static_cast<u32>(y)&0xFFFFFFF8u);const u32 image=b.call_30070?b.call_30070(static_cast<u32>(x),static_cast<u32>(aligned_y),static_cast<u32>(lines*0x0C),0x140u-static_cast<u32>(aligned_y),0xFFu):0u;if(b.call_2e5d4)b.call_2e5d4();function_01ce4(m,b,g,static_cast<u32>(x),static_cast<u32>(y),static_cast<u32>(lines),image);if(b.call_30040)b.call_30040(image);if(b.call_1a288)b.call_1a288();}
    function_01f6c(m,b,a3,g,1u);if(b.call_22d70)b.call_22d70();
}
static_assert(0x021D8u-0x0207Cu==348u);
