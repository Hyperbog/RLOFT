# CODE.2 semantic audit checkpoint 27

Status: **PASS**

## Range and accounting

- Audited object-relative range: **`29414..2C293`**.
- Bytes closed by this checkpoint: **11,904**.
- Cumulative semantic audit: **180,868 / 239,698 = 75.4566%**.
- Semantic defect classes remain **6**; no new semantic defect was established in this checkpoint.
- Immutable CODE.2 SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Immutable object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Audited span SHA-256 (`29414..2C293`): `0e90de6dcf76da8c87760d372f7e9755e3a504f15d0d1354b2addd13be4445b0`.
- Mandatory overlap around the next frontier `[2C254,2C354)`: `a74e1ea05dbceb218328528a51522d7d160b44cdb12005ef9ac4c774a4bdd978`.
- The previous checkpoint overlap `[293D4,294D4)` reproduced exactly as `a84c239ed70c8dced2ec2902a5d4d786316105b9bb33d04caa6bfe1c3f95d595` before forward work.

## Ownership/family coverage

This higher-throughput pass closes original reconstruction batches **57 through 64** as one ownership-complete semantic checkpoint.

- **Batch 57 (`29414..2992F`)**: preserved the clipped/full renderer distinction, `EF69..EF6F` low-word global writes, signed division in `296C0`, and EFD4 scratch-depth balancing.
- **Batch 58 (`29930..29CEB`)**: preserved VGA register/BIOS ordering, the direct `29B33` CS table, `29B40` latch/mask and partial-register behavior, and retrace-bounded DAC streaming at `29CA0`.
- **Batch 59 (`29CEC..2A113`)**: preserved partial-register plane-mask behavior, signed Bresenham paths, low-word sign gates in `29EAC/29F40`, CX-only shift semantics in `2A020`, and page save/select/restore in `2A078`.
- **Batch 60 (`2A114..2A6AF`)**: preserved the plane-major `2A114/2A178` asymmetry, 16-bit wrap in `2A258`, transparent BL-only byte counts, and all four self-modifying operands at `2A4F6`, `2A54F`, `2A60B`, and `2A672`.
- **Batch 61 (`2A6B0..2AC7F`)**: preserved the self-modifying bytes at `2A816`, `2AA0D`, and `2AA12`, ROL/SHL and 32-/16-bit clipped-blitter asymmetries, `2AAEC` register ABI, the directly referenced `2AC70` table, and VGA retrace/CRTC ordering.
- **Batch 62 (`2AC80..2B087`)**: preserved signed DWORD comparisons in `2ACDC/2AD18/2ADE0`, the two real resource-state calls from `2AD18`, low-bit page toggle through `C050`, entry-stable record base with per-iteration count reload in `2AE30`, the second record-word `+8` reload after rectangle acceptance, and address-distinct inline predicates at `2AEBD` and `2B01D`.
- **Batch 63 (`2B088..2B6CF`)**: preserved the cross-entry carried value and signed neighborhood tables, signed absolute-value comparison with `+8000h` rounding in `2B1BC`, and `2B2F4` mutable cache, signed IDIV quotient/remainders, heading normalization, low-word additions, and final mode-indexed word adjustment.
- **Batch 64 (`2B6D0..2C293`)**: preserved per-loop signed count reloads in all four record-population passes, exact descriptor families, callback-sensitive metadata reloads, `2B2F4` mode ownership, temporary `7D00h/20h` rectangle transformation, and the encoded signed-division sequence ending at `2C293`.

## CODE.1 cross-reference checklist

The fully audited CODE.1 defect catalogue was applied across the complete span: cdecl slot order, live-register call inputs, byte/word partial-register aliases, signed/unsigned branch drift, modulo-32-bit arithmetic, encoded loop-entry behavior, selector/far-pointer and interrupt-frame preservation, global reloads across opaque calls, address-distinct wrappers/shared tails, REP/string semantics, and self-modifying code. No additional CODE.1 defect class reproduced in this CODE.2 span.

## Boundary result

Fresh disassembly confirms `2C293` is the RET ending the Batch-64 signed arithmetic helper and `2C294` begins the next independent renderer/state family with a fresh prologue and 0x40-byte local frame. The next semantic-audit frontier is therefore **`2C294`**.
