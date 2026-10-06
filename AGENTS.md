# Inclement Emerald 2 — standing rules

Source and native runtime behavior are the only ground truth. This repository
keeps no docs, ledgers, handoffs or progress notes: they drift. Do not create
them. Report findings in the conversation and put durable facts in code, tests
or data.

## Working in the tree

- Build with `make -j4 release`. One build at a time.
- Trainer authoring lives in `data/emerald_champions/emerald_champions_battle_teams.txt`;
  run `python3 scripts/emerald_champions_teams.py --write`, then `--check`.
- Commit by explicit path only; never `git add -A`, `commit -a`, reset, clean
  or wholesale restores. Preserve saves, archives and other agents' work.
- Never load a savestate into a different ROM; move a save between builds with
  a native in-game save and a clean boot.
- Overworld objects (map.json objects, local ids): Codex, except in rebuilt regions,
  where the progression generator owns them (see Rebuild).
- UI changes require native screenshots of affected states and menu navigation:
  opening, selection, page changes, cancellation and returned player control.
- Studio instructions live in `tools/studio/skill/SKILL.md`; prefer this
  checked-in source over installed copies.

## Rebuild (owner decision, 2026-10-06)

The game's content layer is being rebuilt from the ground up on the session branch;
`main` stays the old game, used only as reference, until cut-over.

- Source of truth: `data/progression/`. `stages.yaml` orders the story stages (cap,
  field moves, key items, flags set). One file per region lists its areas: maps, the
  stage that opens each, the gate that opens it (citing the script, flag or object),
  trainers (required or optional), items, gifts, NPCs and wild tables. Nothing else
  may state these facts by hand: map objects, generated scripts, wild levels, battle
  order and manifests are generated from it, and tests fail when code and spec differ.
- Gates use the standard barriers only (blocker NPC, turn-back tile, opening, ferry,
  Gym rule); each reads the in-game progression table generated from the spec.
  Story cutscenes are ported by hand and wired to stages.
- One region at a time, from Littleroot: write the region spec from the old game and
  the design rules, get the owner's approval, generate, port its cutscenes, then play it
  headlessly with forced trainer wins. Photograph every walkable tile (explore mosaics)
  and every DexNav, check spec against play, keep a stage save. Then report to the
  owner before the next region: Pokemon available, items, balance concerns,
  recommended changes. The owner decides; changes go into the spec.
- Never state anything as fact that was not seen in play or read in code; say which.
- Rename misnamed labels, flags and texts as they are found, and say so.
- Wild levels belong to places: an area's levels come from the stage that first opens
  it, climbing in route order to a few levels under the cap; tables open no lower than
  the tool that reaches them. No evolved Pokemon below its evolution level, nothing
  above the live cap, legend-class at the cap.
- Gym trainers cannot be skipped: a leader battles only after the Gym's trainers.
- No Mega Stone before the Mega Ring; no friendship evolution before the Stone Badge.
- Wild Pokemon hold no battle items (berries, Honey, medicine, sell items, Everstone,
  evolution items, signature items and Nectars stay); the vendor's badge floors cover them.
- Worthless pickups (potions, ethers, repels, status heals) become battle items or Gems.
- Day Care gift Eggs carry special moves their family learns nowhere else.
- Special places stay special: Dhelmise lives only on the Abandoned Ship.

## Design rules (owner decisions)

- Scope: one playthrough ending at the finale — League → Wally (authored team)
  → five S.S. Tidal cabin teams → Steven → Birth Island/Deoxys (defeat or
  capture) → Buffel. Every reward pays off before or inside it. The Battle
  Frontier and Champions Circuit come later; never gate on them.
- Inclement is the world/story/progression/economy baseline. Universal
  legal-move tutor, no TMs. HMs need their story license, the badge and a party
  member able to learn the move — no move slot. Flight Beacon may use a
  Fly-capable Pokémon from party or PC. Leveler raises the party to the current
  cap. Battles grant no experience; leveling and evolving never change moves.
  Native ability switching.
- Inclement's species stat buffs and additional Ability slots apply to
  player, wild, trainer and partner Pokémon alike. Existing authored teams
  keep their chosen Abilities until deliberately retuned in playtesting. On every difficulty, authored Casual and Regular teams lose one
  additional level (minimum 1); other categories retain their levels. Base
  difficulty offsets live in `GetTrainerLevelReduction`; any
  text quoting them derives the numbers from it. Text is always Instant; there
  is no text speed option.
- No Game Book, player guides or Center battle presets. No
  new ground or hidden items: only replace original pickups or TM gifts.
- No Terastallization anywhere.
- Held items: Center nurses and the visiting Oldale Mart nurse share the
  initial tool handoff, including the Regenerator and Leveler. With the
  Regenerator, a held Berry its own holder eats in battle returns after
  battle. A Berry swapped away, eaten directly by a foe, burned or corroded
  is gone. Knock Off removes an item, Berries included, for the current battle
  only. Thief and Covet theft is temporary in Trainer battles: both sides'
  stolen items return to their original owners afterward, including a stolen
  Berry later eaten or destroyed. Wild theft remains permanent. Items acquired
  through theft or Pickup never unlock vendor stock.
- EVs: no Pokémon, the player's or any trainer's, has EVs before the Knuckle
  Badge (cap 30); the badge opens the EV editors, turns trainers' authored
  spreads on and gives owned Pokémon still without EVs the arrival spread.
  After it, every Pokémon joining the player (catch, gift, starter, trade,
  hatch) arrives with 252 HP / 252 Spe / 4 Def / 2 SpD; owned Pokémon keep
  their chosen spread. Every Center move tutor plans a whole spread and
  applies it at once for a flat ¥500 (free when unchanged, nothing on
  cancel). Evie in Fallarbor uses the same editor at ¥2 per added EV, with free
  reductions. Both editors show real stat previews and direct row adjustment.
- Steam Engine also makes Water never super effective on its holder.
- AI: authored doubles teams and the shared planner are preserved. Opponents may
  know loadouts but never read the player's committed move, target, switch or
  replacement. Tune difficulty with levels first. When levels alone cannot
  make a fight fair on every difficulty, tuning may also change a trainer's
  team, sets or items, keeping the fight's authored doubles plan and never
  weakening the AI's reasoning.
- Rare ability and item combinations on trainer Pokemon (e.g. Joey's Scrappy
  Galarian Farfetch'd with a Leek) are the point of their fights. Tune around
  them rather than through them, and use judgment: when one member decides
  the fight on its own, look at that member (its level, its enablers, a
  non-signature move) before moving the whole team. Changing the combo itself
  is a last resort to raise with the user. Let the trainer's dialogue warn the
  player about it.
- Every trainer must be beatable by a stage-legal team: one the player can
  hold before that battle per the progression manifest
  (`artifacts/progression-manifest/`, checked by `scripts/tuning_pool_check.py
  --trainer`), not merely anything inside its level-cap window. Hard is
  tuned against that team fully kitted: complete EV spreads, best natures,
  perfect IVs and its best legal items. One-hit KO moves and evasion boosts
  are never tuned for. The existing hatchling-only OHKO lessons remain limited
  to their designated families; the main tutor never offers them.
- Wild encounters: ordinary table slots are at least 4%; Legendary/Mythical,
  Ultra Beast and Paradox slots are 5%. Feebas keeps its native tile rule.
  Active storm visitors have a 25% encounter roll, reducing the normal table's
  share. After they settle, their ordinary resident slots are 5%.
  Caught legend-class species stop spawning. Gates live in
  `src/data/pokemon/legendary_signs.h`; legend-class spawns arrive at the
  current cap with authored sets. One Legendary, Mythical, Ultra Beast or
  Paradox Pokémon per party in total. Ordinary and pseudo-legendary
  Pokémon have no category limit. Player Mega Evolution uses that same slot;
  a restricted member may Mega Evolve itself, but other party members cannot
  while it is present; their Mega Stones may stay held.
  Opponents and AI partners are exempt. Wattson awards the Mega Ring after badge three; Norman awards
  starter-pair stones after badge five. Static legends are high stakes: a knockout loses one.
  No wild-table percentages in dialogue. The rival demonstrates DexNav after
  Birch's gift; Birch offers the detailed hunting lesson on request.
- DexNav and the PokéNav atlas share the native wild roster. Show every
  currently available species with its icon, without seen/caught discovery
  gating. Hide encounters until their actual unlock conditions are met.
  Resident legend searching is allowed. Active storm guests stay visible but
  require ordinary wild encounters; they cannot be searched or registered.
  Mirage Tower species may be missed.

- DexNav chains are shiny/Pokérus hunting only. Above zero, search odds are
  chain percent shiny and half-chain percent Pokérus (cap 100); independent
  rolls. Zero uses base odds. Searches expire after 15 seconds, with a visible countdown
  that turns red in the last five seconds. No level/egg-move/IV rewards.
  Search HUD shows icon/name, direction, remaining seconds, Hold A, and Chain
  only above zero.
- Pokérus adds five percentage points to a beneficial nature modifier
  (+10% to +15%);
  ordinary recovery retains the -10% drawback. It grants no EV bonus.
  Direct infections have two total transmissions, one adjacent recipient per
  successful postbattle spread roll; recipients cannot spread. No daily decay.
  The benefit follows future nature changes and survives recovery and boxing.
  Summary stats use gold for the Pokérus-enhanced nature stat, retaining the
  native active/recovered markers; no spread-count or boost-percentage text.
  After 25 consecutive steps in Lavaridge's hot-spring water, any party
  Pokémon that has had Pokérus can receive treatment from the spring NPC.
  Soaking preserves stats and transmission budgets; treatment is chosen per
  Pokémon after an actual-stat preview. Special recovery gives the favored
  nature stat +15% and the other nature stat +5%, both gold; its recovered
  mark is red. Neutral natures
  gain no bonus. Leaving the water resets soak progress. Special recovery ends
  spreading and follows later nature changes, boxing and saving.
