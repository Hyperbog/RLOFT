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
050D9..050DB | alignment | 3
05695..05697 | alignment | 3
05983..05983 | alignment | 1
05A11..05A13 | alignment | 3
05AA3..05AA3 | alignment | 1
05C81..05C83 | alignment | 3
05DCF..05DCF | alignment | 1
05EC3..05EC3 | alignment | 1
05EC4..05EEB | table | 40
062AB..062AB | alignment | 1
06352..06353 | alignment | 2
06441..06443 | alignment | 3
06520..06547 | table | 40
066E6..066E7 | alignment | 2
06C67..06C67 | alignment | 1
0706E..0706F | alignment | 2
07376..07377 | alignment | 2
0739D..0739F | alignment | 3
075C9..075CB | alignment | 3
0779F..0779F | alignment | 1
07871..07873 | alignment | 3
07D43..07D43 | alignment | 1
0808B..0808B | alignment | 1
0808C..080AB | table | 32
0869F..0869F | alignment | 1
086A0..086C3 | table | 36
0871B..0871B | alignment | 1
08A7D..08A7F | alignment | 3
08CD8..08CF3 | table | 28
0906D..0906F | alignment | 3
0934A..0934B | alignment | 2
0934C..09367 | table | 28
094DD..094DF | alignment | 3
<!-- RESIDUAL-RANGES-END -->

**Total proven non-code/data:** **448 bytes**.

Batch 02 evidence: the first three ranges are explicit compiler alignment instructions between directly decoded function boundaries; `00274..002A7` is the thirteen-entry CS jump table whose targets were reconciled one-by-one in `function_002a8`.

Batch 03 evidence: each added range decodes as an explicit compiler no-op separator between directly reconstructed functions. A full CODE.2 object-1 scan of direct relative `CALL`/`JMP`/conditional-branch targets found no incoming target to any of these separator instructions. `006B9..006BF` remains unresolved.

Batch 04 evidence: the eight added ranges decode as compiler no-op alignment forms between directly reconciled routine boundaries. A full object-1 direct branch/call-target scan found no incoming target to any byte in these ranges. `006B9..006BF` remains unresolved.

Batch 05 evidence: the eight alignment ranges decode as explicit compiler no-op separators and have no incoming target in a full object-1 direct branch/call scan. `02D70..02DAF` is the sixteen-entry CS jump table consumed by the indirect dispatcher at `02E51`; all sixteen dword targets were decoded and reconciled. `006B9..006BF` remains unresolved.

Batch 06 evidence: the added ranges decode as explicit compiler no-op alignment instructions between directly reconstructed function boundaries; a full object-1 direct relative control-flow target scan found no incoming target to any byte in these ranges. The zero islands `0380D..0380F` and `03A64..03A77` remain unresolved, as does `006B9..006BF`.

Batch 07 evidence: `050D9..050DB` decodes as `LEA EAX,[EAX]` between the `0461C` RET and the `050DC` entry. A full object-1 direct relative branch/call target scan found no incoming target to the separator. The known zero islands remain unresolved.

Batch 08 evidence: the seven alignment ranges decode as explicit NOP/LEA no-op separators and a full object-1 direct relative control-flow scan found no incoming target to any byte in those ranges. `05EC4..05EEB` is the ten-entry CS jump table indexed by `ESI` at `05F34`; all ten dword targets were decoded and reconciled in `function_05eec`. The known zero islands remain unresolved.

Batch 09 evidence: the five alignment ranges decode as explicit compiler no-op separators and a full object-1 direct relative control-flow target scan found no incoming target to any of them. `06520..06547` is the ten-entry CS jump table selected by `JMP DWORD PTR CS:[EBX*4+6520h]` at `065B1`; all ten table destinations were directly disassembled and reconciled. The known zero islands remain unresolved.

Batch 10 evidence: `0706E..0706F`, `07376..07377`, `0739D..0739F`, and `075C9..075CB` decode as explicit compiler no-op alignment between directly reconstructed routine boundaries. A full object-1 direct relative branch/call target scan found no incoming target to any of these bytes. The known zero islands remain unresolved.

Batch 11 evidence: `0779F`, `07871..07873`, `07D43`, and `0808B` decode as explicit compiler no-op separators between directly reconciled boundaries, and a full object-1 direct relative branch/call scan found no incoming target to any separator byte. `0808C..080AB` is the eight-entry CS dword table used by the indirect dispatcher at `08392`; its decoded targets are `0839A`, `083AD`, `08472`, `08537`, `0854D`, `08587`, `085DD`, and `085FE`. The known zero islands remain unresolved.

Batch 12 evidence: `0869F` is the single zero/alignment byte between the `080AC` dispatcher return and the directly consumed nine-dword table at `086A0`; `0871B` is an explicit `NOP`; and `08A7D..08A7F` is `LEA EAX,[EAX+0]`. A full object-1 direct relative branch/call-target scan found no incoming target to these separator bytes. `086A0..086C3` is the exact nine-dword table copied by `08A80` with values `1B,36,2B,0D,1D,3A,38,38,1B`. The seven-entry table beginning at `08CD8` remains unresolved for the next owning dispatcher.


Batch 13 evidence: `08CD8..08CF3` is the seven-entry CS table consumed by the indirect jump at `08E50`, with targets `08E58`, `08E77`, `08E94`, `08EAD`, `08EC5`, `08EDE`, `08EDE`. `0934C..09367` is the seven-entry CS table consumed at `093AB`, with targets `093B3`, `093BA`, `093CD`, `093CD`, `093C1`, `093CD`, `093C8`. `0906D..0906F` and `094DD..094DF` decode as `LEA EAX,[EAX+0]`; `0934A..0934B` decodes as `MOV EAX,EAX`. A full object-1 direct relative branch/call-target scan found no incoming target to any of these separator bytes. The known zero islands remain unresolved.
