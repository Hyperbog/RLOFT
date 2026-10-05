# CODE.2 semantic audit checkpoint 17

- Audited span: `139AB..148EF` — 3,909 bytes.
- Cumulative semantic audit: **84,192 / 239,698 = 35.1242%**.
- Semantic defect classes: **6**; no new defect established in this checkpoint.
- Span SHA-256: `f4660b4ce5d9f0c53a7f6e3d0f7a57f7f69f11d08da305fc694c957c4ff3ec2f`.
- Mandatory overlap `[0x1396B,0x13A6B)`: `04cb8bb2d084bb3514c3b8f89f9403da90fdb66153c74944c8a1d5c47535ccf1`.
- Immutable CODE.2 SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Immutable object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Semantic result

PASS. `139AB` is the encoded NOP alignment and `139AC` begins the next executable family. The review preserves `139AC`'s signed decimal filename substitution into `CINE00.ANM`; `13AD8`'s mode-`0Ch` stale-stack output boundary; `13C88`'s exact nine-way teardown table and `-2/-1` restore sentinels; `13FD4`'s in-place word-wrap terminator/restore behavior; `14118`'s party-count-dependent `[left|right]` text selection; `1430C`'s first-matching condition scan through terminator `21h`; and `146DC`'s two distinct wait paths plus `7FD4`-dependent preview remap.

The CODE.1 defect checklist was applied to cdecl slot consumption, signed division and comparisons, byte/word partial-register effects, global reloads across opaque calls, truncation, table ownership, and address-distinct control flow. No additional semantic drift was established from the CODE.2 bytes.

`148EE..148EF` is encoded `MOV EAX,EAX` alignment. Next semantic-audit frontier: **`148F0`**.
