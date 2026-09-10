# Final legendary guidance endpoint audit

Reviewed the 54 authoritative rare-wild definitions, all 46 ordinary dialogue blocks mentioning those species or rare-discovery/Sweet Scent guidance (including battle/reward false positives), and all local/dynamic guide caller branches. The source reference trace uses actual map object, background and coordinate events and map hooks, explicit references and script fall-through. It includes `data/event_scripts.s`; the initial conservative trace lacked fall-through and wrongly classified Steven's farewell as inactive. The corrected inventory records it as live.

41 of the 46 candidate blocks have live source callers after the repairs. All 46 inspected guide handler branches have callers. These are source callability results, not proof that every conditional branch was exercised or every map visited. `npcs_a-endpoints.json` lists every candidate and its root event/call chain; `npcs_a-guide-callers.json` lists local research, route-species sign, quest unlock and selected-research branches. The optional Devon researcher routes to the all-entry research menu, which provides current source-derived clues for all 54 discoveries. A dedicated local actor is not required for every species.

Four additional intended guide actors were absent from map data and had no alternate caller. Appended permanent non-trainer actors, preserving all prior objects and every other map field:

| Map / guide | Position | Inbound start | Steps to an interaction neighbor |
|---|---|---|---:|
| Altering Cave 1F researcher | 6,6 | entrance ladder 5,6 | 0 |
| Petalburg Woods 3 ranger | 10,10 | north entrance 8,7 | 4 |
| Scorched Slab B2F warden | 19,16 | entrance ladder 17,15 | 2 |
| Sandstrewn Ruins archaeologist | 5,14 | Desert Underpass entrance 4,13 | 1 |

All placements are collision-free elevation-3 ground (MB_CAVE except Woods MB_NORMAL), have valid existing graphics IDs and scripts, and overlap no event. Source flood fill includes existing object obstacles and ordinary walkable behaviors; it found an accessible adjacent interaction cell from each inbound warp and no newly disconnected dry walkable cells other than the actor's own occupied cell. The Sandstrewn actor stands beside the entrance shore and the center shore cell remains clear. Details and exact paths are in `npcs_a-additional-actor-wiring.json`. No retired trainer was restored. Woods2 has no ranger script; its two local discoveries are already advertised by the main forest sign. Verdanturf's existing warden is wired correctly. The two required permanent unlock actors (Dewford Sing/Meloetta and elevated Route111 Castform/Landorus) were repaired in the preceding pass and remain callable.

Five optional old MtPyre text blocks remain dormant: CresseliaAwakened, CresseliaAwaits, TimeSignDormant, DialgaAwakened, DialgaAwaits. The live elder instead calls the current C research text, stepping from Darkrai to Cresselia and Dialga. Their duplicates were not activated.

A case-insensitive ordinary `.string` scan for stone/landmark/shrine/altar/seal/inspect/respond/awakening/mark language found no remaining active instructions to use the 34 removed rare-encounter stones. Candidate lines were inspected with relevant original scripts. Retained physical interactions are real: Route111's cursed Runerigus evolution tablet; the Abandoned Ship Odd Keystone/Spiritomb socket; Heatran's Magma Stone scene; MtPyre's Pecharunt tombstone and Darkrai scene; Magearna's prototype and its separate Mega Stone gift; Mega/evolution items and ordinary geological flavor. PokeNav landmark comments describe map registration, not removed encounter actors. FRLG stone ruins and unrelated museum/Contest/BP text were not treated as rare-discovery mechanics. No additional prose change was needed in this bounded pass.

Game source writes finished before root's integrated build. No build, emulator, rendering, traversal or runtime test was performed by this reviewer. Existing broad script/C review counts remain separate from this endpoint audit; root's subsequent dialogue reflows were not overwritten.
