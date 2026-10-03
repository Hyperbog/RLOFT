from pathlib import Path
import re

cpp_path=Path("Code2/Code2.cpp")
frag_path=Path("Code2/batch12.inc")
progress_path=Path("Code2/PROGRESS.md")
ledger_path=Path("Code2/RESIDUAL_LEDGER.md")
cont_path=Path("Code2/CODE2_CONTINUITY_ADDENDUM.md")

cpp=cpp_path.read_text()
frag=frag_path.read_text()

assert "constexpr u32 kVerifiedTranslatedBytes = 39537u;" in cpp
assert "constexpr u32 kVerifiedNonCodeBytes = 343u;" in cpp
assert "void function_080ac(" not in cpp
assert "static_assert(0x08CD8u-0x08C88u==80u);" in frag
assert "const u32 mask=1u<<(bit&31u);" in frag
assert "if(index>=static_cast<s32>(load_le32(dg(m,0xC038u)))&&b.call_2f580)" in frag

cpp=cpp.replace("constexpr u32 kVerifiedTranslatedBytes = 39537u;",
                "constexpr u32 kVerifiedTranslatedBytes = 42612u;",1)
cpp=cpp.replace("constexpr u32 kVerifiedNonCodeBytes = 343u;",
                "constexpr u32 kVerifiedNonCodeBytes = 384u;",1)

old_assert='''static_assert(kVerifiedTranslatedBytes == 36959u +
              (0x0779Fu-0x075CCu) + (0x07871u-0x077A0u) +
              (0x07938u-0x07874u) + (0x07B94u-0x079E0u) +
              (0x07D43u-0x07B94u) + (0x0803Cu-0x07D44u) +
              (0x0808Bu-0x0803Cu));'''
new_assert='''static_assert(kVerifiedTranslatedBytes == 39537u +
              (0x0869Fu-0x080ACu) + (0x0871Bu-0x086C4u) +
              (0x0893Cu-0x0871Cu) + (0x08A7Du-0x0893Cu) +
              (0x08C88u-0x08A80u) + (0x08CD8u-0x08C88u));'''
assert old_assert in cpp
cpp=cpp.replace(old_assert,new_assert,1)

needle="\n} // namespace code2\n"
assert cpp.count(needle)==1
cpp=cpp.replace(needle,frag.rstrip()+"\n\n} // namespace code2\n",1)
cpp_path.write_text(cpp)

ledger=ledger_path.read_text()
assert "**Total proven non-code/data:** **343 bytes**." in ledger
marker="<!-- RESIDUAL-RANGES-END -->"
assert marker in ledger
new_ranges='''0869F..0869F | alignment | 1
086A0..086C3 | table | 36
0871B..0871B | alignment | 1
08A7D..08A7F | alignment | 3
'''
ledger=ledger.replace(marker,new_ranges+marker,1)
ledger=ledger.replace("**Total proven non-code/data:** **343 bytes**.",
                      "**Total proven non-code/data:** **384 bytes**.",1)
ledger=ledger.rstrip()+'''\n\nBatch 12 evidence: `0869F` is the single zero/alignment byte between the `080AC` dispatcher return and the directly consumed nine-dword table at `086A0`; `0871B` is an explicit `NOP`; and `08A7D..08A7F` is `LEA EAX,[EAX+0]`. A full object-1 direct relative branch/call-target scan found no incoming target to these separator bytes. `086A0..086C3` is the exact nine-dword table copied by `08A80` with values `1B,36,2B,0D,1D,3A,38,38,1B`. The seven-entry table beginning at `08CD8` remains unresolved for the next owning dispatcher.\n'''
ledger_path.write_text(ledger)

progress=progress_path.read_text()
repls={
    "object%20accounting-16.64%25-red":"object%20accounting-17.94%25-red",
    "**Verified translated instruction bytes:** **39,537**":"**Verified translated instruction bytes:** **42,612**",
    "**Proven non-code/data bytes:** **343**":"**Proven non-code/data bytes:** **384**",
    "**Reconciled object bytes:** **39,880 / 239,698 = 16.6376% (16.64%)**":
        "**Reconciled object bytes:** **42,996 / 239,698 = 17.9376% (17.94%)**",
    "**Unresolved object bytes:** **199,818**":"**Unresolved object bytes:** **196,702**",
}
for a,b in repls.items():
    assert a in progress,a
    progress=progress.replace(a,b,1)
progress=progress.rstrip()+'''\n\n## Batch 12 — left-panel dispatcher + timed status/moving-record core\n\nAdded **3,075 verified translated instruction bytes**:\n\n- `080AC..0869E` — **1,523 bytes**: eight-way left-panel dispatcher using the already decoded `0808C` CS table; preserves signed row `IDIV`, the two-poll press gate, ESI-selected-row lifetime, case-specific capture refresh, save/load/settings actions, and exit restoration.\n- `086C4..0871A` — **87 bytes**: dual per-slot table-word match and `24A9C` notifications.\n- `0871C..0893B` — **544 bytes**: 26-bit timed-status expiration/message path with bit-specific counters, global flags, and signed/byte-width side effects.\n- `0893C..08A7C` — **321 bytes**: four-slot deadline sweeper for nine event deadlines, the `21ACE` refresh timer, and 26 status deadlines.\n- `08A80..08C87` — **520 bytes**: moving-record constructor/collision loop, including high-word packed-coordinate tile arithmetic, copied nine-dword sound-offset table, owner bits, angle/orientation update, and collision recovery.\n- `08C88..08CD7` — **80 bytes**: 16h-byte record compaction; invalid signed indices report through `2F580` but deliberately continue.\n\nAdded **41 directly proven non-code/data bytes**: `0869F`, the 36-byte nine-dword table `086A0..086C3`, `0871B`, and `08A7D..08A7F`.\n\nCumulative accounting: **42,612 translated + 384 proven non-code/data = 42,996 / 239,698 = 17.9376% (17.94%)**. The next unresolved table begins at `08CD8`; its owning dispatcher is intentionally deferred to Batch 13.\n'''
progress_path.write_text(progress)

cont=cont_path.read_text().rstrip()+'''\n\n## Batch 12 checkpoint — 080AC..08CD7\n\nBatch 12 advances the sequential application frontier through `08CD7`: 3,075 instruction bytes plus 41 directly proven non-code/data bytes. The major lift is the eight-way `080AC` left-panel dispatcher and the timed-status/moving-record family through `08C88`.\n\nFidelity notes:\n- `080AC` keeps the selected row live across loop iterations when no full redraw occurs; the binary only recomputes that value on redraw or pointer movement.\n- A press is accepted only after the encoded second `1A1F4` poll at `082B2`.\n- `0871C` retains byte/word decrement widths and the four-slot bit-20/bit-21 global side effects.\n- `0893C` uses signed deadline comparisons and obtains a fresh `19CC4` value only when rescheduling `21ACE`.\n- `08A80` forms its tile word from the high 16-bit halves of the packed coordinates with 16-bit multiply truncation, and uses the exact nine-dword `086A0` sound-offset table without adding an invented bounds check.\n- `08C88` reports an out-of-range signed index but then continues with the encoded compaction arithmetic.\n\nCumulative object accounting after Batch 12 is 42,612 translated + 384 proven non-code/data = 42,996 / 239,698 (17.94%). Next sequential frontier: the seven-entry table at `08CD8` and its owning dispatcher.\n'''
cont_path.write_text(cont)

ranges=[(0x080AC,0x0869F),(0x086C4,0x0871B),(0x0871C,0x0893C),
        (0x0893C,0x08A7D),(0x08A80,0x08C88),(0x08C88,0x08CD8)]
delta=sum(b-a for a,b in ranges)
assert delta==3075,delta
assert 39537+delta==42612
assert 343+41==384
assert 42612+384==42996
assert 239698-42996==196702

print("Batch 12 patch applied: 3075 translated + 41 non-code")
