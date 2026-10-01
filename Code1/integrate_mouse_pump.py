from pathlib import Path

cpp=Path('Code1/Code1.cpp')
s=cpp.read_text()
old="struct NativeBoundaries{bool(*call_84d0)()=nullptr;void(*mouse_int33)(MouseRegs&)=nullptr;};"
new="struct NativeBoundaries{bool(*call_84d0)()=nullptr;void(*mouse_int33)(MouseRegs&)=nullptr;void(*call_8960)()=nullptr;u32(*call_82a4)()=nullptr;void(*call_82ec)()=nullptr;void(*call_8304)()=nullptr;void(*call_82c0)()=nullptr;};"
assert old in s
s=s.replace(old,new,1)
needle="void function_d10c(NativeBoundaries&b,u32 a,u32 c)noexcept{if(!b.call_84d0||!b.call_84d0())return;MouseRegs r{};r.ax=7;r.cx=static_cast<u16>(a);r.dx=static_cast<u16>(c);if(b.mouse_int33)b.mouse_int33(r);} void function_d130(NativeBoundaries&b,u32 a,u32 c)noexcept{if(!b.call_84d0||!b.call_84d0())return;MouseRegs r{};r.ax=8;r.cx=static_cast<u16>(a);r.dx=static_cast<u16>(c);if(b.mouse_int33)b.mouse_int33(r);}\nvoid*function_d1e8(void*dst,const void*src,u32 n)noexcept{return std::memmove(dst,src,static_cast<std::size_t>(n));}"
insert="""void function_d10c(NativeBoundaries&b,u32 a,u32 c)noexcept{if(!b.call_84d0||!b.call_84d0())return;MouseRegs r{};r.ax=7;r.cx=static_cast<u16>(a);r.dx=static_cast<u16>(c);if(b.mouse_int33)b.mouse_int33(r);} void function_d130(NativeBoundaries&b,u32 a,u32 c)noexcept{if(!b.call_84d0||!b.call_84d0())return;MouseRegs r{};r.ax=8;r.cx=static_cast<u16>(a);r.dx=static_cast<u16>(c);if(b.mouse_int33)b.mouse_int33(r);}
// D154..D178: drain mouse snapshots while the first returned word remains nonzero.
void function_d154(Machine&m,NativeBoundaries&b)noexcept{if(!b.call_84d0||!b.call_84d0())return;for(;;){MouseSnapshot*p=function_d0b4(b,m.data);if(b.call_8960)b.call_8960();if(p->buttons==0)return;}}
// D179..D17B is LEA EAX,[EAX+0] alignment padding and is intentionally not translated.
// D17C..D1E7: exact repeated 82A4/D0B4/8960 polling sequence; D1B6 calls 82A4 again.
void function_d17c(Machine&m,NativeBoundaries&b)noexcept{if(b.call_84d0&&b.call_84d0()){for(;;){MouseSnapshot*p=function_d0b4(b,m.data);if(b.call_8960)b.call_8960();if(p->buttons==0)break;}for(;;){const u32 ready=b.call_82a4?b.call_82a4():0;if(ready!=0)break;MouseSnapshot*p=function_d0b4(b,m.data);if(p->buttons!=0)break;if(b.call_8960)b.call_8960();}const u32 ready=b.call_82a4?b.call_82a4():0;if(ready!=0){if(b.call_82ec)b.call_82ec();if(b.call_8304)b.call_8304();}else{for(;;){if(b.call_8960)b.call_8960();MouseSnapshot*p=function_d0b4(b,m.data);if(p->buttons==0)break;}}}else if(b.call_82c0)b.call_82c0();}
void*function_d1e8(void*dst,const void*src,u32 n)noexcept{return std::memmove(dst,src,static_cast<std::size_t>(n));}"""
assert needle in s
s=s.replace(needle,insert,1)
cpp.write_text(s)

p=Path('Code1/PROGRESS.md')
t=p.read_text()
t=t.replace('verified%20conversion-11.48%25-red','verified%20conversion-11.69%25-red')
t=t.replace('**Verified conversion:** 11.48%','**Verified conversion:** 11.69%')
t=t.replace('**Remaining:** 88.52%','**Remaining:** 88.31%')
t=t.replace('Current verified translated bytes: **8,089**.','Current verified translated bytes: **8,234**.')
t=t.replace('Raw coverage: **11.4897%**, displayed conservatively as **11.48%**.','Raw coverage: **11.6957%**, displayed conservatively as **11.69%**.')
t=t.replace('- Structured mouse-facing routines `D08C..D153` plus overlap-safe copy `D1E8..D239`: **274 bytes**.','- Structured mouse-facing routines `D08C..D153`, verified mouse/event pump `D154..D1E7`, plus overlap-safe copy `D1E8..D239`: **419 bytes**.')
t=t.replace('Total integrated verified translated bytes: **8,089**.','Total integrated verified translated bytes: **8,234**.')
start=t.index('## Mouse/event-pump continuation under reconciliation')
end=t.index('## D23A..D5AC DOS/CRT continuation under reconciliation')
t=t[:start]+"## Mouse/event-pump integration\n\n`D154..D178` and `D17C..D1E7` are now physically integrated in `Code1.cpp` after direct byte-for-byte disassembly reconciliation. The three-byte `D179..D17B` alignment LEA remains excluded. This adds **145 verified translated bytes**. The repeated `82A4` call at `D1B6` and the preserved pointer test after `8960` are represented explicitly.\n\n"+t[end:]
# Correct prior staging-accounting mistakes discovered by comparing staging against Code1.cpp.
t=t.replace('Current uncounted instruction-bounded staging totals **3,201 bytes** across startup, DOS-memory, mouse-pump, DOS/CRT, E0D0..E3B2 runtime/interrupt, and DE6F..E0C7 helper work streams.','Current unique uncounted instruction-bounded staging totals **2,406 bytes** across startup, the still-missing portions of DOS-memory, DOS/CRT, and E0D0..E3B2 runtime/interrupt work streams. A reconciliation audit found that DA48..DAA6 and DEA7..E0C7 were already physically present in Code1.cpp and therefore must not be counted as new staging; the ambiguous DE6F..DEA0 bytes are returned to the unresolved boundary queue rather than treated as a proven routine.')
# If exact active paragraph differs, append authoritative correction.
if '2,406 bytes' not in t:
    marker='## Larger-batch policy'
    idx=t.index(marker)
    t=t[:idx]+"## Integration accounting audit\n\nAfter comparing all active staging against `Code1.cpp`, DA48..DAA6 and DEA7..E0C7 were found already integrated and are not new work. `DE6F..DEA0` is not accepted as a proven function boundary: direct disassembly begins with bytes that do not form a justified ordinary entry, so it returns to the unresolved queue. After the 145-byte mouse fold, unique uncounted staged code is **2,406 bytes** (747 startup + 57 genuinely missing DA1C/DAC9 bridge bytes + 880 D23A..D5AC + 722 E0D0..E3B2).\n\n"+t[idx:]
p.write_text(t)

Path('Code1/MousePumpNext.staging.cpp').unlink()
Path('Code1/RuntimeGapDE6F.staging.cpp').unlink()
