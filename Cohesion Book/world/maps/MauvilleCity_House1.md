# MauvilleCity_House1

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../03-mauville.md) · [Map source](../../baseline/source/data/maps/MauvilleCity_House1/map.json) · [Scripts](../../baseline/source/data/maps/MauvilleCity_House1/scripts.inc)

## Current map contract

`MAP_MAUVILLE_CITY_HOUSE1` · `LAYOUT_HOUSE2` · `WEATHER_NONE` · `MUS_RUSTBORO`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MauvilleCity_House1:object_events:001` | 4,4 | [MauvilleCity_House1_EventScript_RockSmashDude](../../baseline/source/data/maps/MauvilleCity_House1/scripts.inc#L4) — MauvilleCity_House1_EventScript_RockSmashDude at (4,4); Woohoo! //  I hear people call me the ROCK SMASH /  GUY, but I find that sort of degrading. //  I think I deserve a bit more respect, /  like maybe the ROCK SMASH DUDE. //  Woohoo! //  Anyways, your POKéMON look pretty /  strong. //  I like that! /  Here, take this HIDDEN MACHINE! / That HM registers ROCK SMASH as a /  field technique for your team. //  If you come across large boulders /  that block your path… //  Well, use that HM move and smash /  them right out of your way! //  Yes, sir! Smash rocks aside, I say! /  Woohoo! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `MauvilleCity_House1:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_MAUVILLE_CITY warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MauvilleCity_House1:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_MAUVILLE_CITY warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
