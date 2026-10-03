# CODE.2 proven non-code/data ledger

This ledger records only CODE.2 object-1 bytes directly proven not to be executable instructions. Appearance or alignment alone is not sufficient evidence.

<!-- RESIDUAL-RANGES-BEGIN -->
00073..00073 | alignment | 1
001FA..001FB | alignment | 2
00271..00273 | alignment | 3
00274..002A7 | table | 52
<!-- RESIDUAL-RANGES-END -->

**Total proven non-code/data:** **58 bytes**.

Batch 02 evidence: the first three ranges are explicit compiler alignment instructions between directly decoded function boundaries; `00274..002A7` is the thirteen-entry CS jump table whose targets were reconciled one-by-one in `function_002a8`.
