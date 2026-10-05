// Batch 104: 33FB4..34267 edge steppers, edge construction, and clipped raster writer.
struct Runtime104Boundary {
    void (*out16)(u16,u16)=nullptr;
};

static u32 app104_sar1(u32 v) noexcept {
    return (v>>1)|(v&0x80000000u);
}
static u32 app104_sar16(u32 v) noexcept {
    return (v>>16)|((v&0x80000000u)?0xFFFF0000u:0u);
}
// Reconstruct the encoded ROL/CWD/(ROL|SHL)/XCHG DX,AX + IDIV sequence.
// It forms signed32(difference_bits)<<16 in EDX:EAX before signed division.
// The binary traps if divisor_bits is zero; valid callers supply a nonzero encoded denominator.
static u32 app104_idiv_shift16(u32 difference_bits,u32 divisor_bits) noexcept {
    const s32 difference=static_cast<s32>(difference_bits);
    const s32 divisor=static_cast<s32>(divisor_bits);
    const long long numerator=static_cast<long long>(difference)*65536LL;
    return static_cast<u32>(static_cast<s32>(numerator/static_cast<long long>(divisor)));
}

bool function_3402c(Machine& m,u32 vertex,u32 state) noexcept {
    for(;;){
        if(vertex==load_le32(dg(m,0xFD24u)))return false;
        const u32 next=(vertex+load_le32(dg(m,state)))&3u;
        const u32 next_off=next<<3;
        const u32 cur_off=vertex<<3;
        const u32 delta_y=load_le32(dg(m,0xFD30u+next_off))-load_le32(dg(m,0xFD30u+cur_off));
        store_le32(dg(m,state+0x04u),delta_y);
        if(delta_y==0u){vertex=next;continue;}

        store_le32(dg(m,state+0x08u),next);
        const u32 cur_a=load_le32(dg(m,0xFF18u+cur_off))<<16;
        const u32 cur_b=load_le32(dg(m,0xFF14u+cur_off))<<16;
        store_le32(dg(m,state+0x0Cu),cur_a);
        store_le32(dg(m,state+0x10u),cur_b);
        const u32 divisor=delta_y<<16;
        const u32 next_a=load_le32(dg(m,0xFF18u+next_off))<<16;
        const u32 next_b=load_le32(dg(m,0xFF14u+next_off))<<16;
        store_le32(dg(m,state+0x14u),app104_idiv_shift16(next_a-cur_a,divisor));
        store_le32(dg(m,state+0x18u),app104_idiv_shift16(next_b-cur_b,divisor));
        const u32 cur_x=load_le32(dg(m,0xFD2Cu+cur_off));
        store_le32(dg(m,state+0x1Cu),cur_x<<16);
        const u32 delta_x=load_le32(dg(m,0xFD2Cu+next_off))-cur_x;
        store_le32(dg(m,state+0x20u),app104_idiv_shift16(delta_x<<16,divisor));
        return true;
    }
}

bool function_33fb4(Machine& m) noexcept {
    u32 count=load_le32(dg(m,0xFDC8u))-1u;
    store_le32(dg(m,0xFDC8u),count);
    if(count==0u){
        if(!function_3402c(m,load_le32(dg(m,0xFDCCu)),0xFDC4u))return false;
    }else{
        store_le32(dg(m,0xFDD0u),load_le32(dg(m,0xFDD0u))+load_le32(dg(m,0xFDD8u)));
        store_le32(dg(m,0xFDD4u),load_le32(dg(m,0xFDD4u))+load_le32(dg(m,0xFDDCu)));
        store_le32(dg(m,0xFDE0u),load_le32(dg(m,0xFDE0u))+load_le32(dg(m,0xFDE4u)));
    }

    count=load_le32(dg(m,0xFDECu))-1u;
    store_le32(dg(m,0xFDECu),count);
    if(count==0u)return function_3402c(m,load_le32(dg(m,0xFDF0u)),0xFDE8u);
    store_le32(dg(m,0xFDF4u),load_le32(dg(m,0xFDF4u))+load_le32(dg(m,0xFDFCu)));
    store_le32(dg(m,0xFDF8u),load_le32(dg(m,0xFDF8u))+load_le32(dg(m,0xFE00u)));
    store_le32(dg(m,0xFE04u),load_le32(dg(m,0xFE04u))+load_le32(dg(m,0xFE08u)));
    return true;
}

void function_34110(Machine& m,Runtime104Boundary& b) noexcept {
    s32 first=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0xFDE2u))));
    if(first>=static_cast<s32>(load_le32(dg(m,0xFDB4u))))return;
    s32 last=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0xFE06u))));
    if(last<=static_cast<s32>(load_le32(dg(m,0xFDB8u))))return;
    const u32 span=static_cast<u32>(last-first+1);
    if(span==0u)return;
    const u32 divisor=span<<16;

    const u32 step_u=app104_idiv_shift16(load_le32(dg(m,0xFDF4u))-load_le32(dg(m,0xFDD0u)),divisor);
    store_le32(dg(m,0xFE0Cu),step_u);
    u32 source_u=app104_sar1(step_u)+load_le32(dg(m,0xFDD0u));
    const u32 step_v=app104_idiv_shift16(load_le32(dg(m,0xFDF8u))-load_le32(dg(m,0xFDD4u)),divisor);
    store_le32(dg(m,0xFE10u),step_v);
    u32 source_v=app104_sar1(step_v)+load_le32(dg(m,0xFDD4u));

    const s32 right_clip=static_cast<s32>(load_le32(dg(m,0xFDB4u)));
    if(last>right_clip)last=right_clip;
    const s32 left_clip=static_cast<s32>(load_le32(dg(m,0xFDB8u)));
    if(first<left_clip){
        const u32 distance=static_cast<u32>(left_clip-first);
        source_u+=step_u*distance;
        source_v+=step_v*distance;
        first=left_clip;
    }

    u32 edx=source_u;
    u32 edi=source_v<<6;
    const u32 scaled_step_v=step_v<<6;
    store_le32(dg(m,0xFE10u),scaled_step_v);
    u32 esi=app104_sar16((edi&0xFFC00000u)+edx)+load_le32(dg(m,0xFDC0u));
    u32 ebx=static_cast<u32>(first)*0x50u;
    ebx+=(load_le32(dg(m,0xFD20u))>>2)+load_le32(dg(m,0xFDBCu));

    const u32 plane=load_le32(dg(m,0xFD20u))&3u;
    const u16 sequencer=static_cast<u16>((static_cast<u16>(1u<<plane)<<8)|0x02u);
    if(b.out16)b.out16(0x03C4u,sequencer);

    u32 ecx=static_cast<u32>(last-first+1);
    s32 row=first;
    do {
        u8 al=*dg(m,esi);
        al=static_cast<u8>(al-*dg(m,0xFE10u+static_cast<u32>(row)));
        ++row;
        *dg(m,ebx)=al;
        edx+=load_le32(dg(m,0xFE0Cu));
        edi+=load_le32(dg(m,0xFE10u));
        esi=app104_sar16((edi&0xFFC00000u)+edx)+load_le32(dg(m,0xFDC0u));
        ebx+=0x50u;
        --ecx;
    } while(ecx!=0u);
}

static_assert(0x3402Cu-0x33FB4u==120u);
static_assert(0x3410Cu-0x3402Cu==224u);
// 3410C..3410D is the physical shared LEAVE/RET tail targeted backward by 34110; its semantics are represented by the early returns above.
// 3410E..3410F is unreferenced XCHG EBX,EBX alignment.
static_assert(0x34268u-0x34110u==344u);
// 34268..3426F is eight unreferenced zero padding bytes; 34270 has independent callers.
