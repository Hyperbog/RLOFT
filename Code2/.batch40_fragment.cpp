// -----------------------------------------------------------------------------
// Batch 40: 1C40C..1C77A framed/tiled panel renderer.
// 1C77B is a bounded NOP separator; the next routine begins at 1C77C.
// -----------------------------------------------------------------------------

struct Application40Boundary {
    void (*call_2ecb4)(u32,u32)=nullptr;
    void (*call_2e590)(u32)=nullptr;
    void (*call_2978c)(u32,u32,u32,u32,u32)=nullptr;
    u32 (*call_30ca2)()=nullptr;
    void (*call_2e5d4)()=nullptr;
};

static u32 app40_mask_al(u32 value,u8 mask) noexcept {
    return (value&0xFFFFFF00u)|static_cast<u8>(static_cast<u8>(value)&mask);
}
static u32 app40_sar1(u32 value) noexcept {
    return (value>>1u)|((value&0x80000000u)!=0u?0x80000000u:0u);
}
static u32 app40_half_toward_zero(u32 value) noexcept {
    const u32 sign=(value&0x80000000u)!=0u?0xFFFFFFFFu:0u;
    return app40_sar1(value-sign);
}
static bool app40_lt_s(u32 a,u32 b) noexcept {
    return static_cast<s32>(a)<static_cast<s32>(b);
}
static u32 app40_rand2(Application40Boundary& b) noexcept {
    const u32 r=b.call_30ca2?b.call_30ca2():0u;
    return (r+r)>>15u;
}
static u32 app40_rand4(Application40Boundary& b) noexcept {
    const u32 r=b.call_30ca2?b.call_30ca2():0u;
    return (r*4u)>>15u;
}
static void app40_draw(Application40Boundary& b,u32 variant,u32 x,u32 y,u32 graphic) noexcept {
    if(b.call_2978c)b.call_2978c(variant,x,y,graphic,0u);
}

void function_1c40c(Machine& m,Application40Boundary& b,
                     u32 page,u32 x,u32 y,u32 width,u32 height) noexcept {
    if(static_cast<s32>(width)<0x40)width=0x40u;
    if(static_cast<s32>(height)<0x50)height=0x50u;

    // AND AL preserves the upper 24 bits after the add; this is not a normal
    // full-width alignment mask.
    width=app40_mask_al(width+0x1Fu,0xE0u);
    height=app40_mask_al(height+0x0Fu,0xF0u);

    const u32 graphic29=load_le32(dg(m,0x258DCu))+0x29u;
    const u32 graphic2a=load_le32(dg(m,0x258DCu))+0x2Au;
    const u32 graphic2b=load_le32(dg(m,0x258DCu))+0x2Bu;
    if(b.call_2ecb4) {
        b.call_2ecb4(graphic29,0u);
        b.call_2ecb4(graphic2a,0u);
        b.call_2ecb4(graphic2b,0u);
    }
    if(b.call_2e590)b.call_2e590(page);

    const u32 left=x-0x10u;
    const u32 top=y-8u;
    const u32 right=x+width;
    const u32 bottom=y+height;
    app40_draw(b,8u,left,top,graphic29);
    app40_draw(b,4u,right,top,graphic29);
    app40_draw(b,7u,right,bottom,graphic29);
    app40_draw(b,0x0Bu,left,bottom,graphic29);

    const u32 mid_x=x+app40_half_toward_zero(width-0x40u);
    app40_draw(b,0u,mid_x,y-0x20u,graphic2a);
    app40_draw(b,1u,mid_x,bottom,graphic2a);
    const u32 mid_y=y+app40_half_toward_zero(height-0x50u);
    app40_draw(b,0u,right,mid_y,graphic2b);

    if(static_cast<s32>(height)>0) {
        u32 row=y;
        while(app40_lt_s(row,bottom)) {
            app40_draw(b,app40_rand2(b)+9u,left,row,graphic29);
            row+=8u;
        }
    }

    {
        u32 row=y;
        u32 mirror=bottom-8u;
        while(app40_lt_s(row,mid_y)) {
            app40_draw(b,app40_rand2(b)+5u,right,row,graphic29);
            app40_draw(b,app40_rand2(b)+5u,right,mirror,graphic29);
            row+=8u;
            mirror-=8u;
        }
    }

    {
        u32 col=x;
        u32 mirror=right-0x10u;
        while(app40_lt_s(col,mid_x)) {
            app40_draw(b,app40_rand2(b)+0x0Cu,col,top,graphic29);
            app40_draw(b,app40_rand2(b)+0x0Cu,mirror,top,graphic29);
            app40_draw(b,app40_rand2(b)+0x0Eu,col,bottom,graphic29);
            app40_draw(b,app40_rand2(b)+0x0Eu,mirror,bottom,graphic29);
            col+=0x10u;
            mirror-=0x10u;
        }
    }

    if(static_cast<s32>(width)>0) {
        u32 col=x;
        while(app40_lt_s(col,right)) {
            if(static_cast<s32>(height)>0) {
                u32 row=y;
                while(app40_lt_s(row,bottom)) {
                    app40_draw(b,app40_rand4(b),col,row,graphic29);
                    row+=8u;
                }
            }
            col+=0x10u;
        }
    }

    if(b.call_2e5d4)b.call_2e5d4();
}
static_assert(0x1C77Bu-0x1C40Cu==879u);
