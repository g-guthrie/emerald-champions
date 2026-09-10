# Dialogue cohesion review — September 9, 2026

The review covered **8,199 labeled dialogue blocks in 405 Hoenn/shared text files**, plus **1,475 encoded C text expressions in eight supporting files**. All assigned dialogue was read, including ordinary townspeople, trainers, gifts, shops, tutorial services, signs, phone calls, television and conditional text. A separate raw-source reconciliation recovered 24 lines missed by the initial extraction. The review did not rely only on searches for old stone terminology.

The corpus includes inactive legacy text physically present in those files. Dormant FRLG map dialogue and unrelated battle/move/Pokédex description databases are outside this campaign-dialogue review. The C coverage covers the local legendary guide, field services, cap implementation, general strings, party/summary/storage UI and battle-set service; it is not a claim to have read every string in every C source file.

## The story carried through the campaign

1. **Opening:** Birch studies changes in Hoenn's weather and habitats. The player begins with two partners, and early rare discoveries make exploration worthwhile. Centers explain the free preparation tools; neighbors and the rival remain people with interests of their own.
2. **Devon and the coast:** Aqua targets Devon's research and Stern's instruments. Briney's voyage, Brawly's visit, Steven's letter and the Museum delivery give concrete destinations and reasons to travel.
3. **Mt. Chimney:** Magma wants more land for people and Pokémon. Cozmo's meteorite powers its volcano machine. Aqua opposes the eruption because it threatens coastal habitats; helping stop Magma does not make the player an Aqua member.
4. **Weather Institute and Mt. Pyre:** Aqua's weather research leads toward the ancient sea Pokémon and the Orbs. The Institute rescue restores its work and supplies Castform. The elders explain why the Orbs should be protected. Legendary sightings have local habitats and explicit campaign requirements.
5. **The awakenings:** The Blue Orb awakens Groudon, but Maxie cannot command it. Stern's submarine gives Aqua a route to the seafloor. At Mossdeep, Magma tries to steal rocket fuel to force an eruption; its dialogue no longer implies it is launching a rocket. Archie awakens Kyogre with the Red Orb and discovers that awakening is not control.
6. **Sootopolis:** Heat, flooding, threatened homes and trapped crews make the cost clear. Wallace and the player seek Rayquaza. The leaders acknowledge their mistakes and return the Orbs; that is the beginning of making amends. The final Gym and League remain the player's campaign goal.
7. **League:** Wally's confidence grows from the original catch through his setback in Mauville to Victory Road. The rival stays friendly and competitive. Wallace recognizes the player's journey, and Birch remembers the two partners from the rescue. Becoming Champion is the campaign payoff; the ending does not present the Frontier as newly unlocked.

## What changed

The main writing pass improved **414 dialogue blocks**, with another **70 blocks reflowed without changing their words**, for **484 changed assembly dialogue blocks**. Sixteen C text expressions were corrected. Exact labels, original text and per-file coverage are recorded under `work/dialogue-review-20260909/`.

- Replaced stale stone/guardian prerequisites with the current rare-wild habitat and badge/story rules. Preserved genuine Mega Stones, native visible legendary encounters, Regi puzzles and other real interactions.
- Removed unsupported claims about the player's precise winning tactics and corrected trainer hints against actual current parties.
- Reworked the main villain scenes, Wally and the rival so they discuss motives, relationships and events instead of repeatedly announcing formations, models or team-building instructions.
- Corrected submarine location/departure claims, weather chronology, gift explanations, Incense breeding advice, licensed field-skill instructions and EV terminology.
- Aligned active Frontier NPCs, phone calls and map descriptions with the Champions Circuit. Unreachable old facility and disabled rematch dialogue was read and distinguished from live behavior, rather than silently restoring those modes.
- Corrected the Slateport/Lilycove harbor and shipyard **dialogue branches** to show ferry readiness at six badges. Corrected Space Center civilians to resume normal conversations immediately after Magma's defeat, rather than after becoming Champion.
- Restored seven missing NPC actors: the Meadow warden, Route111 elevated-ruins researcher, manor historian, Altering Cave researcher, deep-Woods ranger, Scorched Slab warden and Sandstrewn Ruins archaeologist. These connect existing local discovery and research conversations. The Meloetta and Landorus conversations must be callable to perform their approved unlocks. Actor coordinates, packed tile behavior and adjacent-path checks are recorded separately; the existing actors were preserved.
- Added DNA Splicers, Zygarde Cube, N-Solarizer, N-Lunarizer and Reins of Unity to the existing free evolution archive, through its canonical generator. These tools previously existed only in Birch's Champion/completed-Hoenn-Dex reward. The archive opens with Steven's Mega Ring during the campaign; Birch's later reward is now clearly a spare kit. No new quest flag was added.
- Corrected overwide text and multi-line trainer paragraphs that used repeated newlines instead of the dialogue box's scroll control. Dedicated multi-line menu descriptions were preserved.

## Coverage and evidence

| Scope | Reviewed blocks/expressions | Evidence |
|---|---:|---|
| Main campaign, major towns and Gym/League dialogue | 1,755 | `main-reviewed.json` |
| NPC/route/dungeon group A | 1,382 | `npcs_a-reviewed.json` |
| NPC/route/dungeon group B | 1,589 | `npcs_b-reviewed.json` |
| Shared trainers, phone calls, TV, services and conditional text | 3,473 | `shared-reviewed.json` |
| Legendary/field service C expressions | 286 | NPC-A C inventory/report |
| General strings and party/summary/storage/set C expressions | 1,189 | `npcs_b-c-reviewed.json` |

`raw-string-reconciliation.json` identifies every recovered conditional line. `final-assembly-changes.json` and `reflow-edits.json` distinguish changed text from wrapping repairs. Before snapshots isolate this task from earlier legendary work and concurrent trainer/AI changes. Consumer/flag tracing was targeted at story transitions and claims that needed verification; this does not claim manual tracing of every instruction in every script.

### Verification status

- Built the fresh named release `pokeemerald-dialogue-20260909-release.gba` with its matching ELF. A final incremental build integrated the last branch correction and concurrent AI source updates before stamping.
- All required release checks passed: compiled map/tile integrity, canonical trainer agreement and configured abilities, battle-set structure/output agreement, Circuit projection, wild table integrity, whitespace, ROM/ELF identity, release header/checksum and memory bounds.
- The content stamp binds 28,979 declared inputs to this ROM/ELF pair. It is evidence about these built inputs and artifact bytes, not an assurance that later concurrent work cannot change the workspace.
- The normal-font width scan now has no overwide ordinary dialogue lines in its scanned corpus. The five remaining findings are intentionally unmodified debug text. Variable-width substitutions are not exhaustively modeled by this scan.
- Booted the actual release in native mGBA, entered New Game, advanced Birch's revised speech and reached player naming using button inputs only. Captured 30 speech frames and visually inspected 16 representative frames plus the boot/intro setup. The sampled text fits and the sequence progresses. Logs, snapshots and ROM hash are in `work/dialogue-review-20260909/native-opening/`.
- The seven NPC placements were checked against packed tile collision/elevation and adjacent walking routes without disconnecting existing dry paths. The final endpoint audit checked all 54 rare-wild definitions and 46 guide branches, including script fall-through. It found no remaining active instructions for the removed encounter stones. Five duplicate Mt. Pyre text branches remain dormant.

The source actor-path checks are not native interactions with those NPCs. This work did **not** run a complete fresh-save campaign playthrough, every dialogue animation/timing branch, legendary capture sequence, or battle-difficulty assessment. No new permanent tests were added.

Build/verification logs: `work/dialogue-review-20260909/release-build.log`, `release-build-final.log`, and `release-verification.txt`. ROM memory use: 27,899,976 of 33,554,432 bytes (83.15%).
