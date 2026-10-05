
// Batch 133: object-relative 384AA..385AF (262 translated bytes).
struct Runtime133Boundary {
    bool (*dos_get_current_directory)(u8,u8*) = nullptr; // AH=47h, DL=0 means current drive
    u8 (*dos_get_default_drive)() = nullptr;             // AH=19h, zero-based
    void (*set_runtime_error)(u32) = nullptr;            // 37CC2
    u32 (*alloc)(u32) = nullptr;                         // 37B34
    u32 (*copy_path)(u32,const u8*,u32) = nullptr;       // 33971
    u32 first_stream_link = 0u;                          // [2BCF0]
    u32 (*read32)(u32) = nullptr;
    u8 (*read8)(u32) = nullptr;
    void (*flush_stream)(u32) = nullptr;                 // 37D60
};

static u32 function_384aa(Runtime133Boundary& rt, u32 destination, u32 capacity) {
    u8 local[0x90]{};
    if (!rt.dos_get_current_directory || !rt.dos_get_current_directory(0u, local + 3u)) {
        if (rt.set_runtime_error) rt.set_runtime_error(1u);
        return 0u;
    }

    const u8 drive = rt.dos_get_default_drive ? rt.dos_get_default_drive() : 0u;
    local[0] = static_cast<u8>(drive + static_cast<u8>('A'));
    local[1] = static_cast<u8>(':');
    local[2] = static_cast<u8>('\\');

    u32 length = 0u;
    while (length < sizeof(local) && local[length] != 0u) ++length;
    const u32 needed = length + 1u;
    if (destination == 0u) {
        destination = rt.alloc ? rt.alloc(needed) : 0u;
        if (destination == 0u) {
            if (rt.set_runtime_error) rt.set_runtime_error(5u);
            return 0u;
        }
        capacity = needed;
    }
    return rt.copy_path ? rt.copy_path(destination, local, capacity) : destination;
}
static_assert(0x3854Eu - 0x384AAu == 164u);

static u32 function_3855c(u32 value) noexcept {
    if (value >= static_cast<u32>('a') && value <= static_cast<u32>('z')) value -= 0x20u;
    return value;
}
static u32 function_3854e(u32 value) noexcept { return function_3855c(value); }
static_assert(0x3856Eu - 0x3854Eu == 32u);

static u32 function_38579(Runtime133Boundary& rt, u32 mask) {
    u32 link = rt.first_stream_link;
    u32 matched = 0u;
    while (link != 0u) {
        const u32 stream = rt.read32 ? rt.read32(link + 4u) : 0u;
        const u32 flags = rt.read32 ? rt.read32(stream + 0x0Cu) : 0u;
        if ((flags & mask) != 0u) {
            ++matched;
            const u8 state = rt.read8 ? rt.read8(stream + 0x0Du) : 0u;
            if ((state & 0x10u) != 0u && rt.flush_stream) rt.flush_stream(stream);
        }
        link = rt.read32 ? rt.read32(link) : 0u;
    }
    return matched;
}
static u32 function_3856e(Runtime133Boundary& rt) { return function_38579(rt, 0xFFFFFFFFu); }
static_assert(0x385B0u - 0x3856Eu == 66u);
