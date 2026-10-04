// -----------------------------------------------------------------------------
// Batch 31: 171FC..181B3 save-file serialization, party state normalization,
// and world-record predicate cleanup. 179A2..179A3 is compiler alignment and
// 179A4..17A67 is the exact 49-entry CS jump table consumed at 17C50.
// -----------------------------------------------------------------------------

struct Application31Boundary {
    u32 (*call_31380)(u32)=nullptr;
    u32 (*call_312c8)(u32)=nullptr;
    void (*call_312dc)(u32)=nullptr;
    s32 (*call_312b4)(u32)=nullptr;
    void (*call_2f580)(u32)=nullptr;
    u32 (*call_3131c)(u32,u32,u32)=nullptr;
    u32 (*call_31304)(u32,u32,u32)=nullptr;
    u32 (*call_31304_local)(const void*,u32,u32)=nullptr;
    void (*call_01558)()=nullptr;
    u32 (*call_1c1dc)(u32)=nullptr;
    u32 (*call_30ca2)()=nullptr;
    void (*call_1c77c)(u32,u32)=nullptr;
    u32 (*call_2acdc)(u32)=nullptr;
    u32 (*call_1c328)(u32,u32,u32,u32)=nullptr;
};

static void app31_write_error(Machine& m,Application31Boundary& b) noexcept {
    if(b.call_2f580)b.call_2f580(load_le32(dg(m,0x8190u)));
}
static void app31_write_guest(Machine& m,Application31Boundary& b,u32 handle,u32 src,u32 bytes) noexcept {
    const u32 n=b.call_31304?b.call_31304(src,bytes,handle):bytes;
    if(n!=bytes)app31_write_error(m,b);
}
static void app31_write_local(Machine& m,Application31Boundary& b,u32 handle,const void* src,u32 bytes) noexcept {
    const u32 n=b.call_31304_local?b.call_31304_local(src,bytes,handle):bytes;
    if(n!=bytes)app31_write_error(m,b);
}

void function_171fc(Machine& m,Application31Boundary& b,s32 save_number) noexcept {
    if(save_number==-1||*dg(m,0xA79Du)!=0u)return;

    const s32 hundreds=save_number/100;
    const s32 remainder=save_number-hundreds*100;
    *dg(m,0x81A4u)=static_cast<u8>(hundreds+0x30);
    *dg(m,0x81A6u)=static_cast<u8>((save_number%10)+0x30);
    *dg(m,0x81A5u)=static_cast<u8>((remainder/10)+0x30);

    if(b.call_31380)(void)b.call_31380(0x819Cu);
    const u32 old_handle=b.call_312c8?b.call_312c8(0x819Cu):0u;
    if(b.call_312dc)b.call_312dc(old_handle);
    const s32 opened=b.call_312b4?b.call_312b4(0x819Cu):-1;
    const u32 handle=static_cast<u32>(opened);
    if(opened==-1&&b.call_2f580)b.call_2f580(load_le32(dg(m,0x8194u)));

    if(b.call_3131c)(void)b.call_3131c(0x1Du,0u,handle);
    app31_write_guest(m,b,handle,0x25618u,0x2A4u);

    const s32 width=static_cast<s16>(load_le16(dg(m,0x25619u)));
    const s32 height=static_cast<s16>(load_le16(dg(m,0x2561Bu)));
    u32 off0=0x1Du;
    u32 off1=off0+0x2A4u;
    for(s32 row=0;row<height;++row) {
        const u32 bytes=static_cast<u32>(static_cast<s32>(width*2));
        const u32 src=static_cast<u32>(0x198E0 + static_cast<s32>(row*width*2));
        app31_write_guest(m,b,handle,src,bytes);
        off1+=bytes;
    }

    u32 off2=off1;
    for(s32 i=0;i<width;++i) {
        const u32 p=load_le32(dg(m,0xCB3Cu+static_cast<u32>(i)*4u));
        u32 count=0u;
        if(p!=0u) {
            for(;;++count) {
                const u32 q=p+count*10u;
                if(*dg(m,q)==0u&&(*dg(m,q+1u)&0x10u)==0u)break;
            }
        }
        const u16 count16=static_cast<u16>(count);
        app31_write_local(m,b,handle,&count16,2u);
        off2+=2u;
        if(count!=0u) {
            const u32 bytes=count*10u;
            app31_write_guest(m,b,handle,p,bytes);
            off2+=bytes;
        }
    }

    app31_write_guest(m,b,handle,0xCF50u,1u);
    ++off2;
    for(s32 i=0;i<width;++i) {
        const u32 p=load_le32(dg(m,0xCD3Cu+static_cast<u32>(i)*4u));
        u32 count=0u;
        if(p!=0u)while(*dg(m,p+count*10u)!=0u)++count;
        const u16 count16=static_cast<u16>(count);
        app31_write_local(m,b,handle,&count16,2u);
        off2+=2u;
        if(count!=0u) {
            const u32 bytes=count*10u;
            app31_write_guest(m,b,handle,p,bytes);
            off2+=bytes;
        }
    }

    const s32 moving_count=static_cast<s16>(load_le16(dg(m,0x258A2u)));
    const u32 moving_bytes=static_cast<u32>(static_cast<s32>(moving_count*0x16));
    app31_write_guest(m,b,handle,0x14D80u,moving_bytes);
    const u32 off3=off2+moving_bytes;

    app31_write_guest(m,b,handle,0x258A6u,2u);
    const s32 primary_count=static_cast<s16>(load_le16(dg(m,0x258A6u)));
    const u32 primary_bytes=static_cast<u32>(static_cast<s32>(primary_count*0x16));
    app31_write_guest(m,b,handle,0x22238u,primary_bytes);
    const u32 off4=off3+primary_bytes+2u;

    const s32 alternate_count=static_cast<s16>(load_le16(dg(m,0x258A4u)));
    const u32 alternate_bytes=static_cast<u32>(static_cast<s32>(alternate_count*0x16));
    app31_write_guest(m,b,handle,0x17618u,alternate_bytes);
    const u32 off5=off4+alternate_bytes;

    app31_write_guest(m,b,handle,load_le32(dg(m,0xCF44u)),0x514u);
    const u32 off6=off5+0x514u;
    app31_write_guest(m,b,handle,0x25619u,2u);
    app31_write_guest(m,b,handle,0x2561Bu,2u);
    if(b.call_01558)b.call_01558();
    const u32 cell_bytes=static_cast<u32>(static_cast<s32>(width*height));
    app31_write_guest(m,b,handle,0x1760u,cell_bytes);

    const u32 packed_bytes=function_01c28(m);
    app31_write_local(m,b,handle,&packed_bytes,4u);
    app31_write_guest(m,b,handle,0x5760u,packed_bytes);

    if(b.call_3131c)(void)b.call_3131c(1u,0u,handle);
    app31_write_local(m,b,handle,&off0,4u);
    app31_write_local(m,b,handle,&off1,4u);
    app31_write_local(m,b,handle,&off2,4u);
    app31_write_local(m,b,handle,&off3,4u);
    app31_write_local(m,b,handle,&off4,4u);
    app31_write_local(m,b,handle,&off5,4u);
    app31_write_local(m,b,handle,&off6,4u);
    if(b.call_312dc)b.call_312dc(handle);
}
static_assert(0x17838u-0x171FCu==1596u);

void function_17838(Machine& m,Application31Boundary& b) noexcept {
    static constexpr u32 blocked_with_37[]={0x15u,0x14u,0x0Au,0x0Bu,0x06u,0x07u,0x12u,0x08u,0x0Fu};
    static constexpr u32 blocked_without_37[]={0x15u,0x14u,0x0Au,0x0Bu,0x0Cu,0x18u,0x05u,0x12u,0x08u,0x0Fu};
    for(u32 slot=0u;slot<4u;++slot) {
        const u32 base=slot*0x248u;
        const u16 state=load_le16(dg(m,0x218F0u+base));
        if((state&0x8000u)!=0u||(state&0x0100u)==0u)continue;
        *dg(m,0x218F0u+base)=static_cast<u8>(*dg(m,0x218F0u+base)&0xBFu);
        const u32 r37=b.call_1c1dc?b.call_1c1dc(0x37u):0u;
        const u32 area=load_le32(dg(m,0xC6C8u));
        bool blocked=false;
        if(r37!=0u)for(u32 v:blocked_with_37)if(area==v){blocked=true;break;}
        if(r37==0u)for(u32 v:blocked_without_37)if(area==v){blocked=true;break;}
        if(blocked)continue;
        const u32 random=b.call_30ca2?b.call_30ca2():0u;
        const u32 scaled=(random*4u)>>15u;
        store_le32(dg(m,0xBF2Cu),scaled+0x32u);
        if(b.call_1c77c)b.call_1c77c(slot,scaled+0x15Fu);
        *dg(m,0x218F0u+base)=static_cast<u8>(*dg(m,0x218F0u+base)|0x40u);
    }
}
static_assert(0x179A2u-0x17838u==362u);

static u32 app31_resource(Application31Boundary& b,u32 id) noexcept {
    return b.call_1c1dc?b.call_1c1dc(id):0u;
}

void function_17a68(Machine& m,Application31Boundary& b) noexcept {
    function_17838(m,b);
    for(s32 index=0;;++index) {
        const s32 count=static_cast<s16>(load_le16(dg(m,0x258A2u)));
        if(index>=count)return;
        const u32 rec=0x14D80u+static_cast<u32>(index)*0x16u;
        const u16 type=load_le16(dg(m,rec+0x0Eu));
        *dg(m,rec+0x0Bu)=static_cast<u8>(*dg(m,rec+0x0Bu)&0xFDu);
        bool mark=false;
        if((b.call_2acdc?b.call_2acdc(type):0u)!=0u&&static_cast<s16>(load_le16(dg(m,rec+0x12u)))<=0)mark=true;
        const u32 area=load_le32(dg(m,0xC6C8u));
        const u32 phase=load_le32(dg(m,0xCF4Cu))>>16u;
        const bool phase_window=phase>4u&&phase<0x15u;

        bool handled=true;
        switch(type) {
            case 0x00u: if((area==0x12u||area==0x1Fu)&&phase_window)mark=true; break;
            case 0x06u: if((area==0x19u||area==0x1Au)&&phase_window)mark=true; break;
            case 0x08u: if(app31_resource(b,0x3Au)==0u)mark=true; break;
            case 0x0Fu: if(app31_resource(b,0x28u)!=0u&&app31_resource(b,0x29u)!=0u)mark=true; break;
            case 0x11u: if(area==0x12u&&phase_window)mark=true; break;
            case 0x15u: case 0x50u: if(area==0x18u&&phase_window)mark=true; break;
            case 0x18u: case 0x19u: case 0x1Au: if(area==0x0Bu&&phase_window)mark=true; break;
            default: handled=false; break;
        }
        if(!handled&&type>=0x22u&&type<=0x52u) {
            switch(type) {
                case 0x22u: mark|=app31_resource(b,0x1Du)==1u; break;
                case 0x24u: mark|=app31_resource(b,0x1Eu)==1u; break;
                case 0x26u: mark|=app31_resource(b,0x08u)==1u||app31_resource(b,0x13u)==1u; break;
                case 0x27u: mark|=app31_resource(b,0x0Eu)==1u||app31_resource(b,0x0Du)==1u||app31_resource(b,0x18u)==1u; break;
                case 0x28u: mark|=app31_resource(b,0x0Eu)==0u||app31_resource(b,0x0Du)==1u||app31_resource(b,0x18u)==1u; break;
                case 0x29u: case 0x39u: mark|=app31_resource(b,0x2Cu)==1u; break;
                case 0x2Au: case 0x3Au: mark|=app31_resource(b,0x2Cu)==0u; break;
                case 0x2Cu: mark|=app31_resource(b,0x06u)==1u||app31_resource(b,0x11u)==1u||app31_resource(b,0x30u)==0u; break;
                case 0x2Du: mark|=app31_resource(b,0x04u)==1u||app31_resource(b,0x0Fu)==1u||app31_resource(b,0x19u)==1u; break;
                case 0x2Eu: mark|=app31_resource(b,0x07u)==1u||app31_resource(b,0x12u)==1u||app31_resource(b,0x1Bu)==1u; break;
                case 0x2Fu: mark|=app31_resource(b,0x07u)==1u||app31_resource(b,0x12u)==1u||app31_resource(b,0x1Bu)==0u; break;
                case 0x30u: mark|=app31_resource(b,0x09u)==1u||app31_resource(b,0x45u)==1u||app31_resource(b,0x14u)==1u; break;
                case 0x31u: mark|=app31_resource(b,0x0Au)==1u||app31_resource(b,0x15u)==1u||app31_resource(b,0x23u)==1u; break;
                case 0x33u: mark|=app31_resource(b,0x2Du)==1u; break;
                case 0x34u: mark|=app31_resource(b,0x2Fu)==1u; break;
                case 0x35u: mark|=app31_resource(b,0x2Eu)==1u; break;
                case 0x36u: case 0x47u: mark|=app31_resource(b,0x2Fu)==1u||app31_resource(b,0x39u)==0u; break;
                case 0x37u: mark|=app31_resource(b,0x27u)==1u; break;
                case 0x3Du: mark|=app31_resource(b,0x0Cu)==1u||app31_resource(b,0x17u)==1u; break;
                case 0x3Fu: mark|=app31_resource(b,0x05u)==1u||app31_resource(b,0x10u)==1u||app31_resource(b,0x1Cu)==1u; break;
                case 0x40u: mark|=app31_resource(b,0x0Bu)==1u||app31_resource(b,0x16u)==1u; break;
                case 0x42u: mark|=app31_resource(b,0x1Eu)==0u; break;
                case 0x43u: mark|=app31_resource(b,0x04u)==1u||app31_resource(b,0x0Fu)==1u||app31_resource(b,0x19u)==0u; break;
                case 0x44u: mark|=app31_resource(b,0x0Au)==1u||app31_resource(b,0x15u)==1u||app31_resource(b,0x23u)==0u; break;
                case 0x45u: mark|=app31_resource(b,0x05u)==1u||app31_resource(b,0x10u)==1u||app31_resource(b,0x1Cu)==0u; break;
                case 0x46u: mark|=app31_resource(b,0x1Du)==0u; break;
                case 0x4Eu:
                    if(app31_resource(b,0x25u)!=0u) {
                        store_le16(dg(m,rec+0x0Eu),1u);
                        store_le32(dg(m,rec+0x08u),0x10000000u);
                        const u32 desc=load_le32(dg(m,0x22230u))+0x45u;
                        const u32 rank=*dg(m,desc+0x1Du);
                        const u32 roll=b.call_1c328?b.call_1c328(rank,8u,0u,0u):0u;
                        store_le16(dg(m,rec+0x12u),static_cast<u16>(roll));
                    }
                    break;
                case 0x52u: mark|=app31_resource(b,0x14u)==1u||app31_resource(b,0x09u)==1u; break;
                default: break;
            }
        }
        if(mark)*dg(m,rec+0x0Bu)=static_cast<u8>(*dg(m,rec+0x0Bu)|2u);
    }
}
static_assert(0x181B4u-0x17A68u==1868u);
