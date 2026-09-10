# Battle-set preset consumers — September 9, 2026

This is a procedural source trace against the current tree. It identifies preset
owners, consumers, overrides, and reachable exceptions. It is a derived report,
not a source of truth for loadouts and not proof of native execution or battle
quality.

## Canonical owner and choice inventory

The editable owner is
`data/emerald_champions/emerald_champions_hand_audited_battle_sets.json`.
`scripts/generate_emerald_champions_battle_sets.py` projects it into the runtime
tables declared by `include/emerald_champions_battle_sets.h:40-47`. The current
generated inventory contains 6,923 choices:

- Doubles: 1,309 defaults and 2,890 alternatives.
- Singles: 1,309 defaults and 1,415 alternatives.

Every current generated spread totals 508 EVs and every component is divisible
by four. These are conventional Gen 9 EVs: 510 total is legal, 252 is the
per-stat maximum, and the common fully effective allocation is 508. IV writes
are universally stored as 31 by `src/pokemon.c:2516-2527`; consumers do not need
their own IV-minimum policy.

Visible player choices are format-specific. Resolution selects a species/form
default plus that format's alternatives in
`src/emerald_champions_battle_sets.c:173-370`. Transformation and progression
requirements can hide choices. The tutor applies a selected Singles or Doubles
choice through `src/field_specials.c:660-810` and
`src/emerald_champions_battle_sets.c:703-715`.

## Shared application and reapplication

`ApplyPreset` is the common writer
(`src/emerald_champions_battle_sets.c:615-700`). It validates the ability,
moves, transformation requirements, and protected items; writes moves, Nature,
Ability, all six EVs, IV31, and the item; recalculates stats; then attempts the
Belly Drum HP-parity normalization.

Current canonical consumers are:

- Ordinary random wild encounters: `src/wild_encounter.c:692-702` chooses
  uniformly from every raw non-Mega Doubles choice that does not carry a
  protected progression item (`src/emerald_champions_battle_sets.c:734-761`).
  Battle Pike and Pyramid encounters are explicitly excluded. Ultra Beasts and
  Paradox species are intentionally admitted by the ordinary-wild classifier
  (`src/emerald_champions_battle_sets.c:109-129`). The seven Ultra Beasts that
  are also tracked in the acquisition ledger—Blacephalon, Buzzwole, Guzzlord,
  Kartana, Nihilego, Pheromosa, and Stakataka—remain ordinary wild-table
  species (`src/data/pokemon/legendary_signs.h:14-15,28,30,36,38,47`), not rare
  legendary replacements.
- Rare legendary wild replacements: the 54 `RARE_WILD_SIGN` definitions in
  `src/data/pokemon/legendary_signs.h:13-100` are selected from eligible local
  species by `ChooseRareWildLegendarySpecies` (`src/legendary_signs.c:801-833`)
  inside the normal `TryGenerateWildMon` path (`src/wild_encounter.c:710-798`).
  A selected replacement is created at the live cap and then receives
  `ApplyEmeraldChampionsRandomNonMegaSet`; it is no longer a dedicated static
  encounter tied to the former landmark object.
- Evolution: normal, trade, and Shedinja evolution completion paths call the
  recommended Doubles set at `src/evolution_scene.c:591-592,840-843,1255-1260`.
  The selector takes the first raw non-transformation, non-progression-item
  Doubles role (`src/emerald_champions_battle_sets.c:718-735`). Evolution
  therefore overwrites earlier move/Nature/Ability/EV/item customization and
  returns the monster to the campaign's Doubles-first orientation.
- In-game trades: the first visible Doubles choice is applied last, after legacy
  trade fields, at `src/trade.c:4595-4613`.
- Game Corner/prepared gifts: a random non-Mega Doubles choice is applied at
  `src/field_specials.c:463-523`; Game Corner delivery is at
  `min(30, current cap)` (`src/field_specials.c:494-515`).
- The player tutor and headless fixture interface use visible format-specific
  choices. The latter can then replace moves, Nature, Ability, item, or a whole
  EV spread (`src/emerald_champions_agent_prep.c:54-120,159-168`), but that file
  is guarded by `EC_HEADLESS_FIXTURES` at line 3 and is not a gameplay override.
- Special scripted callers, including the opening, regional rival replacement,
  and form gifts, use `ApplyEmeraldChampionsScriptedSet`, which supplies the
  authored item (`src/emerald_champions_battle_sets.c:768-770`).

Difficulty changes levels after trainer-party creation and recalculates stats
without reapplying a preset or HP parity (`src/difficulty.c:53-83`). The Leveler
does make a parity pass after completing its party sequence
(`src/party_menu.c:6007-6017`). Ordinary subsequent level-ups and generic stat
recalculations do not.

## Alternative preset sources and overrides requiring review

These paths are deliberately not simple references to an unchanged canonical
choice and must be included in any full spread audit:

1. Opening rescue and starters — `src/emerald_champions_opening.c:16-60`.
   The level-2 Poochyena and Zigzagoon rescue opponents use two literal sets
   (`:116-126`). Level-5 player starters normally copy the canonical Doubles
   default, but Chikorita replaces move 1 and Torchic uses a complete literal
   set.
2. Non-Hoenn regional rival replacements —
   `ApplyEmeraldChampionsRegionalRivalSet` in `src/emerald_champions_opening.c`. Each replacement starts from the
   species' canonical Doubles default. Opening-stage Bulbasaur, Charmander,
   Rowlet, and Sobble can change a move and/or Ability; all opening Eviolites
   become Sitrus Berry. Later Ivysaur/Charmeleon use Overgrow/Blaze without a
   sun setter; Ivysaur, Charmeleon and Bayleef have move overrides, and Monferno
   uses Fire Punch instead of Thunder Punch beside Lightning Rod partners.
   Drizzile replaces redundant Water Pulse with Ice Beam for Grass coverage.
   The E0032 review removed automatic duplicate-item replacement: campaign
   teams have no Item Clause, so the canonical item is retained.
   EVs remain those of the copied canonical default.
3. Rival reach — `src/battle_setup.c:2298-2429`. The recognizer covers all 18
   Route 103/110/119 May/Brendan battles (trainer IDs 520-537), all six Lilycove
   battles (661-666), and the six explicitly listed Rustboro battles. Regional
   replacement occurs for every recognized stage before the Easy/Normal/Hard
   level reduction. This is not opening-only behavior.
4. Form gifts — `src/data/emerald_champions_forms.h:182-224` and
   `src/field_specials.c:6808-6826`. Indeedee-F, Cosplay Pikachu, Battle Bond
   Greninja, and Kubfu point to canonical Doubles defaults. Partner Pikachu,
   Partner Eevee, and Cosmog use literal sets. Gift level is
   `min(current cap, 25)`.
5. Champions Circuit —
   `scripts/generate_showdown_champions_circuit.py:18-75` combines the pinned
   Showdown random-doubles manifest with named references to the canonical
   Doubles owner. Authored references cannot contain local overrides, but they
   are copied into the generated Circuit header and require regeneration after
   owner changes. Non-authored Circuit templates synthesize their own EVs and
   Natures by role and move category at `src/champions_circuit.c:907-1000`.
   Runtime copies authored templates verbatim or builds procedural templates at
   `src/champions_circuit.c:1390-1411`, then writes all fields directly at
   `:1433-1463`; it does not call common preset application or parity handling.
6. Generic eggs — `data/scripts/gift_pichu.inc:31`,
   `data/maps/LavaridgeTown/scripts.inc:270`, and
   `data/maps/Route117_PokemonDayCare/scripts.inc:18`. Pichu, Wynaut, and Togepi
   are the current Hoenn `giveegg` gifts. They retain universal IV31 but do not
   receive a canonical battle set at creation or hatching. They are acquisition
   alternatives to review, not automatically defects without a contract saying
   every gift must arrive prepared.
7. Live ordinary scripted wild encounters — `ScrCmd_setwildbattle` routes to
   `CreateScriptedWildMon` (`src/scrcmd.c:2465-2484` and
   `src/script_pokemon_util.c:124-141`), which does not apply a canonical set.
   Current reachable Hoenn examples are the Route 120 bridge Kecleon at level 30
   (`data/maps/Route120/scripts.inc:201-229`), shared Devon Scope Kecleon at
   level 30 (`data/scripts/kecleon.inc:57-82`), three level-25 New Mauville
   Voltorb (`data/maps/NewMauville_Inside/scripts.inc:260,285,310`), two level-30
   Aqua Hideout Electrode (`data/maps/AquaHideout_B1F/scripts.inc:32,57`), and
   the postgame level-40 Sudowoodo
   (`data/maps/BattleFrontier_OutsideEast/scripts.inc:114-144`, whose object is
   present at `events.inc:21`). These retain IV31 but use initial moves and do not
   receive canonical moves, Nature, Ability, item, or EVs. They are a live
   consistency exception if the ordinary-wild preset contract is intended to
   include static species.

Legendary acquisition has several surviving non-random-wild providers, so it
must not be summarized as one static-sign consumer:

- Six visible-source definitions—Darkrai, Articuno, Mewtwo, Pecharunt,
  Regigigas, and Zapdos—still call `CreateSelectedLegendarySignEncounter` from
  their map scripts. That function creates the species at live cap plus its
  authored offset (clamped to 1-100) and applies a random non-Mega opponent set
  (`src/legendary_signs.c:598-607,664-680`; definitions at
  `src/data/pokemon/legendary_signs.h:21,69,77,81-82,85`). Magearna is the
  seventh visible-source definition, but Devon delivers it through
  `TryGiveSelectedLegendarySignReward`, not a wild battle
  (`data/maps/RustboroCity_DevonCorp_2F/scripts.inc:108-134`).
- The 16 pre-existing native legendary encounters listed at
  `src/legendary_signs.c:332-342` remain scripted statics. Their map scripts
  call `CreateEmeraldChampionsStaticLegendaryEncounter`, which uses live cap
  plus the script's offset and applies a random non-Mega opponent set
  (`src/legendary_signs.c:682-691`). These are native encounters, not Legendary
  Sign rows.
- Gift rewards use a separate random eligible non-Mega raw-choice filter and
  `ApplyEmeraldChampionsOpponentSet` (`src/legendary_signs.c:986-1017`). This
  covers Magearna at its cap-plus-one sign level, Arceus at the live cap
  (`:1027-1042`), and Circuit legendary rewards at level 80
  (`src/champions_circuit.c:1680-1723`). Game Corner Genesect and Poipole use
  the prepared-gift path already described above at `min(30, current cap)`;
  Phione is instead produced by breeding (`src/daycare.c:898-907`) and does not
  pass through this legendary reward preset writer.

## Level range and HP parity limits

The shared stat owner is `CalculateSpeciesStat`
(`src/pokemon.c:1319-1330`): EV contribution is `ev / 4`, multiplied through the
actual level formula. Consequently, a level-50-only optimization is not enough.
Current ordinary wild data spans levels 2 through 100, and campaign caps are
14, 20, 30, 40, 45, 55, 60, 70, 80, then 100
(`src/caps.c:8-38`). Difficulty deploys an ordinary trainer at
`max(authored level, live cap) - 4/-2/0` for Easy/Normal/Hard, floored at 1
(`src/difficulty.c:36-83`). Shared player/wild/rival sets therefore need to work
over their actual 2-100 campaign range, not merely at level 50.

The 54 rare-wild legendary replacements and the seven ordinary-table Ultra
Beasts are forced to exactly the current live cap before creation
(`src/wild_encounter.c:759-797`), so their current campaign levels are
14/20/30/40/45/55/60/70/80/100 rather than a landmark-specific static level.
The six surviving visible encounters and the 16 native scripted statics instead
use `GetSignLevel`: live cap plus an authored/scripted offset, clamped to 1-100
(`src/legendary_signs.c:598-607,653-690`). Legendary gift levels are
provider-specific: Magearna uses its cap-plus-one sign level, Arceus uses the
live cap, Game Corner prizes use `min(30, cap)`, and Circuit rewards use level
80. These paths all consume canonical choices except breeding-produced Phione.

The Circuit expands the relevant range. Players enter at level 100
(`src/champions_circuit.c:1504-1510`); opponents begin at
`100 - difficulty reduction`, gain one level per six wins, and cap at 255
(`include/champions_circuit.h:7-9`, `src/champions_circuit.c:191-196,1525-1531`).
Battle tents instead use the current cap, 14-100, minus the difficulty reduction
(`src/champions_circuit.c:1572-1585`). Any canonical choice named by the Circuit
supplement must therefore remain sensible at levels 96-255 as well as its
campaign levels.

The latest [universal berry policy](berry_thresholds_2026_09_09.md) makes every
natural HP-triggered berry/item activate at the ordinary floor cutoff plus one,
capped below full HP. Eligible Gluttony berries use half HP plus one; Sitrus is
no longer a special case. Runtime and AI forecasts share the same helper.

The former ±40 HP-EV search and fuzzy preset recognition were deleted.
Application, Leveler use and Circuit construction retain the authored/custom EVs.
The Circuit's Linoone Belly Drum / Extreme Speed / Seed Bomb / Protect,
Figy / Gluttony template (offset1021) can therefore keep its original spread.
Its353 maximum HP at level98 leaves177 after Drum; the new half-HP cutoff177
permits recovery without an EV change. The earlier even-HP exception is obsolete.
The native complete-catalogue application check now requires exact EV equality.

## Dormant legacy paths, not current gameplay defects

- Overworld wild encounters are disabled by
  `include/config/wild_encounter.h:20`, and no current map object uses
  `TRAINER_TYPE_OW_WILD_ENCOUNTER`. The non-canonical path in
  `src/wild_encounter_ow.c` is only a latent hazard if that feature is enabled.
- Berry pests are disabled by `include/config/overworld.h:40`; their scripted
  wild path in `src/berry.c` is likewise dormant.
- The original Frontier Brain tables in `src/frontier_util.c:262-658` still use
  66-point legacy spreads, consumed by the old facility setup at `:2625-2654`.
  On a fresh save, however, all Tower, Dome, Arena, Palace, Factory, Pyramid,
  and Pike desk actors route to the Champions Circuit entry; the active entry is
  `data/maps/BattleFrontier_BattleTowerLobby/scripts.inc:440-467`. With old-save
  compatibility explicitly unsupported, the compiled Brain data is dormant
  legacy rather than a current gameplay migration defect.

## Remaining migration remnants

Current Birch and Pokémon Center Nurse text is already migrated: Birch says
EVs, 510 total, and 252 per stat at `data/text/birch_speech.inc:49-56`; the Nurse
says EVs and Battle Sets at `data/text/pkmn_center_nurse.inc:29-31`. No active
runtime UI Stat Point wording was found.

Operational/current prose still carrying the superseded terminology includes
`docs/BATTLE_RULESET.md:5`, `docs/SYSTEMS.md:30-32`,
`tools/agent_player/player_context.md:45-49,65`,
`tools/agent_player/README.md:74`, and the agent schema/compatibility names in
`tools/agent_player/arsenal.schema.example.json:18` and
`tools/agent_player/agent_player.py:49`. The UI renderer's scenario IDs remain
`stat-point-*` at `scripts/render_emerald_champions_ui.py:525-606,1277-1292`;
those are automation identifiers rather than player-facing strings.

Generator/source-description remnants include
`scripts/generate_showdown_champions_singles.py:114`, the generated random-
singles manifests' `adaptations` strings, and one Gyarados source-note string in
the hand-audited and generated battle-set JSON. Historical `work/`, `release/`,
old trainer-authoring reports, and Cohesion Book snapshots describe their own
prior state and should not be reported as current runtime defects.
