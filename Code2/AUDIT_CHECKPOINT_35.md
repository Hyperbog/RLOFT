# CODE.2 semantic audit checkpoint 35

## Scope

- Previous frontier: `0x35286`
- Audited ownership-complete range: `0x35286..0x35A0B`
- Bytes audited this checkpoint: **1,926**
- Cumulative semantic audit: **219,644 / 239,698 = 91.6336%**
- Semantic defect classes: **6** (no new class established)
- Next frontier: `0x35A0C`

This pass closes reconstruction batches **111 through 116**.

## Immutable source revalidation

Revalidated directly from `/mnt/data/CODE.2` before forward work:

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- extracted object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`
- mandatory prior-frontier overlap `[0x35246,0x35306)`: `634e0145a056a4fa874c88ce98ac733dfe33dfdcfccc7afcb3e3a93f1bdc8c61`
- checkpoint span `[0x35286,0x35A0C)`: `e203dfcc9f9e5fe13c773153102c10543230ad870bbea578eb44feb85618e4a5`
- next-frontier overlap `[0x359CC,0x35A8C)`: `227395515c55295300db2689fc837c7db595105ac6d236c98e5af9826849704e`

## Reconstruction evidence reviewed

The immutable object-1 instructions were compared with the committed batch ownership descriptions and integrated reconstruction semantics for batches 111-116. The binary remains controlling evidence.

## Ownership families reviewed

### `35286..35417` — DOS/runtime wrappers, environment lookup, decimal parse, embedded data (Batch 111)

The AH=36h free-space wrapper preserves its low-word outputs (`DX/BX/AX/CX`) and failure path through `37CB2`. `352BE` and `352D8` remain address-distinct callback/forwarding sequences. The environment lookup preserves the prefix compare followed by an explicit `=` test. Decimal parsing retains wrapped ctype indexing through `(byte+1)&FFh`, optional sign handling, 32-bit multiply/add wrap, and final modular negation. `353A0` jumps over `353A2..35417`, which remains non-code runtime text/data.

### `35418..3562E` — runtime startup body (Batch 112)

Preserved stack alignment and publication through `12648/12634`, DOS version byte/word register effects, extender signatures and their distinct branches, PSP/environment selector handling, and the exact DOS/DPMI memory-resize setup. The environment/program-path scan retains the encoded zero-count/ZF dependency and the `no87=` handling rather than normalizing it into a host string routine. Startup push/pop balance and capped mode-1 zero fill remain instruction-faithful. `3562F` is excluded because it is independently targeted from `352E9` as well as reached by the startup fall-through.

### `3562F..3567F` — termination/console/DS-restore entries (Batch 113)

Retained three address-distinct entries: stack-sharing termination at `3562F`, DOS console-output/termination at `35632`, and CS-qualified DS restoration at `3566C`. The saved DS word at `35675..35676` and zero alignment through `3567F` remain data/non-code rather than executable reconstruction.

### `35680..35877` — signed multiply/projection helpers (Batch 114)

The paired multiply helpers preserve asymmetric sign handling and 16-bit partial-product behavior. `35730` retains its signed lower clamp followed by unsigned `DIV`. `35798` preserves PUSHA/POPA register restoration, signed IMUL/IDIV projection arithmetic, the `5800h` low-word threshold test, and word-only writes to its two output destinations. The data/alignment immediately following the routine remains non-code as previously classified.

### `35878..358D3` — direct-copy versus seek/read/tell bridge (Batch 115)

The direct-source path remains selector-aware `REP MOVSD` plus residual `REP MOVSB`, with the encoded returned source offset. The alternate path preserves the exact `38407 -> 38106 -> 3967C` seek/read/tell ordering. The `358D1..358D3` LEA alignment remains non-code.

### `358D4..35A0B` — compact loader-size/header scan (Batch 116)

Preserved the raw 0xCC-byte stack-local frame and the fact that the four `35878` destinations are stack locals rather than DGROUP addresses. Flag bit 0 retains the direct-source versus opened-handle split. The PE-style header probe reads the 3Ch pointer, then two bytes at that location and validates `1604h`; validation failure preserves cleanup even when source mode differs. The AC-byte header read, unsigned record-count loop, 18h record reads, accumulated size, conditional close, and final `+0Fh` result remain faithful. `35A09..35A0B` is alignment; `35A0C` begins the independently owned large loader.

## Defect-checklist review

Rechecked cdecl slot order, partial-register writes, signed/unsigned branches, modular arithmetic, selector/segment state, stack-local versus DGROUP ownership, address-distinct entry points, opaque-call reloads, and data/code boundaries. No new mismatch was established and no reconstruction edit is required.

## Result

Checkpoint 35 establishes semantic closure through object offset **`0x35A0B`**. The semantic audit is now **219,644 / 239,698 = 91.6336%** complete, leaving **20,054 bytes (8.3664%)**. The next independent semantic frontier is **`0x35A0C`**.
