// Batch 94: 338C8..33959 fixed-point column scaler.
struct Application94Boundary {
    // Binary local [EBP-8] has no defining write on the nonnegative-start path.
    // Supply the incoming stack residue explicitly rather than inventing a value.
    u32 incoming_local_8=0u;
};

void function_338c8(Machine& m,Application94Boundary& b,u32 subtract_pair,
                    u32 scale,u32 source,u32 destination) noexcept {
    const s32 divisor=static_cast<s32>(scale);
    const s32 signed_step=static_cast<s32>(0x00800000u)/divisor;
    const u32 step=static_cast<u32>(signed_step);

    u32 count=scale;
    u32 start=static_cast<u32>(static_cast<s32>(0x100u-scale)>>1);
    start+=static_cast<u32>(static_cast<s32>(scale)>>3);
    u32 phase=b.incoming_local_8;
    if(static_cast<s32>(start)<0){
        const u32 magnitude=0u-start;
        phase=step*magnitude;
        start=0u;
        count=0x100u;
    }

    destination+=0x50u*start;
    u32 combined=count+start;
    if(static_cast<s32>(combined)>0x100){
        combined-=0x100u;
        count-=combined;
    }

    u32 iterations=count&0xFFu;
    if(iterations==0u)iterations=0x100u;
    u32 subtract_byte=subtract_pair&0xFFu;
    const u32 other_byte=(subtract_pair>>8)&0xFFu;
    for(u32 i=0u;i<iterations;++i){
        const u32 source_index=phase>>16;
        const u32 sample=static_cast<u32>(*dg(m,source+source_index));
        *dg(m,destination)=static_cast<u8>(sample-subtract_byte);
        destination+=0x50u;
        phase+=step;
        subtract_byte=(subtract_byte==other_byte)?(subtract_pair&0xFFu):other_byte;
    }
}
static_assert(0x3395Au-0x338C8u==146u);
// 3395A..3395B is XCHG EBX,EBX alignment with no incoming direct target.
