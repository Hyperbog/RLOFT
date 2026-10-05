
// Batch 140: verified CODE.2 39838..39A2B runtime arena allocation/free helpers.
// The original routines receive the arena base in EBX and temporarily load DS
// from EDX. Object-relative reconstruction keeps the arena structure explicit.
[[nodiscard]] u32 function_39838(Machine& m,u32 arena,u32 request) noexcept {
    if(request==0u)return 0u;
    u32 size=request+7u;
    if(size<request)return 0u;
    size&=~3u;
    if(size<12u)size=12u;
    if(size>load_le32(dg(m,arena+0x14u)))return 0u;
    u32 cur=load_le32(dg(m,arena+0x0Cu));
    u32 rover=load_le32(dg(m,arena+0x10u));
    if(size<=rover){cur=load_le32(dg(m,arena+0x28u));rover=0u;}
    for(;;){
        const u32 block=load_le32(dg(m,cur));
        if(size<=block){
            const u32 remain=block-size;
            store_le32(dg(m,arena+0x10u),rover);
            store_le32(dg(m,arena+0x18u),load_le32(dg(m,arena+0x18u))+1u);
            const u32 next=load_le32(dg(m,cur+8u));
            if(remain>=12u){
                const u32 split=cur+size;
                store_le32(dg(m,arena+0x0Cu),split);
                store_le32(dg(m,split),remain);
                store_le32(dg(m,cur),size);
                const u32 prev=load_le32(dg(m,cur+4u));
                store_le32(dg(m,split+4u),prev); store_le32(dg(m,split+8u),next);
                store_le32(dg(m,prev+8u),split); store_le32(dg(m,next+4u),split);
            }else{
                store_le32(dg(m,arena+0x1Cu),load_le32(dg(m,arena+0x1Cu))-1u);
                const u32 prev=load_le32(dg(m,cur+4u));
                store_le32(dg(m,prev+8u),next); store_le32(dg(m,next+4u),prev);
                store_le32(dg(m,arena+0x0Cu),prev);
            }
            store_le32(dg(m,cur),load_le32(dg(m,cur))|1u);
            return cur+4u;
        }
        rover=(rover>=block)?rover-block:0u;
        cur=load_le32(dg(m,cur+8u));
        if(cur==arena+0x20u){store_le32(dg(m,arena+0x14u),rover);return 0u;}
    }
}
static_assert(0x398DBu-0x39838u==163u);

void function_398e0(Machine& m,u32 arena,u32 payload) noexcept {
    if(payload==0u)return;
    u32 block=payload-4u;
    u32 size=load_le32(dg(m,block));
    if((size&1u)==0u)return;
    size&=~1u;
    u32 after=block+size;
    if((load_le32(dg(m,after))&1u)==0u){
        if(after==load_le32(dg(m,arena+0x0Cu)))store_le32(dg(m,arena+0x0Cu),block);
        size+=load_le32(dg(m,after)); store_le32(dg(m,block),size);
        const u32 p=load_le32(dg(m,after+4u)),n=load_le32(dg(m,after+8u));
        store_le32(dg(m,p+8u),n); store_le32(dg(m,n+4u),p);
        store_le32(dg(m,arena+0x1Cu),load_le32(dg(m,arena+0x1Cu))-1u);
    }else store_le32(dg(m,block),size);
    u32 at=load_le32(dg(m,arena+0x0Cu));
    if(block<at){const u32 prev=load_le32(dg(m,at+4u)); if(block<=prev)at=load_le32(dg(m,arena+0x28u));}
    else {at=load_le32(dg(m,at+8u)); if(block<at){} else {at=arena+0x20u;}}
    // The encoded search may advance several free-list links before insertion.
    while(block>=load_le32(dg(m,at+8u)) && load_le32(dg(m,at+8u))!=arena+0x20u)at=load_le32(dg(m,at+8u));
    u32 prev=load_le32(dg(m,at+4u));
    if(prev+load_le32(dg(m,prev))==block){
        size+=load_le32(dg(m,prev)); store_le32(dg(m,prev),size); block=prev;
        if(payload-4u==load_le32(dg(m,arena+0x0Cu)))store_le32(dg(m,arena+0x0Cu),prev);
    }else{
        store_le32(dg(m,arena+0x1Cu),load_le32(dg(m,arena+0x1Cu))+1u);
        store_le32(dg(m,block+8u),at); store_le32(dg(m,block+4u),prev);
        store_le32(dg(m,prev+8u),block); store_le32(dg(m,at+4u),block);
    }
    store_le32(dg(m,arena+0x18u),load_le32(dg(m,arena+0x18u))-1u);
    if(block<load_le32(dg(m,arena+0x0Cu)) && size>load_le32(dg(m,arena+0x10u)))store_le32(dg(m,arena+0x10u),size);
    if(size>load_le32(dg(m,arena+0x14u)))store_le32(dg(m,arena+0x14u),size);
}
static_assert(0x399EBu-0x398E0u==267u);

void function_399eb(Machine& m,u32 node) noexcept {
    if(node==load_le32(dg(m,0x12628u)))store_le32(dg(m,0x12628u),load_le32(dg(m,node+8u)));
    if(node==load_le32(dg(m,0x12624u)))store_le32(dg(m,0x12624u),load_le32(dg(m,node+8u)));
    const u32 prev=load_le32(dg(m,node+4u)),next=load_le32(dg(m,node+8u));
    if(prev)store_le32(dg(m,prev+8u),next);
    if(next)store_le32(dg(m,next+4u),prev);
}
static_assert(0x39A2Cu-0x399EBu==65u);
static_assert((0x398DBu-0x39838u)+(0x399EBu-0x398E0u)+(0x39A2Cu-0x399EBu)==500u);
