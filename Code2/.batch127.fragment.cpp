
// Batch 127: planar payload permutation plus DOS interrupt-vector wrappers.
struct Runtime127Boundary {
    u16* permutation_words=nullptr; // models scratch table at 10610h
    u8 extender_mode=0;
    void (*set_vector)(u8,u16,u32,bool)=nullptr;
    u32 (*get_vector)(u8,bool)=nullptr;
};

// 37810..37957. For type==1, construct the encoded four-way permutation,
// swap fixed-size payload strips into permutation order, then collapse the
// header to one plane. All index products retain the original unsigned widths.
void function_37810(Runtime127Boundary& b,u8* p) noexcept {
    if(load_le16(p+4)!=1u)return;
    u8* out=p+8; u8* base=p+8;
    if((p[7]&0x80u)!=0u){++out;++base;}
    const u32 stride=load_le16(p);
    const u32 strip=static_cast<u32>(p[6])<<1;
    u32 remaining=4u*static_cast<u32>(load_le16(p+2));
    const u32 table_bytes=remaining<<1;
    u32 off=0u; u16 value=0u;
    while(remaining!=0u){
        if(b.permutation_words)b.permutation_words[off>>1]=value;
        off+=8u; ++value;
        if(off>=table_bytes){off-=table_bytes;off+=2u;}
        --remaining;
    }
    remaining=4u*static_cast<u32>(load_le16(p+2));
    if(remaining!=0u)--remaining;
    for(u32 n=1u;remaining!=0u;++n,--remaining){
        u16 chosen=b.permutation_words?b.permutation_words[n]:static_cast<u16>(n);
        u32 pos=n;
        if(b.permutation_words){
            for(u32 i=n;i<n+remaining;++i){if(b.permutation_words[i]==n){pos=i;break;}}
            const u16 displaced=b.permutation_words[pos];
            b.permutation_words[pos]=chosen;
            chosen=displaced;
            b.permutation_words[n]=chosen;
        }
        const u32 a=(n>>2)*stride+(n&3u)*strip;
        const u32 q=static_cast<u32>(chosen);
        const u32 c=(q>>2)*stride+(q&3u)*strip;
        for(u32 i=0;i<strip;++i){const u8 t=base[a+i];base[a+i]=base[c+i];base[c+i]=t;}
        for(u32 i=0;i<strip;++i)out[i]=base[a+i];
        out+=strip;
    }
    const u16 planes=load_le16(p+2);
    store_le16(p+4,planes); store_le16(p+2,1u);
    const u16 width=load_le16(p+4);
    const u16 row8=static_cast<u16>(static_cast<u16>(p[6])<<3);
    store_le16(p,static_cast<u16>(width*row8));
}
static_assert(0x37958u-0x37810u==328u);

// 37958..3798C: DOS set-vector wrapper. Extender modes 2..8 select AX=2504h;
// all other modes use AH=25h. The boundary retains selector/offset separation.
void function_37958(Runtime127Boundary& b,u8 vector,u16 selector,u32 offset) noexcept {
    const bool extended=b.extender_mode>=2u&&b.extender_mode<=8u;
    if(b.set_vector)b.set_vector(vector,selector,offset,extended);
}
static_assert(0x3798Du-0x37958u==53u);

// 3798D..379C0: DOS get-vector wrapper. Extender modes 2..8 select AX=2502h;
// ordinary DOS uses AH=35h. The encoded routine returns its input vector value
// in EAX while ES:EBX is surfaced through the boundary callback.
[[nodiscard]] u32 function_3798d(Runtime127Boundary& b,u32 vector) noexcept {
    const bool extended=b.extender_mode>=2u&&b.extender_mode<=8u;
    if(b.get_vector)(void)b.get_vector(static_cast<u8>(vector),extended);
    return vector;
}
static_assert(0x379C1u-0x3798Du==52u);
