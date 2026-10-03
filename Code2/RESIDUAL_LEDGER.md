# CODE.2 proven non-code/data ledger

This ledger records only CODE.2 object-1 bytes directly proven not to be executable instructions. Appearance or alignment alone is not sufficient evidence.

<!-- RESIDUAL-RANGES-BEGIN -->
00073..00073 | alignment | 1
001FA..001FB | alignment | 2
00271..00273 | alignment | 3
00274..002A7 | table | 52
006FD..006FF | alignment | 3
00792..00793 | alignment | 2
007E2..007E3 | alignment | 2
00839..0083B | alignment | 3
00931..00933 | alignment | 3
00BFA..00BFB | alignment | 2
010C3..010C3 | alignment | 1
011A5..011A7 | alignment | 3
01227..01227 | alignment | 1
01399..0139B | alignment | 3
01557..01557 | alignment | 1
01831..01833 | alignment | 3
01C4D..01C4F | alignment | 3
01CE2..01CE3 | alignment | 2
01F6B..01F6B | alignment | 1
02079..0207B | alignment | 3
02353..02353 | alignment | 1
023B6..023B7 | alignment | 2
02473..02473 | alignment | 1
026E6..026E7 | alignment | 2
02AE5..02AE7 | alignment | 3
02D6F..02D6F | alignment | 1
02D70..02DAF | table | 64
034AF..034AF | alignment | 1
03631..03633 | alignment | 3
0381F..0381F | alignment | 1
03836..03837 | alignment | 2
03C03..03C03 | alignment | 1
03DE3..03DE3 | alignment | 1
03F75..03F77 | alignment | 3
040FF..040FF | alignment | 1
04225..04227 | alignment | 3
043F1..043F3 | alignment | 3
044CF..044CF | alignment | 1
0461A..0461B | alignment | 2
<!-- RESIDUAL-RANGES-END -->

**Total proven non-code/data:** **190 bytes**.

Batch 02 evidence: the first three ranges are explicit compiler alignment instructions between directly decoded function boundaries; `00274..002A7` is the thirteen-entry CS jump table whose targets were reconciled one-by-one in `function_002a8`.

Batch 03 evidence: each added range decodes as an explicit compiler no-op separator between directly reconstructed functions. A full CODE.2 object-1 scan of direct relative `CALL`/`JMP`/conditional-branch targets found no incoming target to any of these separator instructions. `006B9..006BF` remains unresolved.

Batch 04 evidence: the eight added ranges decode as compiler no-op alignment forms between directly reconciled routine boundaries. A full object-1 direct branch/call-target scan found no incoming target to any byte in these ranges. `006B9..006BF` remains unresolved.

Batch 05 evidence: the eight alignment ranges decode as explicit compiler no-op separators and have no incoming target in a full object-1 direct branch/call scan. `02D70..02DAF` is the sixteen-entry CS jump table consumed by the indirect dispatcher at `02E51`; all sixteen dword targets were decoded and reconciled. `006B9..006BF` remains unresolved.

Batch 06 evidence: the added ranges decode as explicit compiler no-op alignment instructions between directly reconstructed function boundaries; a full object-1 direct relative control-flow target scan found no incoming target to any byte in these ranges. The zero islands `0380D..0380F` and `03A64..03A77` remain unresolved, as does `006B9..006BF`.
