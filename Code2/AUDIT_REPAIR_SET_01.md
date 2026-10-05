# CODE.2 semantic audit repair set 01

## Controlling evidence

Immutable CODE.2 SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.

Immutable object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

This repair set contains only mismatches already established by direct immutable disassembly in semantic-audit checkpoints 39, 40, 42, 45, and 46. It does not attempt to infer or clean up unrelated code.

## Repairs staged

1. `372F6`: speaker/PIT gate initialization now corresponds to `TEST AL,03h / JNE`: setup executes only when `(gate & 3) == 0`.
2. `3736D`: the temporary divisor slot is explicitly restored from the original radix before division, overwritten with quotient, and reloaded as loop state, preserving the encoded mutable `[ESP+24h]` alias transition.
3. `37433`: the independently emitted second conversion family explicitly restores the divisor slot, overwrites it with remainder, reloads that remainder for the alphabet index, and retains quotient separately as the loop condition.
4. `37810`: second payload-strip address is derived from the matched permutation-table position `pos`, not the displaced table value.
5. `37D60`: the zero-unread path explicitly carries the zero position value into the shared post-seek `-1` comparison, matching the encoded control/register dependency.
6. `37EFA`: the first `3A0A8` pre-processing gate uses the zero-result predicate; the later positive device-publication use remains unchanged.

The deterministic integrator/validator is `Code2/.audit_repairs_39_46.py`.

## Deliberately not folded here

`36CEC..372F5` is not a predicate-level repair. It is a 1,546-byte aggregate-accounted region lacking explicit source representation and requires its own complete reconstruction from immutable CODE.2. It remains open until that reconstruction is integrated and audited.

## Audit status

These six source corrections are evidence-complete and repair-ready. Semantic inspection continues independently from the current frontier, but final semantic certification remains blocked by the `36CEC..372F5` explicit-reconstruction gap and any later defects discovered by the remaining audit.
