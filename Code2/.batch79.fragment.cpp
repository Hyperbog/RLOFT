// Batch 79: 32150..3221E 256-entry RGB palette rescaler.
void function_32150(Machine& m,u32 palette) noexcept {
    s32 strength=static_cast<u32>(*dg(m,load_le32(dg(m,0xF050u))+0x6Cu));
    if(strength>0x3F)strength=0x3F;
    if(strength<0)strength=0;
    if(strength==0)return;

    for(s32 i=0;i<=0xFF;++i){
        const u32 p=palette+static_cast<u32>(i)*3u;
        s32 peak=0;
        for(u32 j=0;j<3u;++j){const s32 v=*dg(m,p+j);if(v>peak)peak=v;}
        const s32 adjustment=(strength*(0x3F-peak)+0x1F)/0x3F;
        const s32 target=peak+adjustment;
        const s32 half=app74_sar1(peak);
        for(u32 j=0;j<3u;++j){
            const u8 v=*dg(m,p+j);
            if(target==0 || v==0u)continue;
            const s32 scaled=(static_cast<s32>(v)*target+half)/peak;
            *dg(m,p+j)=static_cast<u8>(scaled);
        }
    }
}
static_assert(0x3221Fu-0x32150u==207u);
// 3221F is NOP alignment with no incoming direct control-flow target.
