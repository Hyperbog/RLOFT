
// Batch 138: verified CODE.2 39589..3968D runtime-startup/handler helpers.
struct Runtime138Boundary {
    u32 (*stack_available)()=nullptr;          // 3A165
    void (*runtime_prepare)()=nullptr;         // 3A38A
    u32 (*call_20608)(u32,u32)=nullptr;
    void (*call_352be)(u32)=nullptr;
    void (*invoke_handler)(u32,u32)=nullptr;   // address, pushed dword argument
    s32 (*seek)(u32,s32,u8)=nullptr;           // 38407
};

// 39589..395E1: establish the runtime scratch-stack watermark, perform runtime
// preparation, then forward the published 2BD00/2BD04 pair through 20608 and
// pass its result to 352BE.  The original only subtracts the aligned 1264C
// amount from ESP when it is strictly below 3A165's available-stack result.
void function_39589(Machine& m,Runtime138Boundary& b) noexcept {
    const u32 raw=load_le32(dg(m,0x1264Cu));
    const u32 aligned=(raw+3u)&0xFFFFFFFCu;
    const u32 available=b.stack_available?b.stack_available():0u;
    const u32 scratch=(aligned<available)?aligned:0u;
    // 12650 receives ESP-after-optional-subtraction + the unrounded 1264C.
    // The portable reconstruction exposes the same displacement from the
    // caller's current abstract ESP without manufacturing a host stack pointer.
    store_le32(dg(m,0x12650u),m.cpu.esp-scratch+raw);
    if(b.runtime_prepare)b.runtime_prepare();
    const u32 a=load_le32(dg(m,0x2BD00u));
    const u32 d=load_le32(dg(m,0x2BD04u));
    const u32 r=b.call_20608?b.call_20608(a,d):0u;
    if(b.call_352be)b.call_352be(r);
}
static_assert(0x395E2u-0x39589u==89u);

// 395E2..3962C: repeatedly choose the highest-threshold non-state-2 record in
// the four 6-byte records [12720,12738), invoke its callback with the original
// EAX dword, then mark that record state 2.  Selection is threshold <= AL.
void function_395e2(Machine& m,Runtime138Boundary& b,u32 eax) noexcept {
    const u8 limit=static_cast<u8>(eax);
    for(;;){
        u32 best=0x12738u;
        u8 threshold=limit;
        for(u32 p=0x12720u;p<0x12738u;p+=6u){
            if(*dg(m,p)==2u)continue;
            const u8 t=*dg(m,p+1u);
            if(t<=threshold){best=p;threshold=t;}
        }
        if(best==0x12738u)break;
        const u32 callback=load_le32(dg(m,best+2u));
        if(callback!=0u&&b.invoke_handler)b.invoke_handler(callback,eax);
        *dg(m,best)=2u;
    }
}
static_assert(0x3962Du-0x395E2u==75u);

// 3962D..3967B: analogous one-record upper-threshold dispatcher over
// [12738,1273E).  DL is replaced by incoming AL and DH by incoming DL before
// the callback argument is pushed; the callback is suppressed when the chosen
// threshold is above the original DL bound.  The record is still marked 2.
void function_3962d(Machine& m,Runtime138Boundary& b,u32 eax,u32 edx) noexcept {
    const u8 low=static_cast<u8>(eax);
    const u8 high=static_cast<u8>(edx);
    const u32 callback_arg=(edx&0xFFFF0000u)|(static_cast<u32>(high)<<8)|low;
    for(;;){
        u32 best=0x1273Eu;
        u8 threshold=low;
        for(u32 p=0x12738u;p<0x1273Eu;p+=6u){
            if(*dg(m,p)==2u)continue;
            const u8 t=*dg(m,p+1u);
            if(threshold<=t){best=p;threshold=t;}
        }
        if(best==0x1273Eu)break;
        const u32 callback=load_le32(dg(m,best+2u));
        if(threshold<=high&&callback!=0u&&b.invoke_handler)b.invoke_handler(callback,callback_arg);
        *dg(m,best)=2u;
    }
}
static_assert(0x3967Cu-0x3962Du==79u);

// 3967C..3968D: seek-to-end forwarding wrapper: 38407(handle,0,1).
[[nodiscard]] s32 function_3967c(Runtime138Boundary& b,u32 handle) noexcept {
    return b.seek?b.seek(handle,0,1u):-1;
}
static_assert(0x3968Eu-0x3967Cu==18u);
