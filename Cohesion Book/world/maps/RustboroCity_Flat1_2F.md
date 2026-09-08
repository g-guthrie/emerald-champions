# RustboroCity_Flat1_2F

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/RustboroCity_Flat1_2F/map.json) · [Scripts](../../baseline/source/data/maps/RustboroCity_Flat1_2F/scripts.inc)

## Current map contract

`MAP_RUSTBORO_CITY_FLAT1_2F` · `LAYOUT_RUSTBORO_CITY_FLAT1_2F` · `WEATHER_NONE` · `MUS_RUSTBORO`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `RustboroCity_Flat1_2F:object_events:001` | 4,6 | [RustboroCity_Flat1_2F_EventScript_WaldasMom](../../baseline/source/data/maps/RustboroCity_Flat1_2F/scripts.inc#L70) — RustboroCity_Flat1_2F_EventScript_WaldasMom at (4,6); Oh, it's so hard every day… //  What's hard? /  You need to ask? //  It's trying to figure out what to /  make for meals every day. //  It really isn't easy coming up with /  meals every day. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `RustboroCity_Flat1_2F:object_events:002` | 9,5 | Passive/staged OBJ_EVENT_GFX_TWIN at (9,5); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `RustboroCity_Flat1_2F:object_events:003` | 9,4 | [RustboroCity_Flat1_2F_EventScript_PokeDoll](../../baseline/source/data/maps/RustboroCity_Flat1_2F/scripts.inc#L74) — RustboroCity_Flat1_2F_EventScript_PokeDoll at (9,4); It's a POKéMON plush DOLL! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `RustboroCity_Flat1_2F:object_events:004` | 10,5 | [RustboroCity_Flat1_2F_EventScript_PokeDoll](../../baseline/source/data/maps/RustboroCity_Flat1_2F/scripts.inc#L74) — RustboroCity_Flat1_2F_EventScript_PokeDoll at (10,5); It's a POKéMON plush DOLL! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `RustboroCity_Flat1_2F:object_events:005` | 10,6 | [RustboroCity_Flat1_2F_EventScript_PokeDoll](../../baseline/source/data/maps/RustboroCity_Flat1_2F/scripts.inc#L74) — RustboroCity_Flat1_2F_EventScript_PokeDoll at (10,6); It's a POKéMON plush DOLL! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `RustboroCity_Flat1_2F:object_events:006` | 8,5 | [RustboroCity_Flat1_2F_EventScript_WaldasDad](../../baseline/source/data/maps/RustboroCity_Flat1_2F/scripts.inc#L4) — RustboroCity_Flat1_2F_EventScript_WaldasDad at (8,5); Oh, hello! /  Welcome to the PEPPER household. //  I have a question for you. /  Have you ever baby-sat? //  You see, I'm a new father, so raising /  a child is all new to me. //  And I have a problem. My daughter /  WALDA doesn't laugh enough. //  I think she'd laugh for me if I told /  her something funny. //  Do you know of a funny word or /  phrase you can tell me? / I've been saying “{STR_VAR_1}” /  to amuse her lately. //  Do you know of a better word or /  a phrase that might work? | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `RustboroCity_Flat1_2F:object_events:007` | 8,6 | [RustboroCity_Flat1_2F_EventScript_PokeDoll](../../baseline/source/data/maps/RustboroCity_Flat1_2F/scripts.inc#L74) — RustboroCity_Flat1_2F_EventScript_PokeDoll at (8,6); It's a POKéMON plush DOLL! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `RustboroCity_Flat1_2F:object_events:008` | 9,7 | [RustboroCity_Flat1_2F_EventScript_PokeDoll](../../baseline/source/data/maps/RustboroCity_Flat1_2F/scripts.inc#L74) — RustboroCity_Flat1_2F_EventScript_PokeDoll at (9,7); It's a POKéMON plush DOLL! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `RustboroCity_Flat1_2F:warp_events:001` | 2,1 | Warp from (2,1, elevation 0) to MAP_RUSTBORO_CITY_FLAT1_1F warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
