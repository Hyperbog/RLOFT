
// Batch 132: object-relative 38106..384A9 (932 translated bytes).
struct Runtime132Boundary {
    u32 (*descriptor_flags)(u32) = nullptr;       // 3A0C4
    void (*set_runtime_error)(u32) = nullptr;     // 37CC2
    void (*translate_dos_error)(u16) = nullptr;   // 390D7
    s32 (*dos_read)(u32,u32,u32) = nullptr;       // AH=3Fh, carry folded into sign
    s32 (*dos_write)(u32,u32,u32) = nullptr;      // AH=40h, carry folded into sign
    s32 (*dos_seek)(u32,s32,u8) = nullptr;        // AH=42h, signed error / 32-bit position
    s32 (*dos_delete)(u32) = nullptr;             // AH=41h
    u32 (*runtime_version)() = nullptr;            // 3A165
    u32 (*runtime_version_old)() = nullptr;        // 3A1A7
    u8 (*read8)(u32) = nullptr;
    void (*write8)(u32,u8) = nullptr;
    u8 (*get_default_drive)() = nullptr;           // AH=19h, zero-based
    u8 (*set_default_drive)(u8) = nullptr;         // AH=0Eh, zero-based; returns drive count
};

static s32 function_38106(Runtime132Boundary& rt, u32 handle, u32 dst, u32 count) {
    const u32 flags = rt.descriptor_flags ? rt.descriptor_flags(handle) : 0u;
    if (flags == 0u) {
        if (rt.set_runtime_error) rt.set_runtime_error(4u);
        return -1;
    }
    if ((flags & 1u) == 0u) {
        if (rt.set_runtime_error) rt.set_runtime_error(6u);
        return -1;
    }
    if ((flags & 0x40u) != 0u) {
        const s32 got = rt.dos_read ? rt.dos_read(handle, dst, count) : -1;
        if (got < 0) {
            if (rt.translate_dos_error) rt.translate_dos_error(static_cast<u16>(got));
            return -1;
        }
        return got;
    }

    u32 remaining = count;
    u32 out = dst;
    s32 logical = 0;
    while (remaining != 0u) {
        const s32 got = rt.dos_read ? rt.dos_read(handle, out, remaining) : -1;
        if (got < 0) {
            if (rt.translate_dos_error) rt.translate_dos_error(static_cast<u16>(got));
            return -1;
        }
        if (got == 0) break;

        const u32 raw = static_cast<u32>(got);
        u32 consumed = 0u;
        u32 produced = 0u;
        while (consumed < raw) {
            const u8 ch = rt.read8 ? rt.read8(out + consumed) : 0u;
            if (ch == 0x1Au) {
                const s32 rewind = static_cast<s32>(consumed) - got + 1;
                if (rt.dos_seek) (void)rt.dos_seek(handle, rewind, 1u);
                return logical;
            }
            if (ch != 0x0Du) {
                if (rt.write8) rt.write8(out + produced, ch);
                ++produced;
                ++logical;
            }
            ++consumed;
        }
        out += produced;
        remaining -= produced;
        if ((flags & 0x20u) != 0u) break;
    }
    return logical;
}

static s32 function_3821f(Runtime132Boundary& rt, u32 handle, u32 src, u32 count) {
    const u32 flags = rt.descriptor_flags ? rt.descriptor_flags(handle) : 0u;
    if (flags == 0u) {
        if (rt.set_runtime_error) rt.set_runtime_error(4u);
        return -1;
    }
    if ((flags & 2u) == 0u) {
        if (rt.set_runtime_error) rt.set_runtime_error(6u);
        return -1;
    }

    if ((flags & 0x80u) != 0u) {
        const s32 end = rt.dos_seek ? rt.dos_seek(handle, 0, 2u) : -1;
        if (end < 0) {
            if (rt.translate_dos_error) rt.translate_dos_error(static_cast<u16>(end));
            return -1;
        }
    }

    if ((flags & 0x40u) != 0u) {
        const s32 wrote = rt.dos_write ? rt.dos_write(handle, src, count) : -1;
        if (wrote < 0) {
            if (rt.translate_dos_error) rt.translate_dos_error(static_cast<u16>(wrote));
            return -1;
        }
        if (static_cast<u32>(wrote) != count && rt.set_runtime_error) rt.set_runtime_error(12u);
        return wrote;
    }

    u32 chunk = 0x200u;
    const u32 ver = rt.runtime_version ? rt.runtime_version() : 0x230u;
    if (ver < 0xB0u && rt.runtime_version_old) (void)rt.runtime_version_old();
    if (ver < 0x230u) chunk = 0x80u;

    u8 buffer[0x200]{};
    u32 input_done = 0u;
    u32 committed_input = 0u;
    u32 buffered = 0u;
    while (input_done < count) {
        const u8 ch = rt.read8 ? rt.read8(src + input_done) : 0u;
        if (ch == 0x0Au) {
            buffer[buffered++] = 0x0Du;
            if (buffered == chunk) {
                const s32 wrote = rt.dos_write ? rt.dos_write(handle, 0u, chunk) : -1;
                if (wrote < 0) { if (rt.translate_dos_error) rt.translate_dos_error(static_cast<u16>(wrote)); return -1; }
                if (static_cast<u32>(wrote) != chunk) { if (rt.set_runtime_error) rt.set_runtime_error(12u); return static_cast<s32>(committed_input + static_cast<u32>(wrote)); }
                committed_input = input_done;
                buffered = 0u;
            }
        }
        buffer[buffered++] = ch;
        ++input_done;
        if (buffered == chunk) {
            const s32 wrote = rt.dos_write ? rt.dos_write(handle, 0u, chunk) : -1;
            if (wrote < 0) { if (rt.translate_dos_error) rt.translate_dos_error(static_cast<u16>(wrote)); return -1; }
            if (static_cast<u32>(wrote) != chunk) { if (rt.set_runtime_error) rt.set_runtime_error(12u); return static_cast<s32>(committed_input + static_cast<u32>(wrote)); }
            committed_input = input_done;
            buffered = 0u;
        }
    }
    if (buffered != 0u) {
        const s32 wrote = rt.dos_write ? rt.dos_write(handle, 0u, buffered) : -1;
        if (wrote < 0) { if (rt.translate_dos_error) rt.translate_dos_error(static_cast<u16>(wrote)); return -1; }
        if (static_cast<u32>(wrote) != buffered) { if (rt.set_runtime_error) rt.set_runtime_error(12u); return static_cast<s32>(committed_input + static_cast<u32>(wrote)); }
    }
    return static_cast<s32>(count);
}

static s32 function_383ed(Runtime132Boundary& rt, u32 handle, s32 offset, u32 origin) {
    return rt.dos_seek ? rt.dos_seek(handle, offset, static_cast<u8>(origin)) : -1;
}

static s32 function_38407(Runtime132Boundary& rt, u32 handle, s32 offset, u8 origin) {
    const s32 pos = rt.dos_seek ? rt.dos_seek(handle, offset, origin) : -1;
    if (pos < 0) {
        if (rt.translate_dos_error) rt.translate_dos_error(static_cast<u16>(pos));
        return -1;
    }
    return pos;
}

static s32 function_38452(Runtime132Boundary& rt, u32 path) {
    const s32 rc = rt.dos_delete ? rt.dos_delete(path) : -1;
    if (rc < 0) {
        if (rt.translate_dos_error) rt.translate_dos_error(static_cast<u16>(rc));
        return rc;
    }
    return 0;
}

static u16 function_38474(Runtime132Boundary& rt, u32 out_drive) {
    const u32 drive = static_cast<u32>(rt.get_default_drive ? rt.get_default_drive() : 0u) + 1u;
    if (rt.write8) {
        rt.write8(out_drive + 0u, static_cast<u8>(drive));
        rt.write8(out_drive + 1u, 0u); rt.write8(out_drive + 2u, 0u); rt.write8(out_drive + 3u, 0u);
    }
    return 0u;
}

static u32 function_3848c(Runtime132Boundary& rt, u32 one_based_drive, u32 out_count) {
    const u8 count = rt.set_default_drive ? rt.set_default_drive(static_cast<u8>(one_based_drive - 1u)) : 0u;
    if (rt.write8) {
        rt.write8(out_count + 0u, count);
        rt.write8(out_count + 1u, 0u); rt.write8(out_count + 2u, 0u); rt.write8(out_count + 3u, 0u);
    }
    return 0u;
}
