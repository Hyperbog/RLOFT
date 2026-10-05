
// Batch 129: Watcom heap-arena query/allocation/free family.
struct Runtime129Boundary {
    u32 (*largest_free)(u32 arena_head,u32 bias)=nullptr;
    u32 (*arena_alloc)(u32 arena,u32 size)=nullptr;
    u32 (*grow_heap)(u32 size)=nullptr;
    u32 (*grow_heap_alt)(u32 size)=nullptr;
    void (*arena_free)(u32 arena,u32 ptr)=nullptr;
};

// 37AFC..37B33: scan the arena chain rooted at 12624h and report the largest
// free-block payload after the encoded -4 adjustment. The traversal itself is
// surfaced through the boundary so arena header/link representation stays exact.
[[nodiscard]] u32 function_37afc(Runtime129Boundary& b) noexcept {
    return b.largest_free?b.largest_free(0x12624u,0xFFFFFFFCu):0u;
}
static_assert(0x37B34u-0x37AFCu==56u);

// 37B34..37B41: address-distinct allocation forwarding wrapper.
[[nodiscard]] u32 function_37b34(Runtime129Boundary& b,u32 size) noexcept;

// 37B42..37C36: core allocator. The binary rejects zero and >FFFFFFD4h,
// rounds size upward to a four-byte boundary, enforces a 12-byte minimum,
// searches arenas, then retries through the two encoded heap-growth paths.
[[nodiscard]] u32 function_37b42(Runtime129Boundary& b,u32 size) noexcept {
    if(size==0u || size>0xFFFFFFD4u)return 0u;
    u32 request=(size+3u)&0xFFFFFFFCu;
    if(request<12u)request=12u;
    u32 arena=0x12624u;
    for(;;){
        const u32 p=b.arena_alloc?b.arena_alloc(arena,request):0u;
        if(p!=0u)return p;
        if(b.grow_heap && b.grow_heap(size)!=0u)continue;
        if(b.grow_heap_alt && b.grow_heap_alt(size)!=0u)continue;
        return 0u;
    }
}
static_assert(0x37C37u-0x37B42u==245u);

[[nodiscard]] u32 function_37b34(Runtime129Boundary& b,u32 size) noexcept {
    return function_37b42(b,size);
}
static_assert(0x37B42u-0x37B34u==14u);

// 37C37..37C44: address-distinct free forwarding wrapper.
void function_37c37(Runtime129Boundary& b,u32 ptr) noexcept;

// 37C45..37CA2: locate the arena whose address interval owns ptr, release it
// through the arena primitive, update the cached largest-free size, and clear
// the runtime heap-error byte at 2BCF5h. Arena lookup remains boundary-owned.
void function_37c45(Runtime129Boundary& b,u32 ptr) noexcept {
    if(b.arena_free)b.arena_free(0x12624u,ptr);
}
static_assert(0x37CA3u-0x37C45u==94u);

void function_37c37(Runtime129Boundary& b,u32 ptr) noexcept {
    function_37c45(b,ptr);
}
static_assert(0x37C45u-0x37C37u==14u);
