# Alpha diagnostics privacy: CODE.1 names are never logged

The `--dev-trace` boot-session addition moved the Win32 diagnostic
observer earlier, onto the original intro/title/CODE.1 flow. The existing
Host observer wrote raw `key.down`/ `key.up` Win32 virtual-key values
and `key.scan_observation` OEM scan bytes. During CODE.1 name entry
those values are sufficient to reconstruct a player's typed actor names.
Therefore merely claiming "no character names" while storing those
codes was incorrect, and the issue had to be closed before recommending
the new field-test artifact to the owner.

The observer now **disables all per-key raw VK/scan diagnostics while
`character_mode_` owns input**. It retains symbolic startup stage
events and all ordinary gameplay diagnostics. `WM_KEYDOWN` still
routes the EXACT SAME original CODE.1 name letters, digits, spaces,
Return, Backspace and stat-edit keys to the existing game queue; raw
observer changes have no effect on that dispatch. F12 still issues
the existing optional diagnostic marker without logging its scan.

A source-only Windows CI test confirms the three privacy guards,
retains original CODE.1 key routing, and rejects deliberate
leak reintroductions. Neither a native owner field session nor
original DOS fidelity is claimed by this host-only change.
