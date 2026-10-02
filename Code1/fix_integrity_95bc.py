from pathlib import Path

cpp=Path('Code1/Code1.cpp')
text=cpp.read_text()

# This is a fidelity repair to already-counted bytes. Do not change coverage.
assert 'constexpr u32 kVerifiedTranslatedBytes=69652u;' in text

old='void(*record_load_8d0c)(u32,u32,u32,u32)=nullptr;void(*install_8398)()=nullptr;void(*restore_8424)()=nullptr;'
new='void(*record_load_8d0c)(u32,u32,u32,u32)=nullptr;u32(*install_int24_e280)(u32,u32)=nullptr;void(*install_8398)()=nullptr;void(*restore_8424)()=nullptr;'
assert old in text
text=text.replace(old,new,1)

old='store_le32(dgroup_ptr(m,dgroup(0x2b10u)),0xffffffffu);if(b.install_8398)b.install_8398();if(b.display_64dc)b.display_64dc(0u);u32 cfg=0;'
new='store_le32(dgroup_ptr(m,dgroup(0x2b10u)),0xffffffffu);if(b.install_int24_e280)(void)b.install_int24_e280(0x95a8u,static_cast<u32>(m.cpu.cs));if(b.install_8398)b.install_8398();function_64b0(m,0u);u32 cfg=0;'
assert old in text
text=text.replace(old,new,1)

old='else cfg=b.runtime_e360?b.runtime_e360(0u):0u;'
new='else{const u32 argv=load_le32(dgroup_ptr(m,dgroup(0x30a0u)));cfg=b.runtime_e360?b.runtime_e360(ma32(b,argv+4u)):0u;}'
assert old in text
text=text.replace(old,new,1)

old='if(b.mouse_d10c)b.mouse_d10c(0x27eu,0u);if(b.mouse_d130)b.mouse_d130(0xc7u,0u);'
new='if(b.mouse_d10c)b.mouse_d10c(0u,0x27eu);if(b.mouse_d130)b.mouse_d130(0u,0xc7u);'
assert old in text
text=text.replace(old,new,1)

cpp.write_text(text)

note='''\n\n## 2026-10-02 integrity repair: 95BC\nA post-interruption audit re-disassembled `95BC..9723` directly from immutable CODE.1 and found four semantic discrepancies in the older batch-11 lift. The integrated C++ now preserves the encoded `E280(95A8, CS)` callback installation before `8398`, calls `64B0(0)` rather than `64DC(0)`, passes `argv[1]` from the guest argv table at `30A0` to `E360`, and preserves the cdecl argument order of the `D10C(0,27E)` and `D130(0,C7)` calls. This changes no coverage count because all affected bytes were already counted; it is a fidelity correction only.\n'''
for name in ['Code1/PROGRESS.md','Code1/STAGING_LEDGER.md','Code1/DECOMPILATION_CONTINUITY.md']:
    p=Path(name)
    s=p.read_text()
    if '2026-10-02 integrity repair: 95BC' not in s:
        p.write_text(s+note)

print('repaired 95BC integrity issues')
