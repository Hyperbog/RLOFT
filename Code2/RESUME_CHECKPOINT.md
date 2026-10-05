# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `187cc8d00bc0d777b3e1cda73756e024002bd703` — `Integrate CODE.2 decompilation batch 101`.
- Verified translated instruction bytes: **215,095**.
- Proven non-code/data bytes: **4,293**.
- Reconciled object bytes: **219,388 / 239,698 = 91.5268% (91.53%)**.
- Unresolved object bytes: **20,310**.
- Last integrated ownership fold: `33C90..33DDF`.
- Next independently decoded instruction boundary: **`33DE0`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x33D90,0x33E70)` — 224 bytes; SHA-256 `92a52541ffdb90ad8795f8f2a155c075640ed6b6eb7c22a79eec2a1fe04b06a2`.
- Narrow `[0x33DD0,0x33E30)` — 96 bytes; SHA-256 `7c98c60104fbe79985e05d06e934f54246d335d5f84b2c32fb14aaa32da7b582`.
- Narrow bytes: `380075f05bc3e811cdffff5bc3000000c8000000565752515366bac40366b8020f66ef8b3dbcfd000081c7f8500000bb800000008a65088ac42a8394fe0000b940000000f3aa81ef900000004b75e85b595a5f5ec9c387dbc800000056575251`.

Interpretation: `33DDC` is the final mouse-helper RET; `33DDD..33DDF` is unreferenced zero alignment; `33DE0` begins an independently called VGA fill routine. No Batch 102 bytes are counted.
