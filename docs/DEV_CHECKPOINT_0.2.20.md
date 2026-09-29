# Development checkpoint 0.2.20

## Continuity base

This checkpoint continues directly from `Ravenloft_Alpha_0.2.19-dev-incremental.zip`. No earlier source branch was substituted. It preserves the green `0x1BDAC` scheduler / `0xD550` NPC→party attack work and adds the verified reciprocal player→NPC opening combat chain.

- 0.2.19 SHA-256: `6447ad4fed88f451b1ed25d37d99e98b2443fa4aa97dceb949ea4efebadb8185`
- `Ravenloft_Decomp_Final.zip` SHA-256: `331c85756a36ba359c1f0823f612bd24e759965b361f73dbfc5910546f177729`
- original `game.gog` SHA-256: `a915b600a59b7512cc3894d154a6225a312a74526cc9c5965542107eddc37e6a`

## Mechanical-demo milestone

Verified mechanical-demo completion: **90%**.

The opening assassin can now be damaged and killed through the normal reconstructed player-hand attack path rather than a development-only death shortcut:

`CODE.2 0x26018 -> 0xE5F0 -> 0xC80C / 0x1C328 / 0xC910 -> 0xE284 -> 0x2195C`

The shipped Quick Start Ehliana long-sword path is regression-driven through hit, miss, cooldown expiry, repeated attacks, lethal damage, assassin suppression, BF44 state mutation, and the four original quest drops.

## Newly assembly-mapped components

- `CODE.2 0x2195C..0x21ADA` -> `ObjectSpawn2195C::execute`
- `CODE.2 0xE284` common NPC damage/death + opening type-9 special -> `NpcDamageE284::apply`
- `CODE.2 0xC7C4` + `0xC80C..0xC90C` -> `PlayerHitTestC80C`
- `CODE.2 0xC910..0xC99E` -> `PlayerDamageModifierC910`
- `CODE.2 0xE578..0xE5CB` -> `PlayerAttackStateE578`
- `CODE.2 0x24B48..0x24BF0` gameplay-state subset -> `PlayerAttackCooldown24B48`
- `CODE.2 0x1E4A0..0x1E532` -> `PlayerAttackTimers1E4A0`
- `CODE.2 0xE5F0..0xEA81` normal melee branch -> `PlayerNpcAttackE5F0`
- `CODE.2 0x26018` opening-relevant hand dispatcher -> `PlayerHandAttack26018`

## Exact opening death/drop behavior

The reconstructed `0xE284` type-9 death branch calls `0x2195C` four times in original order:

1. object 136, scatter `0x6E`
2. object 135, scatter `0x78`
3. object 233, scatter `0x82`
4. object 116, scatter `0x8C`

After the fourth drop it writes BF44=1. `0x2195C` itself now owns the opening-use scatter/collision/fallback placement semantics rather than `OpeningDemoSession` fabricating positions.

## Production player attack verification

The regression executes the actual shipped Quick Start party and type-9 assassin. It verifies:

- item 252/type 10 dispatch through `0x26018`;
- exact `0xC80C` d20 values and hit requirements;
- `0x1C328` damage RNG consumption;
- `0xC910` damage filtering;
- `0xE284` HP/death mutation;
- `0x24B48` hand recovery deadline writes;
- `0x1E4A0` expiry clearing rather than manually zeroing recovery fields;
- lethal attack produces exactly four runtime drops and suppresses the assassin.

## Intentional remaining boundaries

This milestone is complete for the demo's ordinary opening melee path, not a claim that every global player weapon family is converted. `0x26018` throwable/other item families and `0xE5F0` special weapon IDs remain explicit reconstruction targets outside the opening assassin requirement.

## Verification gate

The checkpoint must pass from a fresh build directory:

- project configure/build;
- CTest;
- full regression against the read-only original-data overlay;
- direct `game.gog` ISO validation;
- all shipped CINE00..CINE12 decode-to-end checks;
- end-to-end normal player-hand attack -> assassin death -> original four-drop path.

The source archive excludes transient build directories and original Ravenloft game data.
