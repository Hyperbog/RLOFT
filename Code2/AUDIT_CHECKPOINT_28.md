# CODE.2 semantic audit checkpoint 28

Status: **PASS**

## Range and accounting

- Audited object-relative range: **`2C294..2F0B3`**.
- Bytes closed by this checkpoint: **11,808**.
- Cumulative semantic audit: **192,676 / 239,698 = 80.3828%**.
- Semantic defect classes remain **6**; no new semantic defect was established in this checkpoint.
- Immutable CODE.2 SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Immutable object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Audited span SHA-256 (`2C294..2F0B3`): `ad382041e469e9fff06c660d61c77dc1ef0a279d2cf299df8d8ea3e48b557a26`.
- Previous-frontier overlap `[2C254,2C354)` reproduced exactly as `a74e1ea05dbceb218328528a51522d7d160b44cdb12005ef9ac4c774a4bdd978` before forward work.
- Mandatory overlap around the next frontier `[2F074,2F174)`: `a416f5b98763682e728abd2551ae753b2a4fca7a13c605ecb670bdb9a2e52585`.

## Ownership/family coverage

This higher-throughput pass closes original reconstruction batches **65 through 68** as one ownership-complete semantic checkpoint.

- **Batch 65 (`2C294..2D003`)**: preserved `2C294`'s non-linear saved-rectangle restore order, dynamically re-read clipping globals and render-kind fields, callback-sensitive metadata/source reloads in `2C8E4`, independent transition branches in `2CAD4`, and `2CEB8`'s word-only transition writes plus explicit `258EC` reload after `2E510`.
- **Batch 66 (`2D004..2DF2B`)**: preserved the saved-but-not-restored coordinate locals in `2D004`, signed versus unsigned comparison distinctions in `2D3D8`/`2D5D0`, AX-only acceptance tests in `2DBAC`, the first-qualifying-tile early return in `2DA7C`, and word/byte-specific tile-code mutations around `198E0`.
- **Batch 67 (`2DF2C..2E7EF`)**: preserved the exact eight-entry CS table at `2DFEC`, signed-high-word-only distance clamp, word-sized negation before sign extension in `2E120`, page-stack state in `2E590/2E5D4`, and the intentionally dead post-mask return branch in `2E614`.
- **Batch 68 (`2E7F0..2F0B3`)**: preserved both exact seven-entry CS tables, partial initialization of nine-byte item records, `2ECB4` continue-after-error indexing and selector dispatch, `2EFD0` low-nibble cleanup routing, and the corrected ABI signatures at `28020`/`286C4` retaining the ignored first argument and stream-valued second argument.

## Cross-cutting semantic review

Fresh CODE.2 disassembly was reconciled against the reconstruction with the fully audited CODE.1 defect catalogue used only as a checklist. The pass explicitly rechecked cdecl stack-slot order, live-register inputs, signed/unsigned branches, byte/word partial-register aliases, 16-/32-bit truncation and wrap, callback-sensitive global reloads, exact table ownership, page-state restoration, address-distinct paths, and error-path continuation. No additional CODE.1 defect class reproduced in this span.

The `2F0B2..2F0B3` encoded `MOV EAX,EAX` remains alignment owned by Batch 68. Fresh disassembly confirms **`2F0B4`** begins the next independent record-selection routine with a new register-save/local-frame prologue. The next semantic-audit frontier is therefore **`2F0B4`**.
