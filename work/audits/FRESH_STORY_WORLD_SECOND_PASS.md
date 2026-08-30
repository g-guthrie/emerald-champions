# Emerald Champions fresh story/world second pass

Date: 2026-08-29
Checkout: `/Users/gguthrie/Projects/Pokémon Emerald Champions/game-source`
Branch / starting HEAD: `release/full-source-audit` / `709b73fbb`
Authority used: executable map JSON, assembled event scripts, story C helpers, constants, and live verifier code. Documentation was not used as proof.

## Outcome

The focused source gates pass after this pass. The audit found and fixed one progression-class defect family (the postgame S.S. Ticket was narratively granted but ignored at two ports, physically required at the Frontier return port, and unsafe when both item stores were full), one misleading Legendary Sign completion branch, two impossible dead background events, and a cluster of reachable stale item/mechanics/exclusivity copy.

This is source/static and `event_scripts.o` evidence, not a fresh-save playthrough or full-ROM release proof. No full ROM build was requested or run.

## Exact coverage

The source walker loaded all 540 non-FRLG Hoenn maps from `data/maps/map_groups.json`, parsed every map JSON, indexed every assembled label, and followed the campaign spine in play order. It covered:

- 540/540 map payloads; 490 have map-local `scripts.inc` files and 50 are data-only/dynamic rooms.
- 2,965 object events, 403 coordinate events, 718 background events: 4,086 physical event records total after deleting two invalid null signs.
- 1,402 warp records and 152 map connections.
- 809 map-script registrations and 104,701 assembled script lines across the map scripts, `data/scripts`, and `data/event_scripts.s`.
- 17,938 checked control-flow, dialogue, movement, map-script, and trainer-script references.
- 391 `specialvar` return contracts.
- 161 direct `giveitem` sites, five scripted gift-Pokémon delivery sites, and every finite map pickup covered by the reward and progression gates.
- 10,846 selected story dialogue lines and 47,623 literal Hoenn dialogue lines measured against the native 216-pixel window.

Hoenn group inventory (sum 540):

```text
TownsAndRoutes 57
IndoorLittleroot 5; IndoorOldale 5; IndoorDewford 6; IndoorLavaridge 7
IndoorFallarbor 8; IndoorVerdanturf 9; IndoorPacifidlog 7; IndoorPetalburg 7
IndoorSlateport 14; IndoorMauville 8; IndoorRustboro 17; IndoorFortree 10
IndoorLilycove 23; IndoorMossdeep 13; IndoorSootopolis 15; IndoorEverGrande 15
IndoorRoute104 2; IndoorRoute111 2; IndoorRoute112 2; IndoorRoute114 3
IndoorRoute116 1; IndoorRoute117 1; IndoorRoute121 1
Dungeons 108; IndoorDynamic 61; SpecialArea 89; IndoorRoute104Prototype 2
IndoorRoute109 1; IndoorRoute110 13; IndoorRoute113 1; IndoorRoute123 1
IndoorRoute119 3; IndoorRoute124 1; EmeraldChampionsExpansion 22
```

### Campaign-order manual source trace

| Phase | Source path followed | Result |
|---|---|---|
| Opening through Stone Badge | Truck/Littleroot, starter region choice, Routes 101-104, Petalburg, Woods, Rustboro, Roxanne | Starter/Pokédex state, first badge, Cut, Devon Goods, rewards, rival/object hides, and all local NPC/sign branches resolve. |
| Devon, Dewford, Slateport | Rusturf recovery, Devon president, Briney, Granite Cave/Steven, Brawly, Museum/Stern/Archie | Letter/Parts/PokéNav delivery is fail-closed; Brawly gates the Mega Ring and Slateport sailing; the Museum follow-up battles permit a one-survivor continuation. |
| Mauville through Norman | Route 110, Wattson/New Mauville, Route 111/Fallarbor/Meteor Falls, Mt. Chimney/Jagged Pass, Flannery, Petalburg Gym | Badge/HM/Go-Goggles ordering and story gates resolve; Winstrate sequence is non-stranding; Rotom and restored branches retain retry state. |
| Fortree and Mt. Pyre | Routes 118-123, Weather Institute, Fortree/Winona, Steven/Kecleon, Mt. Pyre | Castform has party/PC/no-room outcomes; Devon Scope and Fly advance only on delivery; visible/conditional Sign prerequisites and object persistence resolve. |
| Lilycove, Mossdeep, Seafloor | Lilycove/Aqua Hideout, Tate and Liza, Space Center, Dive, Seafloor Cavern | Badge-six and badge-seven ordering holds; the three-grunt Space Center sequence allows a one-survivor continuation; Dive gates Seafloor access. |
| Sootopolis climax | Groudon/Kyogre, Cave of Origin, Sky Pillar/Rayquaza, Juan | Native legendary fights are capture-only persistent, the Cave of Origin route is unblocked, and Juan requires the Mind Badge. |
| League | Victory Road/Wally, lobby badge guard, Sidney/Phoebe/Glacia/Drake/Wallace, Hall of Fame | Eight badges are explicit; all five League battles offer retirement after a prior fight leaves one usable Pokémon; whiteout resets the run. |
| Postgame | Forced home scene, Lati roamer, three ferry ports, four event islands, Frontier reception, Circuit/mastery | Ticket entitlement and full-storage fallback are now coherent at every port; island passes/returns, Frontier entry, Circuit rewards, and Arceus mastery sources resolve statically. |
| Restored branches | All 22 expansion maps, including woods/manor/meadows, ruins, Altering Cave, Scorched Slab, Seaspray, Jirachi and Diancie rooms | All are connected, all restored warps are reciprocal, 129 objects/40 pickups resolve, and encounter/clue gates are source-backed. |

## Findings and fixes

### SW-01 fixed: the S.S. Ticket was a dead reward and its full-storage path could trap progression

Evidence before the fix:

- `PlayersHouse_1F_EventScript_GetSSTicketAndSeeLatiTV` tried the Bag and PC, but `PlayersHouse_1F_EventScript_SSTicketNoRoom` ended while the on-frame house state remained 3. The forced scene could immediately retrigger instead of letting the player make room.
- Slateport and Lilycove attendants checked only `FLAG_SYS_GAME_CLEAR`, then narrated that the player flashed a Ticket. The Ticket and `FLAG_RECEIVED_SS_TICKET` were not consulted.
- `BattleFrontier_OutsideWest_EventScript_FerryAttendant` checked the physical `ITEM_SS_TICKET`. A player using a full-storage entitlement fallback could arrive at the Frontier but be refused the return ferry.

Fix:

- Bag delivery remains first, PC item storage remains second, and the saturated-storage edge now registers the ferry pass and continues the Dad/Lati scene instead of ending in state 3 (`data/scripts/players_house.inc`, `PlayersHouse_1F_EventScript_SSTicketNoRoom`).
- Slateport, Lilycove, and Frontier now gate on the durable `FLAG_RECEIVED_SS_TICKET` entitlement.
- All three ports now say they confirmed a registered ferry pass; stale physical-ticket narration symbols were removed.
- `verify_emerald_champions_progression.py` enforces game-clear -> registered pass -> destination selection at the mainland ports, receipt-flag return service at the Frontier, the full-storage continuation, and coherent narration.

### SW-02 fixed: Devon's Legendary Sign completion advice described only the old subset

`TryDiscoverEligibleLegendarySign` exhausts only `LEGENDARY_SOURCE_CONDITIONAL_WILD`, but the old fallback said every wild Sign was awake and pointed only to Mt. Pyre's three. That advice omitted visible encounters, ordinary-wild roots, breeding, Game Corner, Circuit rewards, and mastery even though all can still block Arceus.

The replacement at `RustboroCity_DevonCorp_2F_Text_AllLegendarySignsRecorded` distinguishes the remotely tuned conditional-wild subset and routes the player to the remaining source classes without claiming completion. `verify_legendary_sign_completion_guidance` ties the copy to all seven live source categories.

### SW-03 fixed: two dead Meteor Falls events were far outside their layouts

Both `MeteorFalls_1F_1R` (30x42) and `MeteorFalls_1F_2R` (30x32) carried a null-script background sign at `(9,58)`. The entries could never be reached and were not meaningful placeholders. They were removed. The progression gate now bounds every background event, including null/inert entries, while preserving the one-tile border allowance used by native signs.

### SW-04 fixed: reachable item/mechanics dialogue contradicted live code

- Verdanturf Mart: renamed stale `XSpecialIsCrucial`; corrected Nest Ball from an HP/weakened claim to its live lower-level formula and removed the false Verdanturf-only stock claim.
- Lavaridge Mart: renamed stale `XSpeedFirstStrike`; current copy correctly explains no-Bag trainer battles.
- Slateport: renamed stale `EffortRibbonWoman` to `StatPointGuide`; no EV-training reward is advertised.
- Mossdeep Mart: removed the false Mossdeep-exclusive Net/Dive Ball claim and updated Dive Ball from seabed-only to the live surfing/fishing/underwater contexts.
- Petalburg Mart and Rustboro School: removed claims that poison drains HP while walking; `OW_POISON_DAMAGE GEN_LATEST` makes poison damage battle-only while the status persists.

The reward gate now scans every live non-FRLG, non-debug, non-trainer dialogue source for X-item, vitamin, EV-grind, and stale reward language. Current unreviewed hits: zero.

### SW-05 fixed: commerce copy made unverifiable exclusivity promises

Absolute claims were removed from the Slateport Market sign, Slateport Mart, the Lilycove rooftop sale, and the Mt. Chimney specialty line. A repo-wide live-Hoenn dialogue scan now fails commerce blocks containing `only place/shop/made/sold/get/buy` or `nowhere else`. Current unreviewed commerce exclusivity claims: zero.

## Source contracts confirmed

- Badge order and Leveler caps: 1/20, 2/30, 3/40, 4/45, 5/55, 6/60, 7/70, 8/80; field-move license flags align with Cut/Flash/Rock Smash/Strength/Surf/Fly/Dive/Waterfall.
- Mega: Brawly precedes the Mega Ring; Dewford sailing requires Brawly plus Ring ownership; Devon's Pidgeotite path checks Ring ownership; the complete archive opens at badge eight.
- Story delivery: Letter, Devon Parts, Great Ball handoff, Dowsing Machine, Powder Jar, Devon Scope, Magma Emblem, Go-Goggles, all HMs, and S.S. Ticket do not advance their owning state before successful delivery or an explicit functional fallback.
- Gift Pokémon: Beldum, Castform, revived fossils, Game Corner prizes, Circuit rewards, and Arceus distinguish party, PC, and no-room results; no completion flag is set on `MON_CANT_GIVE`.
- Legendary persistence: 82 Sign definitions; 25 visible quests; seven ordinary-wild roots; 32 modern physical one-off Pokémon objects; 11 native static roots checked for capture-only terminal state. Knockouts do not set the terminal caught/defeated flags on those roots.
- Legendary availability: 101 legendary-class families have acquisition roots; 82 prerequisite chains terminate; 12 finite Circuit rewards precede the win-40 mastery reward; all four event-island passes are obtainable and return routes exist.
- Restored world: 22 connected maps, 129 objects, 40 unique pickups, 21 wild tables, 17 Sign objects, four static sanctuaries.
- Fossils/evolution: all 11 complete fossils revive repeatably; 30 trade evolutions have solo alternatives; all 15 required evolution items are obtainable.
- Flags/vars: current numeric scan found no duplicate nonzero Hoenn flag values and no duplicate numeric var values; all assembled symbolic uses resolved in `event_scripts.o`.

## Gate results

All commands were run from the game-source root with `PYTHONDONTWRITEBYTECODE=1` for Python gates.

| Gate | Result |
|---|---|
| `scripts/verify_emerald_champions_story.py` | PASS; 10,846 selected story lines and 47,623 literal Hoenn lines fit 216 px |
| `scripts/verify_emerald_champions_progression.py` | PASS; 540 maps, 4,086 events, 1,402 warps, 17,938 references / 104,701 lines, 391 specialvar contracts |
| `scripts/verify_emerald_champions_reward_economy.py` | PASS; 24 unique world Mega Stones, 202 visible-pickup flags, 112 hidden-pickup flags, 91 direct reward-state checks |
| `scripts/verify_legendary_availability.py` | PASS; 101 families, 82 terminating chains, 25 visible quests, seven ordinary-wild roots, four island passes |
| `scripts/verify_legendary_signs_and_circuit.py` | PASS; 82 Signs, 32 physical one-offs, 311 Circuit variants / 444 templates |
| `scripts/verify_restored_emerald_champions_world.py` | PASS; 22 maps / 129 objects / 40 pickups / 21 wild tables / 17 Signs / four sanctuaries |
| `scripts/verify_solo_evolution_access.py` | PASS; 30 solo trade alternatives / 15 required items |
| `scripts/verify_fossil_revival.py` | PASS; 11 repeatable complete fossils with safe delivery |
| `make -j4 build/emerald/data/event_scripts.o` | PASS; rebuilt, 2,665,840 bytes, SHA-256 `ac60162c3ef5345013a5bb52e447b38f3757a712692e64fd7cf2d6eac43a7079` |

## Exact files changed by this pass

- `data/scripts/players_house.inc`
- `data/maps/BattleFrontier_OutsideWest/scripts.inc`
- `data/maps/LavaridgeTown_Mart/scripts.inc`
- `data/maps/LilycoveCity_DepartmentStoreRooftop/scripts.inc`
- `data/maps/LilycoveCity_Harbor/scripts.inc`
- `data/maps/LittlerootTown_BrendansHouse_1F/scripts.inc`
- `data/maps/MeteorFalls_1F_1R/map.json`
- `data/maps/MeteorFalls_1F_2R/map.json`
- `data/maps/MossdeepCity_Mart/scripts.inc`
- `data/maps/PetalburgCity_Mart/scripts.inc`
- `data/maps/RustboroCity_DevonCorp_2F/scripts.inc` (only the Legendary Sign guidance block; concurrent fossil-delivery edits were preserved)
- `data/maps/RustboroCity_PokemonSchool/scripts.inc`
- `data/maps/SlateportCity/map.json`
- `data/maps/SlateportCity/scripts.inc`
- `data/maps/SlateportCity_Harbor/scripts.inc`
- `data/maps/SlateportCity_Mart/scripts.inc`
- `data/maps/VerdanturfTown_Mart/scripts.inc`
- `scripts/verify_emerald_champions_progression.py`
- `scripts/verify_emerald_champions_reward_economy.py`
- `scripts/verify_emerald_champions_story.py`
- `work/audits/FRESH_STORY_WORLD_SECOND_PASS.md`

No trainer-party data, trainer dialogue, competitive-set generator/source data, or `docs/emerald_champions_master_battle_design.txt` was edited by this pass.

## Remaining human-only risks

1. No fresh-save emulator campaign was played. Cutscene movement, collision geometry, camera choreography, map reload timing, and the saturated-storage S.S. Ticket fallback need scenario saves/runtime observation.
2. Structural reachability is not runtime reachability. A pure JSON/explicit-script graph reaches 435/540 maps from Littleroot; the remaining 105 include C-driven Dive/facility/secret-base transitions and explicitly unused rooms. The focused restored-world graph is proven, but all dynamic transitions still need runtime traversal.
3. The static walker proves labels, coordinates, assets, destinations, indices, and named state invariants. It cannot prove that every NPC tile can be approached from every collision state. Null-script templates remain common in facility battle rooms, cutscenes, decorations, and dynamic actors; ordinary mute-NPC polish (for example the lone Lounge 9 girl and one Shipyard scientist) remains a visual/playtest decision rather than a progression defect.
4. Literal text width is exact; runtime substitutions in legacy TV, Match Call, apprentice, and record strings remain data-dependent and need rendered boundary-case checks.
5. `OW_TRAINER_REMATCHES FALSE` deliberately leaves native rematch branches unreachable. Circuit is the repeatable endgame, but a human pass should confirm no reachable Match Call copy promises a rematch that can never occur.
6. Old-save migrations, full PC/Bag extremes, all legendary knockout/catch/reload permutations, Frontier streak flow, and each island's return ferry were source-audited but not executed.
7. Trainer party quality and competitive-set generation were explicitly owned by other passes and excluded here.

The current evidence supports a coherent assembled story/world source, not a claim that the game is perfect or release-ready.
