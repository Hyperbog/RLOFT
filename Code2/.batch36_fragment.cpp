// -----------------------------------------------------------------------------
// Batch 36: 1AE30..1B1C1 input-axis state and direction-priority helper.
// 1AF85..1AF8F is bounded zero padding; 1B1C2..1B1C3 is MOV EAX,EAX
// alignment. The next routine begins at 1B1C4.
// -----------------------------------------------------------------------------

struct Application36Boundary {
    Application35Boundary* app35=nullptr;
    void (*interrupt_33_0b_read)(u16& cx,u16& dx)=nullptr;
};

static Application34Boundary* app36_a34(Application36Boundary& b) noexcept {
    return b.app35?b.app35->app34:nullptr;
}
static Application33Boundary* app36_a33(Application36Boundary& b) noexcept {
    Application34Boundary* const a34=app36_a34(b);
    return a34?a34->app33:nullptr;
}
static s32 app36_sar16(u32 bits) noexcept {
    u32 shifted=bits>>16u;
    if((bits&0x80000000u)!=0u)shifted|=0xFFFF0000u;
    return static_cast<s32>(shifted);
}
static u32 app36_x86_abs32(s32 value) noexcept {
    const u32 bits=static_cast<u32>(value);
    return value<0 ? 0u-bits : bits;
}
static s32 app36_sign3(s32 value) noexcept {
    return value<0 ? -1 : (value>0 ? 1 : 0);
}

void function_1ae30(Machine& m,Application36Boundary& b) noexcept {
    Application34Boundary* const a34=app36_a34(b);
    if(load_le16(dg(m,0x85FCu))==0u) {
        store_le32(dg(m,0xC6ECu),0u);
        store_le32(dg(m,0xC6E8u),0u);
        store_le32(dg(m,0xC6F0u),0u);
        return;
    }

    if(a34&&a34->call_2f868)a34->call_2f868();
    if(load_le16(dg(m,0x85FCu))!=0u&&a34&&a34->call_33cb0) {
        const u32 state=a34->call_33cb0();
        store_le16(dg(m,0x9E35u),load_le16(dg(m,state)));
    }

    if(load_le16(dg(m,0xCF48u))==0u) {
        u16 cx=0u,dx=0u;
        if(b.interrupt_33_0b_read)b.interrupt_33_0b_read(cx,dx);
        store_le32(dg(m,0xC6E8u),static_cast<u32>(static_cast<s32>(static_cast<s16>(dx))));
        store_le32(dg(m,0xC6ECu),static_cast<u32>(static_cast<s32>(static_cast<s16>(cx))));
        store_le32(dg(m,0xC6F0u),0u);

        const u32 right=static_cast<u32>(static_cast<u8>(load_le16(dg(m,0x9E35u))))&2u;
        if(right!=0u) {
            if(load_le32(dg(m,0xC04Cu))==0u)
                store_le32(dg(m,0xC04Cu),load_le32(dg(m,0xC6E8u)));
            store_le32(dg(m,0xC04Cu),load_le32(dg(m,0xC04Cu))-8u);
            store_le32(dg(m,0xC6E8u),load_le32(dg(m,0xC04Cu)));
        } else {
            store_le32(dg(m,0xC04Cu),0u);
        }

        s32 axis=static_cast<s32>(load_le32(dg(m,0xC6E8u)));
        if(axis<-0xA0) {
            axis=-0xA0;
            store_le32(dg(m,0xC6E8u),static_cast<u32>(axis));
        }
        if(axis>0xA0)
            store_le32(dg(m,0xC6E8u),0xA0u);
        return;
    }

    store_le32(dg(m,0xC6ECu),0u);
    store_le32(dg(m,0xC6E8u),0u);
    store_le32(dg(m,0xC6F0u),0u);
    const u16 buttons=load_le16(dg(m,0x9E35u));
    if(buttons!=0u) {
        if((static_cast<u8>(buttons)&1u)==0u)return;
        if(b.app35) {
            const u32 x=static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x9E31u)))));
            const u32 y=static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x9E33u)))));
            function_1ac38(m,*b.app35,x,y);
        }
        return;
    }

    store_le16(dg(m,0xA6BEu),0u);
    if(load_le16(dg(m,0xA6BCu))!=0u) {
        store_le16(dg(m,0xA6BCu),0u);
        if(a34&&a34->call_1f820)a34->call_1f820(); // encoded tail JMP
    }
}
static_assert(0x1AF85u-0x1AE30u==341u);

[[nodiscard]] u32 function_1af90(Machine& m,Application36Boundary& b,
                                  u32 delta0,u32 delta1,u32 fallback,u32 rank) noexcept {
    Application33Boundary* const a33=app36_a33(b);
    if(a33&&a33->call_30d30)a33->call_30d30(0x140B0u,0u,8u);
    else for(u32 i=0u;i<8u;++i)*dg(m,0x140B0u+i)=0u;

    const s32 dy=app36_sar16(delta1);
    const s32 dx=app36_sar16(delta0);
    const s32 sy=app36_sign3(dy);
    const s32 sx=app36_sign3(dx);

    auto put=[&](u32 off,u8 value) noexcept { *dg(m,0x140B0u+off)=value; };
    if(dx==0) {
        if(sy==-1) { put(4u,3u); put(6u,2u); put(2u,1u); }
        else if(sy==1) { put(0u,3u); put(6u,2u); put(2u,1u); }
    } else if(dy==0) {
        if(sx==-1) { put(6u,3u); put(4u,2u); put(0u,1u); }
        else if(sx==1) { put(2u,3u); put(4u,2u); put(0u,1u); }
    } else {
        const u32 ax=app36_x86_abs32(dx);
        const u32 ay=app36_x86_abs32(dy);
        if(static_cast<s32>(ax)>=static_cast<s32>(ay)) {
            if(sx==-1) {
                put(6u,4u);
                if(sy==sx) { put(4u,3u); put(0u,2u); put(2u,1u); }
                else { put(0u,3u); put(4u,2u); put(2u,1u); }
            } else {
                put(2u,4u);
                if(sy==-1) { put(4u,3u); put(0u,2u); put(6u,1u); }
                else { put(0u,3u); put(4u,2u); put(6u,1u); }
            }
        } else if(static_cast<s32>(ay)>static_cast<s32>(ax)) {
            if(sy==-1) {
                put(4u,4u);
                if(sx==sy) { put(6u,3u); put(2u,2u); put(0u,1u); }
                else { put(2u,3u); put(6u,2u); put(0u,1u); }
            } else {
                put(0u,4u);
                if(sx==-1) { put(6u,3u); put(2u,2u); }
                else { put(2u,3u); put(6u,2u); }
                put(4u,1u);
            }
        }
    }

    const u32 wanted=4u-rank;
    u32 result=fallback; // aliases the caller's third stack-argument slot in the binary.
    if(wanted==0u) {
        const u32 random=a33&&a33->call_30ca2?a33->call_30ca2():0u;
        result=(random*8u)>>15u;
    } else {
        for(u32 i=0u;i<8u;++i) {
            if(static_cast<u32>(*dg(m,0x140B0u+i))==wanted) { result=i; break; }
        }
    }
    return result;
}
static_assert(0x1B1C2u-0x1AF90u==562u);
static_assert((0x1AF85u-0x1AE30u)+(0x1B1C2u-0x1AF90u)==903u);
