from pathlib import Path

cpp_p=Path('Code1/Code1.cpp')
audit_p=Path('Code1/FINAL_BINARY_AUDIT.md')
cpp=cpp_p.read_text()

repls={
"const u32 page=*dgroup_ptr(m,dgroup(0x7138u));if(b.call_6588)b.call_6588(page);function_38a4(m,b,0x5b,0x88);if(b.call_55dc)b.call_55dc(page,0x5b,0x88,0x7a,0x30);while(b.call_53b4&&b.call_53b4()!=0u){}":"const u32 page=load_le32(dgroup_ptr(m,dgroup(0x7138u)));if(b.call_6588)b.call_6588(low8(page));function_38a4(m,b,0x5b,0x88);if(b.call_55dc)b.call_55dc(page,0x5b,0x88,0x7a,0x30);while(b.call_53b4&&b.call_53b4()!=0u){}",
"if(b.call_67c8)b.call_67c8(temp,0,0,0);if(b.call_7830)b.call_7830(temp);":"if(b.call_67c8)b.call_67c8(0,0,0,temp);if(b.call_7830)b.call_7830(temp);",
"auto draw_pages=[&](){for(int i=0;i<2;++i){const u32 page=*dgroup_ptr(m,dgroup(0x7138u));if(b.call_6588)b.call_6588(page);if(b.call_67c8)b.call_67c8(load_le32(dgroup_ptr(m,dgroup(0x0958u))),0x5b,0x88,0);if(b.call_6c60)b.call_6c60(0,0x5b,0xbd,0x2e,7);if(gen.call_53e4)gen.call_53e4(page);*dgroup_ptr(m,dgroup(0x7138u))^=1u;}};draw_pages();":"auto draw_pages=[&](){for(int i=0;i<2;++i){const u32 page=load_le32(dgroup_ptr(m,dgroup(0x7138u)));if(b.call_6588)b.call_6588(low8(page));if(b.call_67c8)b.call_67c8(0,0x5b,0x88,load_le32(dgroup_ptr(m,dgroup(0x0958u))));if(b.call_6c60)b.call_6c60(0,0x5b,0xbd,0x2e,7);if(gen.call_53e4)gen.call_53e4(page);*dgroup_ptr(m,dgroup(0x7138u))^=1u;}};draw_pages();",
"if(b.call_67c8)b.call_67c8(panel,0xe5,0,0);":"if(b.call_67c8)b.call_67c8(0,0xe5,0,panel);",
"if(b.call_67c8)b.call_67c8(capture,0x96,0x8c,0);":"if(b.call_67c8)b.call_67c8(0,0x96,0x8c,capture);",
}
expected={
"const u32 page=*dgroup_ptr(m,dgroup(0x7138u));if(b.call_6588)b.call_6588(page);function_38a4(m,b,0x5b,0x88);if(b.call_55dc)b.call_55dc(page,0x5b,0x88,0x7a,0x30);while(b.call_53b4&&b.call_53b4()!=0u){}":1,
"if(b.call_67c8)b.call_67c8(temp,0,0,0);if(b.call_7830)b.call_7830(temp);":3,
"auto draw_pages=[&](){for(int i=0;i<2;++i){const u32 page=*dgroup_ptr(m,dgroup(0x7138u));if(b.call_6588)b.call_6588(page);if(b.call_67c8)b.call_67c8(load_le32(dgroup_ptr(m,dgroup(0x0958u))),0x5b,0x88,0);if(b.call_6c60)b.call_6c60(0,0x5b,0xbd,0x2e,7);if(gen.call_53e4)gen.call_53e4(page);*dgroup_ptr(m,dgroup(0x7138u))^=1u;}};draw_pages();":1,
"if(b.call_67c8)b.call_67c8(panel,0xe5,0,0);":3,
"if(b.call_67c8)b.call_67c8(capture,0x96,0x8c,0);":2,
}
for old,new in repls.items():
    n=cpp.count(old)
    if n==0 and new in cpp:
        continue
    assert n==expected[old], (old[:80],n,expected[old])
    cpp=cpp.replace(old,new)

assert 'constexpr u32 kVerifiedTranslatedBytes=69361u;' in cpp
cpp_p.write_text(cpp)

audit=audit_p.read_text()
marker='## Semantic audit checkpoint 3 — `38A4..4976`'
if marker not in audit:
    audit += '''\n\n## Semantic audit checkpoint 3 — `38A4..4976`\n\nFresh disassembly from the immutable original `CODE.1` corrected three representation errors without changing byte coverage. `67C8` is cdecl with encoded push order proving arguments `(0, x, y, handle)`; several calls in `3C58`, `40FC`, and `45FC` had the handle placed in the first slot. Page state `7138` is also width-sensitive: `6588` receives a zero-extended byte, while `55DC` and `53E4` receive the full dword. The checkpoint preserves those distinct accesses instead of collapsing them into a single page variable width. `38A4`, `44E4`, and `4520` were rechecked and required no correction. Object accounting remains **69,361 translated instruction bytes + 1,041 proven non-code/data bytes = 70,402 bytes**.\n'''
    audit_p.write_text(audit)

print('semantic checkpoint 3 applied')
