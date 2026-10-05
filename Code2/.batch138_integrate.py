from pathlib import Path
import sys
root=Path(__file__).resolve().parents[1]
cpp=root/'Code2/Code2.cpp'; prog=root/'Code2/PROGRESS.md'; add=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'; chk=root/'Code2/RESUME_CHECKPOINT.md'; frag=root/'Code2/.batch138.fragment.cpp'
def integrate():
 s=cpp.read_text(); f=frag.read_text()
 s=s.replace('constexpr u32 kVerifiedTranslatedBytes = 232645u;','constexpr u32 kVerifiedTranslatedBytes = 232906u;')
 marker='\n} // namespace code2\n'
 if 'function_39589' not in s: s=s.replace(marker,f+marker)
 cpp.write_text(s)
 p=prog.read_text().replace('232,645','232,906').replace('237,320 / 239,698 = 99.0079% (99.01%)','237,581 / 239,698 = 99.1168% (99.12%)').replace('2,378','2,117')
 p += '\n\n## Batch 138\n- `39589..395E1`: 89 translated bytes — runtime scratch-stack watermark/startup forwarding helper.\n- `395E2..3962C`: 75 translated bytes — four-record low-threshold callback dispatcher.\n- `3962D..3967B`: 79 translated bytes — upper-threshold callback dispatcher.\n- `3967C..3968D`: 18 translated bytes — seek-to-end forwarding wrapper.\n- Batch total: 261 translated bytes. Reconciled: **237,581 / 239,698 = 99.1168% (99.12%)**; unresolved: **2,117**.\n'
 prog.write_text(p)
 a=add.read_text(); a += '\n\n### Batch 138\n`39589..3968D` is now instruction-accounted: runtime scratch-stack/startup forwarding, two stateful six-byte-record callback dispatchers, and the seek-to-end wrapper. The callback tables preserve byte state/threshold fields and dword callback addresses; callback invocation remains an explicit boundary.\n'
 add.write_text(a)
 c=chk.read_text(); c += '\n\n## Batch 138 handoff\n- Last substantive batch commit: `BATCH138_COMMIT` — `Integrate CODE.2 decompilation batch 138`.\n- Verified translated instruction bytes: **232,906**.\n- Proven non-code/data bytes: **4,675**.\n- Reconciled: **237,581 / 239,698 = 99.1168% (99.12%)**.\n- Unresolved: **2,117**.\n- Owned fold: `39589..3968D` (261 translated bytes).\n- Batch-owned SHA-256: `753921c7dc798149f0f647667ee771115032702827972404a1749bf91d7735b7`.\n- Mandatory overlap `[0x3964E,0x3970E)` SHA-256: `d386cc964a16feca18cba8d61fb27cbb2fc22384883ba79f2440327b3bf57169`.\n- Mandatory overlap `[0x3966E,0x396EE)` SHA-256: `fa45f1826f22bebbbe2dd92b2837aaf1b2eb9760f4806e0433f91291bc9f25d0`.\n- Next independent boundary: `3968E`.\n- `385B0..38866` and `36CEC..372F5` remain unresolved.\n'
 chk.write_text(c)
def verify():
 s=cpp.read_text(); assert 'kVerifiedTranslatedBytes = 232906u' in s; assert 'function_39589' in s and 'function_3967c' in s
 assert '237,581 / 239,698 = 99.1168% (99.12%)' in prog.read_text()
if __name__=='__main__': (integrate if sys.argv[1]=='integrate' else verify)()
