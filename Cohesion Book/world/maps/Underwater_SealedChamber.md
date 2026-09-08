# Underwater_SealedChamber

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/Underwater_SealedChamber/map.json) · [Scripts](../../baseline/source/data/maps/Underwater_SealedChamber/scripts.inc)

## Current map contract

`MAP_UNDERWATER_SEALED_CHAMBER` · `LAYOUT_UNDERWATER_SEALED_CHAMBER` · `WEATHER_UNDERWATER_BUBBLES` · `MUS_UNDERWATER`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Underwater_SealedChamber:bg_events:001` | 12,43 | [Underwater_SealedChamber_EventScript_Braille](../../baseline/source/data/maps/Underwater_SealedChamber/scripts.inc#L19) — Underwater_SealedChamber_EventScript_Braille at (12,43); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Underwater_SealedChamber:warp_events:001` | 7,1 | Warp from (7,1, elevation 0) to MAP_UNDERWATER_ROUTE134 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Underwater_SealedChamber:map_scripts:001` | MAP_SCRIPT_ON_DIVE_WARP | [Underwater_SealedChamber_OnDive](../../baseline/source/data/maps/Underwater_SealedChamber/scripts.inc#L5) — MAP_SCRIPT_ON_DIVE_WARP calls Underwater_SealedChamber_OnDive. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
