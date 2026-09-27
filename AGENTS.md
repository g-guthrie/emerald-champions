# Emerald Champions — standing rules

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
- Codex owns overworld presentation (map.json objects, local ids).

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
- Inclement's stat buffs and ability changes apply to player-owned and wild
  Pokémon only, never lowering a stat; trainer-owned Pokémon and tuned teams
  never change. Difficulty offsets live in `GetTrainerLevelReduction`; any
  text quoting them derives the numbers from it. Text is always Instant; there
  is no text speed option.
- No Game Book, player guides or Center battle presets. No
  new ground or hidden items: only replace original pickups or TM gifts.
- No Terastallization anywhere.
- Held items: the first Center nurse gives the Regenerator Key Item with the
  Leveler. With it, a held Berry its own holder eats in battle returns after
  battle. A Berry stolen, swapped away, eaten by a foe, burned or corroded is
  gone. Knock Off removes an item, Berries included, for the current battle
  only; Thief and Covet transfer permanently.
- EVs: every Pokémon joining the player (catch, gift, starter, trade, hatch)
  arrives with 252 HP / 52 Atk, Def, SpA, SpD / 50 Spe; owned Pokémon keep
  their chosen spread. Every Center move tutor plans a whole spread and
  applies it at once for a flat ¥500 (free when unchanged, nothing on
  cancel). Evie in Fallarbor keeps her ¥2-per-EV service.
- Steam Engine also makes Water never super effective on its holder.
- AI: authored doubles teams and the shared planner are preserved. Opponents may
  know loadouts but never read the player's committed move, target, switch or
  replacement. Tune difficulty with levels, not by weakening sets or strategy.
- Every trainer must be beatable by a stage-legal team.
- Wild encounters: ordinary table slots are at least 4%; Legendary/Mythical,
  Ultra Beast and Paradox slots are 5%. Feebas keeps its native tile rule.
  Storm visitors take their own share, reducing the normal table's share.
  Caught legend-class species stop spawning. Gates live in
  `src/data/pokemon/legendary_signs.h`; legend-class spawns arrive at the
  current cap with authored sets. One Legendary, Mythical, Ultra Beast or
  Paradox per party in total; ordinary and pseudo-legendary Pokemon have no
  category limit. Static legends are high stakes: a knockout loses one.
  No percentages in dialogue.
- DexNav and the PokéNav atlas share the native wild roster. Show every
  currently available species with its icon, without seen/caught discovery
  gating. Hide encounters until their actual unlock conditions are met.
  Resident legend searching is allowed. Active storm guests stay visible but
  require ordinary wild encounters; they cannot be searched or registered.
  Mirage Tower species may be missed.
