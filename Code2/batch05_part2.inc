// 026E8..02A33: emit the three map text planes, then append the packed text
// listing. In block mode the two overlay planes replace non-space base bytes.
void function_026e8(Machine& m,Application05Boundary& b,u32 stream,u32 sink,u32 block_mode) noexcept {
    if(block_mode==0u) {
        if(function_02354(b)==0u){if(b.call_2e6d0)b.call_2e6d0();return;}
        if(app05_s16(m,0x2561Bu)>0x50)app05_putc(b,0x0Fu,stream);
    }
    const s32 height=app05_s16(m,0x12754u);
    const u32 alloc_size=static_cast<u32>(height);
    const u32 base=b.call_3104c?b.call_3104c(alloc_size):0u;
    const u32 overlay=b.call_3104c?b.call_3104c(alloc_size):0u;
    const u32 labels=b.call_3104c?b.call_3104c(alloc_size):0u;
    for(s32 col=0;col<app05_s16(m,0x12756u);++col) {
        app05_fill(m,b,base,0x20u,alloc_size);app05_fill(m,b,overlay,0x20u,alloc_size);app05_fill(m,b,labels,0x20u,alloc_size);
        for(s32 row=0;row<height;++row) {
            const s32 width=app05_s16(m,0x12756u);
            const u32 index=static_cast<u32>(width)*static_cast<u32>(row)+static_cast<u32>(col);
            u32 code=*dg(m,0x1760u+index);
            u8 out=0x20u;
            if(code!=0xFFu) {
                if(code<0x20u)code=function_009b0(m,index,col,row);
                if(code>=0x20u&&code<=0x33u)out=*dg(m,0x6011u+code);
                else if(code<0x20u)out=*dg(m,0x6030u);
            }
            *dg(m,base+static_cast<u32>(row))=out;
        }
        for(s32 row=0;row<height;++row) {
            const s32 width=app05_s16(m,0x12756u);
            const u32 index=static_cast<u32>(width)*static_cast<u32>(row)+static_cast<u32>(col);
            if(*dg(m,0x1760u+index)==0xFFu)continue;
            s32 r=function_00700(m,col,row,0);
            while(r!=-1) {
                const u32 off=static_cast<u32>(r)*0x16u;
                const u16 type=load_le16(dg(m,0x22246u+off));
                *dg(m,overlay+static_cast<u32>(row))=(type>=0x3Cu&&type<=0x41u)?0x2Au:0xFEu;
                r=function_00700(m,col,row,r+1);
            }
        }
        u32 label_ptr=labels;
        for(s32 row=0;row<height;++row) {
            const App05LabelResult lr=app05_label_for_cell(m,col,row);
            if(lr.found!=0u){*dg(m,label_ptr)=lr.first;label_ptr+=2u;*dg(m,label_ptr-1u)=lr.second;}
            ++label_ptr;
        }
        if(block_mode==0u) {
            for(s32 i=0;i<height;++i)app05_putc(b,*dg(m,base+static_cast<u32>(i)),stream);
            app05_putc(b,0x0Du,stream);
            for(s32 i=0;i<height;++i)app05_putc(b,*dg(m,overlay+static_cast<u32>(i)),stream);
            app05_putc(b,0x0Du,stream);
            for(s32 i=0;i<height;++i)app05_putc(b,*dg(m,labels+static_cast<u32>(i)),stream);
            app05_putc(b,*dg(m,0x5F60u),stream);app05_putc(b,*dg(m,0x5F61u),stream);
        } else {
            for(s32 i=0;i<height;++i) {
                const u32 k=static_cast<u32>(i);
                if(*dg(m,overlay+k)!=0x20u)*dg(m,base+k)=*dg(m,overlay+k);
                if(*dg(m,labels+k)!=0x20u)*dg(m,base+k)=*dg(m,labels+k);
            }
            app05_write(b,base,alloc_size,sink);app05_write(b,0x5F60u,2u,sink);
        }
    }
    function_02474(m,b,stream,sink,block_mode);
    if(block_mode==0u){app05_putc(b,0x0Cu,stream);if(b.call_31491)b.call_31491(stream);}
    if(b.call_310b0){b.call_310b0(base);b.call_310b0(overlay);b.call_310b0(labels);}
}
static_assert(0x02A34u-0x026E8u==844u);

// 02A34..02AE4: derive the three decimal template bytes in the original order,
// prepare/open the destination, export in block mode, and close it.
void function_02a34(Machine& m,Application05Boundary& b,s32 value) noexcept {
    const s32 hundreds=value/100;
    *dg(m,0x5FE7u)=static_cast<u8>(hundreds+0x30);
    *dg(m,0x5FE8u)=static_cast<u8>((value%10)+0x30);
    *dg(m,0x5FE9u)=static_cast<u8>(((value-hundreds*100)/10)+0x30);
    if(b.call_31380)b.call_31380(0x5FDAu);
    const u32 q=b.call_312c8?b.call_312c8(0x5FDAu):0u;
    if(b.call_312dc)b.call_312dc(q);
    const u32 h=b.call_312b4?b.call_312b4(0x5FDAu):0xFFFFFFFFu;
    if(h==0xFFFFFFFFu){if(b.call_2e6d0)b.call_2e6d0();return;}
    function_026e8(m,b,0u,h,1u);
    if(b.call_312dc)b.call_312dc(h);
}
static_assert(0x02AE5u-0x02A34u==177u);

// 02AE8..02D6E: load a numbered map/resource file while preserving the current
// packed-text buffer for the live level. Runtime seek/read calls remain explicit.
[[nodiscard]] u32 function_02ae8(Machine& m,Application05Boundary& b,s32 index) noexcept {
    if(load_le32(dg(m,0x6050u))==0u) {
        const u32 used=function_01c08(m)-0x5760u;store_le32(dg(m,0x6054u),used);
        const u32 save=b.call_3104c?b.call_3104c(used):0u;store_le32(dg(m,0x6050u),save);
        app05_copy(m,b,save,0x5760u,used);
    }
    app05_fill(m,b,0x1760u,0xFFu,0x4000u);app05_fill(m,b,0x5760u,0u,0x800u);
    if(b.ui&&b.ui->call_1c368)b.ui->call_1c368(0x6058u,static_cast<u32>(index)+0x26Du);
    if(static_cast<u32>(index)==load_le32(dg(m,0xC6C8u))) {
        if(b.ui)function_01558(m,*b.ui);
        app05_fill(m,b,0x5760u,0u,0x800u);
        app05_copy(m,b,0x5760u,load_le32(dg(m,0x6050u)),load_le32(dg(m,0x6054u)));
        const u32 save=load_le32(dg(m,0x6050u));store_le32(dg(m,0x6054u),0u);
        if(b.call_310b0)b.call_310b0(save);
        store_le32(dg(m,0x6050u),0u);return 1u;
    }
    const s32 hundreds=index/100;
    *dg(m,0x81A4u)=static_cast<u8>(hundreds+0x30);
    *dg(m,0x81A6u)=static_cast<u8>((index%10)+0x30);
    *dg(m,0x81A5u)=static_cast<u8>(((index-hundreds*100)/10)+0x30);
    const u32 h=b.call_312a0?b.call_312a0(0x819Cu):0xFFFFFFFFu;
    if(h==0xFFFFFFFFu)return 0u;
    if(load_le32(dg(m,0x6048u))!=0u){const u32 p=load_le32(dg(m,0x6048u));if(b.call_310b0)b.call_310b0(p);store_le32(dg(m,0x6048u),0u);}
    if(b.call_3131c)b.call_3131c(0x0Du,0u,h);
    u32 pos=b.call_312ec_local32?b.call_312ec_local32(h):0u;
    if(b.call_3131c)b.call_3131c(pos,0u,h);
    if(b.call_312ec_guest)b.call_312ec_guest(0x604Cu,2u,h);
    const s32 count=app05_s16(m,0x604Cu);
    if(count!=0) {
        const u32 bytes=static_cast<u32>(count)*0x16u;
        const u32 p=b.call_3104c?b.call_3104c(bytes):0u;store_le32(dg(m,0x6048u),p);
        if(b.call_312ec_guest)b.call_312ec_guest(p,bytes,h);
    }
    if(b.call_3131c)b.call_3131c(0x19u,0u,h);
    pos=b.call_312ec_local32?b.call_312ec_local32(h):0u;
    if(b.call_3131c)b.call_3131c(pos,0u,h);
    if(b.call_312ec_guest){
        b.call_312ec_guest(0x12756u,2u,h);b.call_312ec_guest(0x12754u,2u,h);
        const s32 w=app05_s16(m,0x12756u), hh=app05_s16(m,0x12754u);
        b.call_312ec_guest(0x1760u,static_cast<u32>(w)*static_cast<u32>(hh),h);
    }
    pos=b.call_312ec_local32?b.call_312ec_local32(h):0u;
    if(static_cast<s32>(pos)<0x7FF&&b.call_312ec_guest)b.call_312ec_guest(0x5760u,pos,h);
    if(b.call_312dc)b.call_312dc(h);
    return 1u;
}
static_assert(0x02D6Fu-0x02AE8u==647u);

static s32 app05_rect_word(Machine& m,u32 action,u32 byte_off) noexcept {
    return app05_s16(m,0x5F62u+action*8u+byte_off);
}
static void app05_panel_region(Machine& m,Application05Boundary& b,u32 action,u32 sprite,u32 image_off,u32 w,u32 h,bool select_page=true) noexcept {
    if(!b.ui||!b.app)return;
    const u32 x=static_cast<u32>(app05_rect_word(m,action,0u));
    const u32 y=static_cast<u32>(app05_rect_word(m,action,2u));
    if(select_page&&b.ui->call_291b0)b.ui->call_291b0(*dg(m,0xC050u));
    if(b.app->call_2975c)b.app->call_2975c(sprite,x,y,load_le32(dg(m,0x258DCu))+image_off,0u);
    if(b.ui->call_1a8c8)b.ui->call_1a8c8(load_le32(dg(m,0xC050u)),x,y,w,h);
}
static void app05_redraw_panel_map(Machine& m,Application05Boundary& b) noexcept {
    if(!b.ui||!b.app||!b.gfx)return;
    if(b.ui->call_291b0)b.ui->call_291b0(*dg(m,0xC050u));
    function_0139c(m,*b.ui,*b.app,*b.gfx,static_cast<s32>(load_le32(dg(m,0x12744u))),
                   static_cast<s32>(load_le32(dg(m,0x12740u))),0x30,0x2F,1u);
    if(b.ui->call_1a510)b.ui->call_1a510(load_le32(dg(m,0xC050u)));
    *dg(m,0xC050u)=static_cast<u8>(*dg(m,0xC050u)^1u);
}
static void app05_select_level_camera(Machine& m) noexcept {
    if(*dg(m,0x5FCBu)!=0u) {
        s32 x=app05_s16(m,0x255D2u)-0x18, y=app05_s16(m,0x255D6u)-0x18;
        if(x<-40)x=-40;
        if(y<-40)y=-40;
        store_le32(dg(m,0x12744u),static_cast<u32>(x));store_le32(dg(m,0x12740u),static_cast<u32>(y));
    } else {store_le32(dg(m,0x12744u),0xFFFFFFFCu);store_le32(dg(m,0x12740u),0xFFFFFFFCu);}
}

// 02DB0..034AE: 16-way control-panel dispatcher driven by the 64-byte CS jump
// table at 02D70. Rectangles are inclusive on all four bounds.
void function_02db0(Machine& m,Application05Boundary& b,s32 x,s32 y,u32 flags) noexcept {
    const u32 saved_x=load_le32(dg(m,0x12744u));
    s32 index=0;
    for(;;++index) {
        const s32 x0=app05_rect_word(m,static_cast<u32>(index),0u);
        if(x0==-1)break;
        if(x>=x0&&x<=app05_rect_word(m,static_cast<u32>(index),4u)&&
           y>=app05_rect_word(m,static_cast<u32>(index),2u)&&y<=app05_rect_word(m,static_cast<u32>(index),6u))break;
    }
    u32 action=0u;
    if(app05_rect_word(m,static_cast<u32>(index),0u)==-1) {
        store_le32(dg(m,0x12744u),saved_x);
        if((flags&2u)==0u)return;
        action=11u;
    } else if((flags&2u)!=0u) {
        action=(index>=0&&index<=3)?static_cast<u32>(index+12):11u;
    } else action=static_cast<u32>(index);
    store_le32(dg(m,0x12744u),saved_x);
    if(action<=3u){*dg(m,0x5FCAu)=static_cast<u8>(1u<<(action&0x1Fu));return;}
    if(action>=12u&&action<=15u){*dg(m,0x5FCAu)=static_cast<u8>(1u<<((action-8u)&0x1Fu));return;}
    if(action==4u) {
        if(*dg(m,0x5FD9u)==0u||*dg(m,0x5FCBu)==0u)return;
        const u32 px=static_cast<u32>(app05_rect_word(m,action,0u)), py=static_cast<u32>(app05_rect_word(m,action,2u));
        if(b.ui&&b.ui->call_291b0)b.ui->call_291b0(*dg(m,0xC050u));
        if(b.call_298f0)b.call_298f0(0u,px,py,load_le32(dg(m,0x258DCu))+0x3Bu,0u,0xD370u);
        if(b.ui&&b.ui->call_1a8c8)b.ui->call_1a8c8(load_le32(dg(m,0xC050u)),px,py,0x3Fu,0x28u);
        if(b.ui&&b.app&&b.gfx)function_0207c(m,*b.ui,*b.app,*b.gfx);
        app05_panel_region(m,b,action,0u,0x3Bu,0x3Fu,0x28u);return;
    }
    if(action==5u) {
        if(*dg(m,0x5FD9u)==0u||*dg(m,0x5FCBu)==0u)return;
        const u32 px=static_cast<u32>(app05_rect_word(m,action,0u)), py=static_cast<u32>(app05_rect_word(m,action,2u));
        if(b.ui&&b.ui->call_291b0)b.ui->call_291b0(*dg(m,0xC050u));
        if(b.call_298f0)b.call_298f0(0u,px,py,load_le32(dg(m,0x258DCu))+0x3Fu,0u,0xD370u);
        if(b.ui&&b.ui->call_1a8c8)b.ui->call_1a8c8(load_le32(dg(m,0xC050u)),px,py,0x1Au,0x28u);
        function_021d8(m,b);app05_panel_region(m,b,action,0u,0x3Fu,0x1Au,0x28u);return;
    }
    if(action==6u||action==7u) {
        do {
            s32 level=static_cast<s32>(load_le32(dg(m,0x1274Cu)));
            if(action==6u){++level;if(level>31)level=0;}else{--level;if(level<0)level=31;}
            store_le32(dg(m,0x1274Cu),static_cast<u32>(level));
            *dg(m,0x5FCBu)=static_cast<u8>(static_cast<u32>(level)==load_le32(dg(m,0xC6C8u)));
            app05_select_level_camera(m);
        } while(function_02ae8(m,b,static_cast<s32>(load_le32(dg(m,0x1274Cu))))==0u);
        app05_redraw_panel_map(m,b);return;
    }
    if(action==8u) {
        *dg(m,0x5FD9u)=static_cast<u8>(*dg(m,0x5FD9u)^1u);
        app05_panel_region(m,b,action,*dg(m,0x5FD9u),0x38u,0x20u,0x30u);
        if(b.ui&&b.app&&b.gfx){function_0139c(m,*b.ui,*b.app,*b.gfx,static_cast<s32>(load_le32(dg(m,0x12744u))),static_cast<s32>(load_le32(dg(m,0x12740u))),0x30,0x2F,1u);if(b.ui->call_1a510)b.ui->call_1a510(load_le32(dg(m,0xC050u)));*dg(m,0xC050u)=static_cast<u8>(*dg(m,0xC050u)^1u);}
        return;
    }
    if(action==9u) {
        if(*dg(m,0x5FCBu)==0u)return;
        app05_panel_region(m,b,action,3u,0x38u,0x20u,0x30u);
        function_026e8(m,b,0xF764u,0u,0u);
        app05_panel_region(m,b,action,2u,0x38u,0x20u,0x30u,false);return;
    }
    if(action==10u) {
        if(*dg(m,0x5FCBu)==0u)return;
        app05_panel_region(m,b,action,5u,0x38u,0x20u,0x30u);
        function_02a34(m,b,static_cast<s32>(load_le32(dg(m,0xC6C8u))));
        app05_panel_region(m,b,action,4u,0x38u,0x20u,0x30u,false);return;
    }
    if(action==11u) {
        app05_panel_region(m,b,action,7u,0x38u,0x20u,0x30u);
        *dg(m,0x5FCAu)=0u;if(b.call_1f8fc)b.call_1f8fc();if(b.call_1ae30)b.call_1ae30();
    }
}
static_assert(0x034AFu-0x02DB0u==1791u);

// 034B0..03630: apply the first active navigation bit to the two viewport
// offsets, then redraw and page-flip. Signed clamps match the encoded branches.
void function_034b0(Machine& m,Application05Boundary& b) noexcept {
    s32 row=static_cast<s32>(load_le32(dg(m,0x12740u)));
    s32 col=static_cast<s32>(load_le32(dg(m,0x12744u)));
    const u8 mask=*dg(m,0x5FCAu);
    if((mask&1u)!=0u){if(row>-40)row=static_cast<s32>(static_cast<u32>(row)-4u);}
    else if((mask&2u)!=0u){const s32 lim=app05_s16(m,0x12754u)-0x2B;if(row<lim||row<-4)row=static_cast<s32>(static_cast<u32>(row)+4u);}
    else if((mask&4u)!=0u){if(col>-40)col=static_cast<s32>(static_cast<u32>(col)-4u);}
    else if((mask&8u)!=0u){const s32 lim=app05_s16(m,0x12756u)-0x2C;if(col<lim||col<-4)col=static_cast<s32>(static_cast<u32>(col)+4u);}
    else if((mask&0x10u)!=0u){row=static_cast<s32>(static_cast<u32>(row)-0x28u);if(row<-40)row=-40;}
    else if((mask&0x20u)!=0u){const s32 lim=app05_s16(m,0x12754u)-0x2F;row=static_cast<s32>(static_cast<u32>(row)+0x28u);if(row>lim)row=lim;}
    else if((mask&0x40u)!=0u){col=static_cast<s32>(static_cast<u32>(col)-0x28u);if(col<-40)col=-40;}
    else if((mask&0x80u)!=0u){const s32 lim=app05_s16(m,0x12756u)-0x30;col=static_cast<s32>(static_cast<u32>(col)+0x28u);if(col>lim)col=lim;}
    store_le32(dg(m,0x12740u),static_cast<u32>(row));store_le32(dg(m,0x12744u),static_cast<u32>(col));
    app05_redraw_panel_map(m,b);
}
static_assert(0x03631u-0x034B0u==385u);
